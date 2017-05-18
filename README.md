# future
Implementation of `std::experimental::future` from the C++ Concurrency TS

## Notes on the possible implementation of `std::experimental::future.then(exec, func)`

There are two cases:

  1. The executor is natively two-way
  2. The executor is natively one-way

### The executor is natively two-way.
  
Natively two-way executors have either `then_execute` or `bulk_then_execute` functions.

There are two sub-cases:

  1. The executor returns the same type of future.
  2. The executor returns a different type of future.

1.1 When the executor's future is the same as the `predecessor` future, `predecessor.then(exec, f)` should just call `execution::then_execute(exec, f, predecessor)`.

This assumes `then_execute()` knows what to do and will not simply turn around and call `predecessor.then()`, which would create a cycle.

1.2 When the executor's future is a different type of future, we should again call `execution::then_execute` and also introduce a continuation that returns `future_from_executor.get()`.

This assumes that the future type has a mechanism for creating and containing a continuation.

Cases 1.1 and 1.2 ensure that the predecessor future is always presented to the executor, in the cases where the executor is able to consume it.

### The executor is natively one-way.

Natively one-way executors are oblivious to futures, because they have either `execute` or `bulk_execute` functions, and these functions neither consume nor produce futures.

At first glance, it may seem like we could call `execution::execute()` with a function which:

  1. Captures the predecessor future
  2. Waits for the predecessor future to become ready
  3. Calls the user function using the result of the predecessor as a parameter

The problem with this scheme is that 2. completely consumes an agent by waiting for the predecessor to become ready. This would be an expensive implemention in situations where agents are scarce, such as when the executor is backed by a small thread pool.

Instead, in Case 2., `future.then()` should create a continuation (capturing the executor) and this continuation should call `execution::execute()`. If the future is not ready at the time `.then()` is called, then `promise.set_value()` would call this continuation.

This assumes that the future type has a mechanism for creating and containing a continuation.

