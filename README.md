# future
Implementation of `std::experimental::future` from the C++ Concurrency TS

## Notes on the possible implementation of `std::experimental::future.then(exec, func)`

There are two cases:

  1. The executor is natively two-way
  2. The executor is natively one-way

### The executor is natively two-way.
  
Natively two-way executors interoperate with futures, because they have either `then_execute` or `bulk_then_execute` functions, and these functions consume and produce futures.

In Case 1., `predecessor_future.then(exec, f)` should call `execution::then_execute(exec, f, predecessor)`. The type of future returned by this function is given by `execution::executor_future_t`.

This assumes `then_execute()` knows what to do and will not simply turn around and call `predecessor.then()`, which would create a cycle. Presumably, either the executor or underlying execution context has access to the predecessor future's internal representation, and it uses this access to create a continuation appropriately.

Case 1. ensures that the predecessor future is always presented to the executor in the cases where the executor is able to natively consume it.

### The executor is natively one-way.

Natively one-way executors are oblivious to futures, because they have either `execute` or `bulk_execute` functions, and these functions neither consume nor produce futures.

At first glance, it may seem like we could call `execution::execute()` with a function which:

  1. Captures the predecessor future
  2. Waits for the predecessor future to become ready
  3. Calls the user function using the result of the predecessor as a parameter

The problem with this scheme is that 2. completely consumes an agent by waiting for the predecessor to become ready. This would be an expensive implemention in situations where agents are scarce, such as when the executor is backed by a small thread pool.

Instead, in Case 2., `future.then()` should create a continuation (capturing the executor) and this continuation should call `execution::execute()`. If the future is not ready at the point when `.then()` is called, then `promise.set_value()` would call this continuation.

This assumes that the future type has a mechanism for creating and containing a continuation.

#### Sample implementation of `std::experimental::future.then(one_way_exec, func)`

The following is a sample implementation of `.then()` for one-way executors:

    template<class T, class OneWayExecutor, class F>
    experimental::future<
      result_of_t<
        decay_t<F>(experimental::future<T>)
      >
    >
      then(const OneWayExecutor& exec, F&& func)
    {
      using result_type = result_of_t<
        decay_t<F>(experimental::future<T>)
      >;
    
      // XXX packaged_task is a pessimization because we may not require storing the
      //     continuation inside of set_continuation()
      packaged_task<result_type(experimental::future<T>)> continuation(forward<F>(func));
      experimental::future<result_type> result_future = continuation.get_future();

      // set the predecessor's continuation and executor which will execute that continuation
      predecessor.__set_continuation(exec, std::move(continuation));
    
      return result_future;
    }

Note that the call to `.__set_continuation()` in the above is the only part of the implementation of `.then()` that relies on `experimental::future`'s internal representation.
It may make sense to introduce `.set_continuation()` as a lower-level operation required by all `Future`s (or all continuation-aware `Future`s) separate from `.then()`.
The idea is that `future.set_continuation()` attaches a continuation function to the future, and this continuation function is invoked when the future becomes ready.

There are two cases:

  1. The future is ready at the point when `.set_continuation()` is invoked. Therefore, `execution::execute(exec, continuation)` is immediately invoked in the calling thread.
  2. The future is not yet ready at the point when `.set_continuation()` is invoked. Therefore, the continuation is stored for later. `execution::execute(exec, continuation)` will be invoked
     in the thread that makes the future ready (e.g. the thread which calls `promise.set_value()`.

`future.set_continuation()` returns `void` and invalidates the future.

If we had `.set_continuation()` as a primitive to work with, it would be possible to make `future.then(exec, f)` purely a sugared form of `execution::then_execute(exec, f, future)`.
In that scenario, `future.then(exec, f)` would unconditionally call `execution::then_execute(exec, f, predecessor)`, and it would be the responsibility of `execution::then_execute()`
to implement the logic for Cases 1. and 2. above.

#### Sample implementation of a hypothetical `.set_continuation()` function for `experimental::future<T>`'s asynchronous state object

    template<class Executor, class Function>
    void set_continuation(const Executor& exec, Function&& f)
    {
      // create a continuation that calls execution::execute()
      auto continuation = [exec, f = move(f)] (experimental::future<T> predecessor_future) mutable
      {
        execution::execute(exec, [f = move(f), predecessor_future = move(predecessor_future)] () mutable
        {
          f(move(predecessor_future));
        });
      };

      unique_lock<mutex> lock(this->mutex_);

      if(this->is_ready_)
      {
        // the state is ready, invoke the continuation immediately

        // unlock here so that the future passed to the continuation below doesn't block in .get()
        lock.unlock();

        continuation(this->to_future());
      }
      else
      {
        // the state is not yet ready, store the continuation for later
        this->continuation_ = move(continuation);
      }
    }

