/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rrabetsy <rrabetsy@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 16:07:33 by rrabetsy          #+#    #+#             */
/*   Updated: 2026/10/22 16:07:35 by rrabetsy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_HPP
#define LEXER_HPP
#include "Token.hpp"
#include <vector>
class Lexer
{
private:
	Lexer(/* args */);

public:
	~Lexer();
	static std::vector<Token> tokenize(const std::string &input);
};

#endif
