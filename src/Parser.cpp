/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rrabetsy <rrabetsy@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 21:10:30 by rrabetsy          #+#    #+#             */
/*   Updated: 2026/10/22 21:11:29 by rrabetsy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

Parser::Parser(/* args */)
{
}

Parser::~Parser()
{
}

Polynomial Parser::parse(const std::vector<Token> &tokens)
{
	bool foundEqual = false;
	std::vector<Term> termsLeft;
	std::vector<Term> termsRight;

	std::vector<Term> *terms = &termsLeft;

	if (tokens.size() == 0)
		throw std::runtime_error("Empty tokens");

	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (tokens[i].isTypeEqual())
		{
			if (foundEqual)
				throw std::runtime_error("Unexpected '=' token");
			if (termsLeft.size() == 0)
				throw std::runtime_error("Left side of equation is empty");
			terms = &termsRight;
			foundEqual = true;
			continue;
		}
		terms->push_back(parseTerm(tokens, i));
	}
	if (!foundEqual)
		throw std::runtime_error("Missing '=' token");
	if (termsRight.size() == 0)
		throw std::runtime_error("Right side of equation is empty");
	return Polynomial(termsLeft, termsRight);
}
Term Parser::parseTerm(const std::vector<Token> &tokens, size_t &cursor)
{
	if (cursor >= tokens.size())
		throw std::runtime_error("Syntax error: Unexpected end of input while parsing term.");

	double coefficient = 1.0;
	int degree = 0;
	int sign = 1;

	if (tokens[cursor].isTypePlus() || tokens[cursor].isTypeMinus())
	{
		if (tokens[cursor].getType() == Token::Type::MINUS)
			sign = -sign;
		cursor++;

		if (cursor >= tokens.size())
			throw std::runtime_error("Syntax error: Expected a term or number after sign.");
	}

	// 2. Lecture du coefficient numérique optionnel (ex: 9.3)
	bool hasCoeff = false;
	if (tokens[cursor].getType() == Token::Type::NUM)
	{
		coefficient = std::stod(tokens[cursor].getValue());
		hasCoeff = true;
		cursor++;
	}

	// 3. Gestion de la variable (* X ^ 2)
	if (cursor < tokens.size() && tokens[cursor].isTypeStart())
	{
		if (!hasCoeff)
			throw std::runtime_error("Syntax error: Unexpected '*' without coefficient.");
		cursor++;

		if (cursor >= tokens.size() || tokens[cursor].getType() != Token::Type::X)
			throw std::runtime_error("Syntax error: Expected variable 'X' after '*'.");
	}

	if (cursor < tokens.size() && tokens[cursor].isTypeX())
	{
		cursor++;
		degree = 1;
		if (cursor < tokens.size() && tokens[cursor].isTypeCaret())
		{
			cursor++;
			if (cursor >= tokens.size() || tokens[cursor].getType() != Token::Type::NUM)
				throw std::runtime_error("Syntax error: Expected an integer exponent after '^'.");

			degree = std::stoi(tokens[cursor].getValue());
			cursor++; // On passe le nombre de l'exposant
		}
	}
	else if (!hasCoeff)
	{
		throw std::runtime_error("Syntax error: Invalid term format.");
	}

	return Term(coefficient * sign, degree);
}