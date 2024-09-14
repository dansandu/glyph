#pragma once

#include "dansandu/glyph/symbol.hpp"

#include <exception>
#include <string>
#include <vector>

namespace dansandu::glyph::error
{

class GrammarError : public std::exception
{
public:
    explicit GrammarError(std::string message) : message_{std::move(message)}
    {
    }

    const char* what() const noexcept override
    {
        return message_.c_str();
    }

private:
    std::string message_;
};

class TokenizationError : public std::exception
{
public:
    explicit TokenizationError(std::string message) : message_{std::move(message)}
    {
    }

    const char* what() const noexcept override
    {
        return message_.c_str();
    }

private:
    std::string message_;
};

class SyntaxError : public std::exception
{
public:
    SyntaxError(std::string message, const int lineNumber, const int columnNumber,
                const dansandu::glyph::symbol::Symbol encounteredSymbol,
                std::vector<dansandu::glyph::symbol::Symbol> expectedSymbols)
        : message_{std::move(message)},
          lineNumber_{lineNumber},
          columnNumber_{columnNumber},
          encounteredSymbol_{encounteredSymbol},
          expectedSymbols_{std::move(expectedSymbols)}
    {
    }

    const char* what() const noexcept override
    {
        return message_.c_str();
    }

    int getLineNumber() const noexcept
    {
        return lineNumber_;
    }

    int getColumnNumber() const noexcept
    {
        return columnNumber_;
    }

    dansandu::glyph::symbol::Symbol getEncounteredSymbol() const noexcept
    {
        return encounteredSymbol_;
    }

    const std::vector<dansandu::glyph::symbol::Symbol>& getExpectedSymbols() const noexcept
    {
        return expectedSymbols_;
    }

private:
    std::string message_;
    int lineNumber_;
    int columnNumber_;
    dansandu::glyph::symbol::Symbol encounteredSymbol_;
    std::vector<dansandu::glyph::symbol::Symbol> expectedSymbols_;
};

}
