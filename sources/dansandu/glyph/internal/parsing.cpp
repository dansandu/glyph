#include "dansandu/glyph/internal/parsing.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/ballotin/string.hpp"
#include "dansandu/glyph/error.hpp"
#include "dansandu/glyph/internal/grammar.hpp"
#include "dansandu/glyph/node.hpp"
#include "dansandu/glyph/symbol.hpp"
#include "dansandu/glyph/token.hpp"

#include <functional>
#include <stdexcept>
#include <vector>

using dansandu::ballotin::string::join;
using dansandu::glyph::error::SyntaxError;
using dansandu::glyph::internal::grammar::Grammar;
using dansandu::glyph::internal::parsing_table::Action;
using dansandu::glyph::internal::parsing_table::Cell;
using dansandu::glyph::node::Node;
using dansandu::glyph::symbol::Symbol;
using dansandu::glyph::token::Token;

namespace dansandu::glyph::internal::parsing
{

void parse(const std::vector<Token>& tokens, const std::vector<std::vector<Cell>>& parsingTable, const Grammar& grammar,
           const std::function<void(const Node&)>& visitor)
{
    auto tokenPosition = tokens.cbegin();
    auto stateStack = std::vector<int>{grammar.getStartRuleIndex()};
    while (!stateStack.empty())
    {
        while (tokenPosition != tokens.cend() && tokenPosition->getSymbol() == grammar.getDiscardedSymbolPlaceholder())
        {
            ++tokenPosition;
        }

        const auto token =
            tokenPosition != tokens.cend() ? *tokenPosition : Token{grammar.getEndOfStringSymbol(), -1, -1};
        const auto state = stateStack.back();
        const auto cell = parsingTable[token.getSymbol().getIdentifierIndex()][state];
        if (cell.action == Action::shift)
        {
            stateStack.push_back(cell.parameter);
            visitor(Node{token});
            ++tokenPosition;
        }
        else if (cell.action == Action::reduce || cell.action == Action::accept)
        {
            const auto& reductionRule = grammar.getRules()[cell.parameter];
            const auto reductionSize = reductionRule.rightSide.size();
            if (stateStack.size() < reductionSize)
            {
                THROW(std::logic_error, "invalid state reached -- insufficient stack size for reduction");
            }
            stateStack.erase(stateStack.end() - reductionSize, stateStack.end());
            if (cell.action == Action::reduce)
            {
                if (stateStack.empty())
                {
                    THROW(std::logic_error, "invalid state reached -- insufficient stack size for reduction");
                }
                const auto newState =
                    parsingTable[reductionRule.leftSide.getIdentifierIndex()][stateStack.back()].parameter;
                stateStack.push_back(newState);
                visitor(Node{cell.parameter});
            }
            else
            {
                stateStack.pop_back();
                visitor(Node{cell.parameter});
            }
        }
        else
        {
            auto expectedSymbols = std::vector<std::string>{};
            for (auto symbolIndex = 0; symbolIndex < static_cast<int>(parsingTable.size()); ++symbolIndex)
            {
                if (parsingTable[symbolIndex][state].action != Action::error)
                {
                    expectedSymbols.push_back(grammar.getIdentifier(Symbol{symbolIndex}));
                }
            }
            THROW(SyntaxError, "invalid syntax at column ", token.begin() + 1, " with symbol '",
                  grammar.getIdentifier(token.getSymbol()),
                  "' -- the following symbols were expected: ", join(expectedSymbols, ", "));
        }
    }
}

}
