#include "test.hpp"

namespace
{

const char *typeName(Token::Type type)
{
	switch (type)
	{
	case Token::Type::NUM:
		return "NUM";
	case Token::Type::X:
		return "X";
	case Token::Type::CARET:
		return "CARET";
	case Token::Type::PLUS:
		return "PLUS";
	case Token::Type::MINUS:
		return "MINUS";
	case Token::Type::STAR:
		return "STAR";
	case Token::Type::EQUAL:
		return "EQUAL";
	}
	return "UNKNOWN";
}

std::vector<std::string> typesOf(const std::vector<Token> &tokens)
{
	std::vector<std::string> types;
	types.reserve(tokens.size());
	for (std::size_t i = 0; i < tokens.size(); ++i)
		types.push_back(typeName(tokens[i].getType()));
	return types;
}

std::vector<std::size_t> positionsOf(const std::vector<Token> &tokens)
{
	std::vector<std::size_t> positions;
	positions.reserve(tokens.size());
	for (std::size_t i = 0; i < tokens.size(); ++i)
		positions.push_back(tokens[i].getIndex());
	return positions;
}

} // namespace

VTEST_SUITE([]{
	Vtest::describe("tokenize",[]{
		Vtest::it("découpe un terme simple",[]{
			Vtest::expect(typesOf(Lexer::tokenize("5 * X^0")))
				.toEqual({"NUM", "STAR", "X", "CARET", "NUM"});
		});

		Vtest::it("garde les décimales dans un seul NUM",[]{
			const Token token = Lexer::tokenize("9.3 * X^2")[0];
			Vtest::expect(token.getType()).toBe(Token::Type::NUM);
			Vtest::expect(token.getValue()).toBe(std::string("9.3"));
			Vtest::expect(token.getIndex()).toBe(std::size_t(0));
		});

		Vtest::it("tokenise l'exemple du sujet",[]{
			Vtest::expect(typesOf(Lexer::tokenize("4 * X^0 + 4 * X^1 = 1 * X^0")))
				.toEqual({
					"NUM", "STAR", "X", "CARET", "NUM",
					"PLUS",
					"NUM", "STAR", "X", "CARET", "NUM",
					"EQUAL",
					"NUM", "STAR", "X", "CARET", "NUM"
				});
		});

		Vtest::it("tokenise l'exemple du sujet avec des décimales",[]{
			Vtest::expect(typesOf(Lexer::tokenize("5 * X^0 + 9.3 * X^2 = 1 * X^0")))
				.toEqual({
					"NUM", "STAR", "X", "CARET", "NUM",
					"PLUS",
					"NUM", "STAR", "X", "CARET", "NUM",
					"EQUAL",
					"NUM", "STAR", "X", "CARET", "NUM"
				});
		});

		Vtest::it("signale un caractère inconnu avec sa position",[]{
			// "?" est à l'index 2 (0-based, la même convention que Token.getIndex())
			Vtest::expect([]{ Lexer::tokenize("5 ? X^2"); }).toThrow(std::regex("\\?"));
			Vtest::expect([]{ Lexer::tokenize("5 ? X^2"); }).toThrow(std::regex("2"));
		});

		Vtest::it("renseigne la position réelle de chaque jeton",[]{
			Vtest::expect(positionsOf(Lexer::tokenize("5 * X^0"))).toEqual({0, 2, 4, 5, 6});
		});

		Vtest::it("positionne l'erreur sur le vrai caractère, espaces compris",[]{
			Vtest::expect([]{ Lexer::tokenize("  ?"); }).toThrow(std::regex("2"));
		});

		Vtest::it("conserve la casse d'origine dans value de X",[]{
			const Token token = Lexer::tokenize("X^2")[0];
			Vtest::expect(token.getType()).toBe(Token::Type::X);
			Vtest::expect(token.getValue()).toBe(std::string("X"));
		});

		Vtest::it("renseigne une position réelle après un nombre multi-caractère",[]{
			// "9.3 * X^2" : 0:'9' 1:'.' 2:'3' 3:' ' 4:'*' 5:' ' 6:'X' 7:'^' 8:'2'
			Vtest::expect(positionsOf(Lexer::tokenize("9.3 * X^2"))).toEqual({0, 4, 6, 7, 8});
		});

		Vtest::it("positionne l'erreur après un nombre multi-caractère",[]{
			Vtest::expect([]{ Lexer::tokenize("9.3 ?"); }).toThrow(std::regex("4"));
		});

		Vtest::it("ignore toute la whitespace, pas seulement l'espace",[]{
			Vtest::expect(typesOf(Lexer::tokenize("5\t*\tX^0")))
				.toEqual({"NUM", "STAR", "X", "CARET", "NUM"});
		});

		Vtest::it("gère tabulations et sauts de ligne",[]{
			Vtest::expect(typesOf(Lexer::tokenize("5\t*\nX^0")))
				.toEqual({"NUM", "STAR", "X", "CARET", "NUM"});
		});

		Vtest::it("tokenise sans aucun espace",[]{
			Vtest::expect(typesOf(Lexer::tokenize("5*X^0=4*X^1")))
				.toEqual({
					"NUM", "STAR", "X", "CARET", "NUM",
					"EQUAL",
					"NUM", "STAR", "X", "CARET", "NUM"
				});
		});

		Vtest::it("produit le flux complet avec signes",[]{
			Vtest::expect(typesOf(Lexer::tokenize("5 * X^0 = -4 * X^1")))
				.toEqual({
					"NUM", "STAR", "X", "CARET", "NUM",
					"EQUAL",
					"MINUS", "NUM", "STAR", "X", "CARET", "NUM"
				});
		});

		Vtest::it("renvoie un flux vide pour une entrée vide",[]{
			Vtest::expect(Lexer::tokenize("").size()).toBe(std::size_t(0));
		});

		Vtest::it("rejette un nombre avec point initial",[]{
			Vtest::expect([]{ Lexer::tokenize(".5 * X^0"); }).toThrow(std::regex("0"));
		});

		Vtest::it("rejette un nombre avec point final",[]{
			Vtest::expect([]{ Lexer::tokenize("5. * X^0"); }).toThrow(std::regex("1"));
		});

		Vtest::it("rejette deux points dans un nombre",[]{
			Vtest::expect([]{ Lexer::tokenize("1.2.3"); }).toThrow(std::regex("3"));
		});

		Vtest::it("rejette le x minuscule (le sujet impose X majuscule)",[]{
			Vtest::expect([]{ Lexer::tokenize("5 * x^0"); }).toThrow(std::regex("4"));
		});

		Vtest::it("ne lève pas d'exception sur une entrée valide",[]{
			Vtest::expect([]{ Lexer::tokenize("5"); }).Not().toThrow();
		});
	});
});
