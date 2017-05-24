// Copyright (c) 2017, NVIDIA CORPORATION. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#define __EXPECTED_REQUIRES(...) typename std::enable_if<(__VA_ARGS__)>::type* = nullptr

#include <experimental/variant>
#include <type_traits>
#include <stdexcept>

namespace std
{
namespace experimental
{
inline namespace v1
{
namespace detail
{


template<class T>
class expected
  : private std::experimental::variant<
      typename std::conditional<std::is_void<T>::value, std::experimental::monostate, T>::type,
      std::exception_ptr
    >
{
  private:
    using super_t = std::experimental::variant<
      typename std::conditional<std::is_void<T>::value, std::experimental::monostate, T>::type,
      std::exception_ptr
    >;

  public:
    template<class Deduced = T, __FUTURE_REQUIRES(!std::is_void<Deduced>::value)>
    explicit expected(const Deduced& value)
      : super_t(value)
    {}

    template<class Deduced = T, __FUTURE_REQUIRES(!std::is_void<Deduced>::value)>
    explicit expected(Deduced&& value)
      : super_t(std::move(value))
    {}

    template<class Deduced = T, __FUTURE_REQUIRES(std::is_void<Deduced>::value)>
    expected()
      : super_t(std::experimental::monostate())
    {}

    explicit expected(std::exception_ptr e)
      : super_t(e)
    {}

  private:
    struct value_or_throw_visitor
    {
      template<class Deduced = T,
               __EXPECTED_REQUIRES(std::is_void<Deduced>::value)
              >
      void operator()(monostate) const
      {
      }

      template<class Deduced = T,
               __EXPECTED_REQUIRES(!std::is_void<Deduced>::value)
              >
      T operator()(Deduced& value) const
      {
        return std::move(value);
      }

      T operator()(exception_ptr e) const
      {
        std::rethrow_exception(e);
      }
    };

  public:
    T value_or_throw()
    {
      return std::experimental::visit(value_or_throw_visitor(), static_cast<super_t&>(*this));
    }
};


} // end detail
} // end v1
} // end experimental
} // end std

#undef __EXPECTED_REQUIRES

