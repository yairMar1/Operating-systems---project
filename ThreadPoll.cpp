#include "ThreadPoll.hpp"
#include <iostream>

using namespace std;

// Constructor that initializes the thread pool with num_threads
ThreadPoll::ThreadPoll(size_t num_threads)
    : stop(false) {
    for (size_t i = 0; i < num_threads; ++i) {
        // Create and launch threads, each identified by its index i
        threads.emplace_back([this, i]() {
            thread_worker(i);  // Pass the thread ID to the worker function
        });
    }
}

// Destructor to stop the thread pool and join threads
ThreadPoll::~ThreadPoll() {
    stop = true;  // Signal to stop the threads
    condition.notify_all();  // Wake up all threads to finish their work
    for (auto& thread : threads) {
        if (thread.joinable())
            thread.join();  // Wait for each thread to finish
    }
}

// Function to add a new task to the task queue
void ThreadPoll::enqueue(int client_id, function<void(int)> task) {
    {
        unique_lock<mutex> lock(queue_mutex);
        tasks.push({ client_id, task });
    }
    condition.notify_one();
}

// Get the thread ID of the current thread
size_t ThreadPoll::getThreadID() {
    return static_cast<size_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
}

// Worker function for each thread
void ThreadPoll::thread_worker(size_t thread_id) {
    while (!stop) {
        pair<int, function<void(int)>> task;
        {
            unique_lock<mutex> lock(queue_mutex);
            condition.wait(lock, [this] { return stop || !tasks.empty(); });
            if (stop && tasks.empty())
                return;

            task = tasks.front();
            tasks.pop();
        }

        int client_id = task.first;
        // Output which thread is serving which client
        cout << "Thread " << thread_id << " is serving client " << client_id << endl;

        // Execute the task and pass thread_id to it
        task.second(thread_id);
    }
}
