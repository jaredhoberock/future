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

  std::cout << "OK" << std::endl;
}
