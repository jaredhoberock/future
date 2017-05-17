#include <experimental/execution>
#include <iostream>

struct has_execute_member
{
  const has_execute_member& context() const noexcept
  {
    return *this;
  }

  template<class Function>
  void execute(Function&& f) const
  {
    f();
  }

  bool operator==(const has_execute_member&) const noexcept
  {
    return true;
  }

  bool operator!=(const has_execute_member&) const noexcept
  {
    return false;
  }
};


struct has_execute_free_function
{
  const has_execute_free_function& context() const noexcept
  {
    return *this;
  }

  bool operator==(const has_execute_free_function&) const noexcept
  {
    return true;
  }

  bool operator!=(const has_execute_free_function&) const noexcept
  {
    return false;
  }
};


template<class Function>
void execute(const has_execute_free_function&, Function&& f)
{
  f();
}


template<class Executor>
void test_execute(const Executor& exec)
{
  bool function_called = false;

  std::experimental::execution::execute(exec, [&function_called]
  {
    function_called = true;
  });

  // wait until the function is called
  while(!function_called) {}
}


int main()
{
  static_assert(std::experimental::execution::has_execute_member<has_execute_member>::value, "member execute() is not detected."); 
  static_assert(!std::experimental::execution::has_execute_free_function<has_execute_member>::value, "free function execute() is detected."); 

  static_assert(!std::experimental::execution::has_execute_member<has_execute_free_function>::value, "member execute() is detected."); 
  static_assert(std::experimental::execution::has_execute_free_function<has_execute_free_function>::value, "free function execute() is not detected."); 

  test_execute(has_execute_member());
  test_execute(has_execute_free_function());

  std::cout << "OK" << std::endl;
}

