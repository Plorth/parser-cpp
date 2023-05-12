#include <cassert>
#include <cstdlib>

#include <plorth/parser.hpp>

using plorth::parser::parse_symbol_or_word;
using plorth::parser::ast::token;

static auto parse(const std::u32string& source)
{
  auto begin = std::cbegin(source);
  const auto end = std::cend(source);
  plorth::parser::position position;

  return parse_symbol_or_word<std::u32string::const_iterator>(
    begin,
    end,
    position
  );
}

static void test_eof_before_the_word()
{
  const auto result = parse(U"");

  assert(!result);
}

static void test_no_arrow_found()
{
  const auto result = parse(U"invalid");

  assert(result);
  assert((*result.value())->type() == token::type::symbol);
}

static void test_no_symbol_found()
{
  const auto result = parse(U"->");

  assert(!result);
}

static void test_parse()
{
  const auto result = parse(U"-> foo");

  assert(!!result);
  assert(
    std::static_pointer_cast<plorth::parser::ast::word>(
      (*result.value())
    )->symbol()->id() == U"foo"
  );
}

int main(int argc, char** argv)
{
  test_eof_before_the_word();
  test_no_arrow_found();
  test_no_symbol_found();
  test_parse();

  return EXIT_SUCCESS;
}
