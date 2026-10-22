#ifndef PARSER_HPP
#define PARSER_HPP
#include "Polynomial.hpp"
#include "Token.hpp"
class Parser
{
private:
	Parser();

public:
	~Parser();
	static Polynomial parse(const std::vector<Token> &tokens);
	static Term parseTerm(const std::vector<Token> &tokens, size_t &cursor);
};

#endif
