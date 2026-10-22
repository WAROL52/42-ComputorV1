/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rrabetsy <rrabetsy@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 15:33:32 by rrabetsy          #+#    #+#             */
/*   Updated: 2026/10/22 15:39:38 by rrabetsy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

class Token
{
public:
	enum class Type
	{
		NUM,
		X,
		CARET,
		PLUS,
		MINUS,
		STAR,
		EQUAL
	};

private:
	const Type type;
	const size_t index;
	const std::string value;

public:
	Token(Type type, size_t index, std::string value);
	~Token();

	inline Type getType() const
	{
		return this->type;
	}
	inline size_t getIndex() const
	{
		return this->index;
	}
	inline const std::string &getValue() const
	{
		return this->value;
	}
	inline size_t getCursor() const
	{
		return this->value.length() + this->index;
	}

	inline bool isTypeNum() const
	{
		return this->type == Type::NUM;
	}
	inline bool isTypeX() const
	{
		return this->type == Type::X;
	}
	inline bool isTypeCaret() const
	{
		return this->type == Type::CARET;
	}
	inline bool isTypePlus() const
	{
		return this->type == Type::PLUS;
	}
	inline bool isTypeMinus() const
	{
		return this->type == Type::MINUS;
	}
	inline bool isTypeStart() const
	{
		return this->type == Type::STAR;
	}
	inline bool isTypeEqual() const
	{
		return this->type == Type::EQUAL;
	}

	static Token from(const std::string &input, size_t index = 0);
};

#endif
