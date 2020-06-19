#include <cassert>
#include <cstdlib>

#include <plorth/parser.hpp>

using plorth::parser::parse_array;

static auto parse(const std::u32string& source)
{
  auto begin = std::cbegin(source);
  const auto end = std::cend(source);
  plorth::parser::position position;

  return parse_array<std::u32string::const_iterator>(begin, end, position);
}

static void test_eof_before_the_array()
{
  const auto result = parse(U"");

  assert(!result);
}

static void test_no_bracket_found()
{
  const auto result = parse(U"invalid");

  assert(!result);
}

static void test_unterminated_array()
{
  const auto result = parse(U"[");

  assert(!result);
}

static void test_unterminated_array_after_element()
{
  const auto result = parse(U"[\"foo\"");

  assert(!result);
}

static void test_unterminated_array_after_comma()
{
  const auto result = parse(U"[\"foo\",");

  assert(!result);
}

static void test_elements_without_comma()
{
  const auto result = parse(U"[\"foo\" \"bar\"");

  assert(!result);
}

static void test_empty_array()
{
  const auto result = parse(U"[]");

  assert(!!result);
  assert((*result.value())->elements().size() == 0);
}

static void test_array_with_one_element()
{
  const auto result = parse(U"[\"foo\"]");

  assert(!!result);
  assert((*result.value())->elements().size() == 1);
}

static void test_array_with_multiple_elements()
{
  const auto result = parse(U"[\"foo\", \"bar\", \"baz\"]");

  assert(!!result);
  assert((*result.value())->elements().size() == 3);
}

static void test_array_with_multiple_elements_with_dangling_comma()
{
  const auto result = parse(U"[\"foo\", \"bar\", \"baz\",]");

  assert(!!result);
  assert((*result.value())->elements().size() == 3);
}

int main(int argc, char** argv)
{
  test_eof_before_the_array();
  test_no_bracket_found();
  test_unterminated_array();
  test_unterminated_array_after_element();
  test_unterminated_array_after_comma();
  test_elements_without_comma();

  test_empty_array();
  test_array_with_one_element();
  test_array_with_multiple_elements();
  test_array_with_multiple_elements_with_dangling_comma();

  return EXIT_SUCCESS;
}
