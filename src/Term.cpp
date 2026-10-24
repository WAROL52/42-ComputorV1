/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Term.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rrabetsy <rrabetsy@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 17:25:49 by rrabetsy          #+#    #+#             */
/*   Updated: 2026/10/24 18:19:35 by rrabetsy         ###   ########.fr       */
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
		if (i)
		{
			os << " ";
			if (term.getCoef() >= 0)
			{
				os << "+ ";
			}
		}
		os << term;
		i++;
	}
	return os;
}