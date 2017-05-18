# future
Implementation of std::experimental::future from the C++ Concurrency TS

## Notes on the possible implementation of `future.then(exec, func)`

There are two cases:

  1. The executor is two-way
  2. The executor is one-way

1. The executor is two-way. Therefore, it has either then_execute or bulk_then_execute functions.

There are two sub-cases:

  1. The executor returns the same type of future.
  2. The executor returns a different type of future.

1.1 When the executor's future is the same as the `predecessor` future, `predecessor.then(exec, f)` should just call `execution::then_execute(exec, f, predecessor)`.

This assumes `then_execute()` knows what to do and will not simply turn around and call `predecessor.then()`, which would create a cycle.

1.2 When the executor's future is a different type of future, we should again call `execution::then_execute` and also introduce a continuation that returns `future_from_executor.get()`.

This assumes that the future has a mechanism for creating and containing a continuation.

Cases 1.1 and 1.2 ensure that the predecessor future is always presented to the executor, in the cases where the executor is able to consume it.

2. Otherwise, the executor is oblivious to futures, because it is a one-way executor.

In this case, `future.then()` should create a continuation (capturing the executor) and this continuation should call `execution::execute()`. If the future is not ready at the time `.then()` is called, then `promise.set_value()` would call this continuation.

This assumes that future has a mechanism for creating and containing a continuation.

