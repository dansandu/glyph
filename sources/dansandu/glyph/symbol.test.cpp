#include "catchorg/catch/catch.hpp"
#include "dansandu/glyph/symbol.hpp"

using dansandu::glyph::symbol::Symbol;

TEST_CASE("Symbol")
{
    const auto identifierIndex = 7;
    const auto symbol = Symbol{identifierIndex};

    REQUIRE(symbol.getIdentifierIndex() == identifierIndex);

    REQUIRE(symbol < Symbol{8});

    REQUIRE(Symbol{8} > symbol);

    REQUIRE(symbol != Symbol{8});

    REQUIRE(symbol == symbol);
}
