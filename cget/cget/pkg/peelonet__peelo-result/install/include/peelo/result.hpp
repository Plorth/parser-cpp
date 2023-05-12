/*
 * Copyright (c) 2020, peelo.net
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef PEELO_RESULT_HPP_GUARD
#define PEELO_RESULT_HPP_GUARD

#include <memory>

namespace peelo
{
  template<class T, class E>
  class result
  {
  public:
    using value_type = T;
    using error_type = E;

    enum class type
    {
      ok = 1,
      error = 0
    };

    static inline result<T, E> ok(const T& value)
    {
      return result(type::ok, new value_type(value), nullptr);
    }

    static inline result<T, E> error(const E& error)
    {
      return result(type::error, nullptr, new error_type(error));
    }

    result(const result& that)
      : m_type(that.m_type)
      , m_value(that.m_value ? new value_type(*that.m_value) : nullptr)
      , m_error(that.m_error ? new error_type(*that.m_error) : nullptr) {}

    template<class T2, class E2>
    result(const result<T2, E2>& that)
      : m_type(!!that ? type::ok : type::error)
      , m_value(that.value() ? new value_type(*that.value()) : nullptr)
      , m_error(that.error() ? new error_type(*that.error()) : nullptr) {}

    result& operator=(const result& that)
    {
      if (this != &that)
      {
        m_type = that.m_type;
        m_value.reset(that.m_value ? new value_type(*that.m_value) : nullptr);
        m_error.reset(that.m_error ? new error_type(*that.m_error) : nullptr);
      }

      return *this;
    }

    template<class T2, class E2>
    result& operator=(const result<T2, E2>& that)
    {
      m_type = !!that ? type::ok : type::error;
      m_value.reset(that.value() ? new value_type(*that.value()) : nullptr);
      m_error.reset(that.error() ? new error_type(*that.error()) : nullptr);

      return *this;
    }

    inline enum type type() const
    {
      return m_type;
    }

    inline const std::unique_ptr<value_type>& value() const
    {
      return m_value;
    }

    inline const std::unique_ptr<error_type>& error() const
    {
      return m_error;
    }

    inline explicit operator bool() const
    {
      return m_type == type::ok;
    }

    inline bool operator!() const
    {
      return m_type == type::error;
    }

    inline bool equals(const result& that) const
    {
      return m_type == that.m_type
        && m_value == that.m_value
        && m_error == that.m_error;
    }

    inline bool operator==(const result& that) const
    {
      return equals(that);
    }

    inline bool operator!=(const result& that) const
    {
      return !equals(that);
    }

  private:
    result(
      enum type type,
      value_type* value,
      error_type* error
    )
      : m_type(type)
      , m_value(value)
      , m_error(error) {}

  private:
    enum type m_type;
    std::unique_ptr<value_type> m_value;
    std::unique_ptr<error_type> m_error;
  };
}

#endif /* !PEELO_RESULT_HPP_GUARD */
