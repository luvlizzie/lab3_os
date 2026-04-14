#ifndef COMMON_H
#define COMMON_H

#include <cstddef>

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
static constexpr int MAX_MARKERS = 100;
static constexpr int SLEEP_MS = 5;

// Thread states
enum class ThreadState {
    WAITING_FOR_START,
    RUNNING,
    CANT_PROCEED,
    WAITING_FOR_SIGNAL,
    TERMINATE
};

// Marker thread data structure
struct MarkerData {
    int id;                      // Marker ID (1-based)
    int* array;                  // Pointer to shared array
    size_t arraySize;            // Size of array
    int markedCount;             // Number of marked elements
    int blockedIndex;            // Index that couldn't be marked (-1 if none)
    ThreadState state;           // Current thread state
    
    // Synchronization primitives
    #ifdef _WIN32
    HANDLE startEvent;           // Event to start work
    HANDLE cantProceedEvent;     // Event signaling can't proceed
    HANDLE continueEvent;        // Event to continue work
    HANDLE terminateEvent;       // Event to terminate
    HANDLE threadHandle;         // Thread handle
    #else
    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_cond_t startCond;
    pthread_cond_t cantProceedCond;
    pthread_cond_t continueCond;
    pthread_cond_t terminateCond;
    int startFlag;
    int cantProceedFlag;
    int continueFlag;
    int terminateFlag;
    #endif
};

// Thread function
#ifdef _WIN32
DWORD WINAPI markerThread(LPVOID param);
#else
void* markerThread(void* param);
#endif

// Utility functions
void sleepMs(int milliseconds);

#endif
