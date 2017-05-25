# Notes on the performance of `experimental::make_ready_future`

One straightforward way to implement `make_ready_future` is by introducing a `promise`, getting the `promise`'s `future`, and then immediately
calling `.set_value()` on the `promise`:

    std::future<void> make_ready_future()
    {
      std::promise<void> p;
      std::future<void> result = p.get_future();
      p.set_value();
      return result;
    }

The problem with this implementation is that the cost of introducing a
`promise`-`future` channel is significant. The benchmark program
`make_ready_future.cpp` in this directory demonstrates the overhead of this implementation:

    $ time ./make_ready_future 10000000

    real    0m1.862s
    user    0m1.252s
    sys     0m0.608s

Because a separate thread is not required to fulfill the `promise`, an
efficient implementation of `make_ready_future` need not introduce a `promise`
at all. Avoiding the introduction of a `promise` yields an implementation with
minimal overhead. The benchmark program `experimental_make_ready_future.cpp` in
this directory demonstrates:

    $ time ./experimental_make_ready_future 10000000
    
    real    0m0.004s
    user    0m0.000s
    sys     0m0.000s


