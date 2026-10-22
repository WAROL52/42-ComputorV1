/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rrabetsy <rrabetsy@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 16:07:45 by rrabetsy          #+#    #+#             */
/*   Updated: 2026/10/22 16:07:46 by rrabetsy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Token.hpp"
#include <stdexcept>

Token::Token(Type type, size_t index, std::string value)
	: type(type), index(index), value(value)
{
}
Token::~Token()
{
}

Token Token::from(const std::string &input, size_t index = 0)
{
	if (index >= input.size())
	{
		throw std::out_of_range("Index out of string bounds.");
	}
	if (std::isdigit(input[index]))
	{
		size_t start = index;
		std::string numStr = "";
		bool hasDot = 0;
		while (index < input.size() && (std::isdigit(input[index]) || input[index] == '.'))
		{
			numStr += input[index];
			if (hasDot && input[index] == '.')
			{
				throw std::runtime_error("Invalid number format: multiple decimal points found.");
			}
			hasDot = input[index] == '.';
			index++;
		}
		return Token(Type::NUM, start, numStr);
	}
	const char c = input[index];
	switch (c)
	{
	case 'X':
		return Token(Type::X, index, "X");
	case '^':
		return Token(Type::CARET, index, "^");
	case '+':
		return Token(Type::PLUS, index, "+");
	case '-':
		return Token(Type::MINUS, index, "-");
	case '*':
		return Token(Type::STAR, index, "*");
	case '=':
		return Token(Type::STAR, index, "=");
	default:
		throw std::runtime_error(std::string("Unknown character found: ") + c);
		break;
	}
}