#pragma once

#include <stdexcept>

namespace dansandu::glyph::error
{

class GrammarError : public std::runtime_error
{
    using runtime_error::runtime_error;
};

class ParsingError : public std::runtime_error
{
    using runtime_error::runtime_error;
};

class SyntaxError : public std::runtime_error
{
    using runtime_error::runtime_error;
};

class TokenizationError : public std::runtime_error
{
    using runtime_error::runtime_error;
};

}
