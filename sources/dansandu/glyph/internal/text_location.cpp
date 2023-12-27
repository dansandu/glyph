#include "dansandu/glyph/internal/text_location.hpp"
#include "dansandu/ballotin/exception.hpp"

#include <algorithm>
#include <string>
#include <string_view>

namespace dansandu::glyph::internal::text_location
{

TextLocation getTextLocation(const std::string_view text, const int tokenBegin, const int tokenEnd)
{
    const auto textSize = static_cast<int>(text.size());

    if (tokenBegin < 0 || tokenBegin > textSize || tokenEnd < 0 || tokenEnd > textSize || tokenBegin > tokenEnd)
    {
        THROW(std::logic_error, "invalid token segment [", tokenBegin, ", ", tokenEnd, ") in text of size ", textSize);
    }

    if (textSize == 0)
    {
        return TextLocation{0, 0, "(empty text)"};
    }

    auto textLocation = TextLocation{};

    textLocation.lineNumber = 1 + std::count(text.begin(), text.begin() + tokenBegin, '\n');

    auto lineBegin = text.rfind('\n', tokenBegin);
    if (lineBegin == std::string_view::npos)
    {
        lineBegin = 0;
    }
    else if (tokenBegin < textSize)
    {
        ++lineBegin;
    }

    auto lineEnd = text.find('\n', tokenEnd);
    if (lineEnd == std::string_view::npos)
    {
        lineEnd = textSize;
    }

    textLocation.columnNumber = tokenBegin - lineBegin + 1;

    auto underscore = tokenBegin < tokenEnd ? std::string(tokenEnd - tokenBegin - 1, '~') : std::string();
    if (tokenBegin == textSize)
    {
        underscore += " (end of text)";
    }

    textLocation.highlight = std::string(text.begin() + lineBegin, text.begin() + lineEnd) + "\n" +
                             std::string(tokenBegin - lineBegin, ' ') + "^" + underscore;

    return textLocation;
}

}
