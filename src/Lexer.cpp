/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rrabetsy <rrabetsy@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 16:07:55 by rrabetsy          #+#    #+#             */
/*   Updated: 2026/10/22 16:14:20 by rrabetsy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Lexer.hpp"
#include <cctype>

Lexer::Lexer(/* args */)
{
}

Lexer::~Lexer()
{
}

std::vector<Token> Lexer::tokenize(const std::string &input)
{
	std::vector<Token> tokens;
	size_t index = 0;
	while (index < input.size())
	{
		if (std::isspace(static_cast<unsigned char>(input[index])))
		{
			index++;
			continue;
		}
		Token token = Token::from(input, index);
		tokens.push_back(token);
		index = token.getCursor();
	}
	return tokens;
}