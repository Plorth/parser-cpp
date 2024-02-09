#include <cassert>

#include <plorth/parser.hpp>

using plorth::parser::parse_quote;

static auto
parse(const std::u32string& source)
{
  auto begin = std::cbegin(source);
  const auto end = std::cend(source);
  plorth::parser::position position;

  return parse_quote<std::u32string::const_iterator>(begin, end, position);
}

static void
test_eof_before_the_quote()
{
  const auto result = parse(U"");

  assert(!result);
}

static void
test_no_parenthesis_found()
{
  const auto result = parse(U"invalid");

  assert(!result);
}

static void
test_unterminated_quote()
{
  const auto result = parse(U"(");

  assert(!result);
}

static void
test_unterminated_quote_after_child()
{
  const auto result = parse(U"(foo");

  assert(!result);
}

static void
test_empty_quote()
{
  const auto result = parse(U"()");

  assert(result.has_value());
  assert((*result)->children().size() == 0);
}

static void
test_quote_with_one_child()
{
  const auto result = parse(U"( foo )");

  assert(result.has_value());
  assert((*result)->children().size() == 1);
}

static void
test_quote_with_multiple_children()
{
  const auto result = parse(U"( foo \"bar\" [] )");

  assert(result.has_value());
  assert((*result)->children().size() == 3);
}

int
main()
{
  test_eof_before_the_quote();
  test_no_parenthesis_found();
  test_unterminated_quote();
  test_unterminated_quote_after_child();

  test_empty_quote();
  test_quote_with_one_child();
  test_quote_with_multiple_children();
}
