#include <cassert>

#include <plorth/parser.hpp>

using plorth::parser::parse_symbol;

static auto
parse(const std::u32string& source)
{
  auto begin = std::cbegin(source);
  const auto end = std::cend(source);
  plorth::parser::position position;

  return parse_symbol<std::u32string::const_iterator>(begin, end, position);
}

static void
test_eof_before_the_symbol()
{
  const auto result = parse(U"");

  assert(!result);
}

static void
test_non_symbol()
{
  const auto result = parse(U"[foo]");

  assert(!result);
}

static void
test_symbol()
{
  const auto result = parse(U"foo-bar-baz");

  assert(result.has_value());
  assert((*result)->id() == U"foo-bar-baz");
}

int
main()
{
  test_eof_before_the_symbol();
  test_non_symbol();
  test_symbol();
}
