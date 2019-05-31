#pragma once
#include <map>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace dansandu::glyph::implementation::grammar {

constexpr auto startSymbol = "Start";

constexpr auto endOfString = "$";

using SymbolTable = std::map<std::string, std::vector<std::string>>;

class GrammarError : std::runtime_error {
    using runtime_error::runtime_error;
};

struct Rule {
    std::string leftSide;
    std::vector<std::string> rightSide;
};

bool operator==(const Rule& left, const Rule& right);

bool operator!=(const Rule& left, const Rule& right);

std::ostream& operator<<(std::ostream& stream, const Rule& rule);

class Grammar {
public:
    explicit Grammar(std::string grammar);

    const std::vector<Rule>& getRules() const { return rules_; }

    const std::vector<std::string>& getNonterminals() const { return nonterminals_; }

    const std::vector<std::string>& getTerminals() const { return terminals_; }

    const std::string& asString() const { return asString_; }

private:
    std::string asString_;
    std::vector<Rule> rules_;
    std::vector<std::string> nonterminals_;
    std::vector<std::string> terminals_;
};

SymbolTable getFirstTable(const Grammar& grammar);

}
