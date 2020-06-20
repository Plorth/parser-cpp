#include <cassert>
#include <cstdlib>

#include <plorth/parser.hpp>

using plorth::parser::parse_word;

static auto parse(const std::u32string& source)
{
  auto begin = std::cbegin(source);
  const auto end = std::cend(source);
  plorth::parser::position position;

  return parse_word<std::u32string::const_iterator>(begin, end, position);
}

static void test_eof_before_the_word()
{
  const auto result = parse(U"");

  assert(!result);
}

static void test_no_colon_found()
{
  const auto result = parse(U"invalid");

  assert(!result);
}

static void test_no_symbol_found()
{
  const auto result = parse(U": [ bar ;");

  assert(!result);
}

static void test_unterminated_word()
{
  const auto result = parse(U": foo bar baz");

  assert(!result);
}

static void test_parse()
{
  const auto result = parse(U": foo bar baz ;");

  assert(!!result);
  assert((*result.value())->symbol()->id() == U"foo");
  assert((*result.value())->quote()->children().size() == 2);
}

int main(int argc, char** argv)
{
  test_eof_before_the_word();
  test_no_colon_found();
  test_no_symbol_found();
  test_unterminated_word();
  test_parse();

  return EXIT_SUCCESS;
}
