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