#include "catchorg/catch/catch.hpp"
#include "dansandu/glyph/node.hpp"
#include "dansandu/glyph/token.hpp"

using dansandu::glyph::node::Node;
using dansandu::glyph::token::Token;

TEST_CASE("Node") {
    auto token = Token{"number", 0, 1};

    SECTION("with token") {
        auto node = Node{token};

        REQUIRE(node.isToken());

        REQUIRE(!node.isRule());

        REQUIRE(node.getChildrenCount() == 0);

        REQUIRE(node.getToken() == token);
    }

    SECTION("with production rule") {
        auto anotherToken = Token{"add", 2, 3};
        constexpr auto ruleIndex = 23;
        auto node = Node{ruleIndex, {Node{token}, Node{anotherToken}}};

        REQUIRE(node.isRule());

        REQUIRE(!node.isToken());

        REQUIRE(node.getRuleIndex() == ruleIndex);

        REQUIRE(node.getChildrenCount() == 2);

        REQUIRE(node.getChild(0).getToken() == token);

        REQUIRE(node.getChild(1).getToken() == anotherToken);
    }
}
