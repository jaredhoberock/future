#pragma once

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


template<class Function>
class scope_exit
{
  public:
    template<class F>
    explicit scope_exit(F&& f) noexcept
      : exit_function_(std::forward<F>(f))
    {}

    ~scope_exit()
    {
      exit_function_();
    }

  private:
    Function exit_function_;
};


template<class F>
scope_exit<typename std::decay<F>::type> make_scope_exit(F&& exit_function)
{
  return scope_exit<typename std::decay<F>::type>(std::forward<F>(exit_function));
}


} // end detail
} // end v1
} // end experimental
} // end std

