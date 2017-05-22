#include <experimental/future>

int main()
{
  size_t n = 1 << 24;

  for(size_t i = 0; i < n; ++i)
  {
    auto f = std::experimental::make_ready_future();
  }
}

