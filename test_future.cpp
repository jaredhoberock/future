#include <experimental/future>
#include <cassert>

int main()
{
  {
    // default construction
    std::experimental::future<int> f0;
    assert(!f0.valid());
  }

  std::cout << "OK" << std::endl;
}

