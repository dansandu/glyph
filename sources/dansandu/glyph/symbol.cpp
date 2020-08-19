#include "dansandu/glyph/symbol.hpp"

namespace dansandu::glyph::symbol
{

std::ostream& operator<<(std::ostream& stream, Symbol symbol)
{
    return stream << "Symbol(" << symbol.getIdentifierIndex() << ")";
}

}
