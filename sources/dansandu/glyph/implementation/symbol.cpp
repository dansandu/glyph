#include "dansandu/glyph/implementation/symbol.hpp"

namespace dansandu::glyph::implementation::symbol
{

std::ostream& operator<<(std::ostream& stream, Symbol symbol)
{
    return stream << "Symbol(" << symbol.getIdentifierIndex() << ")";
}

}
