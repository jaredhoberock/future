#include <experimental/future>
#include <cassert>
#include <iostream>

void test()
{
  {
    // test non-void get_future
    std::experimental::promise<int> p;

    std::experimental::future<int> f = p.get_future();

    assert(f.valid());

    try
    {
      std::experimental::future<int> f = p.get_future();
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
    // test void get_future
    std::experimental::promise<void> p;

    std::experimental::future<void> f = p.get_future();

    assert(f.valid());

    try
    {
      std::experimental::future<void> f = p.get_future();
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
    // test non-void set_value() / .get()
    std::experimental::promise<int> p;

    int expected_result = 13;
    p.set_value(expected_result);

    int result = p.get_future().get();

    assert(expected_result == result);
  }

  {
    // test void set_value() / .get();
    std::experimental::promise<void> p;

    p.set_value();

    p.get_future().get();
  }

  {
    // test non-void superfluous set_value()
    std::experimental::promise<int> p;

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

  {
    // test void superfluous set_value()
    std::experimental::promise<void> p;

    p.set_value();

    try
    {
      // calling set_value() againt should throw promise_already_satisfied
      p.set_value();
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
  test();

  std::cout << "OK" << std::endl;
}

