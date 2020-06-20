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

#include <peelo/unicode/ctype/isgraph.hpp>
#include <plorth/parser/position.hpp>

namespace plorth::parser::utils
{
  /**
   * Advances to the next character in source code and returns the current one
   * while updating the position.
   */
  template<class IteratorT>
  inline char32_t advance(IteratorT& current, struct position& position)
  {
    const auto c = *current++;

    if (c == '\n')
    {
      ++position.line;
      position.column = 1;
    } else {
      ++position.column;
    }

    return c;
  }

  /**
   * Returns true if next character to be read from source code matches with
   * one given as argument.
   */
  template<class IteratorT>
  inline bool peek(
    const IteratorT& current,
    const IteratorT& end,
    char32_t expected
  )
  {
    return current < end && *current == expected;
  }

  /**
   * Returns true and advances to next character in the source code if current
   * character in the source code matches with one given as argument, while
   * updating the position.
   */
  template<class IteratorT>
  inline bool peek_advance(
    IteratorT& current,
    const IteratorT& end,
    struct position& position,
    char32_t expected
  )
  {
    if (peek(current, end, expected))
    {
      advance(current, position);

      return true;
    }

    return false;
  }

  /**
   * Skips whitespace and comments from the source code.
   *
   * \return True if end of input has been reached, false otherwise.
   */
  template<class IteratorT>
  bool skip_whitespace(
    IteratorT& current,
    const IteratorT& end,
    struct position& position
  )
  {
    while (current < end)
    {
      // Skip line comments.
      if (peek_advance(current, end, position, '#'))
      {
        while (current < end)
        {
          if (peek_advance(current, end, position, '\n')
              || peek_advance(current, end, position, '\r'))
          {
            break;
          } else {
            advance(current, position);
          }
        }
      }
      else if (current < end && !std::isspace(*current))
      {
        return false;
      } else {
        advance(current, position);
      }
    }

    return true;
  }

  static inline bool isword(char32_t c)
  {
    return c != '(' && c != ')' && c != '[' && c != ']' && c != '{'
      && c != '}' && c != ':' && c != ';' && c != ','
      && peelo::unicode::ctype::isgraph(c);
  }
}
