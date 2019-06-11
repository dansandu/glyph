#include "dansandu/glyph/implementation/parsing.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/ballotin/string.hpp"
#include "dansandu/glyph/implementation/grammar.hpp"
#include "dansandu/glyph/node.hpp"
#include "dansandu/glyph/token.hpp"

#include <iterator>
#include <stdexcept>
#include <vector>

using dansandu::ballotin::string::join;
using dansandu::glyph::implementation::grammar::endOfString;
using dansandu::glyph::implementation::grammar::Rule;
using dansandu::glyph::implementation::parsing_table::Action;
using dansandu::glyph::implementation::parsing_table::ParsingTable;
using dansandu::glyph::node::Node;
using dansandu::glyph::token::Token;

namespace dansandu::glyph::implementation::parsing {

Node parse(std::vector<Token> tokens, const ParsingTable& parsingTable, const std::vector<Rule>& rules) {
    tokens.push_back({endOfString, -1, -1});
    auto states = std::vector<int>{parsingTable.startRuleIndex};
    auto trees = std::vector<Node>{};
    auto token = tokens.cbegin();
    const auto& table = parsingTable.table;
    while (!states.empty()) {
        auto state = states.back();
        auto row = table.find(token->getIdentifier());
        if (row == table.cend())
            THROW(SyntaxError, "unrecognized symbol '", token->getIdentifier(), "' at column ", token->begin());
        auto cell = row->second.at(state);
        if (cell.action == Action::shift) {
            states.push_back(cell.parameter);
            trees.emplace_back(*token);
            ++token;
        } else if (cell.action == Action::reduce || cell.action == Action::accept) {
            const auto& reductionRule = rules.at(cell.parameter);
            auto reductionSize = reductionRule.rightSide.size();
            if (states.size() < reductionSize || trees.size() < reductionSize)
                THROW(std::runtime_error,
                      "invalid state reached -- couldn't perform reduce action because the stack has missing elements");
            auto newNode = Node{cell.parameter, std::vector<Node>{std::make_move_iterator(trees.end() - reductionSize),
                                                                  std::make_move_iterator(trees.end())}};
            states.erase(states.end() - reductionSize, states.end());
            trees.erase(trees.end() - reductionSize, trees.end());
            if (cell.action == Action::reduce) {
                if (states.empty())
                    THROW(std::runtime_error,
                          "invalid state reached -- missing element in state stack on reduce action");
                auto newState = table.at(reductionRule.leftSide).at(states.back()).parameter;
                states.push_back(newState);
                trees.push_back(std::move(newNode));
            } else {
                return newNode;
            }
        } else if (cell.action == Action::error) {
            auto expectedSymbols = std::vector<std::string>{};
            for (const auto& row : table)
                if (row.second.at(state).action != Action::error)
                    expectedSymbols.push_back(row.first);
            THROW(SyntaxError, "invalid syntax at column ", token->begin(), " -- the following symbols were expected ",
                  join(expectedSymbols, ", "));
        } else {
            THROW(std::runtime_error, "invalid state reached -- unrecognized cell action");
        }
    }
    THROW(std::runtime_error, "invalid state reached");
}

}
