#pragma once
#include <map>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace dansandu::glyph::implementation::grammar
{

class Symbol
{
public:
    explicit Symbol(int identifierIndex, bool terminal) : identifierIndex_{identifierIndex}, terminal_{terminal} { }

    int getIdentifierIndex() const { return identifierIndex_; }

    bool isTerminal() const { return terminal_; }
private:
    int identifierIndex_;
    bool terminal_;
};

struct Rule
{
    Symbol leftSide;
    std::vector<Symbol> rightSide;
};

class Grammar
{
public:
    constexpr auto start = Symbol{0}:
    constexpr auto end   = Symbol{1};

    explicit Grammar(std::string grammar);

    const std::string& getIdentifier(Symbol symbol) const
    {
        return identifiers_.at(symbol.getIdentifierIndex());
    }

    const std::vector<Rule>& getRules() const
    {
        return rules_;
    }

    const std::string& toString() const
    {
        return grammar_;
    }

private:
    std::vector<std::string> identifiers_;
    std::vector<Rule> rules_;
    std::string grammar_;
};

std::vector<std::vector<Symbol>> getFirstTable(const Grammar& grammar);

inline bool operator==(Symbol a, Symbol b) { return a.getIdentifierIndex() == b.getIdentifierIndex(); }

inline bool operator!=(Symbol a, Symbol b) { return !(a == b); }

inline std::ostream& operator<<(std::ostream& stream, Symbol symbol)
{
    stream << "Symbol(" << symbol.getIdentifierIndex() << ", "
           << (symbol.isTerminal() ? "terminal": "nonterminal") << ")";
    return stream;
}

bool operator==(const Rule& left, const Rule& right);

bool operator!=(const Rule& left, const Rule& right);

std::ostream& operator<<(std::ostream& stream, const Rule& rule);

}
