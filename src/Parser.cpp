/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rrabetsy <rrabetsy@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 21:10:30 by rrabetsy          #+#    #+#             */
/*   Updated: 2026/10/24 17:37:19 by rrabetsy         ###   ########.fr       */
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
			if (i + 1 == tokens.size() || i == 0)
				throw std::runtime_error("Left side of equation is empty");
			if (!tokens[i - 1].isTypeNum())
				throw std::runtime_error("Unexpected '=' token");
			terms = &termsRight;
			terms->push_back(Term());
			foundEqual = true;
			continue;
		}
		if (i == 0)
			terms->push_back(Term());
		else if ((tokens[i].isTypePlus() || tokens[i].isTypeMinus()) && tokens[i - 1].isTypeNum())
			terms->push_back(Term());

		if (!parseTerm(tokens, i, terms->at(terms->size() - 1)))
			throw std::runtime_error("Unexpected tokens");
		if (i == tokens.size() - 1 && !tokens[i].isTypeNum())
			throw std::runtime_error("Unexpected tokens");
	}
	if (!foundEqual)
		throw std::runtime_error("Missing '=' token");
	if (termsRight.size() == 0)
		throw std::runtime_error("Right side of equation is empty");
	return Polynomial(termsLeft, termsRight);
}

bool isInclude(const std::vector<Token::Type> &types, Token::Type type)
{
	for (auto &&it : types)
		if (it == type)
			return true;
	return false;
}

#define BEFORE_SIGN {Token::Type::NUM, Token::Type::EQUAL}
#define BEFORE_NUM {Token::Type::PLUS, Token::Type::MINUS, Token::Type::CARET, Token::Type::EQUAL}
#define BEFORE_STAR {Token::Type::NUM}
#define BEFORE_X {Token::Type::STAR}
#define BEFORE_CARET {Token::Type::X}
#define BEFORE_EQUAL {Token::Type::NUM}

bool Parser::parseTerm(const std::vector<Token> &tokens, size_t cursor, Term &term)
{
	if (cursor >= tokens.size())
		throw std::runtime_error("Syntax error: Unexpected end of input while parsing term.");
	Token::Type tokenType = tokens[cursor].getType();
	if (tokenType == Token::Type::NUM)
	{
		if (cursor == 0)
		{
			term.setCoef(std::stod(tokens[cursor].getValue()));
			return true;
		}
		Token::Type prevTokenType = tokens[cursor - 1].getType();
		if (prevTokenType == Token::Type::CARET)
		{
			for (auto &&c : tokens[cursor].getValue())
				if (!std::isdigit(c))
					return false;
			if (tokens.size() > cursor + 1)
				if (!isInclude({Token::Type::PLUS, Token::Type::MINUS, Token::Type::EQUAL}, tokens[cursor + 1].getType()))
					return false;

			term.setExp(std::stoul(tokens[cursor].getValue()));
			return true;
		}
		if (prevTokenType == Token::Type::MINUS)
		{
			term.setCoef(-std::stod(tokens[cursor].getValue()));
			return true;
		}
		if (isInclude(BEFORE_NUM, prevTokenType))
		{
			term.setCoef(std::stod(tokens[cursor].getValue()));
			return true;
		}
	}
	if (tokenType == Token::Type::PLUS)
	{
		if (cursor == 0)
			return true;
		if (isInclude(BEFORE_SIGN, tokens[cursor - 1].getType()))
			return true;
	}
	if (tokenType == Token::Type::MINUS)
	{
		if (cursor == 0)
			return true;
		if (isInclude(BEFORE_SIGN, tokens[cursor - 1].getType()))
			return true;
	}
	if (tokenType == Token::Type::STAR)
	{
		if (cursor == 0 || tokens.size() == cursor + 1)
			return false;
		if (isInclude(BEFORE_STAR, tokens[cursor - 1].getType()))
			return true;
	}
	if (tokenType == Token::Type::X)
	{
		if (cursor == 0)
			return false;
		if (isInclude(BEFORE_X, tokens[cursor - 1].getType()))
			return true;
	}
	if (tokenType == Token::Type::CARET)
	{
		if (cursor == 0)
			return false;
		if (isInclude(BEFORE_CARET, tokens[cursor - 1].getType()))
			return true;
	}
	if (tokenType == Token::Type::EQUAL)
	{
		if (cursor == 0)
			return false;
		if (isInclude(BEFORE_EQUAL, tokens[cursor - 1].getType()))
			return true;
	}
	return false;
}
