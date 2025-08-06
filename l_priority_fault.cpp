#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include <iostream>
#include <atomic>
#include <vector>
#include <cstring>
#include <csignal>

pthread_mutex_t sharedMutex = PTHREAD_MUTEX_INITIALIZER;
std::atomic<bool> stopMediums = false;

// Set thread CPU affinity to CPU 0
void SetAffinity() {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
}

// Set thread priority using SCHED_FIFO
void SetPriority(int priority) {
    sched_param sch;
    sch.sched_priority = priority;
    if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &sch) != 0) {
        std::cerr << "Failed to set priority: " << std::strerror(errno) << "\n";
    }
}

void* LowPriorityTask(void*) {
    SetAffinity();
    SetPriority(10); // Lowest

    std::cout << "[Low] Trying to lock mutex\n";
    pthread_mutex_lock(&sharedMutex);
    std::cout << "[Low] Locked mutex\n";
    while (true) {
        // Never releases the mutex
        usleep(100000);
    }
    return nullptr;
}

void* HighPriorityTask(void*) {
    usleep(50000); // Let low lock the mutex first
    SetAffinity();
    SetPriority(90); // Highest

    std::cout << "[High] Trying to lock mutex\n";
    pthread_mutex_lock(&sharedMutex); // Should block
    std::cout << "[High] Acquired mutex (should never happen!)\n";
    pthread_mutex_unlock(&sharedMutex);
    return nullptr;
}

void* MediumPriorityTask(void* arg) {
    int id = *(int*)arg;
    SetAffinity();
    SetPriority(50); // Medium priority

    while (!stopMediums.load()) {
        std::cout << "[Medium " << id << "] Working...\n";
        usleep(100000);
    }

    std::cout << "[Medium " << id << "] Stopping.\n";
    return nullptr;
}

int main() {
    std::cout << "Starting priority inversion demo on Linux\n";

    pthread_t low, high;
    pthread_create(&low, nullptr, LowPriorityTask, nullptr);
    pthread_create(&high, nullptr, HighPriorityTask, nullptr);

    std::vector<pthread_t> mediums;
    std::vector<int> ids;
    for (int i = 0; i < 3; ++i) {
        ids.push_back(i);
        pthread_t tid;
        pthread_create(&tid, nullptr, MediumPriorityTask, &ids[i]);
        mediums.push_back(tid);
    }

    sleep(3); // Let the medium threads flood for 3 seconds
    stopMediums = true;

    sleep(3); // Give time to see if high thread acquires mutex (it won't)

    std::cout << "\n[Main] Time limit reached. Terminating program.\n";
    kill(getpid(), SIGKILL); // Hard termination like TerminateProcess
    return 0;
}
