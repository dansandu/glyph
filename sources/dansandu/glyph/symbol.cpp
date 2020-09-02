#include "dansandu/glyph/symbol.hpp"

namespace dansandu::glyph::symbol
{

std::ostream& operator<<(std::ostream& stream, const Symbol symbol)
{
    return stream << "Symbol(" << symbol.getIdentifierIndex() << ")";
}

}
