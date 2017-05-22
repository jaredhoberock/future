#include <experimental/future>
#include <cassert>

int main()
{
  {
    // non-void default construction
    std::experimental::future<int> f0;
    assert(!f0.valid());
  }

  {
    // void default construction
    std::experimental::future<void> f0;
    assert(!f0.valid());
  }

  {
    // from promise

    {
      // non-void move construction
      std::experimental::future<int> f0 = std::experimental::promise<int>().get_future();
      assert(f0.valid());

      std::experimental::future<int> f1 = std::move(f0);
      assert(!f0.valid());
      assert(f1.valid());
    }

    {
      // void move construction
      std::experimental::future<void> f0 = std::experimental::promise<void>().get_future();
      assert(f0.valid());

      std::experimental::future<void> f1 = std::move(f0);
      assert(!f0.valid());
      assert(f1.valid());
    }

    {
      // non-void move assignment
      std::experimental::future<int> f1 = std::experimental::promise<int>().get_future();
      assert(f1.valid());

      std::experimental::future<int> f2;
      assert(!f2.valid());

      f2 = std::move(f1);
      assert(!f1.valid());
      assert(f2.valid());
    }

    {
      // void move assignment
      std::experimental::future<void> f1 = std::experimental::promise<void>().get_future();
      assert(f1.valid());

      std::experimental::future<void> f2;
      assert(!f2.valid());

      f2 = std::move(f1);
      assert(!f1.valid());
      assert(f2.valid());
    }

    {
      // non-void get
      std::experimental::promise<int> p;

      std::experimental::future<int> f = p.get_future();
      assert(f.valid());

      p.set_value(13);

      int result = f.get();
      assert(result == 13);
      assert(!f.valid());
    }

    {
      // void get
      std::experimental::promise<void> p;

      std::experimental::future<void> f = p.get_future();
      assert(f.valid());

      p.set_value();

      f.get();
      assert(!f.valid());
    }

    {
      // non-void get exception
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

    {
      // void get exception
      std::experimental::promise<void> p;

      std::experimental::future<void> f = p.get_future();
      assert(f.valid());

      p.set_exception(std::make_exception_ptr(std::runtime_error("error")));

      try
      {
        f.get();
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
      // non-void move construction
      std::experimental::future<int> f0 = std::experimental::make_ready_future<int>(13);
      assert(f0.valid());

      std::experimental::future<int> f1 = std::move(f0);
      assert(!f0.valid());
      assert(f1.valid());
    }

    {
      // void move construction
      std::experimental::future<void> f0 = std::experimental::make_ready_future();
      assert(f0.valid());

      std::experimental::future<void> f1 = std::move(f0);
      assert(!f0.valid());
      assert(f1.valid());
    }

    {
      // non-void move assignment
      std::experimental::future<int> f1 = std::experimental::make_ready_future<int>(13);
      assert(f1.valid());

      std::experimental::future<int> f2;
      assert(!f2.valid());

      f2 = std::move(f1);
      assert(!f1.valid());
      assert(f2.valid());
    }

    {
      // void move assignment
      std::experimental::future<void> f1 = std::experimental::make_ready_future();
      assert(f1.valid());

      std::experimental::future<void> f2;
      assert(!f2.valid());

      f2 = std::move(f1);
      assert(!f1.valid());
      assert(f2.valid());
    }

    {
      // non-void get
      std::experimental::future<int> f = std::experimental::make_ready_future<int>(13);
      assert(f.valid());

      int result = f.get();
      assert(result == 13);
      assert(!f.valid());
    }

    {
      // void get
      std::experimental::future<void> f = std::experimental::make_ready_future();
      assert(f.valid());

      f.get();
      assert(!f.valid());
    }

    {
      // non-void get exception
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

    {
      // void get exception
      std::experimental::future<void> f = std::experimental::make_exceptional_future<void>(std::runtime_error("error"));
      assert(f.valid());

      try
      {
        f.get();
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

