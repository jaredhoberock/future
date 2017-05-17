#include <experimental/execution>
#include <future>
#include <iostream>
#include <cassert>


struct has_then_execute_member
{
  const has_then_execute_member& context() const noexcept
  {
    return *this;
  }

  template<class Function>
  std::future<std::result_of_t<Function()>>
  then_execute(Function&& f, std::future<void>& predecessor_future) const
  {
    return std::async(std::launch::async, [func = std::forward<Function>(f), fut = std::move(predecessor_future)] () mutable
    {
      fut.get();
      return func();
    });
  }

  template<class Function, class T>
  std::future<std::result_of_t<Function(T&)>>
  then_execute(Function&& f, std::future<T>& predecessor_future) const
  {
    return std::async(std::launch::async, [func = std::forward<Function>(f), fut = std::move(predecessor_future)] () mutable
    {
      T predecessor = fut.get();
      return func(predecessor);
    });
  }

  bool operator==(const has_then_execute_member&) const noexcept
  {
    return true;
  }

  bool operator!=(const has_then_execute_member&) const noexcept
  {
    return false;
  }
};


struct has_then_execute_free_function
{
  const has_then_execute_free_function& context() const noexcept
  {
    return *this;
  }

  bool operator==(const has_then_execute_free_function&) const noexcept
  {
    return true;
  }

  bool operator!=(const has_then_execute_free_function&) const noexcept
  {
    return false;
  }
};


template<class Function>
std::future<std::result_of_t<Function()>> then_execute(const has_then_execute_free_function&, Function&& f, std::future<void>& predecessor_future)
{
  return has_then_execute_member().then_execute(std::forward<Function>(f), predecessor_future);
}

template<class Function, class T>
std::future<std::result_of_t<Function(T&)>> then_execute(const has_then_execute_free_function&, Function&& f, std::future<T>& predecessor_future)
{
  return has_then_execute_member().then_execute(std::forward<Function>(f), predecessor_future);
}


template<class Executor>
void test_then_execute(const Executor& exec)
{
  {
    // non-void predecessor to non-void result

    std::future<int> predecessor_future = std::async([] { return 1; });

    auto result_future = std::experimental::execution::then_execute(exec, [](int& predecessor)
    {
      return 1 + predecessor;
    },
    predecessor_future
    );

    assert(2 == result_future.get());
  }

  {
    // void predecessor to non-void result
    std::future<void> predecessor_future = std::async([]{});

    auto result_future = std::experimental::execution::then_execute(exec, []
    {
      return 1;
    },
    predecessor_future
    );

    assert(1 == result_future.get());
  }

  {
    // non-void predecessor_future to void result

    std::future<int> predecessor_future = std::async([] { return 1; });

    int result = 0;
    auto result_future = std::experimental::execution::then_execute(exec, [&](int& predecessor)
    {
      result = predecessor;
    },
    predecessor_future
    );

    result_future.wait();

    assert(1 == result);
  }

  {
    // void predecessor_future to void result

    std::future<void> predecessor_future = std::async([]{});

    int expect_thirteen = 0;
    auto result_future = std::experimental::execution::then_execute(exec, [&]
    {
      expect_thirteen = 13;
    },
    predecessor_future
    );

    result_future.wait();

    assert(13 == expect_thirteen);
  }
}


int main()
{
  static_assert(std::experimental::execution::has_then_execute_member<has_then_execute_member>::value, "member then_execute() is not detected."); 
  static_assert(!std::experimental::execution::has_execute_free_function<has_then_execute_member>::value, "free function then_execute() is detected."); 

  static_assert(!std::experimental::execution::has_then_execute_member<has_then_execute_free_function>::value, "member then_execute() is detected."); 
  static_assert(std::experimental::execution::has_then_execute_free_function<has_then_execute_free_function>::value, "free function then_execute() is not detected."); 

  test_then_execute(has_then_execute_member());
  test_then_execute(has_then_execute_free_function());

  std::cout << "OK" << std::endl;
}

