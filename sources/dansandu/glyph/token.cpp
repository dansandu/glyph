#include "dansandu/glyph/token.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/glyph/error.hpp"

#include <ostream>

namespace dansandu::glyph::token
{

bool operator==(const Token& left, const Token& right)
{
    return left.getSymbol() == right.getSymbol() && left.begin() == right.begin() && left.end() == right.end();
}

bool operator!=(const Token& left, const Token& right)
{
    return !(left == right);
}

std::ostream& operator<<(std::ostream& stream, const Token& token)
{
    return stream << "Token(" << token.getSymbol() << ", " << token.begin() << ", " << token.end() << ")";
}

}
