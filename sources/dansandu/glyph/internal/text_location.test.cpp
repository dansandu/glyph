#include "dansandu/glyph/internal/text_location.hpp"
#include "catchorg/catch/catch.hpp"

using dansandu::glyph::internal::text_location::getTextLocation;

TEST_CASE("TextLocation")
{
    SECTION("empty text")
    {
        const auto textLocation = getTextLocation("", 0, 0);

        REQUIRE(textLocation.lineNumber == 0);
        REQUIRE(textLocation.columnNumber == 0);
        REQUIRE(textLocation.highlight == "(empty text)");
    }

    SECTION("one line text with token at the end")
    {
        const auto textLocation = getTextLocation("line", 4, 4);

        REQUIRE(textLocation.lineNumber == 1);
        REQUIRE(textLocation.columnNumber == 5);
        REQUIRE(textLocation.highlight == "line\n    ^ (end of text)");
    }

    SECTION("one line text with token at the beginning")
    {
        const auto textLocation = getTextLocation("Single line.", 0, 0);

        REQUIRE(textLocation.lineNumber == 1);
        REQUIRE(textLocation.columnNumber == 1);
        REQUIRE(textLocation.highlight == "Single line.\n^");
    }

    SECTION("multiline text with token on first line")
    {
        const auto textLocation = getTextLocation("First line.\nSecond line.\nThird line.", 0, 5);

        REQUIRE(textLocation.lineNumber == 1);
        REQUIRE(textLocation.columnNumber == 1);
        REQUIRE(textLocation.highlight == "First line.\n^~~~~");
    }

    SECTION("multiline text with token on second line")
    {
        const auto textLocation = getTextLocation("First line.\nSecond line here.\nThe third line.", 19, 23);

        REQUIRE(textLocation.lineNumber == 2);
        REQUIRE(textLocation.columnNumber == 8);
        REQUIRE(textLocation.highlight == "Second line here.\n       ^~~~");
    }

    SECTION("multiline text with token on third line")
    {
        const auto textLocation = getTextLocation("First line.\nSecond line here.\nThe third line.", 40, 45);

        REQUIRE(textLocation.lineNumber == 3);
        REQUIRE(textLocation.columnNumber == 11);
        REQUIRE(textLocation.highlight == "The third line.\n          ^~~~~");
    }
}
