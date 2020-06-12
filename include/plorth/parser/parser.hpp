/*
 * Copyright (c) 2020, Rauli Laine
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once

#include <cctype>
#include <optional>

#include <peelo/result.hpp>
#include <peelo/unicode/ctype/isgraph.hpp>
#include <peelo/unicode/ctype/isvalid.hpp>
#include <plorth/parser/ast.hpp>

namespace plorth::parser
{
  /**
   * Structure for parsing error.
   */
  struct error
  {
    /** Position in source code where the error was encountered. */
    struct position position;
    /** Error message. */
    std::u32string message;
  };

  /**
   * Parser for the Plorth programming language.
   */
  template<class IteratorT>
  class parser
  {
  public:
    /**
     * Constructs new parser.
     *
     * \param begin  Beginning of the source code.
     * \param end    End of the source code.
     * \param file   Optional file name information from which the source code
     *               was read from.
     * \param line   Initial line number of the source code.
     * \param column Initial column number of the source code.
     */
    explicit parser(
      const IteratorT& begin,
      const IteratorT& end,
      const std::u32string& file = U"<eval>",
      int line = 1,
      int column = 1
    )
      : m_current(begin)
      , m_end(end)
      , m_position({ file, line, column }) {}

    /**
     * Returns current position in source code.
     */
    inline const struct position& position() const
    {
      return m_position;
    }

    /**
     * Attempts to parse an AST token from the source code.
     */
    peelo::result<std::shared_ptr<ast::token>, error> parse()
    {
      using result_type = peelo::result<std::shared_ptr<ast::token>, error>;

      if (skip_whitespace())
      {
        return result_type::error({
          m_position,
          U"Unexpected end of input; Missing value."
        });
      }

      switch (peek())
      {
        case '"':
        case '\'':
          return parse_string();

        case '(':
          return parse_quote();

        case '[':
          return parse_array();

        case '{':
          return parse_object();

        case ':':
          return parse_word();
      }

      return parse_symbol();
    }

    /**
     * Attempts to parse an array token from the source code.
     */
    peelo::result<std::shared_ptr<ast::array>, error> parse_array()
    {
      using result_type = peelo::result<std::shared_ptr<ast::array>, error>;
      struct position position;
      ast::array::container_type elements;

      if (skip_whitespace())
      {
        return result_type::error({
          m_position,
          U"Unexpected end of input; Missing array."
        });
      }

      position = m_position;

      if (!peek_read('['))
      {
        return result_type::error({
          position,
          U"Unexpected input; Missing array."
        });
      }

      for (;;)
      {
        if (skip_whitespace())
        {
          return result_type::error({
            position,
            U"Unterminated array; Missing `]'."
          });
        }
        else if (peek_read(']'))
        {
          break;
        } else {
          const auto result = parse();

          if (result)
          {
            elements.push_back(*result.value());
            if (skip_whitespace() || (!peek(',') && !peek(']')))
            {
              return result_type::error({
                position,
                U"Unterminated array; Missing `]'."
              });
            }
            peek_read(',');
          } else {
            return result_type::error(*result.error());
          }
        }
      }

      return result_type::ok(std::make_shared<ast::array>(position, elements));
    }

    /**
     * Attempts to parse object literal token from the source code.
     */
    peelo::result<std::shared_ptr<ast::object>, error> parse_object()
    {
      using result_type = peelo::result<std::shared_ptr<ast::object>, error>;
      struct position position;
      ast::object::container_type properties;

      if (skip_whitespace())
      {
        return result_type::error({
          m_position,
          U"Unexpected end of input; Missing object."
        });
      }

      position = m_position;

      if (!peek_read('{'))
      {
        return result_type::error({
          position,
          U"Unexpected input; Missing object."
        });
      }

      for (;;)
      {
        if (skip_whitespace())
        {
          return result_type::error({
            position,
            U"Unterminated object; Missing `}'."
          });
        }
        else if (peek_read('}'))
        {
          break;
        } else {
          const auto key_result = parse_string();

          if (!key_result)
          {
            return result_type::error(*key_result.error());
          }

          if (skip_whitespace())
          {
            return result_type::error({
              position,
              U"Unterminated object; Missing `}'."
            });
          }

          if (!peek_read(':'))
          {
            return result_type::error({
              m_position,
              U"Missing `:' after property key."
            });
          }

          const auto value_result = parse();

          if (value_result)
          {
            properties.push_back({
              (*key_result.value())->value(),
              *value_result.value()
            });
          } else {
            return result_type::error(*value_result.error());
          }

          if (skip_whitespace() || (!peek(',') && !peek('}')))
          {
            return result_type::error({
              m_position,
              U"Unterminated object; Missing `}'."
            });
          }
          peek_read(',');
        }
      }

      return result_type::ok(std::make_shared<ast::object>(
        position,
        properties
      ));
    }

    /**
     * Attempts to parse quote literal token from the source code.
     */
    peelo::result<std::shared_ptr<ast::quote>, error> parse_quote()
    {
      using result_type = peelo::result<std::shared_ptr<ast::quote>, error>;
      struct position position;
      ast::quote::container_type children;

      if (skip_whitespace())
      {
        return result_type::error({
          m_position,
          U"Unexpected end of input; Missing quote."
        });
      }

      position = m_position;

      if (!peek_read('('))
      {
        return result_type::error({
          m_position,
          U"Unexpected input; Missing quote."
        });
      }

      for (;;)
      {
        if (skip_whitespace())
        {
          return result_type::error({
            position,
            U"Unterminated quote; Missing `)'."
          });
        }
        else if (peek_read(')'))
        {
          break;
        } else {
          const auto result = parse();

          if (result)
          {
            children.push_back(*result.value());
          } else {
            return result_type::error(*result.error());
          }
        }
      }

      return result_type::ok(std::make_shared<ast::quote>(position, children));
    }

    /**
     * Attempts to parse string literal token from the source code.
     */
    peelo::result<std::shared_ptr<ast::string>, error> parse_string()
    {
      using result_type = peelo::result<std::shared_ptr<ast::string>, error>;
      struct position position;
      char32_t separator;
      std::u32string buffer;

      if (skip_whitespace())
      {
        return result_type::error({
          m_position,
          U"Unexpected end of input; Missing string."
        });
      }

      position = m_position;

      if (peek_read('"'))
      {
        separator = '"';
      }
      else if (peek_read('\''))
      {
        separator = '\'';
      } else {
        return result_type::error({
          m_position,
          U"Unexpected input; Missing string."
        });
      }

      for (;;)
      {
        if (eof())
        {
          return result_type::error({
            position,
            std::u32string(U"Unterminated string; Missing `")
              + separator
              + U"'."
          });
        }
        else if (peek_read(separator))
        {
          break;
        }
        else if (peek_read('\\'))
        {
          const auto result = parse_escape_sequence(buffer);

          if (result)
          {
            return result_type::error(*result);
          }
        } else {
          buffer.append(1, read());
        }
      }

      return result_type::ok(std::make_shared<ast::string>(position, buffer));
    }

    /**
     * Attempts to parse symbol from the source code.
     */
    peelo::result<std::shared_ptr<ast::symbol>, error> parse_symbol()
    {
      using result_type = peelo::result<std::shared_ptr<ast::symbol>, error>;
      struct position position;
      std::u32string buffer;

      if (skip_whitespace())
      {
        return result_type::error({
          m_position,
          U"Unexpected end of input; Missing symbol."
        });
      }

      position = m_position;

      if (!isword(peek()))
      {
        return result_type::error({
          m_position,
          U"Unexpected input; Missing symbol."
        });
      }

      do
      {
        buffer.append(1, read());
      }
      while (!eof() && isword(peek()));

      return result_type::ok(std::make_shared<ast::symbol>(position, buffer));
    }

    /**
     * Attempts to parse word definition from source code.
     */
    peelo::result<std::shared_ptr<ast::word>, error> parse_word()
    {
      using result_type = peelo::result<std::shared_ptr<ast::word>, error>;
      struct position position;
      ast::quote::container_type children;

      if (skip_whitespace())
      {
        return result_type::error({
          m_position,
          U"Unexpected end of input; Missing word."
        });
      }

      position = m_position;

      if (!peek_read(':'))
      {
        return result_type::error({
          position,
          U"Unexpected input; Missing word."
        });
      }

      const auto symbol = parse_symbol();

      if (!symbol)
      {
        return result_type::error(*symbol.error());
      }

      for (;;)
      {
        if (skip_whitespace())
        {
          return result_type::error({
            position,
            U"Unterminated word; Missing `;'."
          });
        }
        else if (peek_read(';'))
        {
          break;
        } else {
          const auto child = parse();

          if (child)
          {
            children.push_back(*child.value());
          } else {
            return result_type::error(*child.error());
          }
        }
      }

      return result_type::ok(std::make_shared<ast::word>(
        position,
        *symbol.value(),
        std::make_shared<ast::quote>(position, children)
      ));
    }

    /**
     * Tests whether given Unicode character is accepted as part of symbol or
     * not.
     */
    static bool isword(char32_t c)
    {
      return c != '(' && c != ')' && c != '[' && c != ']' && c != '{'
        && c != '}' && c != ':' && c != ';' && c != ','
        && peelo::unicode::ctype::isgraph(c);
    }

  private:
    /**
     * Returns true if there are no more characters to be read from the source
     * code.
     */
    inline bool eof() const
    {
      return m_current >= m_end;
    }

    /**
     * Advances to next character to be read from the source code and returns
     * the current one.
     */
    inline char32_t read()
    {
      const auto c = *m_current++;

      if (c == '\n')
      {
        ++m_position.line;
        m_position.column = 1;
      } else {
        ++m_position.column;
      }

      return c;
    }

    /**
     * Returns the next character to be read from the source code without
     * advancing any further.
     */
    inline char32_t peek() const
    {
      return *m_current;
    }

    /**
     * Returns true if the next character to be read from the source code
     * matches with the one given as argument.
     */
    inline bool peek(char32_t expected) const
    {
      return !eof() && peek() == expected;
    }

    /**
     * Returns true and advances to the next character in the source code if
     * next character to be read from the source code matches with the one
     * given as argument.
     */
    inline bool peek_read(char32_t expected)
    {
      if (peek(expected))
      {
        read();

        return true;
      }

      return false;
    }

    /**
     * Skips whitespace and comments from the source code.
     *
     * \return True if end of input has been reached, false otherwise.
     */
    bool skip_whitespace()
    {
      while (!eof())
      {
        // Skip line comments.
        if (peek_read('#'))
        {
          while (!eof())
          {
            if (peek_read('\n') || peek_read('\r'))
            {
              break;
            } else {
              read();
            }
          }
        }
        else if (!std::isspace(peek()))
        {
          return false;
        } else {
          read();
        }
      }

      return true;
    }

    /**
     * Attempts to parse an escape sequence found inside string literal.
     */
    std::optional<error> parse_escape_sequence(std::u32string& buffer)
    {
      if (eof())
      {
        return std::make_optional<error>({
          m_position,
          U"Unexpected end of input; Missing escape sequence."
        });
      }

      switch (read())
      {
        case 'b':
          buffer.append(1, 010);
          break;

        case 't':
          buffer.append(1, 011);
          break;

        case 'n':
          buffer.append(1, 012);
          break;

        case 'f':
          buffer.append(1, 014);
          break;

        case 'r':
          buffer.append(1, 015);
          break;

        case '"':
        case '\'':
        case '\\':
        case '/':
          buffer.append(1, *(m_current - 1));
          break;

        case 'u':
          {
            char32_t result = 0;

            for (int i = 0; i < 4; ++i)
            {
              if (eof())
              {
                return std::make_optional<error>({
                  m_position,
                  U"Unterminated escape sequence."
                });
              }
              else if (!std::isxdigit(peek()))
              {
                return std::make_optional<error>({
                  m_position,
                  U"Illegal Unicode hex escape sequence."
                });
              }

              if (peek() >= 'A' && peek() <= 'F')
              {
                result = result * 16 + (read() - 'A' + 10);
              }
              else if (peek() >= 'a' && peek() <= 'f')
              {
                result = result * 16 + (read() - 'a' + 10);
              } else {
                result = result * 16 + (read() - '0');
              }
            }

            if (!peelo::unicode::ctype::isvalid(result))
            {
              return std::make_optional<error>({
                m_position,
                U"Illegal Unicode hex escape sequence."
              });
            }

            buffer.append(1, result);
          }
          break;

        default:
          return std::make_optional<error>({
            m_position,
            U"Illegal escape sequence in string literal."
          });
      }

      return std::nullopt;
    }

    parser(const parser&) = delete;
    parser(parser&&) = delete;
    void operator=(const parser&) = delete;
    void operator=(parser&&) = delete;

  private:
    /** Current position in source code. */
    IteratorT m_current;
    /** End of the source code. */
    const IteratorT m_end;
    /** Line and column number tracking. */
    struct position m_position;
  };

  /**
   * Constructs an parser that parses given string.
   */
  inline parser<std::u32string::const_iterator> make_parser(
    const std::u32string& source,
    const std::u32string& file = U"<eval>",
    int line = 1,
    int column = 1
  )
  {
    return parser(std::begin(source), std::end(source), file, line, column);
  }
}
