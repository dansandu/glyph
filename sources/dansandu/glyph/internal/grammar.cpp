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

using dansandu::ballotin::container::uniquePushBack;
using dansandu::ballotin::string::format;
using dansandu::ballotin::string::join;
using dansandu::ballotin::string::split;
using dansandu::ballotin::string::trim;
using dansandu::glyph::error::GrammarError;
using dansandu::glyph::internal::rule::Rule;
using dansandu::glyph::symbol::Symbol;

namespace dansandu::glyph::internal::grammar
{

template<typename T, typename U>
auto find(const std::vector<T>& container, const U& value)
{
    return std::find(container.cbegin(), container.cend(), value);
}

Grammar::Grammar(const std::string_view grammar)
{
    static const auto productionRulePattern =
        std::regex{R"( *[a-zA-Z0-9]+ *-> *(?:(?:[a-zA-Z0-9]+ +)*[a-zA-Z0-9]+ *)?)"};

    auto leftSideColumn = std::vector<std::string>{};
    auto rightSideColumn = std::vector<std::vector<std::string>>{};

    for (const auto& line : split(grammar, "\n"))
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
