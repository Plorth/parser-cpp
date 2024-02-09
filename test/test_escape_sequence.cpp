#include <cassert>
#include <cstdlib>

#include <plorth/parser.hpp>

using plorth::parser::parse_escape_sequence;

static auto parse(const std::u32string& source)
{
  auto begin = std::cbegin(source);
  const auto end = std::cend(source);
  plorth::parser::position position;

  return parse_escape_sequence<std::u32string::const_iterator>(
    begin,
    end,
    position
  );
}

static void test_eof()
{
  assert(!parse(U""));
}

static void test_no_backslash()
{
  assert(!parse(U"foo"));
}

static void test_eof_after_backslash()
{
  assert(!parse(U"\\"));
}

static void test_unrecognized()
{
  assert(!parse(U"\\a"));
  assert(!parse(U"\\c"));
  assert(!parse(U"\\#"));
}

static void test_recognized()
{
  assert(parse(U"\\b").value() == 010);
  assert(parse(U"\\t").value() == 011);
  assert(parse(U"\\n").value() == 012);
  assert(parse(U"\\f").value() == 014);
  assert(parse(U"\\r").value() == 015);
  assert(parse(U"\\\"").value() == U'"');
  assert(parse(U"\\'").value() == U'\'');
  assert(parse(U"\\\\").value() == U'\\');
  assert(parse(U"\\/").value() == U'/');
}

static void test_hex_escape()
{
  assert(parse(U"\\u00e4").value() == 0x00e4);
  assert(parse(U"\\u0fe3").value() == 0x0fe3);
}

static void test_unterminated_hex_escape()
{
  assert(!parse(U"\\u"));
  assert(!parse(U"\\u0"));
  assert(!parse(U"\\u00"));
  assert(!parse(U"\\u000"));
}

static void test_non_hex_hex_escape()
{
  assert(!parse(U"\\uxe43"));
}

static void test_invalid_hex_escape()
{
  assert(!parse(U"\\ud805"));
}

int main(int argc, char** argv)
{
  test_eof();
  test_no_backslash();
  test_eof_after_backslash();
  test_unrecognized();
  test_recognized();
  test_hex_escape();
  test_unterminated_hex_escape();
  test_non_hex_hex_escape();
  test_invalid_hex_escape();

  return EXIT_SUCCESS;
}
