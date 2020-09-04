#include "catchorg/catch/catch.hpp"
#include "dansandu/glyph/symbol.hpp"
#include "dansandu/glyph/token.hpp"

using dansandu::glyph::symbol::Symbol;
using dansandu::glyph::token::Token;

TEST_CASE("Token")
{
    const auto symbol = Symbol{3};

    const auto begin = 5;

    const auto end = 10;

    const auto token = Token{symbol, begin, end};

    REQUIRE(token.getSymbol() == symbol);

    REQUIRE(token.begin() == begin);

    REQUIRE(token.end() == end);

    REQUIRE(token == token);

    REQUIRE(token == Token{symbol, begin, end});

    REQUIRE(token != Token{Symbol{2}, begin, end});

    REQUIRE(token != Token{symbol, 0, end});

    REQUIRE(token != Token{symbol, begin, 0});
}
