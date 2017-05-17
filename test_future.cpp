#include <experimental/future>
#include <cassert>

int main()
{
  {
    // default construction
    std::experimental::future<int> f0;
    assert(!f0.valid());
  }

  {
    // from promise

    {
      // move construction
      std::experimental::future<int> f0 = std::experimental::promise<int>().get_future();
      assert(f0.valid());

      std::experimental::future<int> f1 = std::move(f0);
      assert(!f0.valid());
      assert(f1.valid());
    }

    {
      // move assignment
      std::experimental::future<int> f1 = std::experimental::promise<int>().get_future();
      assert(f1.valid());

      std::experimental::future<int> f2;
      assert(!f2.valid());

      f2 = std::move(f1);
      assert(!f1.valid());
      assert(f2.valid());
    }

    {
      // get value
      std::experimental::promise<int> p;

      std::experimental::future<int> f = p.get_future();
      assert(f.valid());

      p.set_value(13);

      int result = f.get();
      assert(result == 13);
      assert(!f.valid());
    }

    {
      // get exception
      std::experimental::promise<int> p;

      std::experimental::future<int> f = p.get_future();
      assert(f.valid());

      p.set_exception(std::make_exception_ptr(std::runtime_error("error")));

      try
      {
        int result = f.get();
        assert(0);
      }
      catch(std::runtime_error e)
      {
        assert(std::string(e.what()) == "error");
      }
      catch(...)
      {
        assert(0);
      }
    }
  }


  {
    // from make_ready_future

    {
      // move construction
      std::experimental::future<int> f0 = std::experimental::make_ready_future<int>(13);
      assert(f0.valid());

      std::experimental::future<int> f1 = std::move(f0);
      assert(!f0.valid());
      assert(f1.valid());
    }

    {
      // move assignment
      std::experimental::future<int> f1 = std::experimental::make_ready_future<int>(13);
      assert(f1.valid());

      std::experimental::future<int> f2;
      assert(!f2.valid());

      f2 = std::move(f1);
      assert(!f1.valid());
      assert(f2.valid());
    }

    {
      // get value
      std::experimental::future<int> f = std::experimental::make_ready_future<int>(13);
      assert(f.valid());

      int result = f.get();
      assert(result == 13);
      assert(!f.valid());
    }

    {
      // get exception
      std::experimental::future<int> f = std::experimental::make_exceptional_future<int>(std::runtime_error("error"));
      assert(f.valid());

      try
      {
        int result = f.get();
        assert(0);
      }
      catch(std::runtime_error e)
      {
        assert(std::string(e.what()) == "error");
      }
      catch(...)
      {
        assert(0);
      }
    }
  }

  std::cout << "OK" << std::endl;
}

