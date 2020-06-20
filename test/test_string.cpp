#include <cassert>
#include <cstdlib>

#include <plorth/parser.hpp>

using plorth::parser::parse_string;

static auto parse(const std::u32string& source)
{
  auto begin = std::cbegin(source);
  const auto end = std::cend(source);
  plorth::parser::position position;

  return parse_string<std::u32string::const_iterator>(begin, end, position);
}

static void test_eof_before_the_string()
{
  const auto result = parse(U"");

  assert(!result);
}

static void test_no_string_delimiter_found()
{
  const auto result = parse(U"invalid");

  assert(!result);
}

static void test_unterminated_string()
{
  const auto result = parse(U"\"foo");

  assert(!result);
}

static void test_quotation_mark_separator()
{
  const auto result = parse(U"\"foo\"");

  assert(!!result);
  assert((*result.value())->value() == U"foo");
}

static void test_apostrophe_separator()
{
  const auto result = parse(U"'foo'");

  assert(!!result);
  assert((*result.value())->value() == U"foo");
}

static void test_escape_sequence()
{
  const auto result = parse(U"\"\\t\\u0023\"");

  assert(!!result);
  assert((*result.value())->value() == U"\t#");
}

int main(int argc, char** argv)
{
  test_eof_before_the_string();
  test_no_string_delimiter_found();
  test_unterminated_string();

  test_quotation_mark_separator();
  test_apostrophe_separator();
  test_escape_sequence();

  return EXIT_SUCCESS;
}
