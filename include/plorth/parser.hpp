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

#include <peelo/result.hpp>
#include <peelo/unicode/ctype/isvalid.hpp>
#include <peelo/unicode/ctype/isxdigit.hpp>
#include <plorth/parser/ast.hpp>
#include <plorth/parser/error.hpp>
#include <plorth/parser/utils.hpp>

namespace plorth::parser
{
  using parse_result = peelo::result<
    std::vector<std::shared_ptr<ast::token>>,
    error
  >;
  using parse_token_result = peelo::result<
    std::shared_ptr<ast::token>,
    error
  >;
  using parse_array_result = peelo::result<
    std::shared_ptr<ast::array>,
    error
  >;
  using parse_object_result = peelo::result<
    std::shared_ptr<ast::object>,
    error
  >;
  using parse_quote_result = peelo::result<
    std::shared_ptr<ast::quote>,
    error
  >;
  using parse_string_result = peelo::result<
    std::shared_ptr<ast::string>,
    error
  >;
  using parse_word_result = peelo::result<
    std::shared_ptr<ast::word>,
    error
  >;
  using parse_symbol_result = peelo::result<
    std::shared_ptr<ast::symbol>,
    error
  >;
  using parse_escape_sequence_result = peelo::result<
    char32_t,
    error
  >;

  /**
   * Attempts to parse an entire Plorth program and returns the AST tokens
   * encountered in the source code in an vector.
   *
   * \param current  Iterator pointing to current position in source code.
   * \param end      Iterator pointing to end of the source code.
   * \param position Current source code position.
   */
  template<class IteratorT>
  parse_result parse(
    IteratorT& current,
    const IteratorT& end,
    struct position& position
  )
  {
    std::vector<std::shared_ptr<ast::token>> tokens;

    while (current < end)
    {
      const auto token_result = parse_token(current, end, position);

      if (!token_result)
      {
        return parse_result::error(token_result.error());
      }
      tokens.push_back(token_result.value());
    }

    return parse_result::ok(tokens);
  }

  /**
   * Attempts to parse single AST token.
   *
   * \param current  Iterator pointing to current position in source code.
   * \param end      Iterator pointing to end of the source code.
   * \param position Current source code position.
   */
  template<class IteratorT>
  parse_token_result parse_token(
    IteratorT& current,
    const IteratorT& end,
    struct position& position
  )
  {
    if (utils::skip_whitespace(current, end, position))
    {
      return parse_token_result::error({
        position,
        U"Unexpected end of input; Missing value."
      });
    }

    switch (*current)
    {
      case '[':
        return parse_array(current, end, position);

      case '{':
        return parse_object(current, end, position);

      case '(':
        return parse_quote(current, end, position);

      case '"':
      case '\'':
        return parse_string(current, end, position);
    }

    return parse_symbol_or_word(current, end, position);
  }

  /**
   * Attempts to parse array literal AST token.
   *
   * \param current  Iterator pointing to current position in source code.
   * \param end      Iterator pointing to end of the source code.
   * \param position Current source code position.
   */
  template<class IteratorT>
  parse_array_result parse_array(
    IteratorT& current,
    const IteratorT& end,
    struct position& position
  )
  {
    struct position array_position = position;
    ast::array::container_type elements;

    if (utils::skip_whitespace(current, end, position))
    {
      return parse_array_result::error({
        array_position,
        U"Unexpected end of input; Missing array."
      });
    }

    if (!utils::peek_advance(current, end, position, U'['))
    {
      return parse_array_result::error({
        array_position,
        U"Unexpected input; Missing array."
      });
    }

    for (;;)
    {
      if (utils::skip_whitespace(current, end, position))
      {
        return parse_array_result::error({
          array_position,
          U"Unterminated array; Missing `]'."
        });
      }
      else if (utils::peek_advance(current, end, position, U']'))
      {
        break;
      } else {
        const auto value_result = parse_token(current, end, position);

        if (value_result)
        {
          elements.push_back(value_result.value());
          if (utils::skip_whitespace(current, end, position)
              || (!utils::peek(current, end, U',')
                && !utils::peek(current, end, U']')))
          {
            return parse_array_result::error({
              array_position,
              U"Unterminated array; Missing `]'."
            });
          }
          utils::peek_advance(current, end, position, U',');
        } else {
          return parse_array_result::error(value_result.error());
        }
      }
    }

    return parse_array_result::ok(
      std::make_shared<ast::array>(array_position, elements)
    );
  }

  /**
   * Attempts to parse object literal AST token.
   *
   * \param current  Iterator pointing to current position in source code.
   * \param end      Iterator pointing to end of the source code.
   * \param position Current source code position.
   */
  template<class IteratorT>
  parse_object_result parse_object(
    IteratorT& current,
    const IteratorT& end,
    struct position& position
  )
  {
    struct position object_position;
    ast::object::container_type properties;

    if (utils::skip_whitespace(current, end, position))
    {
      return parse_object_result::error({
        position,
        U"Unexpected end of input; Missing object."
      });
    }

    object_position = position;

    if (!utils::peek_advance(current, end, position, U'{'))
    {
      return parse_object_result::error({
        object_position,
        U"Unexpected input; Missing object."
      });
    }

    for (;;)
    {
      if (utils::skip_whitespace(current, end, position))
      {
        return parse_object_result::error({
          object_position,
          U"Unterminated object; Missing `}'."
        });
      }
      else if (utils::peek_advance(current, end, position, U'}'))
      {
        break;
      }

      const auto key_result = parse_string(current, end, position);

      if (!key_result)
      {
        return parse_object_result::error(key_result.error());
      }

      if (utils::skip_whitespace(current, end, position))
      {
        return parse_object_result::error({
          object_position,
          U"Unterminated object; Missing `:'."
        });
      }
      else if (!utils::peek_advance(current, end, position, U':'))
      {
        return parse_object_result::error({
          object_position,
          U"Missing `:' after property key."
        });
      }

      const auto value_result = parse_token(current, end, position);

      if (!value_result)
      {
        return parse_object_result::error(value_result.error());
      }

      properties.push_back(std::make_pair(
        key_result.value()->value(),
        value_result.value()
      ));

      if (utils::skip_whitespace(current, end, position)
          || (!utils::peek(current, end, U',')
            && !utils::peek(current, end, U'}')))
      {
        return parse_object_result::error({
          object_position,
          U"Unterminated object; Missing `}'."
        });
      }

      utils::peek_advance(current, end, position, U',');
    }

    return parse_object_result::ok(
      std::make_shared<ast::object>(object_position, properties)
    );
  }

  /**
   * Attempts to parse quote literal AST token.
   *
   * \param current  Iterator pointing to current position in source code.
   * \param end      Iterator pointing to end of the source code.
   * \param position Current source code position.
   */
  template<class IteratorT>
  parse_quote_result parse_quote(
    IteratorT& current,
    const IteratorT& end,
    struct position& position
  )
  {
    struct position quote_position;
    ast::quote::container_type children;

    if (utils::skip_whitespace(current, end, position))
    {
      return parse_quote_result::error({
        position,
        U"Unexpected end of input; Missing quote."
      });
    }

    quote_position = position;

    if (!utils::peek_advance(current, end, position, U'('))
    {
      return parse_quote_result::error({
        quote_position,
        U"Unexpected input; Missing quote."
      });
    }

    for (;;)
    {
      if (utils::skip_whitespace(current, end, position))
      {
        return parse_quote_result::error({
          quote_position,
          U"Unterminated quote; Missing `)'."
        });
      }
      else if (utils::peek_advance(current, end, position, U')'))
      {
        break;
      } else {
        const auto child_result = parse_token(current, end, position);

        if (child_result)
        {
          children.push_back(child_result.value());
        } else {
          return parse_quote_result::error(child_result.error());
        }
      }
    }

    return parse_quote_result::ok(
      std::make_shared<ast::quote>(quote_position, children)
    );
  }

  /**
   * Attempts to parse string literal escape sequence.
   *
   * \param current  Iterator pointing to current position in source code.
   * \param end      Iterator pointing to end of the source code.
   * \param position Current source code position.
   */
  template<class IteratorT>
  parse_escape_sequence_result parse_escape_sequence(
    IteratorT& current,
    const IteratorT& end,
    struct position& position
  )
  {
    char32_t result;

    if (current >= end)
    {
      return parse_escape_sequence_result::error({
        position,
        U"Unexpected end of input; Missing escape sequence."
      });
    }

    if (!utils::peek_advance(current, end, position, U'\\'))
    {
      return parse_escape_sequence_result::error({
        position,
        U"Unexpected input; Missing escape sequence."
      });
    }

    if (current >= end)
    {
      return parse_escape_sequence_result::error({
        position,
        U"Unexpected end of input; Missing escape sequence."
      });
    }

    switch (utils::advance(current, position))
    {
      case 'b':
        result = 010;
        break;

      case 't':
        result = 011;
        break;

      case 'n':
        result = 012;
        break;

      case 'f':
        result = 014;
        break;

      case 'r':
        result = 015;
        break;

      case '"':
      case '\'':
      case '\\':
      case '/':
        result = *(current - 1);
        break;

      case 'u':
        result = 0;
        for (int i = 0; i < 4; ++i)
        {
          if (current >= end)
          {
            return parse_escape_sequence_result::error({
              position,
              U"Unterminated escape sequence."
            });
          }
          else if (!peelo::unicode::ctype::isxdigit(*current))
          {
            return parse_escape_sequence_result::error({
              position,
              U"Illegal Unicode hex escape sequence."
            });
          }

          if (*current >= 'A' && *current <= 'F')
          {
            result = result * 16 + (*current - 'A' + 10);
          }
          else if (*current >= 'a' && *current <= 'f')
          {
            result = result * 16 + (*current - 'a' + 10);
          } else {
            result = result * 16 + (*current - '0');
          }

          utils::advance(current, position);
        }

        if (!peelo::unicode::ctype::isvalid(result))
        {
          return parse_escape_sequence_result::error({
            position,
            U"Illegal Unicode hex escape sequence."
          });
        }
        break;

      default:
        return parse_escape_sequence_result::error({
          position,
          U"Illegal escape sequence in string literal."
        });
    }

    return parse_escape_sequence_result::ok(result);
  }

  /**
   * Attempts to parse string literal AST token.
   *
   * \param current  Iterator pointing to current position in source code.
   * \param end      Iterator pointing to end of the source code.
   * \param position Current source code position.
   */
  template<class IteratorT>
  parse_string_result parse_string(
    IteratorT& current,
    const IteratorT& end,
    struct position& position
  )
  {
    struct position string_position;
    char32_t separator;
    std::u32string buffer;

    if (utils::skip_whitespace(current, end, position))
    {
      return parse_string_result::error({
        position,
        U"Unexpected end of input; Missing string."
      });
    }

    string_position = position;

    if (utils::peek_advance(current, end, position, U'"'))
    {
      separator = U'"';
    }
    else if (utils::peek_advance(current, end, position, U'\''))
    {
      separator = U'\'';
    } else {
      return parse_string_result::error({
        string_position,
        U"Unexpected input; Missing string."
      });
    }

    for (;;)
    {
      if (current >= end)
      {
        return parse_string_result::error({
          string_position,
          std::u32string(U"Unterminated string; Missing `")
          + separator
          + U"'."
        });
      }
      else if (utils::peek_advance(current, end, position, separator))
      {
        break;
      }
      else if (utils::peek(current, end, U'\\'))
      {
        const auto escape_sequence_result = parse_escape_sequence(
          current,
          end,
          position
        );

        if (!escape_sequence_result)
        {
          return parse_string_result::error(escape_sequence_result.error());
        }
        buffer.append(1, escape_sequence_result.value());
      } else {
        buffer.append(1, utils::advance(current, position));
      }
    }

    return parse_string_result::ok(
      std::make_shared<ast::string>(string_position, buffer)
    );
  }

  /**
   * Attempts to parse symbol AST token.
   *
   * \param current  Iterator pointing to current position in source code.
   * \param end      Iterator pointing to end of the source code.
   * \param position Current source code position.
   */
  template<class IteratorT>
  parse_symbol_result parse_symbol(
    IteratorT& current,
    const IteratorT& end,
    struct position& position
  )
  {
    struct position symbol_position;
    std::u32string buffer;

    if (utils::skip_whitespace(current, end, position))
    {
      return parse_symbol_result::error({
        position,
        U"Unexpected end of input; Missing symbol."
      });
    }

    symbol_position = position;

    if (!utils::isword(*current))
    {
      return parse_symbol_result::error({
        symbol_position,
        U"Unexpected input; Missing symbol."
      });
    }

    do
    {
      buffer.append(1, utils::advance(current, position));
    }
    while (current < end && utils::isword(*current));

    return parse_symbol_result::ok(
      std::make_shared<ast::symbol>(symbol_position, buffer)
    );
  }

  /**
   * Attempts to parse either symbol or word definition AST token.
   *
   * \param current  Iterator pointing to current position in source code.
   * \param end      Iterator pointing to end of the source code.
   * \param position Current source code position.
   */
  template<class IteratorT>
  parse_token_result parse_symbol_or_word(
    IteratorT& current,
    const IteratorT& end,
    struct position& position
  )
  {
    struct position symbol_or_word_position;
    std::u32string buffer;

    if (utils::skip_whitespace(current, end, position))
    {
      return parse_symbol_result::error({
        position,
        U"Unexpected end of input; Missing symbol or word definition."
      });
    }

    symbol_or_word_position = position;

    if (!utils::isword(*current))
    {
      return parse_symbol_result::error({
        symbol_or_word_position,
        U"Unexpected input; Missing symbol or word definition."
      });
    }

    do
    {
      buffer.append(1, utils::advance(current, position));
    }
    while (current < end && utils::isword(*current));

    if (!buffer.compare(U"->"))
    {
      const auto symbol_result = parse_symbol(
        current,
        end,
        symbol_or_word_position
      );

      if (!symbol_result)
      {
        return parse_token_result::error(symbol_result.error());
      }

      return parse_token_result::ok(
        std::make_shared<ast::word>(
          symbol_or_word_position,
          symbol_result.value()
        )
      );
    }

    return parse_token_result::ok(
      std::make_shared<ast::symbol>(symbol_or_word_position, buffer)
    );
  }
}
