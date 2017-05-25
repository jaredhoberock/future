#include <experimental/future>
#include <cstdlib>

int main(int argc, char **argv)
{
  size_t n = 1 << 24;

  if(argc > 1)
  {
    n = atoi(argv[1]);
  }

  for(size_t i = 0; i < n; ++i)
  {
    auto f = std::experimental::make_ready_future();
  }
}

