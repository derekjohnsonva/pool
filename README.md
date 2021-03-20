# pool

1.  Implement a _thread pool_, which runs tasks across a fixed number of threads. Each of these threads should take tasks from a queue and run them, waiting for new tasks whenever the queue is empty. Your thread pool will start tasks in the order they are submitted and also support waiting for a particular task submitted to thread pool to complete. Each task in the thread pool will be identified by a name.

    Your implementation must start threads **_once_** when the thread pool is created and only deallocate them when the `Stop()` method is called. You **_may not_** start new threads each time a task is submitted.

    Your thread pool should follow the interface defined in the header file `pool.h` in [our skeleton code](files/pool-skeleton.tar.gz) [_Last updated_ 2020-02-11]. This interface has two classes, one called _Task_ representing tasks to run an done called _ThreadPool_ representing the thread pool itself. You may add additional methods and member variables to these classes for your implementation.

    The Task class must have the following methods:

    *   `Task()`, `virtual ~Task()` — constructor and virtual destructor

    *   `virtual void Run() = 0` — pure virtual (i.e. abstract) method overriden by subclasses. This will be run by threads in the thread pool when a task is run.

    The ThreadPool class must have the following methods:

    *   `ThreadPool(int num_threads)` — constructor, which takes the number of threads to create to run queued tasks.

    *   `void SubmitTask(const std::string &name, Task *task)` — function to submit (enqueue) a task. The task can be identified by `name` in future calls to `WaitForTask()`. Either when the task completes or when the task is waited for, the ThreadPool must be deallocate it as with `delete task`.

        You may assume that for a particular ThreadPool object, `SubmitTask` will be called only with the `name` of a task that has not already been submitted and for which `WaitForTask` has not already been called.

        This method should return immediately after adding the task to a queue, allocating additional space for the queue if necessary. (If this allocation fails, we do not care what happens.) It should never wait for running tasks to finish and free up space to store additional pending tasks.

    *   `void WaitForTask(const std::string &name)` — wait for a particular task, not returning until the task completes. Note that this method may be called any time after the task is submitted, including after it has already finished running.

        You may assume that `WaitForTask` is called exactly once per submitted task.

    *   `void Stop()` — stop the thread pool, terminating all its threads normally. If any thread is in the middle of running a Task, this should wait for that thread to finish running the task rather than interrupting it. Before this returns, all the threads spawned by thread pool should have finished executing and it should be safe to deallocate the thread pool.

    Every method of the `ThreadPool` class except for the `Stop()` method may be called from any thread, including from one of the tasks in submitted to the thread pool. The `Stop()` method may be called from any thread except one that was created by the thread pool.

    Provided that tasks are waited for as they complete, your thread pool should not use more and more memory the more tasks that run. (For example, you should not store a list of the names of all finished tasks from which tasks are never removed until thread pool is stopped (even when those finished tasks were waited for before the thread pool stops).)

    Your implementation must support multiple `ThreadPool` objects being used at the same time, so you should not use global variables.

    _In no case, may any of the methods above (or below, if you took CoA 2) or the threads spawned by the constructor to run queued tasks consume a lot of compute time while waiting for some condition (e.g. a task finishing or a new task being available) to become true._ A thread that needs to wait should arrange to be put into a sleeping/waiting state until the condition is likely true. (That is, use something like a condition variable or semaphore to wait until another thread indicates that something has happened, rather than simply checking a condition in a loop.)

2.  If you have previously taken CoA 2, then as an additional requirement, you must implement the following methods:

    *   `bool CancelTask(const std::string &name)` — stop a task with a particular name from running if it has not already started. This should return `true` if successful, and `false` if the task in question has already started running or completed.

        You may assume that this is called **instead of** calling `WaitForTask` for a task of the same name.

    *   `void Pause()` — after any currently running tasks complete, temporarily stop the thread pool worker threads from running any tasks. This must not return until after all the worker threads are not running tasks.

    *   `void Resume()` — assuming a previous call to Pause was made, cause the thread pool threads to resume processing tasks.

3.  Your submission should include a `Makefile` which produces a statically linked library `libpool.a`, like our skeleton code does.

4.  You can use the supplied `pool-test` and `pool-test-tsan` programs to help test your thread pool implementation.

    Both of these programs are built using the source code in `pool-test.cc`, but with different compiler options:

    *   `./pool-test` which is compiled with [AddressSanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer), which attempts to detect memory errors
    *   `./pool-test-tsan` which is compiled with [ThreadSanitizer](https://github.com/google/sanitizers/wiki/ThreadSanitizerCppManual), which attempts to detect data races. Though this version is likely to be most useful for finding threading-related errors, it is also likely to be much slower.

    Note that this is not a complete test. For example:

    *   the test may not expose all the race conditions that might exist in your code (especially since the exact timing with which your code is run will vary between machines),
    *   it does not determine if your code consumes a lot of compute time while waiting for a condition to become true, and
    *   it does not determine whether you comply with the requirement to not spawn new threads for each task

    If you have previously taken CoA 2, then we have supplied a [pool-test-coa2-extra.cc](files/pool-test-coa2extra.cc) which contains some tests for the extra functions you must implement. To use these tests, add it **in addition** to the supplied `pool-test.cc` to the Makefile, following the pattern used for `pool-test.cc` to add additional rules to the Makefile. Like with supplied `pool-test`, these tests will not completely test the methods you need to implement.

5.  Produce a `.tar.gz` file of your submission like the one `make submit` will produce and submit to the submission site.
