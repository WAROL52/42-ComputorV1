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
};

std::ostream &operator<<(std::ostream &os, const Polynomial &polinom);

#endif
