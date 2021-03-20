#include "pool.h"



Task::Task() {
}

Task::~Task() {
}

ThreadPool::ThreadPool(int num_threads) {
    // initialize the mutex lock and condition variable
    pthread_mutex_init(&threads_mutex, NULL);
    pthread_cond_init(&threads_cv, NULL);
    // initialize the stopping boolean
    is_stopped = false;
    // make the new threads and add them to the vector
    // threads.resize(num_threads);
    for(int i = 0; i < num_threads; i++) {
        pthread_t new_thread = 0;
        threads.push_back(new_thread);
    }
    pthread_mutex_lock(&threads_mutex);
    for (int i = 0; i < threads.size(); ++i) {
        pthread_create(&threads[i], NULL, ThreadTaskWrapper, static_cast<void *>(this));
    }
    pthread_mutex_unlock(&threads_mutex);
}

void ThreadPool::SubmitTask(const std::string &name, Task* task) {
    // set the attributes of the lock
    pthread_mutex_init(&task->task_mutex, NULL);
    pthread_mutex_init(&task->task_finished_mutex, NULL);
    pthread_cond_init(&task->task_cv, NULL);
    task->is_finished = false;

    // Acquire the lock
    pthread_mutex_lock(&threads_mutex);

    // add the task to the tasks list and tasks map
    tasks.push_front(task);
    tasksMap.insert({name, task});

    // signal to the threadTask that we have added a new task
    pthread_cond_signal(&threads_cv);
    pthread_mutex_unlock(&threads_mutex);
}

void ThreadPool::WaitForTask(const std::string &name) {
    pthread_mutex_lock(&threads_mutex);
    // get the task from the tasksmap
    Task * task = tasksMap[name];
    // delete the task from the taskMap
    tasksMap.erase(name);
    pthread_mutex_unlock(&threads_mutex);
    pthread_mutex_lock(&task->task_finished_mutex);
    // task could be finished when we first check or we may have to
    // wait for it to finish
    while(!task->is_finished) {
        pthread_cond_wait(&task->task_cv, &task->task_mutex);
    }
    // task should be finished now
    pthread_mutex_unlock(&task->task_finished_mutex);
    // now that task is finished we can delete it
    delete task;
}
// will nead to join every thread in threads
void ThreadPool::Stop() {
    pthread_mutex_lock(&threads_mutex);
    // set is_stopped and then signal that all threads need to stop running
    is_stopped = true;
    pthread_cond_broadcast(&threads_cv);
    // I think that by this point all running threads have been stopped
    pthread_mutex_unlock(&threads_mutex);
    // Iterate over every thread and join to end the process
    int rc;
    for(pthread_t thread : threads){
        rc = pthread_join(thread, NULL);
        if (rc) {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
    }
    threads.clear();
}

void * ThreadPool::ThreadTaskWrapper(void *arg) {
    return static_cast<ThreadPool *>(arg)->ThreadTask();
}

void * ThreadPool::ThreadTask(void) {
    // Until the stop method is called
    while(!is_stopped) {
        // Check for new tasks to start
        pthread_mutex_lock(&threads_mutex);
        while(tasks.empty()){
            // if no threads are available, wait until we submit a new task
            pthread_cond_wait(&threads_cv, &threads_mutex);
            // could get woken up when stop is called. In this case, finish early
            // This took me so long 
            if(is_stopped){
                pthread_mutex_unlock(&threads_mutex);
                return 0;
            }
        }
        // We now know there is a task to grab
        Task * t = tasks.back();
        tasks.pop_back();

        //unlock the mutex so other threads can get tasks
        pthread_mutex_unlock(&threads_mutex);
        // run the task
        t->Run();
        // When it is done, signal that it is finished
        pthread_mutex_lock(&t->task_mutex);
        t->is_finished = true;
        pthread_cond_signal(&(t->task_cv));
        pthread_mutex_unlock(&(t->task_mutex));
    }
    return 0;
}
