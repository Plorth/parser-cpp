#include <cassert>
#include <cstdlib>

#include <plorth/parser.hpp>

static auto parse(const std::u32string& source)
{
  auto begin = std::cbegin(source);
  const auto end = std::cend(source);
  plorth::parser::position position;

  return plorth::parser::parse<std::u32string::const_iterator>(
    begin,
    end,
    position
  );
}

static void test_parse_error()
{
  assert(!parse(U"\"foo"));
  assert(!parse(U"[foo"));
  assert(!parse(U"{"));
  assert(!parse(U"(foo"));
}

static void test_successful_parse()
{
  assert(!!parse(U"\"foo\""));
  assert(!!parse(U"[foo]"));
  assert(!!parse(U"{\"foo\": \"bar\"}"));
  assert(!!parse(U"(foo)"));
  assert(!!parse(U"foo \"bar\" [baz]"));
}

int main()
{
  test_parse_error();
  test_successful_parse();

  return EXIT_SUCCESS;
}
