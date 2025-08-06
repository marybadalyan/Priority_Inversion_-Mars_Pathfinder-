#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <atomic>
#include <iostream>
#include <vector>

std::atomic<bool> highWaiting = false;
std::atomic<bool> stopMediums = false;
pthread_mutex_t sharedMutex;

void* LowPriorityTask(void*) {
    struct sched_param param;
    param.sched_priority = 10;
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);

    std::cout << "[Low] Trying to lock mutex\n";
    pthread_mutex_lock(&sharedMutex);
    std::cout << "[Low] Locked mutex\n";

    for (int i = 0; i < 20; ++i) {
        std::cout << "[Low] Working...\n";
        usleep(100'000); // 100 ms
    }

    std::cout << "[Low] Releasing mutex\n";
    pthread_mutex_unlock(&sharedMutex);
    return nullptr;
}

void* HighPriorityTask(void*) {
    usleep(50'000); // Let low get the lock first

    struct sched_param param;
    param.sched_priority = 80;
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);

    std::cout << "[High] Trying to lock mutex\n";
    highWaiting = true;

    pthread_mutex_lock(&sharedMutex); // Will block
    highWaiting = false;

    std::cout << "[High] Acquired mutex!\n";
    pthread_mutex_unlock(&sharedMutex);
    return nullptr;
}

void* MediumPriorityTask(void* arg) {
    int id = *(int*)arg;
    struct sched_param param;
    param.sched_priority = 50;
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);

    while (!stopMediums.load()) {
        if (highWaiting.load()) {
            // Pause so low can run
            usleep(100'000);
            continue;
        }

        std::cout << "[Medium " << id << "] Working...\n";
        usleep(100'000);
    }

    std::cout << "[Medium " << id << "] Stopping.\n";
    return nullptr;
}

int main() {
    // Initialize mutex with default attributes
    pthread_mutex_init(&sharedMutex, nullptr);

    pthread_t low, high;
    pthread_create(&low, nullptr, LowPriorityTask, nullptr);
    pthread_create(&high, nullptr, HighPriorityTask, nullptr);

    const int N = 3;
    pthread_t mediums[N];
    int ids[N];
    for (int i = 0; i < N; ++i) {
        ids[i] = i;
        pthread_create(&mediums[i], nullptr, MediumPriorityTask, &ids[i]);
    }

    sleep(8); // Let things play out
    stopMediums = true;

    for (int i = 0; i < N; ++i)
        pthread_join(mediums[i], nullptr);

    pthread_join(low, nullptr);
    pthread_join(high, nullptr);

    pthread_mutex_destroy(&sharedMutex);
    std::cout << "\n[Main] Done.\n";
    return 0;
}
