#include "test.hpp"
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

#include "Parser.hpp"

namespace
{

	Polynomial parse(const std::string &source)
	{
		return Parser::parse(Lexer::tokenize(source));
	}

	double coefOf(const Term &term)
	{
		return term.getCoef();
	}

	double expOf(const Term &term)
	{
		return static_cast<double>(term.getExp());
	}

	/* Acces borne : un indice absent leve une erreur lisible au lieu de lire hors
	   du vecteur (comportement indefini). */
	const Term &termAt(const std::vector<Term> &terms, std::size_t index)
	{
		if (index >= terms.size())
			throw std::runtime_error("terme " + std::to_string(index) + " absent, il y en a " + std::to_string(terms.size()));
		return terms[index];
	}

	void expectTerm(const Term &term, double coef, double exp)
	{
		Vtest::expect(coefOf(term)).toBe(coef);
		Vtest::expect(expOf(term)).toBe(exp);
	}

	void expectThrows(const std::string &source)
	{
		Vtest::expect([source]
					  { Parser::parse(Lexer::tokenize(source)); })
			.toThrow();
	}

	/* En C++ il n'y a pas de TypeError : le pendant de "ne leve pas un TypeError"
	   est "leve une std::exception" (ni rien du tout, ni un type inconnu). */
	std::string thrownKind(const std::string &source)
	{
		try
		{
			Parser::parse(Lexer::tokenize(source));
		}
		catch (const std::exception &)
		{
			return "std::exception";
		}
		catch (...)
		{
			return "inconnue";
		}
		return "aucune";
	}

} // namespace

VTEST_SUITE([]
			{
				Vtest::describe("parse", []
								{
		Vtest::it("sépare les deux membres",[]{
			Polynomial polynome = parse("5 * X^0 + 4 * X^1 = 4 * X^0");
			Vtest::expect(polynome.getTermsLeft().size()).toBe(std::size_t(2));
			Vtest::expect(polynome.getTermsRight().size()).toBe(std::size_t(1));
			expectTerm(termAt(polynome.getTermsLeft(), 0), 5, 0);
			expectTerm(termAt(polynome.getTermsLeft(), 1), 4, 1);
			expectTerm(termAt(polynome.getTermsRight(), 0), 4, 0);
		});

		Vtest::it("applique le signe au coefficient",[]{
			Polynomial polynome = parse("5 * X^0 - 9.3 * X^2 = 1 * X^0");
			expectTerm(termAt(polynome.getTermsLeft(), 1), -9.3, 2);
		});

		Vtest::it("refuse une équation sans signe égal",[]{
			expectThrows("5 * X^0 + 4 * X^1");
		});

		Vtest::it("refuse une équation avec plusieurs signes égaux",[]{
			expectThrows("5 * X^0 + 4 * X^1 = 4 * X^0 = 3 * X^0");
		});

		Vtest::it("gère les coefficients négatifs",[]{
			Polynomial polynome = parse("-5 * X^0 + 4 * X^1 = 1 * X^0");
			expectTerm(termAt(polynome.getTermsLeft(), 0), -5, 0);
		});

		Vtest::it("rejette un exposant négatif",[]{
			expectThrows("5 * X^0 + 4 * X^-1 = 1 * X^0");
		});

		Vtest::it("rejette un exposant non entier",[]{
			expectThrows("5 * X^2.5 = 1 * X^0");
		});

		Vtest::it("rejette les signes répétés",[]{
			expectThrows("5 * X^0 + - 4 * X^1 = 1 * X^0");
		});

		Vtest::it("rejette deux termes collés sans opérateur",[]{
			expectThrows("5 * X^0 4 * X^1 = 1 * X^0");
		});

		Vtest::it("rejette un membre droit vide",[]{
			expectThrows("5 * X^0 =");
		});

		Vtest::it("rejette un membre gauche vide",[]{
			expectThrows("= 5 * X^0");
		});

		Vtest::it("échoue proprement sur une entrée tronquée, sans crash",[]{
			Vtest::expect(thrownKind("5 * X^0 = 1 * X^")).toBe("std::exception");
		});

		Vtest::it("accepte un nombre nu comme terme de degré 0",[]{
			Polynomial polynome = parse("5 * X^0 = 4");
			Vtest::expect(polynome.getTermsLeft().size()).toBe(std::size_t(1));
			Vtest::expect(polynome.getTermsRight().size()).toBe(std::size_t(1));
			expectTerm(termAt(polynome.getTermsLeft(), 0), 5, 0);
			expectTerm(termAt(polynome.getTermsRight(), 0), 4, 0);
		});

		Vtest::it("échoue proprement sur un signe en fin d'entrée",[]{
			Vtest::expect(thrownKind("5 * X^0 = -")).toBe("std::exception");
		});

		Vtest::it("rejette un exposant précédé du signe +",[]{
			expectThrows("5 * X^0 = 5 * X^+2");
		});

		Vtest::it("applique un signe négatif en tête de membre droit",[]{
			Polynomial polynome = parse("5 * X^0 = - 4 * X^1");
			expectTerm(termAt(polynome.getTermsRight(), 0), -4, 1);
		});

		Vtest::it("applique un signe + en tête de membre droit",[]{
			Polynomial polynome = parse("5 * X^0 = + 4 * X^1");
			expectTerm(termAt(polynome.getTermsRight(), 0), 4, 1);
		});

		Vtest::it("accepte plusieurs termes à droite",[]{
			Polynomial polynome = parse("5 * X^0 = 1 * X^0 + 2 * X^1");
			Vtest::expect(polynome.getTermsRight().size()).toBe(std::size_t(2));
			expectTerm(termAt(polynome.getTermsRight(), 0), 1, 0);
			expectTerm(termAt(polynome.getTermsRight(), 1), 2, 1);
		});

		Vtest::it("accepte un coefficient nul à gauche",[]{
			Polynomial polynome = parse("0 * X^0 = 5 * X^0");
			expectTerm(termAt(polynome.getTermsLeft(), 0), 0, 0);
		});

		Vtest::it("accepte une équation qui s'annule entièrement",[]{
			Polynomial polynome = parse("5 * X^0 + 4 * X^1 = 5 * X^0 + 4 * X^1");
			Vtest::expect(polynome.getTermsLeft().size()).toBe(std::size_t(2));
			Vtest::expect(polynome.getTermsRight().size()).toBe(std::size_t(2));
			expectTerm(termAt(polynome.getTermsLeft(), 0), 5, 0);
			expectTerm(termAt(polynome.getTermsLeft(), 1), 4, 1);
			expectTerm(termAt(polynome.getTermsRight(), 0), 5, 0);
			expectTerm(termAt(polynome.getTermsRight(), 1), 4, 1);
		});

		Vtest::it("rejette une étoile manquante",[]{
			expectThrows("5 X^0 = 4 * X^0");
		});

		Vtest::it("rejette une entrée vide",[]{
			expectThrows("");
		});

		Vtest::it("rejette une entrée blanche",[]{
			expectThrows("   ");
		});

		Vtest::it("rejette un signe égal seul",[]{
			expectThrows("=");
		});

		Vtest::it("rejette un double signe égal collé",[]{
			expectThrows("5 * X^0 == 4 * X^0");
		}); });

		/* Les six équations du mandatory, telles quelles : elles servent de
		   garde-fou au parseur avant même que le binaire complet n'existe. */
		Vtest::describe("cas du sujet (mandatory)", []{
			Vtest::it("lit l'équation à deux racines",[]{
				Polynomial polynome = parse("5 * X^0 + 4 * X^1 - 9.3 * X^2 = 1 * X^0");
				Vtest::expect(polynome.getTermsLeft().size()).toBe(std::size_t(3));
				Vtest::expect(polynome.getTermsRight().size()).toBe(std::size_t(1));
				expectTerm(termAt(polynome.getTermsLeft(), 0), 5, 0);
				expectTerm(termAt(polynome.getTermsLeft(), 1), 4, 1);
				expectTerm(termAt(polynome.getTermsLeft(), 2), -9.3, 2);
				expectTerm(termAt(polynome.getTermsRight(), 0), 1, 0);
			});

			Vtest::it("lit l'équation de degré 1",[]{
				Polynomial polynome = parse("5 * X^0 + 4 * X^1 = 4 * X^0");
				Vtest::expect(polynome.getTermsLeft().size()).toBe(std::size_t(2));
				Vtest::expect(polynome.getTermsRight().size()).toBe(std::size_t(1));
				expectTerm(termAt(polynome.getTermsLeft(), 0), 5, 0);
				expectTerm(termAt(polynome.getTermsLeft(), 1), 4, 1);
				expectTerm(termAt(polynome.getTermsRight(), 0), 4, 0);
			});

			Vtest::it("lit l'équation de degré 3 avec un coefficient nul",[]{
				// "0 * X^2" doit rester dans le membre gauche : la forme réduite
				// attendue l'affiche ("+ 0 * X^2").
				Polynomial polynome = parse("8 * X^0 - 6 * X^1 + 0 * X^2 - 5.6 * X^3 = 3 * X^0");
				Vtest::expect(polynome.getTermsLeft().size()).toBe(std::size_t(4));
				Vtest::expect(polynome.getTermsRight().size()).toBe(std::size_t(1));
				expectTerm(termAt(polynome.getTermsLeft(), 0), 8, 0);
				expectTerm(termAt(polynome.getTermsLeft(), 1), -6, 1);
				expectTerm(termAt(polynome.getTermsLeft(), 2), 0, 2);
				expectTerm(termAt(polynome.getTermsLeft(), 3), -5.6, 3);
				expectTerm(termAt(polynome.getTermsRight(), 0), 3, 0);
			});

			Vtest::it("lit l'équation qui s'annule",[]{
				Polynomial polynome = parse("6 * X^0 = 6 * X^0");
				Vtest::expect(polynome.getTermsLeft().size()).toBe(std::size_t(1));
				Vtest::expect(polynome.getTermsRight().size()).toBe(std::size_t(1));
				expectTerm(termAt(polynome.getTermsLeft(), 0), 6, 0);
				expectTerm(termAt(polynome.getTermsRight(), 0), 6, 0);
			});

			Vtest::it("lit l'équation sans solution",[]{
				Polynomial polynome = parse("10 * X^0 = 15 * X^0");
				Vtest::expect(polynome.getTermsLeft().size()).toBe(std::size_t(1));
				Vtest::expect(polynome.getTermsRight().size()).toBe(std::size_t(1));
				expectTerm(termAt(polynome.getTermsLeft(), 0), 10, 0);
				expectTerm(termAt(polynome.getTermsRight(), 0), 15, 0);
			});

			Vtest::it("lit l'équation à discriminant négatif",[]{
				Polynomial polynome = parse("1 * X^0 + 2 * X^1 + 5 * X^2 = 0");
				Vtest::expect(polynome.getTermsLeft().size()).toBe(std::size_t(3));
				Vtest::expect(polynome.getTermsRight().size()).toBe(std::size_t(1));
				expectTerm(termAt(polynome.getTermsLeft(), 0), 1, 0);
				expectTerm(termAt(polynome.getTermsLeft(), 1), 2, 1);
				expectTerm(termAt(polynome.getTermsLeft(), 2), 5, 2);
				expectTerm(termAt(polynome.getTermsRight(), 0), 0, 0);
			});
		});

				// Les tests de Term et Polynomial vivent dans test_polynomial.cpp.
			});
