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

#include <functional>
#include <utility>
#include <type_traits>


namespace std
{
namespace experimental
{
inline namespace v1
{
namespace detail
{


// this type is copyable, but can store move-only functions
// it does so by making its copy constructor act like a move constructor
template<class Function>
class copyable_function
{
  public:
    copyable_function(Function&& f)
      : move_only_function_(std::move(f))
    {}

    copyable_function(copyable_function&& other)
      : move_only_function_(std::move(other.move_only_function_))
    {}

    copyable_function(const copyable_function& other)
      : move_only_function_(std::move(other.move_only_function_))
    {}

    copyable_function& operator=(const copyable_function& other)
    {
      move_only_function_ = std::move(other.move_only_function_);
      return *this;
    }

    copyable_function& operator=(copyable_function&& other)
    {
      move_only_function_ = std::move(other.move_only_function_);
      return *this;
    }

    template<class... Args>
    auto operator()(Args&&... args) const
    {
      return move_only_function_(std::forward<Args>(args)...);
    }

  private:
    // this is mutable so that we can move it in our
    // copy constructor and copy assign operator
    mutable Function move_only_function_;
};

template<class Function>
copyable_function<typename std::decay<Function>::type> make_copyable_function(Function&& f)
{
  return copyable_function<typename std::decay<Function>::type>(std::forward<Function>(f));
}


// make a hacky version of the proposed unique_function (https://wg21.link/P0288)
// by wrapping potentially move-only functions in copyable_function before passing
// them to std::function
template<class Signature>
class hacky_unique_function : public std::function<Signature>
{
  private:
    using super_t = std::function<Signature>;

  public:
    template<class Function>
    hacky_unique_function(Function&& f)
      : super_t(detail::make_copyable_function(std::forward<Function>(f)))
    {}

    hacky_unique_function() = default;

    hacky_unique_function(const hacky_unique_function&) = delete;

    hacky_unique_function(hacky_unique_function&& other)
      : super_t(static_cast<super_t&&>(std::move(other)))
    {}

    hacky_unique_function& operator=(hacky_unique_function&& other)
    {
      super_t::operator=(static_cast<super_t&&>(other));

      return *this;
    }
};


} // end detail
} // end v1
} // end experimental
} // end std

