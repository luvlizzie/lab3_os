#include "../include/common.h"
#include <iostream>

using std::string;

// Global console mutex
#ifdef _WIN32
HANDLE consoleMutex = NULL;
#else
pthread_mutex_t consoleMutex = PTHREAD_MUTEX_INITIALIZER;
#endif

void sleepMs(int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}

void consolePrint(const string& message) {
#ifdef _WIN32
    if (consoleMutex) WaitForSingleObject(consoleMutex, INFINITE);
    std::cout << message << std::flush;
    if (consoleMutex) ReleaseMutex(consoleMutex);
#else
    pthread_mutex_lock(&consoleMutex);
    std::cout << message << std::flush;
    pthread_mutex_unlock(&consoleMutex);
#endif
}
