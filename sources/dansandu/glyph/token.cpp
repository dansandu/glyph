#include "dansandu/glyph/token.hpp"

#include <ostream>
#include <string>

namespace dansandu::glyph::token {

bool operator==(const Token& left, const Token& right) {
    return left.getIdentifier() == right.getIdentifier() && left.begin() == right.begin() && left.end() == right.end();
}

bool operator!=(const Token& left, const Token& right) { return !(left == right); }

std::ostream& operator<<(std::ostream& stream, const Token& token) {
    return stream << "Token(" << token.getIdentifier() << ", " << token.begin() << ", " << token.end() << ")";
}

}
