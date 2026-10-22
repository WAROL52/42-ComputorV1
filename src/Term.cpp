/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Term.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rrabetsy <rrabetsy@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 17:25:49 by rrabetsy          #+#    #+#             */
/*   Updated: 2026/10/22 18:50:48 by rrabetsy         ###   ########.fr       */
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

std::ostream &operator<<(std::ostream &os, const Term &term)
{
	int sign = 1;
	if (term.getCoef() < 0)
	{
		sign = -1;
		os << "- ";
	}
	os << term.getCoef() * sign
	   << " * X^"
	   << term.getExp();
	return os;
}

std::ostream &operator<<(std::ostream &os, const std::vector<Term> &terms)
{
	size_t i = 0;
	for (std::vector<Term>::const_iterator it = terms.begin(); it != terms.end(); ++it)
	{
		if (i)
		{
			os << " ";
			if (it->getCoef() >= 0)
			{
				os << "+ ";
			}
		}
		os << *it;
		i++;
	}
	return os;
}