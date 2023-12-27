#pragma once

#include <string>
#include <string_view>

namespace dansandu::glyph::internal::text_location
{

struct TextLocation
{
    int lineNumber;
    int columnNumber;
    std::string highlight;
};

TextLocation getTextLocation(const std::string_view text, const int tokenBegin, const int tokenEnd);

}
