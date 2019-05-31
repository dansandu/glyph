#include "dansandu/glyph/implementation/grammar.hpp"
#include "dansandu/ballotin/container.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/ballotin/string.hpp"

#include <algorithm>
#include <map>
#include <regex>
#include <string>
#include <vector>

using dansandu::ballotin::container::setInsert;
using dansandu::ballotin::container::setUnion;
using dansandu::ballotin::string::format;
using dansandu::ballotin::string::join;
using dansandu::ballotin::string::split;
using dansandu::ballotin::string::trim;

namespace dansandu::glyph::implementation::grammar {

static const auto grammarValidator = std::regex{
    "(\\s*[a-zA-Z]+ *-> *(([a-zA-Z]+ +)*[a-zA-Z]+)?\\s*\\n)*\\s*[a-zA-Z]+ *-> *(([a-zA-Z]+ +)*[a-zA-Z]+)?\\s*"};

bool operator==(const Rule& left, const Rule& right) {
    return left.leftSide == right.leftSide && left.rightSide == right.rightSide;
}

bool operator!=(const Rule& left, const Rule& right) { return !(left == right); }

std::ostream& operator<<(std::ostream& stream, const Rule& rule) {
    return stream << "'" << rule.leftSide << "'"
                  << " -> "
                  << "'" << join(rule.rightSide, "' '") << "'";
}

static std::vector<Rule> getRulesWork(std::string_view grammar) {
    if (!std::regex_match(grammar.cbegin(), grammar.cend(), grammarValidator))
        THROW(GrammarError, "invalid grammar ", grammar);
    auto rules = std::vector<Rule>{};
    for (const auto& line : split(grammar, "\n")) {
        auto rule = split(line, "->");
        if (rule.size() == 2)
            rules.push_back({trim(rule[0]), split(rule[1], " ")});
        else if (rule.size() == 1)
            rules.push_back({trim(rule[0]), {}});
        else
            THROW(GrammarError, "ill-formed right side of production rule ", line);
    }
    return rules;
}

static std::pair<std::vector<std::string>, std::vector<std::string>> getSymbols(const std::vector<Rule>& rules) {
    auto nonterminals = std::vector<std::string>{};
    for (const auto& rule : rules)
        setInsert(nonterminals, rule.leftSide);
    auto terminals = std::vector<std::string>{};
    for (const auto& rule : rules) {
        for (const auto& symbol : rule.rightSide)
            if (std::find(nonterminals.cbegin(), nonterminals.cend(), symbol) == nonterminals.cend())
                setInsert(terminals, symbol);
        if (rule.rightSide.empty())
            setInsert(terminals, "");
    }
    return {std::move(nonterminals), std::move(terminals)};
}

Grammar::Grammar(std::string grammar) : asString_{std::move(grammar)}, rules_{getRulesWork(asString_)} {
    auto symbols = getSymbols(rules_);
    nonterminals_ = std::move(symbols.first);
    terminals_ = std::move(symbols.second);
}

static void populateFirstOf(const std::string& symbol, const std::vector<Rule>& rules, SymbolTable& firstTable) {
    if (firstTable.find(symbol) != firstTable.end())
        return;

    auto& firstSet = firstTable[symbol];
    for (const auto& rule : rules)
        if (symbol == rule.leftSide) {
            if (rule.rightSide.empty())
                setInsert(firstSet, "");
            auto allOfRightHasEmpty = true;
            for (const auto& rightSymbol : rule.rightSide) {
                populateFirstOf(rightSymbol, rules, firstTable);
                const auto& firstOfRight = firstTable[rightSymbol];
                auto emptyPosition = std::find(firstOfRight.cbegin(), firstOfRight.cend(), "");
                if (emptyPosition != firstOfRight.cend()) {
                    auto copy = std::vector<std::string>{firstOfRight.cbegin(), emptyPosition};
                    copy.insert(copy.end(), emptyPosition + 1, firstOfRight.cend());
                    firstSet = setUnion(firstSet, copy);
                } else {
                    firstSet = setUnion(firstSet, firstOfRight);
                    allOfRightHasEmpty = false;
                    break;
                }
            }
            if (allOfRightHasEmpty)
                setInsert(firstSet, "");
        }
}

SymbolTable getFirstTable(const Grammar& grammar) {
    auto firstTable = SymbolTable{};
    for (const auto& terminal : grammar.getTerminals())
        firstTable[terminal] = {terminal};
    for (const auto& nonterminal : grammar.getNonterminals())
        populateFirstOf(nonterminal, grammar.getRules(), firstTable);
    return firstTable;
}

}
