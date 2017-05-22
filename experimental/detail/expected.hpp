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

