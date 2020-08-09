#include "dansandu/glyph/implementation/parsing.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/ballotin/string.hpp"
#include "dansandu/glyph/error.hpp"
#include "dansandu/glyph/implementation/grammar.hpp"
#include "dansandu/glyph/node.hpp"
#include "dansandu/glyph/symbol.hpp"
#include "dansandu/glyph/token.hpp"

#include <stdexcept>
#include <vector>

using dansandu::ballotin::string::join;
using dansandu::glyph::error::SyntaxError;
using dansandu::glyph::implementation::grammar::Grammar;
using dansandu::glyph::implementation::parsing_table::Action;
using dansandu::glyph::implementation::parsing_table::Cell;
using dansandu::glyph::node::Node;
using dansandu::glyph::symbol::Symbol;
using dansandu::glyph::token::Token;

namespace dansandu::glyph::implementation::parsing
{

std::vector<Node> parse(std::vector<Token> tokens, const std::vector<std::vector<Cell>>& parsingTable,
                        const Grammar& grammar)
{
    tokens.push_back({grammar.getEndOfStringSymbol(), -1, -1});
    auto position = tokens.cbegin();
    auto stateStack = std::vector<int>{grammar.getStartRuleIndex()};
    auto tree = std::vector<Node>{};
    while (!stateStack.empty())
    {
        auto symbolIndex = position->getSymbol().getIdentifierIndex();
        if (symbolIndex < 0 || symbolIndex >= grammar.getIdentifiersCount())
        {
            THROW(std::runtime_error, "symbol with index ", symbolIndex, " is not recognized by the grammar");
        }
        auto state = stateStack.back();
        auto cell = parsingTable[symbolIndex][state];
        if (cell.action == Action::shift)
        {
            stateStack.push_back(cell.parameter);
            tree.push_back(Node{*position});
            ++position;
        }
        else if (cell.action == Action::reduce || cell.action == Action::accept)
        {
            const auto& rule = grammar.getRules()[cell.parameter];
            auto size =
                !rule.rightSide.empty() && rule.rightSide[0] == grammar.getEmptySymbol() ? 0 : rule.rightSide.size();
            if (stateStack.size() < size || tree.size() < size)
            {
                THROW(std::runtime_error,
                      "invalid state reached -- couldn't perform reduce action because the stack has missing elements");
            }
            stateStack.erase(stateStack.end() - size, stateStack.end());
            if (cell.action == Action::reduce)
            {
                if (stateStack.empty())
                {
                    THROW(std::runtime_error,
                          "invalid state reached -- missing element in state stack on reduce action");
                }
                auto newState = parsingTable[rule.leftSide.getIdentifierIndex()][stateStack.back()].parameter;
                stateStack.push_back(newState);
                tree.push_back(Node{cell.parameter});
            }
            else
            {
                tree.push_back(Node{cell.parameter});
                return tree;
            }
        }
        else if (cell.action == Action::error)
        {
            auto expectedSymbols = std::vector<std::string>{};
            for (auto symbolIndex = 0; symbolIndex < static_cast<int>(parsingTable.size()); ++symbolIndex)
            {
                if (parsingTable[symbolIndex][state].action != Action::error)
                {
                    expectedSymbols.push_back(grammar.getIdentifier(Symbol{symbolIndex}));
                }
            }
            THROW(SyntaxError, "invalid syntax at column ", position->begin(),
                  " -- the following symbols were expected ", join(expectedSymbols, ", "));
        }
        else
        {
            THROW(std::runtime_error, "invalid state reached -- unrecognized cell action");
        }
    }
    THROW(std::runtime_error, "invalid state reached");
}

}
