#include "dansandu/glyph/arithmetic_formula_parser.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/glyph/parser.hpp"

#include <algorithm>
#include <cmath>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using dansandu::glyph::parser::Parser;
using Node = dansandu::glyph::parser::Parser::Node;

namespace dansandu::glyph::arithmetic_formula_parser {

static constexpr auto grammar = /* 0*/ "Start -> Sums                                                 \n"
                                       /* 1*/ "Sums  -> Sums add Products                                    \n"
                                       /* 2*/ "Sums  -> Sums subtract Products                               \n"
                                       /* 3*/ "Sums  -> Products                                             \n"
                                       /* 4*/ "Products -> Products multiply Exponentials                    \n"
                                       /* 5*/ "Products -> Products divide Exponentials                      \n"
                                       /* 6*/ "Products -> Exponentials                                      \n"
                                       /* 7*/ "Exponentials -> Exponentials power Value                      \n"
                                       /* 8*/ "Exponentials -> Value                                         \n"
                                       /* 9*/ "Value -> identifier                                           \n"
                                       /*10*/ "Value -> number                                               \n"
                                       /*11*/ "Value -> identifier parenthesesStart Sums parenthesesEnd      \n"
                                       /*12*/ "Value -> parenthesesStart Sums parenthesesEnd";

static const auto terminals =
    std::vector<std::pair<std::string, std::string>>{{{"add", "\\+"},
                                                      {"subtract", "\\-"},
                                                      {"multiply", "\\*"},
                                                      {"divide", "\\/"},
                                                      {"power", "\\^"},
                                                      {"identifier", "[a-zA-Z]\\w*"},
                                                      {"number", "(?:[1-9]\\d*|0)(?:\\.\\d+)?"},
                                                      {"parenthesesStart", "\\("},
                                                      {"parenthesesEnd", "\\)"},
                                                      {"whitespace", "\\s+"}}};

static const auto functions =
    std::map<std::string, double (*)(double)>{{{"sin", std::sin}, {"cos", std::cos}, {"log", std::log}}};

static double evaluateWork(const Node& node, std::string_view formula, const std::map<std::string, double>& values) {
    if (node.isToken())
        THROW(std::runtime_error, "ill-formed abstract syntax tree");

    auto ruleIndex = node.getRuleIndex();
    constexpr int fallThroughRules[] = {0, 3, 6, 8};
    if (std::find(std::begin(fallThroughRules), std::end(fallThroughRules), ruleIndex) != std::end(fallThroughRules))
        return evaluateWork(node.getChild(0), formula, values);
    else if (ruleIndex == 1)
        return evaluateWork(node.getChild(0), formula, values) + evaluateWork(node.getChild(2), formula, values);
    else if (ruleIndex == 2)
        return evaluateWork(node.getChild(0), formula, values) - evaluateWork(node.getChild(2), formula, values);
    else if (ruleIndex == 4)
        return evaluateWork(node.getChild(0), formula, values) * evaluateWork(node.getChild(2), formula, values);
    else if (ruleIndex == 5)
        return evaluateWork(node.getChild(0), formula, values) / evaluateWork(node.getChild(2), formula, values);
    else if (ruleIndex == 7)
        return std::pow(evaluateWork(node.getChild(0), formula, values),
                        evaluateWork(node.getChild(2), formula, values));
    else if (ruleIndex == 9) {
        const auto& token = node.getChild(0).getToken();
        auto identifier = std::string{formula.begin() + token.begin, formula.begin() + token.end};
        return values.at(identifier);
    } else if (ruleIndex == 10) {
        const auto& token = node.getChild(0).getToken();
        auto numberAsString = std::string{formula.begin() + token.begin, formula.begin() + token.end};
        return std::stod(numberAsString);
    } else if (ruleIndex == 11) {
        const auto& token = node.getChild(0).getToken();
        auto function = std::string{formula.begin() + token.begin, formula.begin() + token.end};
        return functions.at(function)(evaluateWork(node.getChild(2), formula, values));
    } else if (ruleIndex == 12)
        return evaluateWork(node.getChild(1), formula, values);
    else
        THROW(std::runtime_error, "unrecognized rule index ", ruleIndex);
}

double evaluate(std::string_view formula, const std::map<std::string, double>& values) {
    static auto parser = Parser{grammar, terminals};
    auto tree = parser.parse(formula, {"whitespace"});
    return evaluateWork(tree, formula, values);
}

}
