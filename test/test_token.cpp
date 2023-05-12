#include <cassert>
#include <cstdlib>

#include <plorth/parser.hpp>

using plorth::parser::parse_token;
using plorth::parser::ast::token;

static auto parse(const std::u32string& source)
{
  auto begin = std::cbegin(source);
  const auto end = std::cend(source);
  plorth::parser::position position;

  return parse_token<std::u32string::const_iterator>(begin, end, position);
}

static void test_eof_before_the_token()
{
  const auto result = parse(U"");

  assert(!result);
}

static void test_array()
{
  const auto result = parse(U"[\"foo\", \"bar\", \"baz\"]");

  assert(!!result);
  assert((*result.value())->type() == token::type::array);
}

static void test_object()
{
  const auto result = parse(U"{\"foo\": \"bar\"}");

  assert(!!result);
  assert((*result.value())->type() == token::type::object);
}

static void test_quote()
{
  const auto result = parse(U"(foo bar)");

  assert(!!result);
  assert((*result.value())->type() == token::type::quote);
}

static void test_string_with_quotation_mark()
{
  const auto result = parse(U"\"foo\"");

  assert(!!result);
  assert((*result.value())->type() == token::type::string);
}

static void test_string_with_apostrophe()
{
  const auto result = parse(U"'foo'");

  assert(!!result);
  assert((*result.value())->type() == token::type::string);
}

static void test_word()
{
  const auto result = parse(U"-> foo");

  assert(!!result);
  assert((*result.value())->type() == token::type::word);
}

static void test_symbol()
{
  const auto result = parse(U"foo");

  assert(!!result);
  assert((*result.value())->type() == token::type::symbol);
}

int main(int argc, char** argv)
{
  test_eof_before_the_token();
  test_array();
  test_object();
  test_quote();
  test_string_with_quotation_mark();
  test_string_with_apostrophe();
  test_word();
  test_symbol();

  return EXIT_SUCCESS;
}
