#include "test.hpp"
#include "Polynomial.hpp"
#include "Term.hpp"
#include <cstddef>
#include <sstream>
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
			// Term(size_t exp, double coef) : l'ordre est verrouille ici parce que
			// Parser.cpp:112 l'inverse encore (cf. test_parser.cpp).
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

		Vtest::it("n'ajoute pas de séparateur avant le premier terme",[]{
			Vtest::expect(printed(std::vector<Term>(1, Term(1, -4)))).toBe("- 4 * X^1");
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
			// L'ordre d'arrivee est conserve : c'est au solveur de trier, pas au
			// stockage (cf. la "reduced form" du sujet).
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

	/* ------------------------------------------------------------------ *
	 *  Failles relevees dans Polynomial.                                  *
	 *  Ce test est ROUGE : il documente un bug du code de production, il   *
	 *  n'est pas la pour le faire passer au vert.                          *
	 * ------------------------------------------------------------------ */
	Vtest::describe("faille",[]{
		Vtest::it("setTermsRight doit écrire le membre droit",[]{
			// Faille : Polynomial::setTermsRight ecrit termsLeft
			// (include/Polynomial.hpp:33) : le membre gauche est ecrase et le
			// droit garde ses anciennes valeurs.
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
