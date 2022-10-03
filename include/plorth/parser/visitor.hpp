/*
 * Copyright (c) 2022, Rauli Laine
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

#include <plorth/parser/ast.hpp>

namespace plorth::parser::ast
{
  /**
   * Implementation of visitor pattern for Plorth AST tokens.
   */
  template<class... Args>
  class visitor
  {
  public:
    virtual void visit_array(
      const std::shared_ptr<array>& token,
      Args... args
    ) const {};

    virtual void visit_object(
      const std::shared_ptr<object>& token,
      Args... args
    ) const {};

    virtual void visit_quote(
      const std::shared_ptr<quote>& token,
      Args... args
    ) const {};

    virtual void visit_string(
      const std::shared_ptr<string>& token,
      Args... args
    ) const {};

    virtual void visit_symbol(
      const std::shared_ptr<symbol>& token,
      Args... args
    ) const {};

    virtual void visit_word(
      const std::shared_ptr<word>& token,
      Args... args
    ) const {};

    void visit(const std::shared_ptr<token>& token, Args... args) const
    {
      if (!token)
      {
        return;
      }

      switch (token->type())
      {
        case token::type::array:
          visit_array(std::static_pointer_cast<array>(token), args...);
          break;

        case token::type::object:
          visit_object(std::static_pointer_cast<object>(token), args...);
          break;

        case token::type::quote:
          visit_quote(std::static_pointer_cast<quote>(token), args...);
          break;

        case token::type::string:
          visit_string(std::static_pointer_cast<string>(token), args...);
          break;

        case token::type::symbol:
          visit_symbol(std::static_pointer_cast<symbol>(token), args...);
          break;

        case token::type::word:
          visit_word(std::static_pointer_cast<word>(token), args...);
          break;
      }
    }
  };
}
