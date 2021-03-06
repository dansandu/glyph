#include "dansandu/glyph/internal/grammar.hpp"
#include "dansandu/ballotin/container.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/ballotin/relation.hpp"
#include "dansandu/ballotin/string.hpp"
#include "dansandu/glyph/error.hpp"
#include "dansandu/glyph/internal/multimap.hpp"

#include <algorithm>
#include <regex>
#include <string>
#include <tuple>
#include <vector>

using dansandu::ballotin::container::contains;
using dansandu::ballotin::container::uniquePushBack;
using dansandu::ballotin::relation::TotalOrder;
using dansandu::ballotin::string::format;
using dansandu::ballotin::string::join;
using dansandu::ballotin::string::split;
using dansandu::ballotin::string::trim;
using dansandu::glyph::error::GrammarError;
using dansandu::glyph::internal::multimap::Multimap;
using dansandu::glyph::internal::rule::Rule;
using dansandu::glyph::symbol::Symbol;

namespace dansandu::glyph::internal::grammar
{

template<typename T, typename U>
auto find(const std::vector<T>& container, const U& value)
{
    return std::find(container.cbegin(), container.cend(), value);
}

Grammar::Grammar(const std::string_view grammar) : grammar_{grammar}
{
    static const auto productionRulePattern =
        std::regex{R"( *[a-zA-Z0-9]+ *-> *(?:(?:[a-zA-Z0-9]+ +)*[a-zA-Z0-9]+ *)?)"};

    auto leftSideColumn = std::vector<std::string>{};
    auto rightSideColumn = std::vector<std::vector<std::string>>{};

    for (const auto& line : split(grammar_, "\n"))
    {
        const auto trimmedLine = trim(line);
        if (trimmedLine.empty())
        {
            continue;
        }
        if (!std::regex_match(trimmedLine.cbegin(), trimmedLine.cend(), productionRulePattern))
        {
            THROW(GrammarError, "invalid production rule: ", trimmedLine);
        }
        const auto ruleTokens = split(trimmedLine, "->");
        if (ruleTokens.size() == 2)
        {
            leftSideColumn.push_back(trim(ruleTokens[0]));

            auto rightSide = std::vector<std::string>{};
            for (const auto& identifier : split(ruleTokens[1], " "))
            {
                auto trimmedIdentifier = trim(identifier);
                if (trimmedIdentifier != "Start")
                {
                    rightSide.push_back(std::move(trimmedIdentifier));
                }
                else
                {
                    THROW(GrammarError, "right side of production rule '", trimmedLine,
                          "' cannot contain Start non-terminal");
                }
            }
            rightSideColumn.push_back(std::move(rightSide));
        }
        else if (ruleTokens.size() == 1)
        {
            leftSideColumn.push_back(trim(ruleTokens[0]));
            rightSideColumn.push_back({});
        }
        else
        {
            THROW(GrammarError, "ill-formed right side of production rule ", trimmedLine);
        }
    }

    if (leftSideColumn.empty() || leftSideColumn.front() != "Start")
    {
        THROW(GrammarError, "the first production rule must be the start rule");
    }

    for (auto i = 1U; i < leftSideColumn.size(); ++i)
    {
        if (leftSideColumn[i] == "Start")
        {
            THROW(GrammarError, "there can only be one start production rule");
        }
    }

    // Insert nonterminals first.
    identifiers_ = std::vector<std::string>{{"Start"}};
    for (const auto& identifier : leftSideColumn)
    {
        uniquePushBack(identifiers_, identifier);
    }

    // Insert terminals next and mark the beginning of the terminals (end of string identifier).
    terminalBeginIndex_ = static_cast<int>(identifiers_.size());
    identifiers_.push_back("$");
    identifiers_.push_back("");
    for (const auto& rightSide : rightSideColumn)
    {
        for (const auto& identifier : rightSide)
        {
            // Ensures two things in one search:
            // 1. It's not a non-terminal (first part of the vector).
            // 2. The terminal isn't a duplicate (second part of the vector).
            uniquePushBack(identifiers_, identifier);
        }
    }

    for (auto ruleIndex = 0U; ruleIndex < leftSideColumn.size(); ++ruleIndex)
    {
        const auto identifierIndex =
            static_cast<int>(find(identifiers_, leftSideColumn[ruleIndex]) - identifiers_.cbegin());
        const auto leftSideSymbol = Symbol{identifierIndex};
        auto rightSideSymbols = std::vector<Symbol>{};
        for (const auto& identifier : rightSideColumn[ruleIndex])
        {
            const auto identifierIndex = static_cast<int>(find(identifiers_, identifier) - identifiers_.cbegin());
            rightSideSymbols.push_back(Symbol{identifierIndex});
        }
        rules_.push_back({leftSideSymbol, std::move(rightSideSymbols)});
    }

    generateFirstTable();
}

struct PartialItem : TotalOrder<PartialItem>
{
    PartialItem(const int ruleIndex, const int position) : ruleIndex{ruleIndex}, position{position}
    {
    }

    int ruleIndex;
    int position;
};

inline bool operator<(const PartialItem a, const PartialItem b)
{
    return std::tie(a.ruleIndex, a.position) < std::tie(b.ruleIndex, b.position);
}

void Grammar::generateFirstTable()
{
    auto partitions = Multimap{};

    for (auto terminalIndex = terminalBeginIndex_; terminalIndex < static_cast<int>(identifiers_.size());
         ++terminalIndex)
    {
        partitions[Symbol{terminalIndex}] = {Symbol{terminalIndex}};
    }

    auto blanks = std::vector<Symbol>{};
    auto visitedRulesIndices = std::vector<int>{};

    for (auto rootRuleIndex = 0; rootRuleIndex < static_cast<int>(rules_.size()); ++rootRuleIndex)
    {
        if (contains(visitedRulesIndices, rootRuleIndex))
        {
            continue;
        }

        visitedRulesIndices.push_back(rootRuleIndex);

        auto rulesIndicesPath = std::vector<int>{};
        auto stack = std::vector<std::pair<PartialItem, int>>{{PartialItem{rootRuleIndex, 0}, -1}};

        while (!stack.empty())
        {
            const auto currentItem = stack.back().first;
            if (currentItem.position == static_cast<int>(rules_[currentItem.ruleIndex].rightSide.size()))
            {
                uniquePushBack(blanks, rules_[currentItem.ruleIndex].leftSide);
                stack.pop_back();
                continue;
            }

            const auto currentSymbol = rules_[currentItem.ruleIndex].rightSide[currentItem.position];
            if (isTerminal(currentSymbol))
            {
                uniquePushBack(partitions[rules_[currentItem.ruleIndex].leftSide], currentSymbol);
                stack.pop_back();
                continue;
            }

            const auto parentRuleIndex = stack.back().second;
            while (!rulesIndicesPath.empty() && rulesIndicesPath.back() != parentRuleIndex)
            {
                rulesIndicesPath.pop_back();
            }
            rulesIndicesPath.push_back(currentItem.ruleIndex);
            for (auto i = 0U; i < rulesIndicesPath.size(); ++i)
            {
                if (rules_[rulesIndicesPath[i]].leftSide == currentSymbol)
                {
                    auto cycle = std::vector<Symbol>{};
                    for (auto j = i; j < rulesIndicesPath.size(); ++j)
                    {
                        uniquePushBack(cycle, rules_[rulesIndicesPath[j]].leftSide);
                    }
                    partitions.merge(cycle);
                }
            }

            auto hasUnvisitedChildren = false;
            for (auto ruleIndex = 0U; ruleIndex < rules_.size(); ++ruleIndex)
            {
                if (rules_[ruleIndex].leftSide == currentSymbol && !contains(visitedRulesIndices, ruleIndex))
                {
                    hasUnvisitedChildren = true;
                    stack.push_back({PartialItem{static_cast<int>(ruleIndex), 0}, currentItem.ruleIndex});
                    visitedRulesIndices.push_back(ruleIndex);
                }
            }
            if (!hasUnvisitedChildren)
            {
                partitions[currentSymbol];
                partitions[rules_[currentItem.ruleIndex].leftSide];
                const auto& firstSet = partitions[currentSymbol];
                auto& target = partitions[rules_[currentItem.ruleIndex].leftSide];
                for (const auto& symbol : firstSet)
                {
                    uniquePushBack(target, symbol);
                }
                if (contains(blanks, currentSymbol))
                {
                    ++stack.back().first.position;
                }
                else
                {
                    stack.pop_back();
                }
            }
        }
    }

    firstTable_ = std::vector<std::vector<Symbol>>{identifiers_.size()};
    partitions.forEach([this](const auto& partition, const auto& firstSet) {
        for (const auto& symbol : partition)
        {
            firstTable_[symbol.getIdentifierIndex()] = std::vector<Symbol>{firstSet.cbegin(), firstSet.cend()};
        }
    });
    for (const auto& symbol : blanks)
    {
        firstTable_[symbol.getIdentifierIndex()].push_back(getEmptySymbol());
    }
}

Symbol Grammar::getSymbol(const std::string_view identifier) const
{
    if (const auto position = find(identifiers_, identifier); position != identifiers_.cend())
    {
        return Symbol{static_cast<int>(position - identifiers_.cbegin())};
    }
    else
    {
        THROW(GrammarError, "identifier '", identifier,
              "' does not match any identifiers in grammar: ", join(identifiers_, ", "));
    }
}

Symbol Grammar::getTerminalSymbol(const std::string_view identifier) const
{
    if (const auto position =
            std::find(identifiers_.cbegin() + terminalBeginIndex_ + 2, identifiers_.cend(), identifier);
        position != identifiers_.cend())
    {
        return Symbol{static_cast<int>(position - identifiers_.cbegin())};
    }
    else
    {
        THROW(GrammarError, "identifier '", identifier, "' does not match any terminal identifiers in grammar");
    }
}

}
