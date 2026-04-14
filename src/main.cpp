#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include "../include/common.h"

using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::vector;
using std::unique_ptr;
using std::make_unique;

int getValidatedInt(const string& prompt, int min, int max) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= min && value <= max) {
            cin.ignore(256, '\n');
            return value;
        }
        cerr << "Invalid input. Please enter a number between "
             << min << " and " << max << endl;
        cin.clear();
        cin.ignore(256, '\n');
    }
}

void printArray(const int* array, size_t size) {
    cout << "[";
    for (size_t i = 0; i < size; ++i) {
        cout << array[i];
        if (i < size - 1) cout << ", ";
    }
    cout << "]" << endl;
}

int main() {
    try {
        cout << "\n=== Threads Lab3 - Critical Sections & Events ===" << endl;
        cout << string(50, '=') << endl;
        
        // Step 1: Allocate array
        int arraySize = getValidatedInt("Enter array size: ", MIN_ARRAY_SIZE, MAX_ARRAY_SIZE);
        
        // Step 2: Initialize array with zeros
        auto array = make_unique<int[]>(arraySize);
        std::fill_n(array.get(), arraySize, 0);
        
        // Step 3: Get number of marker threads
        int markerCount = getValidatedInt("Enter number of markers: ", MIN_MARKERS, MAX_MARKERS);
        
        // Step 4: Create marker threads
        vector<MarkerData> markers(markerCount);
        vector<MarkerData*> markerPtrs(markerCount);
        
        for (int i = 0; i < markerCount; ++i) {
            markers[i].id = i + 1;
            markers[i].array = array.get();
            markers[i].arraySize = arraySize;
            markers[i].markedCount = 0;
            markers[i].blockedIndex = -1;
            markers[i].state = ThreadState::WAITING_FOR_START;
            markers[i].terminate = false;
            markers[i].shouldContinue = false;
            
            syncInit(markers[i].sync);
            markerPtrs[i] = &markers[i];
            
            #ifdef _WIN32
            markers[i].sync.threadHandle = CreateThread(NULL, 0, markerThread, &markers[i], 0, NULL);
            #else
            pthread_create(&markers[i].sync.thread, NULL, markerThread, &markers[i]);
            #endif
        }
        
        // Step 5: Signal all markers to start
        cout << "\nStarting all marker threads..." << endl;
        for (auto& marker : markers) {
            syncSignalStart(marker.sync);
        }
        
        // Step 6: Main loop
        vector<bool> activeMarkers(markerCount, true);
        int activeCount = markerCount;
        
        while (activeCount > 0) {
            // Step 6.1: Wait for all active markers to signal they can't proceed
            cout << "\nWaiting for all markers to block..." << endl;
            for (int i = 0; i < markerCount; ++i) {
                if (activeMarkers[i]) {
                    syncWaitForCantProceed(markers[i].sync);
                }
            }
            
            // Step 6.2: Print array contents
            cout << "\nCurrent array contents:" << endl;
            printArray(array.get(), arraySize);
            
            // Step 6.3: Ask which marker to terminate
            int markerToTerminate;
            do {
                markerToTerminate = getValidatedInt(
                    "Enter marker ID to terminate (1-" + std::to_string(markerCount) + "): ",
                    1, markerCount);
            } while (!activeMarkers[markerToTerminate - 1]);
            
            // Step 6.4: Signal selected marker to terminate
            cout << "Terminating marker " << markerToTerminate << "..." << endl;
            syncSignalTerminate(markers[markerToTerminate - 1].sync);
            
            // Step 6.5: Wait for marker to finish
            syncWaitForThread(markers[markerToTerminate - 1].sync);
            activeMarkers[markerToTerminate - 1] = false;
            activeCount--;
            
            // Step 6.6: Print array after marker cleanup
            cout << "\nArray after marker " << markerToTerminate << " cleanup:" << endl;
            printArray(array.get(), arraySize);
            
            // Step 6.7: Signal remaining markers to continue
            if (activeCount > 0) {
                cout << "Signaling remaining markers to continue..." << endl;
                for (int i = 0; i < markerCount; ++i) {
                    if (activeMarkers[i]) {
                        syncSignalContinue(markers[i].sync);
                    }
                }
            }
        }
        
        // Step 7: Cleanup
        cout << "\nAll markers finished. Final array:" << endl;
        printArray(array.get(), arraySize);
        
        for (auto& marker : markers) {
            syncDestroy(marker.sync);
        }
        
        cout << "\n=== Program completed successfully ===" << endl;
        
    } catch (const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
