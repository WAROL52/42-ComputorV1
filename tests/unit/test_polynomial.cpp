#include "test.hpp"
#include "Polynomial.hpp"
#include "Term.hpp"
#include <algorithm>
#include <cstddef>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace
{

std::string printed(const Term &term)
{
	std::ostringstream out;
	out << term;
	return out.str();
}

std::string printed(const std::vector<Term> &terms)
{
	std::ostringstream out;
	out << terms;
	return out.str();
}

std::string printed(const Polynomial &polynome)
{
	std::ostringstream out;
	out << polynome;
	return out.str();
}

} // namespace

VTEST_SUITE([]{
	Vtest::describe("term",[]{
		Vtest::it("vaut 0 * X^0 par défaut",[]{
			Term term;
			Vtest::expect(term.getExp()).toBe(std::size_t(0));
			Vtest::expect(term.getCoef()).toBe(0.0);
		});

		Vtest::it("range l'exposant puis le coefficient",[]{
			// Term(size_t exp, double coef) : l'ordre (exposant puis coefficient)
			// est verrouille ici pour eviter une inversion silencieuse.
			Term term(3, -2.5);
			Vtest::expect(term.getExp()).toBe(std::size_t(3));
			Vtest::expect(term.getCoef()).toBe(-2.5);
		});

		Vtest::it("les setters mettent à jour chaque champ",[]{
			Term term;
			term.setExp(7);
			term.setCoef(-0.25);
			Vtest::expect(term.getExp()).toBe(std::size_t(7));
			Vtest::expect(term.getCoef()).toBe(-0.25);
		});

		Vtest::it("l'exposant est entier et le coefficient réel",[]{
			// Verrou de regression : les deux types de retour etaient inverses,
			// ce qui tronquait 9.3 en 9 et rendait tout coefficient negatif
			// indefini (conversion double -> size_t).
			Vtest::expect(std::is_same<decltype(std::declval<const Term &>().getExp()), std::size_t>::value).toBe(true);
			Vtest::expect(std::is_same<decltype(std::declval<const Term &>().getCoef()), double>::value).toBe(true);
		});

		Vtest::it("est constructible, copiable et assignable",[]{
			// C'est ce qu'exige std::vector<Term>, donc Polynomial.
			Vtest::expect(std::is_default_constructible<Term>::value).toBe(true);
			Vtest::expect(std::is_copy_constructible<Term>::value).toBe(true);
			Vtest::expect(std::is_copy_assignable<Term>::value).toBe(true);
		});

		Vtest::it("imprime un coefficient positif",[]{
			Vtest::expect(printed(Term(2, 5))).toBe("5 * X^2");
		});

		Vtest::it("imprime un coefficient négatif",[]{
			Vtest::expect(printed(Term(2, -5))).toBe("- 5 * X^2");
		});

		Vtest::it("imprime un coefficient décimal négatif",[]{
			Vtest::expect(printed(Term(2, -9.3))).toBe("- 9.3 * X^2");
		});

		Vtest::it("imprime un coefficient nul",[]{
			Vtest::expect(printed(Term(0, 0))).toBe("0 * X^0");
		});
	});

	Vtest::describe("comparaisons de term",[]{
		Vtest::it("compare d'abord par exposant croissant",[]{
			Vtest::expect(Term(1, 2) < Term(2, -9)).toBe(true);
			Vtest::expect(Term(2, 9) < Term(1, 2)).toBe(false);
		});

		Vtest::it("départage les exposants égaux par coefficient",[]{
			Vtest::expect(Term(1, 2) < Term(1, 3)).toBe(true);
			Vtest::expect(Term(1, 3) < Term(1, 2)).toBe(false);
		});

		Vtest::it("compare l'égalité complète",[]{
			Vtest::expect(Term(1, 2) == Term(1, 2)).toBe(true);
			Vtest::expect(Term(1, 2) == Term(2, 2)).toBe(false);
			Vtest::expect(Term(1, 2) != Term(2, 2)).toBe(true);
		});

		Vtest::it("complète les comparaisons dérivées",[]{
			Vtest::expect(Term(2, 9) > Term(1, 2)).toBe(true);
			Vtest::expect(Term(1, 2) <= Term(1, 2)).toBe(true);
			Vtest::expect(Term(1, 3) >= Term(1, 2)).toBe(true);
		});

		Vtest::it("permet de trier un vecteur par exposant",[]{
			std::vector<Term> terms;
			terms.push_back(Term(2, 5));
			terms.push_back(Term(0, 1));
			terms.push_back(Term(1, -3));
			std::sort(terms.begin(), terms.end());
			Vtest::expect(terms[0].getExp()).toBe(std::size_t(0));
			Vtest::expect(terms[1].getExp()).toBe(std::size_t(1));
			Vtest::expect(terms[2].getExp()).toBe(std::size_t(2));
			Vtest::expect(printed(terms)).toBe("1 * X^0 - 3 * X^1 + 5 * X^2");
		});
	});

	Vtest::describe("vecteur de terms",[]{
		Vtest::it("joint les termes positifs avec +",[]{
			std::vector<Term> terms;
			terms.push_back(Term(0, 5));
			terms.push_back(Term(1, 4));
			Vtest::expect(printed(terms)).toBe("5 * X^0 + 4 * X^1");
		});

		Vtest::it("joint un terme négatif avec -",[]{
			std::vector<Term> terms;
			terms.push_back(Term(0, 5));
			terms.push_back(Term(1, -4));
			Vtest::expect(printed(terms)).toBe("5 * X^0 - 4 * X^1");
		});

		Vtest::it("colle le signe au premier terme négatif",[]{
			// Cas 05 du sujet : "Reduced form: -5 * X^0 = 0" (pas "- 5 * X^0").
			Vtest::expect(printed(std::vector<Term>(1, Term(1, -4)))).toBe("-4 * X^1");
		});

		Vtest::it("imprime un vecteur vide comme une chaîne vide",[]{
			// Le "0" d'un membre vide est ajoute par Polynomial, pas par le vecteur.
			Vtest::expect(printed(std::vector<Term>())).toBe("");
		});
	});

	Vtest::describe("polynomial",[]{
		Vtest::it("n'a aucun terme par défaut",[]{
			Polynomial polynome;
			Vtest::expect(polynome.getTermsLeft().size()).toBe(std::size_t(0));
			Vtest::expect(polynome.getTermsRight().size()).toBe(std::size_t(0));
		});

		Vtest::it("conserve les deux membres et leur ordre",[]{
			// Le stockage conserve l'ordre d'arrivee ; reduce() trie et combine
			// ensuite pour produire la "reduced form" du sujet.
			Polynomial polynome({Term(0, 5), Term(1, 4)}, {Term(0, 4)});
			Vtest::expect(polynome.getTermsLeft().size()).toBe(std::size_t(2));
			Vtest::expect(polynome.getTermsRight().size()).toBe(std::size_t(1));
			Vtest::expect(polynome.getTermsLeft()[0].getExp()).toBe(std::size_t(0));
			Vtest::expect(polynome.getTermsLeft()[0].getCoef()).toBe(5.0);
			Vtest::expect(polynome.getTermsLeft()[1].getExp()).toBe(std::size_t(1));
			Vtest::expect(polynome.getTermsLeft()[1].getCoef()).toBe(4.0);
			Vtest::expect(polynome.getTermsRight()[0].getExp()).toBe(std::size_t(0));
			Vtest::expect(polynome.getTermsRight()[0].getCoef()).toBe(4.0);
		});

		Vtest::it("setTermsLeft remplace le membre gauche seulement",[]{
			Polynomial polynome({Term(0, 1)}, {Term(1, 2)});
			polynome.setTermsLeft(std::vector<Term>(1, Term(3, 7)));
			Vtest::expect(polynome.getTermsLeft().size()).toBe(std::size_t(1));
			Vtest::expect(polynome.getTermsLeft()[0].getExp()).toBe(std::size_t(3));
			Vtest::expect(polynome.getTermsRight().size()).toBe(std::size_t(1));
			Vtest::expect(polynome.getTermsRight()[0].getExp()).toBe(std::size_t(1));
			Vtest::expect(polynome.getTermsRight()[0].getCoef()).toBe(2.0);
		});

		Vtest::it("imprime les deux membres",[]{
			Polynomial polynome({Term(0, 5), Term(1, 4)}, {Term(0, 4)});
			Vtest::expect(printed(polynome)).toBe("5 * X^0 + 4 * X^1 = 4 * X^0");
		});

		Vtest::it("imprime un coefficient nul au milieu comme le cas 03",[]{
			// "0 * X^2" doit rester visible dans la forme reduite.
			Polynomial polynome({Term(0, 5), Term(1, -6), Term(2, 0), Term(3, -5.6)}, std::vector<Term>());
			Vtest::expect(printed(polynome)).toBe("5 * X^0 - 6 * X^1 + 0 * X^2 - 5.6 * X^3 = 0");
		});

		Vtest::it("imprime 0 pour un membre vide",[]{
			Vtest::expect(printed(Polynomial())).toBe("0 = 0");
			Vtest::expect(printed(Polynomial({Term(0, 5)}, std::vector<Term>()))).toBe("5 * X^0 = 0");
			Vtest::expect(printed(Polynomial(std::vector<Term>(), {Term(0, 4)}))).toBe("0 = 4 * X^0");
		});

		Vtest::it("la copie est indépendante de l'original",[]{
			Polynomial original({Term(0, 5)}, {Term(1, 4)});
			Polynomial copie = original;
			copie.setTermsLeft(std::vector<Term>(1, Term(0, 1)));
			Vtest::expect(copie.getTermsLeft()[0].getCoef()).toBe(1.0);
			Vtest::expect(original.getTermsLeft()[0].getCoef()).toBe(5.0);
		});

		Vtest::it("est constructible, copiable et assignable",[]{
			// Parser::parse() le renvoie par valeur.
			Vtest::expect(std::is_default_constructible<Polynomial>::value).toBe(true);
			Vtest::expect(std::is_copy_constructible<Polynomial>::value).toBe(true);
			Vtest::expect(std::is_copy_assignable<Polynomial>::value).toBe(true);
		});
	});

	Vtest::describe("reduction",[]{
		Vtest::it("ramène le membre droit à gauche",[]{
			Polynomial polynome({Term(0, 5), Term(1, 4)}, {Term(0, 4)});
			polynome.reduce();
			Vtest::expect(polynome.getTermsRight().empty()).toBe(true);
			Vtest::expect(polynome.getTermsLeft().size()).toBe(std::size_t(2));
			Vtest::expect(polynome.getTermsLeft()[0].getExp()).toBe(std::size_t(0));
			Vtest::expect(polynome.getTermsLeft()[0].getCoef()).toBe(1.0);
			Vtest::expect(polynome.getTermsLeft()[1].getExp()).toBe(std::size_t(1));
			Vtest::expect(polynome.getTermsLeft()[1].getCoef()).toBe(4.0);
			Vtest::expect(printed(polynome)).toBe("1 * X^0 + 4 * X^1 = 0");
		});

		Vtest::it("combine les exposants égaux",[]{
			Polynomial polynome({Term(1, 2), Term(1, 3), Term(0, 4)}, {Term(1, 1)});
			polynome.reduce();
			Vtest::expect(printed(polynome)).toBe("4 * X^0 + 4 * X^1 = 0");
		});

		Vtest::it("trie par exposant croissant",[]{
			Polynomial polynome({Term(2, 5), Term(0, 1), Term(1, -3)}, std::vector<Term>());
			polynome.reduce();
			Vtest::expect(polynome.getTermsLeft()[0].getExp()).toBe(std::size_t(0));
			Vtest::expect(polynome.getTermsLeft()[1].getExp()).toBe(std::size_t(1));
			Vtest::expect(polynome.getTermsLeft()[2].getExp()).toBe(std::size_t(2));
			Vtest::expect(printed(polynome)).toBe("1 * X^0 - 3 * X^1 + 5 * X^2 = 0");
		});

		Vtest::it("conserve les coefficients nuls du cas 03",[]{
			Polynomial polynome({Term(0, 8), Term(1, -6), Term(2, 0), Term(3, -5.6)}, {Term(0, 3)});
			polynome.reduce();
			Vtest::expect(printed(polynome)).toBe("5 * X^0 - 6 * X^1 + 0 * X^2 - 5.6 * X^3 = 0");
			Vtest::expect(polynome.getTermsLeft()[2].getCoef()).toBe(0.0);
			Vtest::expect(polynome.isReduced()).toBe(true);
		});

		Vtest::it("produit 0 * X^0 quand tout s'annule (cas 04)",[]{
			Polynomial polynome({Term(0, 6)}, {Term(0, 6)});
			polynome.reduce();
			Vtest::expect(printed(polynome)).toBe("0 * X^0 = 0");
			Vtest::expect(polynome.getTermsLeft().size()).toBe(std::size_t(1));
			Vtest::expect(polynome.getTermsLeft()[0].getCoef()).toBe(0.0);
		});

		Vtest::it("produit 0 * X^0 sur un polynôme vide",[]{
			Polynomial polynome;
			polynome.reduce();
			Vtest::expect(printed(polynome)).toBe("0 * X^0 = 0");
			Vtest::expect(polynome.isReduced()).toBe(true);
		});

		Vtest::it("est idempotente",[]{
			Polynomial polynome({Term(1, 4), Term(0, 5)}, {Term(0, 4)});
			polynome.reduce();
			const std::string once = printed(polynome);
			polynome.reduce();
			Vtest::expect(printed(polynome)).toBe(once);
		});

		Vtest::it("ramène à zéro les sommes négligeables",[]{
			// 0.1 + 0.2 - 0.3 vaut 5.55e-17 en double ; sans epsilon, le
			// solveur conclurait "No solution" au lieu de "Any real number".
			Polynomial polynome({Term(0, 0.1), Term(0, 0.2)}, {Term(0, 0.3)});
			polynome.reduce();
			Vtest::expect(printed(polynome)).toBe("0 * X^0 = 0");
			Vtest::expect(polynome.getTermsLeft()[0].getCoef()).toBe(0.0);
		});

		Vtest::it("convertit un zéro négatif en zéro",[]{
			Polynomial polynome({Term(0, 5), Term(2, -0.0)}, std::vector<Term>());
			polynome.reduce();
			Vtest::expect(printed(polynome)).toBe("5 * X^0 + 0 * X^2 = 0");
			Vtest::expect(polynome.getTermsLeft()[1].getCoef()).toBe(0.0);
		});
	});

	Vtest::describe("isReduced",[]{
		Vtest::it("reconnaît une forme triée sans membre droit",[]{
			Vtest::expect(Polynomial({Term(0, 5), Term(1, 4)}, std::vector<Term>()).isReduced()).toBe(true);
			Vtest::expect(Polynomial({Term(0, 0)}, std::vector<Term>()).isReduced()).toBe(true);
		});

		Vtest::it("refuse un membre droit non vide",[]{
			Vtest::expect(Polynomial({Term(0, 5)}, {Term(0, 4)}).isReduced()).toBe(false);
		});

		Vtest::it("refuse des exposants non croissants",[]{
			Vtest::expect(Polynomial({Term(1, 4), Term(0, 5)}, std::vector<Term>()).isReduced()).toBe(false);
		});

		Vtest::it("refuse des exposants en double",[]{
			Vtest::expect(Polynomial({Term(1, 4), Term(1, 5)}, std::vector<Term>()).isReduced()).toBe(false);
		});

		Vtest::it("refuse un polynôme vide",[]{
			Vtest::expect(Polynomial().isReduced()).toBe(false);
		});

		Vtest::it("devient vrai après reduce",[]{
			Polynomial polynome({Term(1, 4), Term(0, 5)}, {Term(0, 4)});
			Vtest::expect(polynome.isReduced()).toBe(false);
			polynome.reduce();
			Vtest::expect(polynome.isReduced()).toBe(true);
		});
	});

	Vtest::describe("getDegree",[]{
		Vtest::it("renvoie le plus grand exposant non nul",[]{
			Polynomial polynome({Term(0, 1), Term(1, 4), Term(2, -9.3)}, std::vector<Term>());
			Vtest::expect(polynome.getDegree()).toBe(std::size_t(2));
		});

		Vtest::it("ignore les coefficients nuls",[]{
			Polynomial polynome({Term(0, 5), Term(2, 0), Term(3, -5.6)}, std::vector<Term>());
			Vtest::expect(polynome.getDegree()).toBe(std::size_t(3));
			Polynomial zeroAuBout({Term(0, 5), Term(3, 0)}, std::vector<Term>());
			Vtest::expect(zeroAuBout.getDegree()).toBe(std::size_t(0));
		});

		Vtest::it("renvoie 0 pour le polynôme nul",[]{
			Polynomial polynome({Term(0, 0)}, std::vector<Term>());
			Vtest::expect(polynome.getDegree()).toBe(std::size_t(0));
		});

		Vtest::it("exige une forme réduite",[]{
			Polynomial polynome({Term(0, 5)}, {Term(0, 5)});
			bool lance = false;
			try
			{
				(void)polynome.getDegree();
			}
			catch (const std::logic_error &)
			{
				lance = true;
			}
			Vtest::expect(lance).toBe(true);
		});
	});

	Vtest::describe("formes réduites du sujet",[]{
		Vtest::it("cas 01 : deux racines",[]{
			Polynomial polynome({Term(0, 5), Term(1, 4), Term(2, -9.3)}, {Term(0, 1)});
			polynome.reduce();
			Vtest::expect(printed(polynome)).toBe("4 * X^0 + 4 * X^1 - 9.3 * X^2 = 0");
			Vtest::expect(polynome.getDegree()).toBe(std::size_t(2));
		});

		Vtest::it("cas 02 : degré 1",[]{
			Polynomial polynome({Term(0, 5), Term(1, 4)}, {Term(0, 4)});
			polynome.reduce();
			Vtest::expect(printed(polynome)).toBe("1 * X^0 + 4 * X^1 = 0");
			Vtest::expect(polynome.getDegree()).toBe(std::size_t(1));
		});

		Vtest::it("cas 03 : degré 3 avec coefficient nul",[]{
			Polynomial polynome({Term(0, 8), Term(1, -6), Term(2, 0), Term(3, -5.6)}, {Term(0, 3)});
			polynome.reduce();
			Vtest::expect(printed(polynome)).toBe("5 * X^0 - 6 * X^1 + 0 * X^2 - 5.6 * X^3 = 0");
			Vtest::expect(polynome.getDegree()).toBe(std::size_t(3));
		});

		Vtest::it("cas 04 : toute solution réelle",[]{
			Polynomial polynome({Term(0, 6)}, {Term(0, 6)});
			polynome.reduce();
			Vtest::expect(printed(polynome)).toBe("0 * X^0 = 0");
			Vtest::expect(polynome.getDegree()).toBe(std::size_t(0));
		});

		Vtest::it("cas 05 : aucune solution",[]{
			Polynomial polynome({Term(0, 10)}, {Term(0, 15)});
			polynome.reduce();
			Vtest::expect(printed(polynome)).toBe("-5 * X^0 = 0");
			Vtest::expect(polynome.getDegree()).toBe(std::size_t(0));
		});

		Vtest::it("cas 06 : complexes",[]{
			Polynomial polynome({Term(0, 1), Term(1, 2), Term(2, 5)}, std::vector<Term>());
			polynome.reduce();
			Vtest::expect(printed(polynome)).toBe("1 * X^0 + 2 * X^1 + 5 * X^2 = 0");
			Vtest::expect(polynome.getDegree()).toBe(std::size_t(2));
		});
	});

	/* ------------------------------------------------------------------ *
	 *  Format de la forme réduite : gardes de régression.                 *
	 *  Les écarts avec le sujet ont été corrigés.                         *
	 * ------------------------------------------------------------------ */
	Vtest::describe("format (failles)",[]{
		Vtest::it("colle le signe au premier terme négatif (cas 05)",[]{
			// Le sujet attend "Reduced form: -5 * X^0 = 0" (pas "- 5 * X^0").
			Polynomial polynome({Term(0, -5)}, std::vector<Term>());
			Vtest::expect(printed(polynome)).toBe("-5 * X^0 = 0");
		});

		Vtest::it("imprime un zéro négatif comme zéro",[]{
			// "- 0 * X^2" est une entrée valide. Aujourd'hui l'opérateur produit
			// "+ -0 * X^2", qui n'est même pas re-parseable (double signe).
			Vtest::expect(printed(Polynomial({Term(0, 5), Term(2, -0.0)}, std::vector<Term>())))
				.toBe("5 * X^0 + 0 * X^2 = 0");
			Vtest::expect(printed(Polynomial({Term(0, -0.0)}, std::vector<Term>())))
				.toBe("0 * X^0 = 0");
		});
	});

	/* ------------------------------------------------------------------ *
	 *  Anciennes failles de Polynomial.                                    *
	 *  setTermsRight est corrige : ce test sert de garde de regression.    *
	 * ------------------------------------------------------------------ */
	Vtest::describe("faille",[]{
		Vtest::it("setTermsRight doit écrire le membre droit",[]{
			// Regression : Polynomial::setTermsRight ecrivait termsLeft
			// (include/Polynomial.hpp:33) : le membre gauche etait ecrase et le
			// droit gardait ses anciennes valeurs.
			Polynomial polynome({Term(0, 5)}, {Term(1, 4)});
			polynome.setTermsRight(std::vector<Term>(1, Term(9, 9)));
			Vtest::expect(polynome.getTermsRight().size()).toBe(std::size_t(1));
			Vtest::expect(polynome.getTermsRight()[0].getExp()).toBe(std::size_t(9));
			Vtest::expect(polynome.getTermsRight()[0].getCoef()).toBe(9.0);
			Vtest::expect(polynome.getTermsLeft()[0].getExp()).toBe(std::size_t(0));
			Vtest::expect(polynome.getTermsLeft()[0].getCoef()).toBe(5.0);
		});
	});
});
