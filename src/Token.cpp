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
#include <cctype>
#include <stdexcept>

Token::Token(Type type, size_t index, std::string value)
	: type(type), index(index), value(value)
{
}
Token::~Token()
{
}

Token Token::from(const std::string &input, size_t index)
{
	if (index >= input.size())
	{
		throw std::out_of_range("Index out of string bounds.");
	}
	if (std::isdigit(static_cast<unsigned char>(input[index])))
	{
		size_t start = index;
		std::string numStr;
		while (index < input.size() && std::isdigit(static_cast<unsigned char>(input[index])))
		{
			numStr += input[index];
			index++;
		}
		if (index < input.size() && input[index] == '.')
		{
			if (index + 1 >= input.size() || !std::isdigit(static_cast<unsigned char>(input[index + 1])))
			{
				throw std::runtime_error("Invalid number format at position "
					+ std::to_string(index) + ": a decimal point must be followed by a digit.");
			}
			numStr += input[index];
			index++;
			while (index < input.size() && std::isdigit(static_cast<unsigned char>(input[index])))
			{
				numStr += input[index];
				index++;
			}
		}
		if (index < input.size() && input[index] == '.')
		{
			throw std::runtime_error("Invalid number format at position "
				+ std::to_string(index) + ": multiple decimal points found.");
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
		return Token(Type::EQUAL, index, "=");
	default:
		throw std::runtime_error(std::string("Unknown character found: ") + c
			+ " at position " + std::to_string(index));
	}
}
