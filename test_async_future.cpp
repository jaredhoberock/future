#include <experimental/future>
#include <cassert>

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

int main()
{
  using namespace std::experimental::v1::detail;

  {
    // non-void make_ready()

    async_future<int> f0 = async_future<int>::make_ready(13);
    assert(f0.valid());
    assert(f0.is_ready());

    assert(13 == f0.get());
  }

  {
    // void make_ready()

    async_future<void> f0 = async_future<void>::make_ready();
    assert(f0.valid());
    assert(f0.is_ready());
  }

  {
    // non-void -> non-void then()

    async_future<int> f0 = async_future<int>::make_ready(13);
    assert(f0.valid());
    assert(f0.is_ready());

    has_execute_member exec;
    auto f1 = f0.then(exec, [](async_future<int> predecessor)
    {
      return predecessor.get() + 7;
    });

    int result = f1.get();

    assert(result == 20);
  }

  {
    // non-void -> void then()

    async_future<int> f0 = async_future<int>::make_ready(13);
    assert(f0.valid());
    assert(f0.is_ready());

    has_execute_member exec;
    int result = 0;
    auto f1 = f0.then(exec, [&](async_future<int> predecessor)
    {
      result = predecessor.get() + 7;
    });

    f1.wait();

    assert(result == 20);
  }

  {
    // void -> non-void then()

    async_future<void> f0 = async_future<void>::make_ready();
    assert(f0.valid());
    assert(f0.is_ready());

    has_execute_member exec;
    auto f1 = f0.then(exec, [](async_future<void> predecessor)
    {
      predecessor.wait();
      return 20;
    });

    int result = f1.get();
    assert(result == 20);
  }

  {
    // void -> void then()

    async_future<void> f0 = async_future<void>::make_ready();
    assert(f0.valid());
    assert(f0.is_ready());

    has_execute_member exec;
    int result = 0;
    auto f1 = f0.then(exec, [&](async_future<void> predecessor)
    {
      predecessor.wait();
      result = 20;
    });

    f1.wait();

    assert(result == 20);
  }
  {
    // async_future from promise

    std::experimental::promise<bool> p;
    async_future<bool> f = p.get_async_future();
    bool continuation_has_run = false;

    has_execute_member exec;
    f.then(exec,
        [&continuation_has_run]( async_future<bool> predecessor)
        {

            assert(false == predecessor.get());
            continuation_has_run = true;

        });

    assert(!continuation_has_run);
    p.set_value(false);
    assert(continuation_has_run);
  }
  std::cout << "OK" << std::endl;
}
