#include "catchorg/catch/catch.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/glyph/error.hpp"
#include "dansandu/glyph/node.hpp"
#include "dansandu/glyph/parser.hpp"
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
        : parser_{/* 0*/ "Start -> Sums                                            \n"
                  /* 1*/ "Sums  -> Sums plus Products                              \n"
                  /* 2*/ "Sums  -> Sums minus Products                             \n"
                  /* 3*/ "Sums  -> Products                                        \n"
                  /* 4*/ "Products -> Products multiply Exponentials               \n"
                  /* 5*/ "Products -> Products divide Exponentials                 \n"
                  /* 6*/ "Products -> Exponentials                                 \n"
                  /* 7*/ "Exponentials -> Exponentials power SignedValue           \n"
                  /* 8*/ "Exponentials -> SignedValue                              \n"
                  /* 9*/ "SignedValue  -> Value                                    \n"
                  /*10*/ "SignedValue  -> plus Value                               \n"
                  /*11*/ "SignedValue  -> minus Value                              \n"
                  /*12*/ "Value -> identifier                                      \n"
                  /*13*/ "Value -> number                                          \n"
                  /*14*/ "Value -> identifier parenthesesStart Sums parenthesesEnd \n"
                  /*15*/ "Value -> parenthesesStart Sums parenthesesEnd"},
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
                case 3:
                case 6:
                case 8:
                case 9:
                case 10:
                case 15:
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
                case 7:
                {
                    const auto rhs = pop(valuesStack);
                    const auto lhs = pop(valuesStack);
                    valuesStack.push_back(std::pow(lhs, rhs));
                    break;
                }
                case 11:
                {
                    const auto value = pop(valuesStack);
                    valuesStack.push_back(-value);
                    break;
                }
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
                default:
                    THROW(std::runtime_error, "unrecognized rule index: ", node.getRuleIndex());
                }
            }
        };

        parser_.parse(tokenizer_(formula), visitor);

        if (!tokensStack.empty())
        {
            THROW(std::runtime_error, "tokens stack was not exhausted");
        }

        if (valuesStack.size() != 1)
        {
            THROW(std::runtime_error, "values stack must only contain the result");
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

    REQUIRE_THROWS_AS(parser.evaluate({}, {}, "(50 + 30"), SyntaxError);

    REQUIRE_THROWS_AS(parser.evaluate({}, {}, ""), SyntaxError);

    REQUIRE_THROWS_AS(parser.evaluate({}, {}, "(50 + 30"), SyntaxError);

    REQUIRE_THROWS_AS(parser.evaluate({}, {}, "50+"), SyntaxError);
}
// clang-format on
