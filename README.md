# future
Implementation of `std::experimental::future` from the C++ Concurrency TS

## Notes on the possible implementation of `std::experimental::future.then(exec, func)`

There are two cases:

  1. The executor is natively two-way
  2. The executor is natively one-way

### The executor is natively two-way.
  
Natively two-way executors have either `then_execute` or `bulk_then_execute` functions.

There are two sub-cases:

  1. The executor returns a kind of future which can be converted into the same type of future as the predecessor future.
  2. The executor returns a kind of future which cannot be converted into the same type of future as the predecessor future.

1.1 When the kind of future returned by `then_execute` can be converted into the same kind as the `predecessor` future, `predecessor.then(exec, f)` should just call `execution::then_execute(exec, f, predecessor)`.

This assumes `then_execute()` knows what to do and will not simply turn around and call `predecessor.then()`, which would create a cycle.

1.2 When the kind of future returned by `then_execute` cannot be converted into the same kind as the `predecessor` future, we should again call `execution::then_execute` and also introduce a continuation that returns `future_from_executor.get()`.

This assumes that the future type has a mechanism for creating and containing a continuation.

Cases 1.1 and 1.2 ensure that the predecessor future is always presented to the executor, in the cases where the executor is able to consume it.

XXX It may not actually be necessary to distinguish between cases 1.1 and 1.2. There's no rule that says `.then()` must return the same kind of future as the predecessor future.

### The executor is natively one-way.

Natively one-way executors are oblivious to futures, because they have either `execute` or `bulk_execute` functions, and these functions neither consume nor produce futures.

At first glance, it may seem like we could call `execution::execute()` with a function which:

  1. Captures the predecessor future
  2. Waits for the predecessor future to become ready
  3. Calls the user function using the result of the predecessor as a parameter

The problem with this scheme is that 2. completely consumes an agent by waiting for the predecessor to become ready. This would be an expensive implemention in situations where agents are scarce, such as when the executor is backed by a small thread pool.

Instead, in Case 2., `future.then()` should create a continuation (capturing the executor) and this continuation should call `execution::execute()`. If the future is not ready at the time `.then()` is called, then `promise.set_value()` would call this continuation.

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
      assert(this->valid());
    
      using result_type = result_of_t<
        decay_t<F>(experimental::future<T>)
      >;
    
      packaged_task<result_type(experimental::future<T>&&)> task(forward<F>(func));
      experimental::future<result_type> result_future = task.get_future();
    
      // the predecessor_future's continuation will call execution::execute()
      auto continuation = [=, task = move(task)](experimental::future<T> predecessor_future) mutable
      {
        // wrap up task and the predecessor_future into a lambda that we give to execution::execute()
    
        execution::execute(exec, [task = move(task), predecessor_future = move(predecessor_future)]() mutable
        {
          task(move(predecessor_future));
        });
      };
    
      // XXX note that this next call (set_continuation()) is the only part of the implementation that relies on experimental::future's implementation
      // 
      //  It may make sense to introduce .set_continuation() as a lower-level primitive required by all Futures (or all continuation-aware Futures) distinct from .then()
      //  The idea is that future.set_continuation() attaches a continuation function to the future.
      //
      //  There are two cases:
      //
      //    1. The future is ready, and the continuation is invoked immediately in the current thread.
      //    2. The future is not yet ready, and the continuation is stored for later. The continuation will be invoked in the thread that makes the future ready (e.g. the thread which calls promise.set_value()).
      //
      //  .set_continuation() returns void and invalidates the future.
      //
      // If we had .set_continuation() as a primitive to work with, we could do all of this stuff generically inside
      // of execution::then_execute() rather than forcing the future implementation to deal with it
    
      this->set_continuation(move(continuation));
    
      return result_future;
    }

