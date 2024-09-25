#ifndef THREADPOLL_HPP
#define THREADPOLL_HPP

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

using namespace std;

class ThreadPoll {
public:
    // Constructor: Initializes the thread pool with a given number of threads
    ThreadPoll(size_t num_threads);

    // Destructor: Ensures all threads are properly joined and the pool is stopped
    ~ThreadPoll();

    // Enqueue a task with a client ID to the thread pool
    void enqueue(int client_id, function<void(int)> task);

    // Get the thread ID of the current thread (for debugging purposes)
    size_t getThreadID();

private:
    // Vector of worker threads
    vector<thread> threads;

    // Queue of tasks, each with a client ID and the corresponding task
    queue<pair<int, function<void(int)>>> tasks;

    // Mutex for thread-safe access to the task queue
    mutex queue_mutex;

    // Condition variable to notify threads of available tasks
    condition_variable condition;

    // Atomic boolean to stop threads safely
    atomic<bool> stop;

    // Worker function for each thread in the pool
    void thread_worker(size_t thread_id);
};

#endif // THREADPOLL_HPP
