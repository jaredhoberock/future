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

