#ifndef POOL_H_
#include <string>
#include <pthread.h>
#include <map>
#include <vector>
#include <deque>


class Task {
public:
    Task();
    virtual ~Task();

    virtual void Run() = 0;  // implemented by subclass
    pthread_cond_t task_cv;
    pthread_mutex_t task_finished_mutex;
    pthread_mutex_t task_mutex;
    bool is_finished;

};

struct Running_Thread {
    pthread_t *thread;
    Task *task;
};

class ThreadPool {
public:
    // used to control access to tasks and threads
    ThreadPool(int num_threads);

    // Submit a task with a particular name.
    void SubmitTask(const std::string &name, Task *task);
 
    // Wait for a task by name, if it hasn't been waited for yet. Only returns after the task is completed.
    void WaitForTask(const std::string &name);

    // Stop all threads. All tasks must have been waited for before calling this.
    // You may assume that SubmitTask() is not caled after this is called.
    void Stop();
private:
    pthread_mutex_t threads_mutex;
    pthread_cond_t threads_cv;

    // Allows us to quickly grab the next task
    std::deque<Task *> tasks; 
    // will alow for waiting on a task
    std::map<std::string, Task*> tasksMap;
    // will hold num_threads specified by constructor
    std::vector<pthread_t> threads; 

    bool is_stopped;

    void * ThreadTask(void);
    static void * ThreadTaskWrapper(void * arg);
};
#endif
