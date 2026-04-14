#ifndef COMMON_H
#define COMMON_H

#include <cstddef>
#include <atomic>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

// Constants
static constexpr int MIN_ARRAY_SIZE = 1;
static constexpr int MAX_ARRAY_SIZE = 1000;
static constexpr int MIN_MARKERS = 1;
static constexpr int MAX_MARKERS = 10;
static constexpr int SLEEP_MS = 5;

// Thread states
enum class ThreadState {
    WAITING_FOR_START,
    RUNNING,
    CANT_PROCEED,
    WAITING_FOR_SIGNAL,
    TERMINATE,
    FINISHED
};

// Cross-platform synchronization primitives
#ifdef _WIN32
struct SyncPrimitives {
    HANDLE startEvent;
    HANDLE cantProceedEvent;
    HANDLE continueEvent;
    HANDLE terminateEvent;
    HANDLE threadHandle;
};
#else
struct SyncPrimitives {
    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_cond_t startCond;
    pthread_cond_t cantProceedCond;
    pthread_cond_t continueCond;
    pthread_cond_t terminateCond;
    std::atomic<int> startFlag;
    std::atomic<int> cantProceedFlag;
    std::atomic<int> continueFlag;
    std::atomic<int> terminateFlag;
};
#endif

// Marker thread data structure
struct MarkerData {
    int id;                      // Marker ID (1-based)
    int* array;                  // Pointer to shared array
    size_t arraySize;            // Size of array
    int markedCount;             // Number of marked elements
    int blockedIndex;            // Index that couldn't be marked (-1 if none)
    ThreadState state;           // Current thread state
    SyncPrimitives sync;         // Synchronization primitives
    bool terminate;              // Termination flag
    bool shouldContinue;         // Continue flag
};

// Thread function
#ifdef _WIN32
DWORD WINAPI markerThread(LPVOID param);
#else
void* markerThread(void* param);
#endif

// Utility functions
void sleepMs(int milliseconds);

// Synchronization helpers
void syncInit(SyncPrimitives& sync);
void syncDestroy(SyncPrimitives& sync);
void syncSignalStart(SyncPrimitives& sync);
void syncWaitForStart(SyncPrimitives& sync);
void syncSignalCantProceed(SyncPrimitives& sync);
void syncWaitForCantProceed(SyncPrimitives& sync);
void syncSignalContinue(SyncPrimitives& sync);
void syncWaitForContinue(SyncPrimitives& sync);
void syncSignalTerminate(SyncPrimitives& sync);
void syncWaitForTerminate(SyncPrimitives& sync);
void syncWaitForThread(SyncPrimitives& sync);
int syncCheckTerminate(SyncPrimitives& sync);
int syncCheckContinue(SyncPrimitives& sync);
void syncResetContinue(SyncPrimitives& sync);

#endif
