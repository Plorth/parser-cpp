#include <cassert>

#include <plorth/parser.hpp>

int
main()
{
  const std::u32string source(U"'Hello, World!' println");
  auto begin = std::cbegin(source);
  const auto end = std::cend(source);
  plorth::parser::position position;
  const auto result = plorth::parser::parse(begin, end, position);

  assert(!!result);
  assert(result->size() == 2);
  assert(result->at(0)->type() == plorth::parser::ast::token::type::string);
  assert(result->at(1)->type() == plorth::parser::ast::token::type::symbol);
}
