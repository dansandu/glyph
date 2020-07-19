#include "dansandu/glyph/implementation/grammar.hpp"
#include "dansandu/ballotin/container.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/ballotin/string.hpp"
#include "dansandu/glyph/error.hpp"

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
using dansandu::glyph::error::GrammarError;

namespace dansandu::glyph::implementation::grammar
{

static const auto productionRulePattern = std::regex{R"( *[a-zA-Z]+ *-> *(?:(?:[a-zA-Z]+ +)*[a-zA-Z]+ *)?)"};

static std::pair<std::vector<std::string>, std::vector<Rule>> getIdentifiersAndRules(std::string_view grammar)
{
    auto identifiers = std::vector<std::string>{"Start", "$"};
    auto getSymbol   = [&identifiers](const std::string& identifier)
    {
        if (auto position = std::find(identifiers.cbegin(), identifiers.cend(), identifier); position == identifiers.cend())
        {
            identifiers.push_back(identifier);
            return Symbol{identifiers.size() - 1};
        }
        else
        {
            return Symbol{position - identifiers.cbegin()};
        }
    };

    auto rules = std::vector<Rule>{};
    for (const auto& line : split(grammar, "\n"))
    {
        if (!std::regex_match(line.cbegin(), line.cend(), productionRulePattern))
        {
            THROW(GrammarError, "invalid production rule: ", line);
        }
        auto rule = split(line, "->");
        if (rule.size() == 2)
        {
            rightSide = std::vector<Symbol>{};
            for (const auto& identifier : split(rule[1], " "))
            {
                rightSide.push_back(getSymbol(trim(identifier)));
            }
            rules.push_back({getSymbol(trim(rule[0])), std::move(rightSide)});
        }
        else if (rule.size() == 1)
        {
            rules.push_back({getSymbol(trim(rule[0])), {}});
        }
        else
            THROW(GrammarError, "ill-formed right side of production rule ", line);
    }
    return rules;
}

static std::pair<std::vector<std::string>, std::vector<std::string>> getNonTerminalsAndTerminals(const std::vector<Rule>& rules)
{
    auto nonterminals = std::vector<std::string>{};
    for (const auto& rule : rules)
        setInsert(nonterminals, rule.leftSide);
    auto terminals = std::vector<std::string>{};
    for (const auto& rule : rules)
    {
        for (const auto& symbol : rule.rightSide)
            if (std::find(nonterminals.cbegin(), nonterminals.cend(), symbol) == nonterminals.cend())
                setInsert(terminals, symbol);
        if (rule.rightSide.empty())
            setInsert(terminals, "");
    }
    return {std::move(nonterminals), std::move(terminals)};
}

Grammar::Grammar(std::string grammar)
{
    auto identifiersAndRules = getIdentifiersAndRules(grammar);
    identifiers_ = std::move(identifiersAndRules.first);
    rules_ = std::move(identifiersAndRules.second);
    auto symbols = getSymbols(rules_);
    nonterminals_ = std::move(symbols.first);
    terminals_ = std::move(symbols.second);
    asString_ = grammar;
}

static void populateFirstOf(const std::string& symbol, const std::vector<Rule>& rules, SymbolTable& firstTable)
{
    if (firstTable.find(symbol) != firstTable.end())
        return;

    auto& firstSet = firstTable[symbol];
    for (const auto& rule : rules)
        if (symbol == rule.leftSide)
        {
            if (rule.rightSide.empty())
                setInsert(firstSet, "");
            auto allOfRightHasEmpty = true;
            for (const auto& rightSymbol : rule.rightSide)
            {
                populateFirstOf(rightSymbol, rules, firstTable);
                const auto& firstOfRight = firstTable[rightSymbol];
                auto emptyPosition = std::find(firstOfRight.cbegin(), firstOfRight.cend(), "");
                if (emptyPosition != firstOfRight.cend())
                {
                    auto copy = std::vector<std::string>{firstOfRight.cbegin(), emptyPosition};
                    copy.insert(copy.end(), emptyPosition + 1, firstOfRight.cend());
                    firstSet = setUnion(firstSet, copy);
                }
                else
                {
                    firstSet = setUnion(firstSet, firstOfRight);
                    allOfRightHasEmpty = false;
                    break;
                }
            }
            if (allOfRightHasEmpty)
                setInsert(firstSet, "");
        }
}

SymbolTable getFirstTable(const Grammar& grammar)
{
    auto firstTable = SymbolTable{};
    for (const auto& terminal : grammar.getTerminals())
        firstTable[terminal] = {terminal};
    for (const auto& nonterminal : grammar.getNonterminals())
        populateFirstOf(nonterminal, grammar.getRules(), firstTable);
    return firstTable;
}

bool operator==(const Rule& left, const Rule& right)
{
    return left.leftSide == right.leftSide && left.rightSide == right.rightSide;
}

bool operator!=(const Rule& left, const Rule& right)
{
    return !(left == right);
}

std::ostream& operator<<(std::ostream& stream, const Rule& rule)
{
    return stream << "'" << rule.leftSide << "'"
                  << " -> "
                  << "'" << join(rule.rightSide, "' '") << "'";
}

}
