#pragma once

#include <exception>

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
    explicit SyntaxError(std::string message) : message_{std::move(message)}
    {
    }

    const char* what() const noexcept override
    {
        return message_.c_str();
    }

private:
    std::string message_;
};

}
