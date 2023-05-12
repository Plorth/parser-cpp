#include <cassert>
#include <cstdlib>

#include <plorth/parser/utils.hpp>

using plorth::parser::position;

static void test_advance()
{
  using plorth::parser::utils::advance;
  const std::u32string source = U"f\n";
  auto current = std::begin(source);
  struct position position = { U"<test>", 1, 1 };

  assert(advance(current, position) == U'f');
  assert(position.line == 1);
  assert(position.column == 2);
  assert(advance(current, position) == U'\n');
  assert(position.line == 2);
  assert(position.column == 1);
}

static void test_peek()
{
  using plorth::parser::utils::peek;
  const std::u32string source1 = U"a";
  const std::u32string source2 = U"";

  assert(peek(std::begin(source1), std::end(source1), U'a'));
  assert(!peek(std::begin(source1), std::end(source1), U'b'));
  assert(!peek(std::begin(source2), std::end(source2), U'a'));
  assert(!peek(std::begin(source2), std::end(source2), U'b'));
}

static void test_peek_advance()
{
  using plorth::parser::utils::peek_advance;
  const std::u32string source = U"a";
  auto current = std::begin(source);
  const auto end = std::end(source);
  struct position position = { U"<test>", 1, 1 };

  assert(!peek_advance(current, end, position, U'b'));
  assert(position.line == 1);
  assert(position.column == 1);

  assert(peek_advance(current, end, position, U'a'));
  assert(position.line == 1);
  assert(position.column == 2);

  assert(!peek_advance(current, end, position, U'c'));
  assert(position.line == 1);
  assert(position.column == 2);
}

static inline bool skip_whitespace(const std::u32string& input)
{
  struct position position = { U"<test>", 1, 1 };
  auto current = std::begin(input);
  const auto end = std::end(input);

  return plorth::parser::utils::skip_whitespace(current, end, position);
}

static void test_skip_whitespace()
{
  assert(skip_whitespace(U""));
  assert(skip_whitespace(U" \t\r\n"));
  assert(skip_whitespace(U"# foo"));
  assert(!skip_whitespace(U"# foo\nbar"));
  assert(!skip_whitespace(U"foo # bar"));
}

static void test_isword()
{
  using plorth::parser::utils::isword;

  assert(!isword(U'('));
  assert(!isword(U')'));
  assert(!isword(U'['));
  assert(!isword(U']'));
  assert(!isword(U'{'));
  assert(!isword(U'}'));
  assert(!isword(U','));
  assert(!isword(U' '));

  assert(isword(U'a'));
  assert(isword(U'-'));
  assert(isword(U'\u00e4'));
  assert(isword(U':'));
  assert(isword(U';'));
}

int main(int argc, char** argv)
{
  test_advance();
  test_peek();
  test_peek_advance();
  test_skip_whitespace();
  test_isword();

  return EXIT_SUCCESS;
}
