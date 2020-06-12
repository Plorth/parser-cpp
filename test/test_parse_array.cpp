#include <cassert>
#include <cstdlib>

#include <plorth/parser/parser.hpp>

using plorth::parser::make_parser;

static void test_parse_empty_array()
{
  auto parser = make_parser(U"[]");
  const auto result = parser.parse_array();

  assert(bool(result));
  assert((*result.value())->type() == plorth::parser::ast::token::type::array);
  assert((*result.value())->elements().size() == 0);
}

// TODO: test parse non empty array
// TODO: test parse nested array

static void test_parse_array_eof()
{
  auto parser = make_parser(U"");
  const auto result = parser.parse_array();

  assert(!result);
}

static void test_parse_array_with_non_array()
{
  auto parser = make_parser(U"{\"foo\": \"bar\"}");
  const auto result = parser.parse_array();

  assert(!result);
}

// TODO: test parse unterminated array
// TODO: test parse unterminated array after one element

int main(int argc, char** argv)
{
  test_parse_empty_array();
  // TODO: test parse non empty array
  // TODO: test parse nested array
  test_parse_array_eof();
  test_parse_array_with_non_array();
  // TODO: test parse unterminated array
  // TODO: test parse unterminated array after one element

  return EXIT_SUCCESS;
}
