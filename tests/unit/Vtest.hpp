// Vtest.hpp : Vtest en C++11, header seul
#ifndef MINIVITEST_HPP
#define MINIVITEST_HPP

#include <algorithm>
#include <chrono>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace Vtest {

struct AssertionError : std::runtime_error {
    explicit AssertionError(const std::string &m) : std::runtime_error(m) {}
};

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

    // équivalent de expect.objectContaining : un prédicat (lambda) sur la valeur
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
struct TestCase { std::string name; std::function<void()> fn; };
inline std::vector<TestCase> &tests() { static std::vector<TestCase> t; return t; }
inline std::vector<std::string> &suiteStack() { static std::vector<std::string> s; return s; }

inline void describe(const std::string &name, const std::function<void()> &body) {
    suiteStack().push_back(name);
    body();
    suiteStack().pop_back();
}
inline void it(const std::string &name, const std::function<void()> &fn) {
    std::string full;
    for (std::size_t i = 0; i < suiteStack().size(); ++i) full += suiteStack()[i] + " > ";
    TestCase t = { full + name, fn };
    tests().push_back(t);
}
inline void collect(const std::function<void()> &f) { f(); }

// ---------- exécution ----------
inline int run() {
    typedef std::chrono::steady_clock clock;
    clock::time_point t0 = clock::now();
    int passed = 0, failed = 0;
    for (std::size_t i = 0; i < tests().size(); ++i) {
        const TestCase &t = tests()[i];
        try { t.fn(); ++passed; std::cout << "  \033[32m✓\033[0m " << t.name << "\n"; }
        catch (const std::exception &e) { ++failed; std::cout << "  \033[31m✗\033[0m " << t.name << "\n      " << e.what() << "\n"; }
        catch (...) { ++failed; std::cout << "  \033[31m✗\033[0m " << t.name << "\n      exception inconnue\n"; }
    }
    long ms = (long)std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - t0).count();
    std::cout << "\nTests: " << passed << " passed, " << failed << " failed (" << tests().size() << ") in " << ms << "ms\n";
    return failed ? 1 : 0;
}

} // namespace Vtest

// Enregistre une suite depuis n'importe quel fichier .cpp, avant main()
#define VTEST_CAT2(a, b) a##b
#define VTEST_CAT(a, b) VTEST_CAT2(a, b)
#define VTEST_SUITE(body) static const int VTEST_CAT(mv_suite_, __LINE__) = (Vtest::collect(body), 0)
#define VTEST_MAIN() int main() { return Vtest::run(); }

#endif