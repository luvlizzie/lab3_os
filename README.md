# Lab3_OS - Thread Synchronization

## Description
A cross-platform C++ program demonstrating thread synchronization using:
- **Critical sections** (mutexes on POSIX)
- **Events** (condition variables on POSIX)

## How It Works
1. Main thread creates an array of integers (initialized to zero)
2. Multiple marker threads are created
3. Each marker thread:
   - Generates random indices
   - Marks empty cells with its ID
   - Sleeps 5ms before and after marking
   - When blocked, reports and waits for main's signal

## Building
```bash
chmod +x run_tests.sh
./run_tests.sh
```

## Running
```bash
cd build
./markers.exe
```

## Requirements Met
| Requirement | Implementation |
|-------------|----------------|
| Array allocation | `make_unique<int[]>` |
| Markers count input | `getValidatedInt()` |
| Signal to start | `syncSignalStart()` |
| Wait for all markers | `syncWaitForCantProceed()` |
| Print array | `printArray()` |
| Terminate marker | `syncSignalTerminate()` |
| Cleanup marks | Loop clearing marker's cells |
| Signal continue | `syncSignalContinue()` |
| 5ms sleeps | `sleepMs(SLEEP_MS)` |
```
