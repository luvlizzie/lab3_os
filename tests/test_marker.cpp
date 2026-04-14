#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../include/common.h"

using ::testing::Eq;

class MarkerTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(MarkerTest, Placeholder) {
    EXPECT_TRUE(true);
}
