# Thread_Pool
A thread safe threadpool module using standard C++11 features.
The thread pool is an eager initialization approach for creating a set number of threads on the go, readily available to perform `tasks`.
Using the `ThreadPool` class, you can call upon several fast and thread safe methods to control the execution flow of tasks with varied `priority`:
1. `Add` a Task with it's own PRIORITY to the ThreadPool.
   Along with the threadpool itself, you are provided with a `Task` abstract class for you to inherit from,
   as well as three different task priorities to pair with the task - `HIGH`, `MEDIUM` and `LOW`.
2. `Run` the threadpool, and execute tasks from queue.
3. `Pause` pulling tasks from the queue.
4. `Stop` the threadpool and clear the task queue.
5. `SetNumberOfThreads` - Resize the pool.

#### intended for Linux distros only.

This program utilizes a [custom module for a thread safe queue](./lib/include/waitable_queue.hpp).

You are provided with a [makefile](./Makefile) for you to compile a shared library (both ``DEBUG`` and ``RELEASE`` options) and a [header file](./include/thread_pool.hpp) for you to include in your code.

# Compilation and Usage
To use ThreadPool, simply include the [thread_pool.hpp header file](./include/thread_pool.hpp) in your projects.
* Compile the libraries using either ``make release`` ' or ``make debug``.

* the [header](./include/thread_pool.hpp) contains The `ThreadPool` class and a `Task` abstract class for you to inherit from.

### Initialization
You can create the thread pool with the default platform dependent number of threads, or you can specify your desired number.
If desired, you can also set the `niceness` level of the threadpool.

```c++
thread_pool::ThreadPool tp;     // -> Create default pool, with default niceness
thread_pool::ThreadPool tp(16); // -> Create 16 threads
thread_pool::ThreadPool tp(16, 3); // -> Create 16 threads, a nice level of 3
```
