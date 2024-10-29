#include "dansandu/glyph/token.hpp"

#include <sstream>

namespace dansandu::glyph::token
{

std::ostream& operator<<(std::ostream& stream, const Token& token)
{
    return stream << "Token(" << token.getSymbol() << ", " << token.begin() << ", " << token.end() << ")";
}

std::string Token::toString() const
{
    auto stream = std::ostringstream{};

    stream << *this;

    return stream.str();
}

}
