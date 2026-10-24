#ifndef POLYNOMIAL_HPP
#define POLYNOMIAL_HPP
#include "Term.hpp"

class Polynomial
{
private:
	std::vector<Term> termsLeft;
	std::vector<Term> termsRight;

public:
	Polynomial();
	Polynomial(const std::vector<Term> &termsLeft, const std::vector<Term> &termsRight);
	~Polynomial();

	inline const std::vector<Term> &getTermsLeft() const
	{
		return this->termsLeft;
	}

	inline const std::vector<Term> &getTermsRight() const
	{
		return this->termsRight;
	}

	inline void setTermsLeft(const std::vector<Term> &term)
	{
		this->termsLeft = term;
	}

	inline void setTermsRight(const std::vector<Term> &term)
	{
		this->termsRight = term;
	}

	/* Ramene le membre droit a gauche (gauche - droite), combine les
	   exposants egaux, trie par exposant croissant et vide le membre droit.
	   Les exposants presents dans l'un des deux membres sont conserves,
	   meme si leur coefficient somme est nul (cf. 0 * X^2 du cas 03).
	   Un polynome sans aucun terme devient 0 * X^0. Les sommes
	   |coef| < 1e-9 sont ramenees a 0. Mute en place et renvoie *this. */
	Polynomial &reduce();

	/* Vrai si le membre droit est vide et si les exposants du membre gauche
	   sont strictement croissants (forme triee, sans doublon, non vide). */
	bool isReduced() const;

	/* Degre du membre gauche : plus grand exposant dont le coefficient n'est
	   pas nul (au sens de l'epsilon de reduce). Ne trie rien et n'exige pas
	   isReduced() ; appeler reduce() avant pour le degre de l'equation. */
	size_t getDegree() const;
};

std::ostream &operator<<(std::ostream &os, const Polynomial &polinom);

#endif
