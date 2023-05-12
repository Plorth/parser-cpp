#include <cassert>
#include <cstdlib>

#include <plorth/parser/ast.hpp>
#include <plorth/parser/visitor.hpp>

using plorth::parser::ast::array;
using plorth::parser::ast::object;
using plorth::parser::ast::quote;
using plorth::parser::ast::string;
using plorth::parser::ast::symbol;
using plorth::parser::ast::token;
using plorth::parser::ast::visitor;
using plorth::parser::ast::word;

class test_visitor : public visitor<bool&>
{
public:
  void visit_array(const std::shared_ptr<array>&, bool& flag) const
  {
    flag = true;
  }

  void visit_object(const std::shared_ptr<object>&, bool& flag) const
  {
    flag = true;
  }

  void visit_quote(const std::shared_ptr<quote>&, bool& flag) const
  {
    flag = true;
  }

  void visit_string(const std::shared_ptr<string>&, bool& flag) const
  {
    flag = true;
  }

  void visit_symbol(const std::shared_ptr<symbol>&, bool& flag) const
  {
    flag = true;
  }

  void visit_word(const std::shared_ptr<word>&, bool& flag) const
  {
    flag = true;
  }
};

static const plorth::parser::position position = { U"filename.plorth", 1, 1 };

static void test_visit_array()
{
  test_visitor visitor;
  const array::container_type elements = {};
  auto token = std::make_shared<array>(position, elements);
  bool flag = false;

  visitor.visit(token, flag);

  assert(flag);
}

static void test_visit_object()
{
  test_visitor visitor;
  const object::container_type properties = {};
  auto token = std::make_shared<object>(position, properties);
  bool flag = false;

  visitor.visit(token, flag);

  assert(flag);
}

static void test_visit_quote()
{
  test_visitor visitor;
  const quote::container_type children = {};
  auto token = std::make_shared<quote>(position, children);
  bool flag = false;

  visitor.visit(token, flag);

  assert(flag);
}

static void test_visit_string()
{
  test_visitor visitor;
  auto token = std::make_shared<string>(position, U"");
  bool flag = false;

  visitor.visit(token, flag);

  assert(flag);
}

static void test_visit_symbol()
{
  test_visitor visitor;
  auto token = std::make_shared<symbol>(position, U"");
  bool flag = false;

  visitor.visit(token, flag);

  assert(flag);
}

static void test_visit_word()
{
  test_visitor visitor;
  auto symbol = std::make_shared<class symbol>(position, U"");
  auto token = std::make_shared<word>(position, symbol);
  bool flag = false;

  visitor.visit(token, flag);

  assert(flag);
}

static void test_visit_token()
{
  class another_test_visitor : public visitor<bool&>
  {
  public:
    void visit_token(const std::shared_ptr<token>&, bool& flag) const
    {
      flag = true;
    }
  };
  another_test_visitor visitor;
  auto token = std::make_shared<class symbol>(position, U"test");
  bool flag = false;

  visitor.visit(token, flag);

  assert(flag);
}

int main(int argc, char** argv)
{
  test_visit_array();
  test_visit_object();
  test_visit_quote();
  test_visit_string();
  test_visit_symbol();
  test_visit_word();
  test_visit_token();

  return EXIT_SUCCESS;
}
