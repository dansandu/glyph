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
          tokenizer_{{{"plus", "\\+"},
                      {"minus", "\\-"},
                      {"multiply", "\\*"},
                      {"divide", "\\/"},
                      {"power", "\\^"},
                      {"identifier", "[a-zA-Z]\\w*"},
                      {"number", "([1-9]\\d*|0)(\\.\\d+)?"},
                      {"parenthesesStart", "\\("},
                      {"parenthesesEnd", "\\)"},
                      {"whitespace", "\\s+"}},
                     {"whitespace"}}
    {
    }

    double evaluate(const std::map<std::string, double (*)(double)>& functions,
                    const std::map<std::string, double>& variables,
                    std::string_view formula) const
    {
        const auto identifier = parser_.getTerminalSymbol("identifier");
        const auto number = parser_.getTerminalSymbol("number");

        auto tokensStack = std::vector<Token>{};
        auto valuesStack = std::vector<double>{};

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
                    auto rhs = pop(valuesStack);
                    auto lhs = pop(valuesStack);
                    valuesStack.push_back(lhs + rhs);
                    break;
                }
                case 2:
                {
                    auto rhs = pop(valuesStack);
                    auto lhs = pop(valuesStack);
                    valuesStack.push_back(lhs - rhs);
                    break;
                }
                case 4:
                {
                    auto rhs = pop(valuesStack);
                    auto lhs = pop(valuesStack);
                    valuesStack.push_back(lhs * rhs);
                    break;
                }
                case 5:
                {
                    auto rhs = pop(valuesStack);
                    auto lhs = pop(valuesStack);
                    valuesStack.push_back(lhs / rhs);
                    break;
                }
                case 7:
                {
                    auto rhs = pop(valuesStack);
                    auto lhs = pop(valuesStack);
                    valuesStack.push_back(std::pow(lhs, rhs));
                    break;
                }
                case 11:
                {
                    auto value = pop(valuesStack);
                    valuesStack.push_back(-value);
                    break;
                }
                case 12:
                {
                    auto token = pop(tokensStack);
                    auto id = std::string{formula.begin() + token.begin(), formula.begin() + token.end()};
                    valuesStack.push_back(variables.at(id));
                    break;
                }
                case 13:
                {
                    auto token = pop(tokensStack);
                    auto nr = std::string{formula.begin() + token.begin(), formula.begin() + token.end()};
                    valuesStack.push_back(std::stod(nr));
                    break;
                }
                case 14:
                {
                    auto token = pop(tokensStack);
                    auto argument = pop(valuesStack);
                    auto function = std::string{formula.begin() + token.begin(), formula.begin() + token.end()};
                    valuesStack.push_back(functions.at(function)(argument));
                    break;
                }
                default:
                    THROW(std::runtime_error, "unrecognized rule index: ", node.getRuleIndex());
                }
            }
        };

        const auto mapper = [this](auto id) { return parser_.getTerminalSymbol(id); };

        parser_.parse(tokenizer_(formula, mapper), visitor);

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
}
// clang-format on
