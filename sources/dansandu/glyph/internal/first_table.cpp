#include "dansandu/glyph/internal/first_table.hpp"
#include "dansandu/ballotin/container.hpp"
#include "dansandu/ballotin/relation.hpp"
#include "dansandu/glyph/internal/grammar.hpp"
#include "dansandu/glyph/internal/multimap.hpp"
#include "dansandu/glyph/symbol.hpp"

using dansandu::ballotin::container::contains;
using dansandu::ballotin::container::uniquePushBack;
using dansandu::ballotin::relation::TotalOrder;
using dansandu::glyph::internal::grammar::Grammar;
using dansandu::glyph::internal::multimap::Multimap;
using dansandu::glyph::symbol::Symbol;

namespace dansandu::glyph::internal::first_table
{

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

std::vector<std::vector<Symbol>> getFirstTable(const Grammar& grammar)
{
    auto firstTable = std::vector<std::vector<Symbol>>{grammar.getIdentifiers().size()};
    auto partitions = Multimap{};

    for (const auto& identifier : grammar.getIdentifiers())
    {
        const auto symbol = grammar.getSymbol(identifier);
        if (grammar.isTerminal(symbol))
        {
            partitions[symbol] = {symbol};
        }
    }

    auto blanks = std::vector<Symbol>{};
    auto visitedRulesIndices = std::vector<int>{};

    const auto& rules = grammar.getRules();

    for (auto rootRuleIndex = 0; rootRuleIndex < static_cast<int>(rules.size()); ++rootRuleIndex)
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
            if (currentItem.position == static_cast<int>(rules[currentItem.ruleIndex].rightSide.size()))
            {
                uniquePushBack(blanks, rules[currentItem.ruleIndex].leftSide);
                stack.pop_back();
                continue;
            }

            const auto currentSymbol = rules[currentItem.ruleIndex].rightSide[currentItem.position];
            if (grammar.isTerminal(currentSymbol))
            {
                uniquePushBack(partitions[rules[currentItem.ruleIndex].leftSide], currentSymbol);
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
                if (rules[rulesIndicesPath[i]].leftSide == currentSymbol)
                {
                    auto cycle = std::vector<Symbol>{};
                    for (auto j = i; j < rulesIndicesPath.size(); ++j)
                    {
                        uniquePushBack(cycle, rules[rulesIndicesPath[j]].leftSide);
                    }
                    partitions.merge(cycle);
                }
            }

            auto hasUnvisitedChildren = false;
            for (auto ruleIndex = 0U; ruleIndex < rules.size(); ++ruleIndex)
            {
                if (rules[ruleIndex].leftSide == currentSymbol && !contains(visitedRulesIndices, ruleIndex))
                {
                    hasUnvisitedChildren = true;
                    stack.push_back({PartialItem{static_cast<int>(ruleIndex), 0}, currentItem.ruleIndex});
                    visitedRulesIndices.push_back(ruleIndex);
                }
            }
            if (!hasUnvisitedChildren)
            {
                partitions[currentSymbol];
                partitions[rules[currentItem.ruleIndex].leftSide];
                const auto& firstSet = partitions[currentSymbol];
                auto& target = partitions[rules[currentItem.ruleIndex].leftSide];
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

    partitions.forEach(
        [&firstTable](const auto& partition, const auto& firstSet)
        {
            for (const auto& symbol : partition)
            {
                firstTable[symbol.getIdentifierIndex()] = std::vector<Symbol>{firstSet.cbegin(), firstSet.cend()};
            }
        });
    for (const auto& symbol : blanks)
    {
        firstTable[symbol.getIdentifierIndex()].push_back(grammar.getEmptySymbol());
    }

    return firstTable;
}

}
