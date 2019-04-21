#pragma once

#include <map>
#include <string>
#include <string_view>

namespace dansandu::glyph::arithmetic_formula_parser {

double evaluate(std::string_view formula, const std::map<std::string, double>& values);

}
