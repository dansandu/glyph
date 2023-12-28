#include "dansandu/glyph/internal/parsing.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/ballotin/string.hpp"
#include "dansandu/glyph/error.hpp"
#include "dansandu/glyph/internal/grammar.hpp"
#include "dansandu/glyph/internal/text_location.hpp"
#include "dansandu/glyph/node.hpp"
#include "dansandu/glyph/symbol.hpp"
#include "dansandu/glyph/token.hpp"

#include <functional>
#include <stdexcept>
#include <vector>

using dansandu::ballotin::string::format;
using dansandu::ballotin::string::join;
using dansandu::glyph::error::SyntaxError;
using dansandu::glyph::internal::grammar::Grammar;
using dansandu::glyph::internal::parsing_table::Action;
using dansandu::glyph::internal::parsing_table::Cell;
using dansandu::glyph::internal::text_location::getTextLocation;
using dansandu::glyph::node::Node;
using dansandu::glyph::symbol::Symbol;
using dansandu::glyph::token::Token;

namespace dansandu::glyph::internal::parsing
{

std::vector<Node> parse(const std::string_view text, const std::vector<Token>& tokens,
                        const std::vector<std::vector<Cell>>& parsingTable, const Grammar& grammar)
{
    auto nodes = std::vector<Node>{};

    const auto textSize = static_cast<int>(text.size());

    auto tokenPosition = tokens.cbegin();
    auto stateStack = std::vector<int>{grammar.getStartRuleIndex()};
    while (!stateStack.empty())
    {
        while (tokenPosition != tokens.cend() && tokenPosition->getSymbol() == grammar.getDiscardedSymbolPlaceholder())
        {
            ++tokenPosition;
        }

        const auto token =
            tokenPosition != tokens.cend() ? *tokenPosition : Token{grammar.getEndOfStringSymbol(), textSize, textSize};
        const auto state = stateStack.back();
        const auto cell = parsingTable[token.getSymbol().getIdentifierIndex()][state];
        if (cell.action == Action::shift)
        {
            stateStack.push_back(cell.parameter);
            nodes.push_back(Node{token});
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
                nodes.push_back(Node{cell.parameter});
            }
            else
            {
                stateStack.pop_back();
                nodes.push_back(Node{cell.parameter});
            }
        }
        else
        {
            auto expectedSymbols = std::vector<Symbol>{};
            auto expectedSymbolsString = std::vector<std::string>{};
            for (auto symbolIndex = 0; symbolIndex < static_cast<int>(parsingTable.size()); ++symbolIndex)
            {
                if (parsingTable[symbolIndex][state].action != Action::error)
                {
                    const auto symbol = Symbol{symbolIndex};
                    expectedSymbols.push_back(symbol);
                    expectedSymbolsString.push_back(grammar.getIdentifier(symbol));
                }
            }

            const auto textLocation = getTextLocation(text, token.begin(), token.end());

            throw SyntaxError{format("invalid syntax at line ", textLocation.lineNumber, " and column ",
                                     textLocation.columnNumber, " with symbol '",
                                     grammar.getIdentifier(token.getSymbol()),
                                     "' -- the following symbols were expected: ", join(expectedSymbolsString, ", "),
                                     "\n", textLocation.highlight),
                              textLocation.lineNumber, textLocation.columnNumber, token.getSymbol(), expectedSymbols};
        }
    }

    return nodes;
}

}
