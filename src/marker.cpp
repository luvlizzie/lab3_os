#include <iostream>
#include <cstdlib>
#include "../include/common.h"

using std::cout;
using std::cerr;
using std::endl;
using std::to_string;

#ifdef _WIN32
DWORD WINAPI markerThread(LPVOID param) {
#else
void* markerThread(void* param) {
#endif
    MarkerData* data = static_cast<MarkerData*>(param);
    
    // Initialize random number generator with marker ID
    std::srand(data->id);
    
    // Wait for start signal from main
    syncWaitForStart(data->sync);
    data->state = ThreadState::RUNNING;
    data->terminate = false;
    data->shouldContinue = false;
    
    consolePrint("Marker " + std::to_string(data->id) + " started working\n");
    
    while (!data->terminate) {
        // Generate random index
        int randomIndex = std::rand() % data->arraySize;
        
        // Check if cell is empty (zero)
        if (data->array[randomIndex] == 0) {
            // Sleep 5ms before marking
            sleepMs(SLEEP_MS);
            
            // Mark the cell with marker ID
            data->array[randomIndex] = data->id;
            data->markedCount++;
            
            // Sleep 5ms after marking
            sleepMs(SLEEP_MS);
            
            // Continue to next iteration
            continue;
        } else {
            // Cell is already marked
            data->blockedIndex = randomIndex;
            data->state = ThreadState::CANT_PROCEED;
            
            // Print information
            consolePrint("Marker " + std::to_string(data->id) +
                        ": marked " + std::to_string(data->markedCount) +
                        " elements, cannot mark index " + std::to_string(randomIndex) + "\n");
            
            // Signal main thread that we can't proceed
            syncSignalCantProceed(data->sync);
            
            // Wait for response from main (continue or terminate)
            data->state = ThreadState::WAITING_FOR_SIGNAL;
            data->shouldContinue = false;
            
            // Wait for either continue or terminate signal
            while (!data->terminate && !data->shouldContinue) {
                if (syncCheckTerminate(data->sync)) {
                    data->terminate = true;
                    break;
                }
                if (syncCheckContinue(data->sync)) {
                    data->shouldContinue = true;
                    syncResetContinue(data->sync);
                    break;
                }
                sleepMs(1);
            }
            
            if (data->terminate) {
                break;
            }
            
            // Continue working
            data->state = ThreadState::RUNNING;
            data->blockedIndex = -1;
        }
    }
    
    // Clean up: set marked cells to zero
    int cleanedCount = 0;
    for (size_t i = 0; i < data->arraySize; ++i) {
        if (data->array[i] == data->id) {
            data->array[i] = 0;
            cleanedCount++;
        }
    }
    
    consolePrint("Marker " + std::to_string(data->id) + " finished. Cleaned " +
                std::to_string(cleanedCount) + " cells (marked " +
                std::to_string(data->markedCount) + ").\n");
    
    data->state = ThreadState::FINISHED;
    
#ifdef _WIN32
    return 0;
#else
    return nullptr;
#endif
}
