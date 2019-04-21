#include "dansandu/glyph/implementation/parser.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/glyph/implementation/grammar.hpp"

#include <iterator>
#include <stdexcept>
#include <vector>

using dansandu::glyph::implementation::grammar::endOfString;
using dansandu::glyph::implementation::grammar::Rule;
using dansandu::glyph::implementation::parsing_table::Action;
using dansandu::glyph::implementation::parsing_table::ParsingTable;
using dansandu::glyph::implementation::tokenization::Token;

namespace dansandu::glyph::implementation::parser {

Node::Node(Token token) : ruleIndex_{-1}, token_{std::move(token)} {}

Node::Node(int ruleIndex, std::vector<Node> children)
    : ruleIndex_{ruleIndex}, token_{"", 0, 0}, children_{std::move(children)} {}

bool Node::isRule() const { return ruleIndex_ != -1; }

bool Node::isToken() const { return !isRule(); }

int Node::getRuleIndex() const {
    if (isToken())
        THROW(std::runtime_error, "node doesn't hold a rule");
    return ruleIndex_;
}

const Token& Node::getToken() const {
    if (isRule())
        THROW(std::runtime_error, "node doesn't hold a token");
    return token_;
}

const Node& Node::getChild(int index) const { return children_.at(index); }

bool operator==(const Node& left, const Node& right) {
    return left.ruleIndex_ == right.ruleIndex_ && left.token_ == right.token_ && left.children_ == right.children_;
}

bool operator!=(const Node& left, const Node& right) { return !(left == right); }

std::ostream& operator<<(std::ostream& stream, const Node& node) {
    if (node.isToken())
        return stream << node.token_;
    stream << "Node(" << node.ruleIndex_;
    for (const auto& child : node.children_)
        stream << ", " << child;
    return stream << ")";
}

Node parse(std::vector<Token> tokens, const ParsingTable& parsingTable, const std::vector<Rule>& rules) {
    tokens.push_back({endOfString, -1, -1});
    auto states = std::vector<int>{parsingTable.startRuleIndex};
    auto trees = std::vector<Node>{};
    auto token = tokens.cbegin();
    const auto& table = parsingTable.table;
    while (!states.empty()) {
        auto state = states.back();
        auto cell = table.at(token->identifier).at(state);
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
            THROW(SyntaxError, "invalid syntax");
        } else {
            THROW(std::runtime_error, "invalid state reached -- unrecognized cell action");
        }
    }
    THROW(std::runtime_error, "invalid state reached");
}

}
