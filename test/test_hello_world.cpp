#include <cassert>
#include <cstdlib>

#include <plorth/parser.hpp>

int main(int argc, char** argv)
{
  const std::u32string source(U"'Hello, World!' println");
  auto begin = std::cbegin(source);
  const auto end = std::cend(source);
  plorth::parser::position position;
  const auto result = plorth::parser::parse(begin, end, position);

  assert(!!result);
  assert(result.value().size() == 2);
  assert(result.value()[0]->type() == plorth::parser::ast::token::type::string);
  assert(result.value()[1]->type() == plorth::parser::ast::token::type::symbol);

  return EXIT_SUCCESS;
}
