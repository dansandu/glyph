#include "catchorg/catch/catch.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/glyph/node.hpp"
#include "dansandu/glyph/parser.hpp"
#include "dansandu/glyph/regex_tokenizer.hpp"
#include "dansandu/glyph/token.hpp"

#include <cmath>
#include <map>
#include <string>

using Catch::Detail::Approx;
using dansandu::glyph::node::Node;
using dansandu::glyph::parser::Parser;
using dansandu::glyph::regex_tokenizer::RegexTokenizer;
using dansandu::glyph::token::Token;

template<typename T>
auto pop(std::vector<T>& stack)
{
    auto value = std::move(stack.at(stack.size() - 1));
    stack.pop_back();
    return value;
}

// clang-format off
TEST_CASE("Parser")
{
    const auto parser = Parser{/* 0*/ "Start -> Sums                                            \n"
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
                               /*12*/ "Value -> parenthesesStart Sums parenthesesEnd"};

    const auto functions = std::map<std::string, double (*)(double)>{
        {"sin", std::sin},
        {"cos", std::cos},
        {"ln", std::log}
    };

    const auto variables = std::map<std::string, double>{
        {"x", 0.5},
        {"y", 50.0},
        {"z", 100.0},
        {"pi", 3.14}
    };

    const auto formula = std::string{"z + 20 * y ^ sin(x * pi) * ln(1024)"};
    
    auto tokensStack = std::vector<Token>{};
    auto valuesStack = std::vector<double>{};

    const auto identifier = parser.getTerminalSymbol("identifier");
    const auto number     = parser.getTerminalSymbol("number");
    
    const auto visitor = [&](const Node& node)
    {
        if (node.isToken())
        {
            auto token = node.getToken();
            if (token.getSymbol() == identifier || token.getSymbol() == number)
            {
                tokensStack.push_back(token);
            }
        }
        else
        {
            auto ruleIndex = node.getRuleIndex();
            if (ruleIndex == 0)
            {

            }
            else if (ruleIndex == 1)
            {
                auto rhs = pop(valuesStack);
                auto lhs = pop(valuesStack);
                valuesStack.push_back(lhs + rhs);
            }
            else if (ruleIndex == 2)
            {
                auto rhs = pop(valuesStack);
                auto lhs = pop(valuesStack);
                valuesStack.push_back(lhs - rhs);
            }
            else if (ruleIndex == 3)
            {

            }
            else if (ruleIndex == 4)
            {
                auto rhs = pop(valuesStack);
                auto lhs = pop(valuesStack);
                valuesStack.push_back(lhs * rhs);
            }
            else if (ruleIndex == 5)
            {
                auto rhs = pop(valuesStack);
                auto lhs = pop(valuesStack);
                valuesStack.push_back(lhs / rhs);
            }
            else if (ruleIndex == 6)
            {

            }
            else if (ruleIndex == 7)
            {
                auto rhs = pop(valuesStack);
                auto lhs = pop(valuesStack);
                valuesStack.push_back(std::pow(lhs, rhs));
            }
            else if (ruleIndex == 8)
            {
                
            }
            else if (ruleIndex == 9)
            {
                auto token = pop(tokensStack);
                auto id = std::string{formula.begin() + token.begin(), formula.begin() + token.end()};
                valuesStack.push_back(variables.at(id));
            }
            else if (ruleIndex == 10)
            {
                auto token = pop(tokensStack);
                auto nr = std::string{formula.begin() + token.begin(), formula.begin() + token.end()};
                valuesStack.push_back(std::stod(nr));
            }
            else if (ruleIndex == 11)
            {
                auto token = pop(tokensStack);
                auto argument = pop(valuesStack);
                auto function = std::string{formula.begin() + token.begin(), formula.begin() + token.end()};
                valuesStack.push_back(functions.at(function)(argument));
            }
            else if (ruleIndex == 12)
            {
            
            }
            else
            {
                THROW(std::runtime_error, "unrecognized rule index: ", ruleIndex);
            }
        }
    };

    const auto regexTokenizer = RegexTokenizer{{{"add",              "\\+"},
                                                {"subtract",         "\\-"},
                                                {"multiply",         "\\*"},
                                                {"divide",           "\\/"},
                                                {"power",            "\\^"},
                                                {"identifier",       "[a-zA-Z]\\w*"},
                                                {"number",           "([1-9]\\d*|0)(\\.\\d+)?"},
                                                {"parenthesesStart", "\\("},
                                                {"parenthesesEnd",   "\\)"},
                                                {"whitespace",       "\\s+"}},
                                               [&parser](auto id) { return parser.getTerminalSymbol(id); },
                                               {"whitespace"}};

    const auto tokens = regexTokenizer(formula);

    parser.parse(tokens, visitor);

    REQUIRE(tokensStack.empty());

    REQUIRE(valuesStack.size() == 1);

    REQUIRE(valuesStack.front() == Approx(7031.46320796));
}
// clang-format on