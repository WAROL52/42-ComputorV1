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
		Vtest::expect(term.getCoef()).toBe(coef);
		Vtest::expect(static_cast<double>(term.getExp())).toBe(exp);
	}

} // namespace

/* Bonus : la forme libre du sujet ("5 + 4 * X + X^2= X^2"). Ces tests vivent
   dans leur propre binaire (make unit_bonus) pour que make unit ne mesure que
   le mandatory. Tant que le parseur n'accepte pas la forme libre, ils restent
   rouges : c'est le suivi du bonus, pas une regression du mandatory. */
VTEST_SUITE([]
			{
				Vtest::describe("parse libre (bonus)", []
								{
		/* Un terme de degre 0 peut s'ecrire sans "* X^0". Le sujet garantit par
		   ailleurs que l'entree du mandatory est toujours bien formee, donc
		   refuser ces ecritures n'apporte aucun point : les accepter, si. */
		Vtest::it("accepte un nombre nu au milieu d'un membre",[]{
			Polynomial polynome = parse("5 * X^0 + 3 = 4 * X^0");
			Vtest::expect(polynome.getTermsLeft().size()).toBe(std::size_t(2));
			expectTerm(termAt(polynome.getTermsLeft(), 0), 5, 0);
			expectTerm(termAt(polynome.getTermsLeft(), 1), 3, 0);
			expectTerm(termAt(polynome.getTermsRight(), 0), 4, 0);
		});

		Vtest::it("accepte un nombre nu après un terme complet",[]{
			Polynomial polynome = parse("5 * X^0 = 4 * X^0 + 3");
			Vtest::expect(polynome.getTermsRight().size()).toBe(std::size_t(2));
			expectTerm(termAt(polynome.getTermsRight(), 0), 4, 0);
			expectTerm(termAt(polynome.getTermsRight(), 1), 3, 0);
		});

		Vtest::it("accepte un nombre négatif nu à droite",[]{
			Polynomial polynome = parse("5 * X^0 = -4");
			expectTerm(termAt(polynome.getTermsRight(), 0), -4, 0);
		});

		Vtest::it("accepte un coefficient implicite",[]{
			/* Forme libre : "X^0" vaut 1 * X^0. C'est le cas de "X^2" dans
			   l'exemple du bonus du sujet : "5 + 4 * X + X^2 = X^2". */
			Polynomial polynome = parse("X^0 = 4 * X^0");
			Vtest::expect(polynome.getTermsLeft().size()).toBe(std::size_t(1));
			expectTerm(termAt(polynome.getTermsLeft(), 0), 1, 0);
			expectTerm(termAt(polynome.getTermsRight(), 0), 4, 0);
		});

		Vtest::it("sous-entend l'exposant 1 quand il manque",[]{
			/* Forme libre : "4 * X" vaut 4 * X^1. Sinon l'exemple du bonus est lu
			   comme un degre 0 et la solution affichee est fausse. */
			Polynomial polynome = parse("4 * X = 0");
			expectTerm(termAt(polynome.getTermsLeft(), 0), 4, 1);
		});

		Vtest::it("accepte l'équation en forme libre du sujet",[]{
			// Exemple exact du bonus : "5 + 4 * X + X^2= X^2" (sans espace autour
			// du '=', sans exposant sur 4 * X, sans coefficient sur X^2).
			Polynomial polynome = parse("5 + 4 * X + X^2= X^2");
			Vtest::expect(polynome.getTermsLeft().size()).toBe(std::size_t(3));
			Vtest::expect(polynome.getTermsRight().size()).toBe(std::size_t(1));
			expectTerm(termAt(polynome.getTermsLeft(), 0), 5, 0);
			expectTerm(termAt(polynome.getTermsLeft(), 1), 4, 1);
			expectTerm(termAt(polynome.getTermsLeft(), 2), 1, 2);
			expectTerm(termAt(polynome.getTermsRight(), 0), 1, 2);
		}); });
			});
