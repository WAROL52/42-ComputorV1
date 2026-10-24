/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Term.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rrabetsy <rrabetsy@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 17:25:49 by rrabetsy          #+#    #+#             */
/*   Updated: 2026/10/24 19:18:03 by rrabetsy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Term.hpp"

Term::Term()
	: exp(0), coef(0)
{
}

Term::Term(size_t exp, double coef)
	: exp(exp), coef(coef)
{
}

Term::~Term()
{
}

bool operator<(const Term &lhs, const Term &rhs)
{
	if (lhs.getExp() != rhs.getExp())
		return lhs.getExp() < rhs.getExp();
	return lhs.getCoef() < rhs.getCoef();
}

bool operator==(const Term &lhs, const Term &rhs)
{
	return lhs.getExp() == rhs.getExp() && lhs.getCoef() == rhs.getCoef();
}

bool operator!=(const Term &lhs, const Term &rhs)
{
	return !(lhs == rhs);
}

bool operator>(const Term &lhs, const Term &rhs)
{
	return rhs < lhs;
}

bool operator<=(const Term &lhs, const Term &rhs)
{
	return !(rhs < lhs);
}

bool operator>=(const Term &lhs, const Term &rhs)
{
	return !(lhs < rhs);
}

std::ostream &operator<<(std::ostream &os, const Term &term)
{
	int sign = 1;
	double coef = term.getCoef();
	if (coef == 0)
		coef = 0;
	if (coef < 0)
	{
		sign = -1;
		os << "- ";
	}
	os << coef * sign
	   << " * X^"
	   << term.getExp();
	return os;
}

std::ostream &operator<<(std::ostream &os, const std::vector<Term> &terms)
{
	size_t i = 0;
	for (const auto &term : terms)
	{
		double coef = term.getCoef();
		if (coef == 0)
			coef = 0;
		if (i)
			os << (coef < 0 ? " - " : " + ");
		else if (coef < 0)
			os << "-";
		os << (coef < 0 ? -coef : coef) << " * X^" << term.getExp();
		i++;
	}
	return os;
}