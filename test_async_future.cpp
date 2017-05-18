#include <experimental/future>
#include <cassert>

int main()
{
  using namespace std::experimental::v1::detail;

  {
    // test make_ready()

    async_future<int> f0 = async_future<int>::make_ready(13);
    assert(f0.valid());
    assert(f0.is_ready());

    assert(13 == f0.get());
  }

  std::cout << "OK" << std::endl;
}

