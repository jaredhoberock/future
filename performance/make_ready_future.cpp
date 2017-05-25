#include <future>
#include <cstdlib>

std::future<void> make_ready_future()
{
  std::promise<void> p;
  std::future<void> result = p.get_future();
  p.set_value();
  return result;
}

int main(int argc, char **argv)
{
  size_t n = 1 << 24;

  if(argc > 1)
  {
    n = atoi(argv[1]);
  }

  for(size_t i = 0; i < n; ++i)
  {
    auto f = make_ready_future();
  }
}

