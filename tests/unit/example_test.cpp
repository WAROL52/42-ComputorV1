#include "test.hpp"
#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

using namespace Vtest;

struct Token { std::string type, value; std::size_t pos; };

static std::vector<Token> tokenize(const std::string &in) {
    std::vector<Token> out;
    std::size_t n = in.size();
    for (std::size_t i = 0; i < n;) {
        char c = in[i];
        if (std::isspace((unsigned char)c)) { ++i; continue; }
        if (std::isdigit((unsigned char)c)) {
            std::size_t j = i;
            while (j < n && std::isdigit((unsigned char)in[j])) ++j;
            if (j + 1 < n && in[j] == '.' && std::isdigit((unsigned char)in[j + 1])) { ++j; while (j < n && std::isdigit((unsigned char)in[j])) ++j; }
            out.push_back(Token{"NUM", in.substr(i, j - i), i}); i = j; continue;
        }
        const char *names[] = { "X", "^", "*", "+", "-", "=" };
        const char *types[] = { "X", "CARET", "STAR", "PLUS", "MINUS", "EQUAL" };
        bool found = false;
        for (int k = 0; k < 6; ++k)
            if (c == names[k][0]) { out.push_back(Token{types[k], std::string(1, c), i}); ++i; found = true; break; }
        if (!found) throw std::runtime_error("Unexpected character at position " + std::to_string(i) + ": " + std::string(1, c));
    }
    return out;
}

static std::vector<std::string> typesOf(const std::vector<Token> &ts) {
    std::vector<std::string> r(ts.size());
    std::transform(ts.begin(), ts.end(), r.begin(), [](const Token &t) { return t.type; });
    return r;
}

VTEST_SUITE([] {
    describe("tokenize", [] {
        it("découpe un terme simple", [] {
            expect(typesOf(tokenize("5 * X^0"))).toEqual({ "NUM", "STAR", "X", "CARET", "NUM" });
        });
        it("garde les décimales dans un seul NUM", [] {
            expect(tokenize("9.3 * X^2")[0]).toSatisfy([](const Token &t) { return t.type == "NUM" && t.value == "9.3" && t.pos == 0; });
        });
        it("renseigne la position réelle de chaque jeton", [] {
            std::vector<std::size_t> pos;
            std::vector<Token> ts = tokenize("5 * X^0");
            for (std::size_t i = 0; i < ts.size(); ++i) pos.push_back(ts[i].pos);
            expect(pos).toEqual({ 0, 2, 4, 5, 6 });
        });
        it("signale un caractère inconnu avec sa position", [] {
            expect([] { tokenize("5 ? X^2"); }).toThrow(std::regex("\\?"));
            expect([] { tokenize("5 ? X^2"); }).toThrow("position 2");
        });
        it("renvoie un flux vide pour une entrée vide", [] { expect(tokenize("").size()).toBe(0u); });
        it("Not fonctionne", [] { expect([] { tokenize("5"); }).Not().toThrow(); });
    });
});

