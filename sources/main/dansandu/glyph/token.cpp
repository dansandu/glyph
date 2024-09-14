#include "dansandu/glyph/token.hpp"

#include <ostream>

namespace dansandu::glyph::token
{

std::ostream& operator<<(std::ostream& stream, const Token& token)
{
    return stream << "Token(" << token.getSymbol() << ", " << token.begin() << ", " << token.end() << ")";
}

}
