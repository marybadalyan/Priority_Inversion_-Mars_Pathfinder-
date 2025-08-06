#include <windows.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <atomic>

std::mutex sharedMutex;
std::atomic<bool> highWaiting = false;
std::atomic<bool> stopMediums = false;

void SetAffinity() {
    SetThreadAffinityMask(GetCurrentThread(), 1); // Pin to CPU 0
}

void LowPriorityTask() {
    SetAffinity();
    HANDLE hThread = GetCurrentThread();
    SetThreadPriority(hThread, THREAD_PRIORITY_LOWEST);
    std::cout << "[Low] Trying to lock mutex\n";
    sharedMutex.lock();
    std::cout << "[Low] Locked mutex\n";

    // Simulate work
    for (int i = 0; i < 20; ++i) {
        std::cout << "[Low] Working...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "[Low] Releasing mutex\n";
    sharedMutex.unlock();
}

void HighPriorityTask() {
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Give low a head start
    SetAffinity();
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
    std::cout << "[High] Trying to lock mutex\n";
    highWaiting = true;

    sharedMutex.lock(); // Will block here
    highWaiting = false;

    std::cout << "[High] Acquired mutex!\n";
    sharedMutex.unlock();
}

void MediumPriorityTask(int id) {
    SetAffinity();
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

    while (!stopMediums.load()) {
        if (highWaiting.load()) {
            // High is waiting → back off to let low proceed
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        std::cout << "[Medium " << id << "] Working...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "[Medium " << id << "] Stopping.\n";
}

int main() {
    std::cout << "=== Priority Inversion Demo (Pause Mediums if High is Waiting) ===\n";

    std::thread low(LowPriorityTask);
    std::thread high(HighPriorityTask);

    std::vector<std::thread> mediums;
    for (int i = 0; i < 3; ++i) {
        mediums.emplace_back(MediumPriorityTask, i);
    }

    std::this_thread::sleep_for(std::chrono::seconds(8));
    stopMediums = true;

    for (auto& m : mediums) m.join();
    low.join();
    high.join();

    std::cout << "\n[Main] Done.\n";
    return 0;
}
