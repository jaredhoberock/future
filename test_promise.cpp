#include <experimental/future>
#include <cassert>
#include <iostream>

template<class T>
void test()
{
  {
    // test get_future
    std::experimental::promise<T> p;

    std::experimental::future<T> f = p.get_future();

    assert(f.valid());

    try
    {
      std::experimental::future<T> f = p.get_future();
      assert(0);
    }
    catch(std::future_error e)
    {
      assert(e.code() == std::future_errc::future_already_retrieved);
    }
    catch(...)
    {
      assert(0);
    }
  }

  {
    // test set_value() / .get()
    std::experimental::promise<T> p;

    T expected_result = 13;
    p.set_value(expected_result);

    T result = p.get_future().get();

    assert(expected_result == result);
  }

  {
    // test superfluous set_value()
    std::experimental::promise<T> p;

    p.set_value(13);

    try
    {
      // calling set_value() againt should throw promise_already_satisfied
      p.set_value(13);
      assert(0);
    }
    catch(std::future_error e)
    {
      assert(e.code() == std::future_errc::promise_already_satisfied);
    }
    catch(...)
    {
      assert(0);
    }
  }
}

int main()
{
  test<int>();

  std::cout << "OK" << std::endl;
}

