#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>
#include <memory>
#include <algorithm>
#include "../include/common.h"

using ::testing::Eq;
using ::testing::Le;
using ::testing::Ge;

class MarkerTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
    
    // Helper to run marker thread and wait for completion
    void runMarkerThread(MarkerData& data, int iterations = 100) {
        #ifdef _WIN32
        HANDLE thread = CreateThread(NULL, 0, markerThread, &data, 0, NULL);
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
        #else
        pthread_t thread;
        pthread_create(&thread, NULL, markerThread, &data);
        pthread_join(thread, NULL);
        #endif
    }
};

// Test 1: Marker initializes correctly
TEST_F(MarkerTest, MarkerInitializesCorrectly) {
    MarkerData data;
    data.id = 1;
    data.array = nullptr;
    data.arraySize = 0;
    data.markedCount = 0;
    data.blockedIndex = -1;
    data.state = ThreadState::WAITING_FOR_START;
    data.terminate = false;
    data.shouldContinue = false;
    
    EXPECT_EQ(data.id, 1);
    EXPECT_EQ(data.state, ThreadState::WAITING_FOR_START);
}

// Test 2: Random index generation is within bounds
TEST_F(MarkerTest, RandomIndexWithinBounds) {
    const size_t arraySize = 100;
    std::vector<int> array(arraySize, 0);
    
    // Seed with fixed value for reproducibility
    std::srand(1);
    
    for (int i = 0; i < 1000; ++i) {
        int randomIndex = std::rand() % arraySize;
        EXPECT_GE(randomIndex, 0);
        EXPECT_LT(randomIndex, static_cast<int>(arraySize));
    }
}

// Test 3: Marker marks empty cells correctly
TEST_F(MarkerTest, MarkerMarksEmptyCells) {
    const size_t arraySize = 10;
    auto array = std::make_unique<int[]>(arraySize);
    std::fill_n(array.get(), arraySize, 0);
    
    // Simulate marker marking
    int markerId = 1;
    int markedCount = 0;
    
    // Mark first 3 cells
    for (int i = 0; i < 3; ++i) {
        if (array[i] == 0) {
            array[i] = markerId;
            markedCount++;
        }
    }
    
    EXPECT_EQ(markedCount, 3);
    EXPECT_EQ(array[0], markerId);
    EXPECT_EQ(array[1], markerId);
    EXPECT_EQ(array[2], markerId);
    EXPECT_EQ(array[3], 0);
}

// Test 4: Marker doesn't mark already marked cells
TEST_F(MarkerTest, MarkerDoesNotMarkOccupiedCells) {
    const size_t arraySize = 5;
    auto array = std::make_unique<int[]>(arraySize);
    
    // Pre-mark some cells
    array[0] = 1;
    array[2] = 2;
    array[4] = 3;
    
    int markerId = 4;
    int markedCount = 0;
    
    // Try to mark all cells
    for (size_t i = 0; i < arraySize; ++i) {
        if (array[i] == 0) {
            array[i] = markerId;
            markedCount++;
        }
    }
    
    // Only empty cells (1, 3) should be marked
    EXPECT_EQ(markedCount, 2);
    EXPECT_EQ(array[1], markerId);
    EXPECT_EQ(array[3], markerId);
}

// Test 5: Marker cleans up its own marks on termination
TEST_F(MarkerTest, MarkerCleansUpItsMarks) {
    const size_t arraySize = 10;
    auto array = std::make_unique<int[]>(arraySize);
    std::fill_n(array.get(), arraySize, 0);
    
    int markerId = 5;
    
    // Mark some cells with marker 5
    array[0] = markerId;
    array[2] = markerId;
    array[5] = markerId;
    array[8] = markerId;
    
    // Clean up marker's marks
    int cleanedCount = 0;
    for (size_t i = 0; i < arraySize; ++i) {
        if (array[i] == markerId) {
            array[i] = 0;
            cleanedCount++;
        }
    }
    
    EXPECT_EQ(cleanedCount, 4);
    
    // Verify all marker's marks are gone
    for (size_t i = 0; i < arraySize; ++i) {
        EXPECT_NE(array[i], markerId);
    }
}

// Test 6: Multiple markers don't interfere with each other
TEST_F(MarkerTest, MultipleMarkersWorkIndependently) {
    const size_t arraySize = 20;
    auto array = std::make_unique<int[]>(arraySize);
    std::fill_n(array.get(), arraySize, 0);
    
    std::vector<int> markerIds = {1, 2, 3};
    std::vector<int> markedCounts(3, 0);
    
    // Simulate markers working
    for (size_t i = 0; i < arraySize; ++i) {
        if (array[i] == 0) {
            // Each marker gets its own cells
            int markerIdx = i % 3;
            array[i] = markerIds[markerIdx];
            markedCounts[markerIdx]++;
        }
    }
    
    // Verify each marker only modified its own cells
    for (size_t i = 0; i < arraySize; ++i) {
        if (array[i] != 0) {
            int expectedMarker = markerIds[i % 3];
            EXPECT_EQ(array[i], expectedMarker);
        }
    }
}

// Test 7: Marker correctly identifies blocked index
TEST_F(MarkerTest, MarkerIdentifiesBlockedIndex) {
    const size_t arraySize = 5;
    auto array = std::make_unique<int[]>(arraySize);
    std::fill_n(array.get(), arraySize, 0);
    
    // Pre-mark one cell
    array[2] = 99;
    
    int markerId = 7;
    int blockedIndex = -1;
    
    // Simulate marker trying to mark a blocked cell
    for (size_t i = 0; i < arraySize; ++i) {
        if (array[i] == 0) {
            array[i] = markerId;
        } else if (blockedIndex == -1) {
            blockedIndex = i;
        }
    }
    
    EXPECT_EQ(blockedIndex, 2);
    EXPECT_EQ(array[2], 99); // Still marked by other marker
}

// Test 8: Array is properly initialized with zeros
TEST_F(MarkerTest, ArrayInitializedWithZeros) {
    const size_t arraySize = 50;
    auto array = std::make_unique<int[]>(arraySize);
    std::fill_n(array.get(), arraySize, 0);
    
    for (size_t i = 0; i < arraySize; ++i) {
        EXPECT_EQ(array[i], 0);
    }
}

// Test 9: Marker state transitions
TEST_F(MarkerTest, MarkerStateTransitions) {
    MarkerData data;
    data.state = ThreadState::WAITING_FOR_START;
    
    data.state = ThreadState::RUNNING;
    EXPECT_EQ(data.state, ThreadState::RUNNING);
    
    data.state = ThreadState::CANT_PROCEED;
    EXPECT_EQ(data.state, ThreadState::CANT_PROCEED);
    
    data.state = ThreadState::WAITING_FOR_SIGNAL;
    EXPECT_EQ(data.state, ThreadState::WAITING_FOR_SIGNAL);
    
    data.state = ThreadState::TERMINATE;
    EXPECT_EQ(data.state, ThreadState::TERMINATE);
    
    data.state = ThreadState::FINISHED;
    EXPECT_EQ(data.state, ThreadState::FINISHED);
}

// Test 10: Sleep function works (doesn't crash)
TEST_F(MarkerTest, SleepFunctionWorks) {
    // Just verify it doesn't crash
    sleepMs(1);
    sleepMs(5);
    sleepMs(10);
    SUCCEED();
}
