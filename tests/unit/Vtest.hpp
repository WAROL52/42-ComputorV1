// Vtest.hpp : Vtest en C++11, header seul
#ifndef MINIVITEST_HPP
#define MINIVITEST_HPP

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace Vtest {

struct AssertionError : std::runtime_error {
    explicit AssertionError(const std::string &m) : std::runtime_error(m) {}
};

// ---------- emplacement d'un test dans la source ----------

// Note : GCC ne fournit pas __COLUMN__ (extension Clang), et un argument par
// defaut est evalue sur la DECLARATION de la fonction, pas sur le site d'appel.
// La colonne est donc deduite en relisant le fichier source et en y cherchant
// le marqueur du test (son nom entre guillemets, sinon "it(").
struct SourceLocation
{
	std::string file;
	int line;
	int column;

	SourceLocation() : file(), line(0), column(0) {}
	SourceLocation(const char *f, int l) : file(f ? f : ""), line(l), column(0) {}

	bool known() const { return !this->file.empty(); }

	std::string str() const
	{
		std::ostringstream o;
		o << this->file << ":";
		if (this->line > 0)
			o << this->line;
		else
			o << "?";
		o << ":";
		if (this->column > 0)
			o << this->column;
		else
			o << "?";
		return o.str();
	}
};

// Les lignes d'un fichier source sont lues une seule fois puis memorisees.
inline std::map<std::string, std::vector<std::string> > &sourceLines()
{
	static std::map<std::string, std::vector<std::string> > cache;
	return cache;
}

inline const std::vector<std::string> *linesOf(const std::string &file)
{
	std::map<std::string, std::vector<std::string> >::iterator it = sourceLines().find(file);
	if (it != sourceLines().end())
		return &(it->second);
	std::ifstream stream(file.c_str());
	if (!stream.is_open())
		return NULL;
	std::vector<std::string> lines;
	std::string line;
	while (std::getline(stream, line))
		lines.push_back(line);
	it = sourceLines().insert(std::make_pair(file, lines)).first;
	return &(it->second);
}

// Colonne (1-based) du marqueur dans la ligne, 0 si le marqueur est introuvable.
inline int columnOf(const std::string &file, int line, const std::string &marker)
{
	if (file.empty() || line <= 0 || marker.empty())
		return 0;
	const std::vector<std::string> *lines = linesOf(file);
	if (!lines || (std::size_t)line > lines->size())
		return 0;
	const std::string &text = (*lines)[line - 1];
	std::size_t pos = text.find(marker);
	if (pos == std::string::npos)
		return 0;
	return (int)pos + 1;
}

// Ligne (1-based) de la premiere occurrence du marqueur dans le fichier, 0 si absent.
// C'est ce qui permet de retrouver l'emplacement d'un test enregistre sans macro :
// le nom du test est un litteral dans le fichier source.
inline int lineOfMarker(const std::string &file, const std::string &marker)
{
	if (file.empty() || marker.empty())
		return 0;
	const std::vector<std::string> *lines = linesOf(file);
	if (!lines)
		return 0;
	for (std::size_t i = 0; i < lines->size(); ++i)
		if (lines->at(i).find(marker) != std::string::npos)
			return (int)i + 1;
	return 0;
}

// ---------- contexte du fichier de test courant ----------

// VTEST_SUITE est une macro : elle capture le __FILE__ du fichier de test. C'est
// ce contexte qui permet d'afficher le nom de fichier meme pour un test enregistre
// avec Vtest::it() (donc sans macro, donc sans __LINE__ du site d'appel).
inline std::vector<std::string> &fileStack() { static std::vector<std::string> s; return s; }

inline std::string currentFile()
{
	return fileStack().empty() ? std::string() : fileStack().back();
}

// ---------- affichage des valeurs dans les messages d'échec ----------
namespace detail {
template <class T>
auto repr(const T &v, int) -> decltype(std::declval<std::ostream &>() << v, std::string()) {
    std::ostringstream o; o << v; return o.str();
}
template <class T> std::string repr(const T &, long) { return "<non affichable>"; }
}
template <class T> std::string show(const T &v) { return detail::repr(v, 0); }
inline std::string show(const std::string &s) { return "\"" + s + "\""; }
inline std::string show(const char *s) { return std::string("\"") + s + "\""; }
template <class T> std::string show(const std::vector<T> &v) {
    std::string r = "[";
    for (std::size_t i = 0; i < v.size(); ++i) { if (i) r += ", "; r += show(v[i]); }
    return r + "]";
}

// ---------- expect ----------
template <class T>
class Expectation {
public:
    Expectation(const T &actual, bool negate) : actual_(actual), neg_(negate) {}

    Expectation Not() const { return Expectation(actual_, !neg_); }   // `not` est un mot réservé en C++

    template <class U> void toBe(const U &e) const    { check(actual_ == e, show(actual_), "to be " + show(e)); }
    template <class U> void toEqual(const U &e) const { check(actual_ == e, show(actual_), "to equal " + show(e)); }

    // expect(vec).toEqual({"NUM", "STAR"}) : liste d'initialisation pour les conteneurs
    template <class V = T>
    void toEqual(std::initializer_list<typename V::value_type> il) const {
        bool same = actual_.size() == il.size() && std::equal(actual_.begin(), actual_.end(), il.begin());
        check(same, show(actual_), "to equal " + show(std::vector<typename V::value_type>(il)));
    }

    // équivalent à expect.objectContaining : un prédicat (lambda) sur la valeur
    template <class P> void toSatisfy(P pred) const { check(pred(actual_), show(actual_), "to satisfy the predicate"); }

    void toBeTruthy() const { check(!!actual_, show(actual_), "to be truthy"); }

    void toThrow() const                    { throwing("", nullptr); }
    void toThrow(const std::string &s) const { throwing("containing " + show(s), &s); }
    void toThrow(const std::regex &re) const { bool did; std::string m; run(did, m);
        check(did && std::regex_search(m, re), "function", "to throw a message matching the regex (got " + got(did, m) + ")"); }

private:
    T actual_;
    bool neg_;

    void check(bool pass, const std::string &subject, const std::string &what) const {
        if (pass == neg_) throw AssertionError("expected " + subject + (neg_ ? " not " : " ") + what);
    }
    void run(bool &did, std::string &msg) const {
        did = false;
        try { actual_(); } catch (const std::exception &e) { did = true; msg = e.what(); } catch (...) { did = true; msg = "?"; }
    }
    static std::string got(bool did, const std::string &m) { return did ? show(m) : std::string("no error"); }
    void throwing(const std::string &label, const std::string *needle) const {
        bool did; std::string m; run(did, m);
        bool ok = did && (!needle || m.find(*needle) != std::string::npos);
        check(ok, "function", "to throw" + (label.empty() ? "" : " " + label) + " (got " + got(did, m) + ")");
    }
};

template <class T>
Expectation<typename std::decay<T>::type> expect(const T &v) { return Expectation<typename std::decay<T>::type>(v, false); }

// ---------- describe / it ----------
struct TestCase { std::string name; std::function<void()> fn; SourceLocation loc; };
inline std::vector<TestCase> &tests() { static std::vector<TestCase> t; return t; }
inline std::vector<std::string> &suiteStack() { static std::vector<std::string> s; return s; }

inline void describe(const std::string &name, const std::function<void()> &body) {
    suiteStack().push_back(name);
    body();
    suiteStack().pop_back();
}

inline void it(const std::string &name, const std::function<void()> &fn,
               const char *file = "", int line = 0) {
    std::string full;
    for (std::size_t i = 0; i < suiteStack().size(); ++i) full += suiteStack()[i] + " > ";
    TestCase t;
    t.name = full + name;
    t.fn = fn;
    // fichier : celui de la suite (capture par VTEST_SUITE) si l'appel n'en donne pas
    t.loc.file = (file && *file) ? std::string(file) : currentFile();
    // marqueur : le nom du test entre guillemets, c'est un litteral du fichier source
    const std::string marker = "\"" + name + "\"";
    // ligne : connue si la macro VTEST_IT l'a fournie, sinon retrouvee dans la source
    t.loc.line = line > 0 ? line : lineOfMarker(t.loc.file, marker);
    t.loc.column = columnOf(t.loc.file, t.loc.line, marker);
    if (t.loc.column == 0)
        t.loc.column = columnOf(t.loc.file, t.loc.line, "it(");
    tests().push_back(t);
}
inline void collect(const std::function<void()> &f, const char *file = "") {
    fileStack().push_back(file ? file : "");
    f();
    fileStack().pop_back();
}

// ---------- exécution ----------
inline bool verbose() {
    const char *env = std::getenv("VTEST_VERBOSE");
    return env != NULL && env[0] != '\0' && std::string(env) != "0";
}

inline int run() {
    typedef std::chrono::steady_clock clock;
    clock::time_point t0 = clock::now();
    int passed = 0, failed = 0;
    const bool showLocation = verbose();
    for (std::size_t i = 0; i < tests().size(); ++i) {
        const TestCase &t = tests()[i];
        try {
            t.fn(); ++passed;
            std::cout << "  \033[32m✓ " << t.name << "\033[0m";
            if (showLocation && t.loc.known())
                std::cout << "  \033[90m" << t.loc.str() << "\033[0m";
            std::cout << "\n";
        }
        catch (const std::exception &e) {
            ++failed;
            std::cout << "  \033[31m✗ " << t.name << "\033[0m\n";
            if (t.loc.known())
                std::cout << "      \033[90m" << t.loc.str() << "\033[0m\n";
            std::cout << "      " << e.what() << "\n";
        }
        catch (...) {
            ++failed;
            std::cout << "  \033[31m✗ " << t.name << "\033[0m\n";
            if (t.loc.known())
                std::cout << "      \033[90m" << t.loc.str() << "\033[0m\n";
            std::cout << "      exception inconnue\n";
        }
    }
    long ms = (long)std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - t0).count();
    std::cout << "\nTests: \033[32m" << passed << " passed\033[0m, ";
    if (failed)
        std::cout << "\033[31m" << failed << " failed\033[0m";
    else
        std::cout << "0 failed";
    std::cout << " (" << tests().size() << ") in " << ms << "ms\n";
    return failed ? 1 : 0;
}

} // namespace Vtest

// Enregistre une suite depuis n'importe quel fichier .cpp, avant main().
// __FILE__ est capture ici : c'est le nom du fichier de test affiche dans la sortie.
#define VTEST_CAT2(a, b) a##b
#define VTEST_CAT(a, b) VTEST_CAT2(a, b)
#define VTEST_SUITE(body) static const int VTEST_CAT(mv_suite_, __LINE__) = (Vtest::collect(body, __FILE__), 0)
#define VTEST_MAIN() int main() { return Vtest::run(); }

// __FILE__ et __LINE__ sont expandus sur le SITE D'APPEL (contrairement a un
// argument par defaut, evalue sur la declaration de la fonction).
#define VTEST_IT(name, ...) Vtest::it(name, __VA_ARGS__, __FILE__, __LINE__)

#endif
