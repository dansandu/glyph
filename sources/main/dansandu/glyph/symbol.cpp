#include "dansandu/glyph/symbol.hpp"

#include <sstream>

namespace dansandu::glyph::symbol
{

std::ostream& operator<<(std::ostream& stream, const Symbol symbol)
{
    return stream << "Symbol(" << symbol.getIdentifierIndex() << ")";
}

std::string Symbol::toString() const
{
    auto stream = std::ostringstream{};

    stream << *this;

    return stream.str();
}

}
