/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Term.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rrabetsy <rrabetsy@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 17:09:43 by rrabetsy          #+#    #+#             */
/*   Updated: 2026/10/22 18:50:20 by rrabetsy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TERM_HPP
#define TERM_HPP
#include <iostream>
#include <vector>

class Term
{
private:
	size_t exp;
	double coef;

public:
	Term();
	Term(size_t exp, double coef);
	~Term();

	inline size_t getExp() const
	{
		return this->exp;
	}
	inline double getCoef() const
	{
		return this->coef;
	}
	inline void setExp(size_t exp)
	{
		this->exp = exp;
	}
	inline void setCoef(double coef)
	{
		this->coef = coef;
	}
};

/* Ordre total pour std::sort : exposant croissant, puis coefficient. */
bool operator<(const Term &lhs, const Term &rhs);
bool operator>(const Term &lhs, const Term &rhs);
bool operator<=(const Term &lhs, const Term &rhs);
bool operator>=(const Term &lhs, const Term &rhs);
bool operator==(const Term &lhs, const Term &rhs);
bool operator!=(const Term &lhs, const Term &rhs);

std::ostream &operator<<(std::ostream &os, const Term &term);
std::ostream &operator<<(std::ostream &os, const std::vector<Term> &terms);

#endif