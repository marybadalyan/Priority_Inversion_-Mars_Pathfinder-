#include <windows.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

std::mutex sharedMutex;
std::atomic<bool> stopMediums = false;

void SetAffinity() {
    SetThreadAffinityMask(GetCurrentThread(), 1); // Pin to CPU 0
}

void LowPriorityTask() {
    SetAffinity();
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
    std::cout << "[Low] Trying to lock mutex\n";
    sharedMutex.lock();
    std::cout << "[Low] Locked mutex\n";
    while (true) {
        // Never releases the mutex
    }
}

void HighPriorityTask() {
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Ensure low gets lock first
    SetAffinity();
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
    std::cout << "[High] Trying to lock mutex\n";
    sharedMutex.lock(); // Blocks forever
    std::cout << "[High] Acquired mutex (should never happen!)\n";
    sharedMutex.unlock();
}

void MediumPriorityTask(int id) {
    SetAffinity();
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
    while (!stopMediums.load()) {
        std::cout << "[Medium " << id << "] Working...\n";
    }
    std::cout << "[Medium " << id << "] Stopping.\n";
}

int main() {
    std::cout << "Starting priority inversion demo\n";

    std::thread low(LowPriorityTask);
    std::thread high(HighPriorityTask);

    std::vector<std::thread> mediums;
    for (int i = 0; i < 3; ++i) {
        mediums.emplace_back(MediumPriorityTask, i);
    }

    // Let the medium threads flood for 3 seconds
    std::this_thread::sleep_for(std::chrono::seconds(1));
    stopMediums = true;


    // Wait another 3 seconds to allow low and high to proceed
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "\n[Main] Time limit reached. Terminating program.\n";
    TerminateProcess(GetCurrentProcess(), 0);

    return 0; // Unreachable
}

