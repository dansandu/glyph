#include "catchorg/catch/catch.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/glyph/node.hpp"
#include "dansandu/glyph/parser.hpp"
#include "dansandu/glyph/token.hpp"

#include <cmath>
#include <map>
#include <string>

using Catch::Detail::Approx;
using dansandu::glyph::node::Node;
using dansandu::glyph::parser::Parser;
using dansandu::glyph::token::RegexTokenizer;

static double visitor(const Node& node, std::string_view formula, const std::map<std::string, double>& variables,
                      const std::map<std::string, double (*)(double)>& functions)
{
    if (node.isToken())
    {
        THROW(std::runtime_error, "ill-formed abstract syntax tree");
    }

    auto ruleIndex = node.getRuleIndex();
    constexpr int fallThroughRules[] = {0, 3, 6, 8};
    if (std::find(std::begin(fallThroughRules), std::end(fallThroughRules), ruleIndex) != std::end(fallThroughRules))
    {
        return visitor(node.getChild(0), formula, variables, functions);
    }
    else if (ruleIndex == 1)
    {
        return visitor(node.getChild(0), formula, variables, functions) +
               visitor(node.getChild(2), formula, variables, functions);
    }
    else if (ruleIndex == 2)
    {
        return visitor(node.getChild(0), formula, variables, functions) -
               visitor(node.getChild(2), formula, variables, functions);
    }
    else if (ruleIndex == 4)
    {
        return visitor(node.getChild(0), formula, variables, functions) *
               visitor(node.getChild(2), formula, variables, functions);
    }
    else if (ruleIndex == 5)
    {
        return visitor(node.getChild(0), formula, variables, functions) /
               visitor(node.getChild(2), formula, variables, functions);
    }
    else if (ruleIndex == 7)
    {
        return std::pow(visitor(node.getChild(0), formula, variables, functions),
                        visitor(node.getChild(2), formula, variables, functions));
    }
    else if (ruleIndex == 9)
    {
        const auto& token = node.getChild(0).getToken();
        auto identifier = std::string{formula.begin() + token.begin(), formula.begin() + token.end()};
        return variables.at(identifier);
    }
    else if (ruleIndex == 10)
    {
        const auto& token = node.getChild(0).getToken();
        auto numberAsString = std::string{formula.begin() + token.begin(), formula.begin() + token.end()};
        return std::stod(numberAsString);
    }
    else if (ruleIndex == 11)
    {
        const auto& token = node.getChild(0).getToken();
        auto function = std::string{formula.begin() + token.begin(), formula.begin() + token.end()};
        return functions.at(function)(visitor(node.getChild(2), formula, variables, functions));
    }
    else if (ruleIndex == 12)
    {
        return visitor(node.getChild(1), formula, variables, functions);
    }
    else
    {
        THROW(std::runtime_error, "unrecognized rule index ", ruleIndex);
    }
}

TEST_CASE("Parser")
{
    // clang-format off
    const auto grammar = /* 0*/ "Start -> Sums                                            \n"
                         /* 1*/ "Sums  -> Sums add Products                               \n"
                         /* 2*/ "Sums  -> Sums subtract Products                          \n"
                         /* 3*/ "Sums  -> Products                                        \n"
                         /* 4*/ "Products -> Products multiply Exponentials               \n"
                         /* 5*/ "Products -> Products divide Exponentials                 \n"
                         /* 6*/ "Products -> Exponentials                                 \n"
                         /* 7*/ "Exponentials -> Exponentials power Value                 \n"
                         /* 8*/ "Exponentials -> Value                                    \n"
                         /* 9*/ "Value -> identifier                                      \n"
                         /*10*/ "Value -> number                                          \n"
                         /*11*/ "Value -> identifier parenthesesStart Sums parenthesesEnd \n"
                         /*12*/ "Value -> parenthesesStart Sums parenthesesEnd";

    const auto regexTokenizer = RegexTokenizer{{{"add", "\\+"},
                                                {"subtract", "\\-"},
                                                {"multiply", "\\*"},
                                                {"divide", "\\/"},
                                                {"power", "\\^"},
                                                {"identifier", "[a-zA-Z]\\w*"},
                                                {"number", "(?:[1-9]\\d*|0)(?:\\.\\d+)?"},
                                                {"parenthesesStart", "\\("},
                                                {"parenthesesEnd", "\\)"},
                                                {"whitespace", "\\s+"}},
                                               {{"whitespace"}}};
    // clang-format on

    const auto functions =
        std::map<std::string, double (*)(double)>{{{"sin", std::sin}, {"cos", std::cos}, {"log", std::log}}};

    const auto variables = std::map<std::string, double>{{{"x", 0.5}, {"y", 50.0}, {"z", 100.0}, {"pi", 3.14}}};

    auto formula = "z + 20 * y ^ sin(x * pi) * log(1024)";
    auto parser = Parser{grammar};
    auto tree = parser.parse(formula, regexTokenizer);

    REQUIRE(visitor(tree, formula, variables, functions) == Approx(7031.46320796));
}
