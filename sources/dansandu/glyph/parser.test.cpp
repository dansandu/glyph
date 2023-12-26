#include "dansandu/glyph/parser.hpp"
#include "catchorg/catch/catch.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/glyph/error.hpp"
#include "dansandu/glyph/node.hpp"
#include "dansandu/glyph/regex_tokenizer.hpp"
#include "dansandu/glyph/token.hpp"

#include <cmath>
#include <map>
#include <string>

using Catch::Detail::Approx;
using dansandu::glyph::error::SyntaxError;
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
class ArithmeticParser
{
public:
    ArithmeticParser()
        : parser_{R"(
            /* 0*/ Start -> Sums
            /* 1*/ Sums  -> Sums plus Products
            /* 2*/ Sums  -> Sums minus Products
            /* 3*/ Sums  -> Products
            /* 4*/ Products -> Products multiply Signed
            /* 5*/ Products -> Products divide Signed
            /* 6*/ Products -> Signed
            /* 7*/ Signed -> plus Exponentials
            /* 8*/ Signed -> minus Exponentials
            /* 9*/ Signed -> Exponentials
            /*10*/ Exponentials -> Value power Signed
            /*11*/ Exponentials -> Value
            /*12*/ Value -> identifier
            /*13*/ Value -> number
            /*14*/ Value -> identifier parenthesesStart Sums parenthesesEnd
            /*15*/ Value -> parenthesesStart Sums parenthesesEnd
          )"},
          tokenizer_{{{parser_.getTerminalSymbol("plus"),             "\\+"},
                      {parser_.getTerminalSymbol("minus"),            "\\-"},
                      {parser_.getTerminalSymbol("multiply"),         "\\*"},
                      {parser_.getTerminalSymbol("divide"),           "\\/"},
                      {parser_.getTerminalSymbol("power"),            "\\^"},
                      {parser_.getTerminalSymbol("identifier"),       "[a-zA-Z]\\w*"},
                      {parser_.getTerminalSymbol("number"),           "([1-9]\\d*|0)(\\.\\d+)?"},
                      {parser_.getTerminalSymbol("parenthesesStart"), "\\("},
                      {parser_.getTerminalSymbol("parenthesesEnd"),   "\\)"},
                      {parser_.getDiscardedSymbolPlaceholder(),       "\\s+"}}}
    {
    }

    double evaluate(const std::map<std::string, double (*)(double)>& functions,
                    const std::map<std::string, double>& variables,
                    const std::string_view formula) const
    {
        const auto identifier = parser_.getTerminalSymbol("identifier");
        const auto number = parser_.getTerminalSymbol("number");

        auto tokensStack = std::vector<Token>{};
        auto valuesStack = std::vector<double>{};

        const auto visitor = [&](const Node& node)
        {
            if (node.isToken())
            {
                const auto token = node.getToken();
                if (token.getSymbol() == identifier || token.getSymbol() == number)
                {
                    tokensStack.push_back(token);
                }
            }
            else
            {
                switch (node.getRuleIndex())
                {
                case 0:
                    break;
                case 1:
                {
                    const auto rhs = pop(valuesStack);
                    const auto lhs = pop(valuesStack);
                    valuesStack.push_back(lhs + rhs);
                    break;
                }
                case 2:
                {
                    const auto rhs = pop(valuesStack);
                    const auto lhs = pop(valuesStack);
                    valuesStack.push_back(lhs - rhs);
                    break;
                }
                case 3:
                    break;
                case 4:
                {
                    const auto rhs = pop(valuesStack);
                    const auto lhs = pop(valuesStack);
                    valuesStack.push_back(lhs * rhs);
                    break;
                }
                case 5:
                {
                    const auto rhs = pop(valuesStack);
                    const auto lhs = pop(valuesStack);
                    valuesStack.push_back(lhs / rhs);
                    break;
                }
                case 6:
                    break;
                case 7:
                    break;
                case 8:
                {
                    const auto value = pop(valuesStack);
                    valuesStack.push_back(-value);
                    break;
                }
                case 9:
                    break;
                case 10:
                {
                    const auto rhs = pop(valuesStack);
                    const auto lhs = pop(valuesStack);
                    valuesStack.push_back(std::pow(lhs, rhs));
                    break;
                }
                case 11:
                    break;
                case 12:
                {
                    const auto token = pop(tokensStack);
                    const auto id = std::string{formula.begin() + token.begin(), formula.begin() + token.end()};
                    valuesStack.push_back(variables.at(id));
                    break;
                }
                case 13:
                {
                    const auto token = pop(tokensStack);
                    const auto nr = std::string{formula.begin() + token.begin(), formula.begin() + token.end()};
                    valuesStack.push_back(std::stod(nr));
                    break;
                }
                case 14:
                {
                    const auto token = pop(tokensStack);
                    const auto argument = pop(valuesStack);
                    const auto function = std::string{formula.begin() + token.begin(), formula.begin() + token.end()};
                    valuesStack.push_back(functions.at(function)(argument));
                    break;
                }
                case 15:
                    break;
                default:
                    THROW(std::logic_error, "unrecognized rule index: ", node.getRuleIndex());
                }
            }
        };

        parser_.parse(tokenizer_(formula), visitor);

        if (!tokensStack.empty())
        {
            THROW(std::logic_error, "tokens stack was not exhausted");
        }

        if (valuesStack.size() != 1)
        {
            THROW(std::logic_error, "values stack must only contain the result");
        }

        return valuesStack.front();
    }

private:
    Parser parser_;
    RegexTokenizer tokenizer_;
};

TEST_CASE("Parser")
{
    const auto parser = ArithmeticParser{};

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

    REQUIRE(parser.evaluate(functions, variables, "z + 20 * y ^ sin(x * pi) * ln(1024) + -100") == Approx(6931.46320796));

    REQUIRE(parser.evaluate(functions, variables, "(20 * -z - -y) / (+300.0 + x)") == Approx(-6.48918469));

    REQUIRE(parser.evaluate(functions, variables, "-3^2") == Approx(-9.0));

    REQUIRE(parser.evaluate(functions, variables, "2^3^2") == Approx(512.0));

    REQUIRE(parser.evaluate(functions, variables, "2^-3^2") == Approx(0.001953125));

    REQUIRE(parser.evaluate(functions, variables, "-2 + 5") == Approx(3.0));

    REQUIRE(parser.evaluate(functions, variables, "-2 * 5") == Approx(-10.0));

    REQUIRE_THROWS_AS(parser.evaluate({}, {}, "(50 + 30"), SyntaxError);

    REQUIRE_THROWS_AS(parser.evaluate({}, {}, ""), SyntaxError);

    REQUIRE_THROWS_AS(parser.evaluate({}, {}, "(50 + 30"), SyntaxError);

    REQUIRE_THROWS_AS(parser.evaluate({}, {}, "50+"), SyntaxError);
}
// clang-format on
