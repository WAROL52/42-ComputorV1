/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Polynomial.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rrabetsy <rrabetsy@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 19:03:35 by rrabetsy          #+#    #+#             */
/*   Updated: 2026/10/22 19:03:36 by rrabetsy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Polynomial.hpp"
#include <algorithm>
#include <stdexcept>

namespace
{
	const double ZERO_EPSILON = 1e-9;

	bool isZero(double coef)
	{
		return coef > -ZERO_EPSILON && coef < ZERO_EPSILON;
	}
} // namespace

std::ostream &operator<<(std::ostream &os, const Polynomial &polinom)
{
	os << polinom.getTermsLeft();
	if (polinom.getTermsLeft().size() == 0)
	{
		os << "0";
	}
	os << " = "
	   << polinom.getTermsRight();
	if (polinom.getTermsRight().size() == 0)
	{
		os << "0";
	}
	return os;
}

Polynomial::Polynomial()
{
}

Polynomial::Polynomial(const std::vector<Term> &termsLeft, const std::vector<Term> &termsRight)
	: termsLeft(termsLeft), termsRight(termsRight)
{
}

Polynomial::~Polynomial()
{
}

Polynomial &Polynomial::reduce()
{
	std::vector<Term> combined;
	combined.reserve(this->termsLeft.size() + this->termsRight.size());
	for (size_t i = 0; i < this->termsLeft.size(); ++i)
		combined.push_back(this->termsLeft[i]);
	for (size_t i = 0; i < this->termsRight.size(); ++i)
		combined.push_back(Term(this->termsRight[i].getExp(), -this->termsRight[i].getCoef()));
	std::sort(combined.begin(), combined.end());
	std::vector<Term> reduced;
	reduced.reserve(combined.size());
	size_t i = 0;
	while (i < combined.size())
	{
		const size_t exp = combined[i].getExp();
		double coef = 0.0;
		while (i < combined.size() && combined[i].getExp() == exp)
		{
			coef += combined[i].getCoef();
			++i;
		}
		if (isZero(coef))
			coef = 0.0;
		reduced.push_back(Term(exp, coef));
	}
	if (reduced.empty())
		reduced.push_back(Term(0, 0.0));
	this->termsLeft = reduced;
	this->termsRight.clear();
	return *this;
}

bool Polynomial::isReduced() const
{
	if (!this->termsRight.empty() || this->termsLeft.empty())
		return false;
	for (size_t i = 1; i < this->termsLeft.size(); ++i)
	{
		if (this->termsLeft[i - 1].getExp() >= this->termsLeft[i].getExp())
			return false;
	}
	return true;
}

size_t Polynomial::getDegree() const
{
	if (!this->isReduced())
		throw std::logic_error("Polynomial::getDegree: reduce() must be called first");
	size_t degree = 0;
	for (size_t i = 0; i < this->termsLeft.size(); ++i)
	{
		if (!isZero(this->termsLeft[i].getCoef()))
			degree = this->termsLeft[i].getExp();
	}
	return degree;
}
