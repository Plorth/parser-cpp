#include <cassert>

#include <plorth/parser.hpp>

using plorth::parser::parse_object;

static auto
parse(const std::u32string& source)
{
  auto begin = std::cbegin(source);
  const auto end = std::cend(source);
  plorth::parser::position position;

  return parse_object<std::u32string::const_iterator>(begin, end, position);
}

static void
test_eof_before_the_object()
{
  const auto result = parse(U"");

  assert(!result);
}

static void
test_no_brace_found()
{
  const auto result = parse(U"invalid");

  assert(!result);
}

static void
test_unterminated_object()
{
  const auto result = parse(U"{");

  assert(!result);
}

static void
test_unterminated_object_after_key()
{
  const auto result = parse(U"{\"foo\":");

  assert(!result);
}

static void
test_unterminated_object_after_property()
{
  const auto result = parse(U"{\"foo\": \"bar\"");

  assert(!result);
}

static void
test_unterminated_object_after_comma()
{
  const auto result = parse(U"{\"foo\": \"bar\",");

  assert(!result);
}

static void
test_properties_without_comma()
{
  const auto result = parse(U"{\"foo\": \"bar\" \"baz\": \"quux\"}");

  assert(!result);
}

static void
test_empty_object()
{
  const auto result = parse(U"{}");

  assert(result.has_value());
  assert((*result)->properties().size() == 0);
}

static void
test_object_with_one_property()
{
  const auto result = parse(U"{\"foo\": \"bar\"}");

  assert(result.has_value());
  assert((*result)->properties().size() == 1);
  assert((*result)->properties()[0].first == U"foo");
}

static void
test_object_with_multiple_properties()
{
  const auto result = parse(U"{\"foo\": \"bar\", \"baz\": \"quux\"}");

  assert(result.has_value());
  assert((*result)->properties().size() == 2);
  assert((*result)->properties()[0].first == U"foo");
  assert((*result)->properties()[1].first == U"baz");
}

static void
test_object_with_multiple_properties_with_dangling_comma()
{
  const auto result = parse(U"{\"foo\": \"bar\", \"baz\": \"quux\",}");

  assert(result.has_value());
  assert((*result)->properties().size() == 2);
}

int
main()
{
  test_eof_before_the_object();
  test_no_brace_found();
  test_unterminated_object();
  test_unterminated_object_after_key();
  test_unterminated_object_after_property();
  test_unterminated_object_after_comma();
  test_properties_without_comma();

  test_empty_object();
  test_object_with_one_property();
  test_object_with_multiple_properties();
  test_object_with_multiple_properties_with_dangling_comma();
}
