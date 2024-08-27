#include <gtest/gtest.h>
#include "typewise-alert.h"

// Helper function to capture stdout output
std::string captureStdout(void (*func)()) {
    testing::internal::CaptureStdout();
    func();
    return testing::internal::GetCapturedStdout();
}

// Test for getTemperatureRange function
TEST(TypeWiseAlertTests, GetTemperatureRange) {
    // Test for PASSIVE_COOLING
    TemperatureRange range = getTemperatureRange(PASSIVE_COOLING);
    EXPECT_EQ(range.lowerLimit, 0);
    EXPECT_EQ(range.upperLimit, 35);

    // Test for HI_ACTIVE_COOLING
    range = getTemperatureRange(HI_ACTIVE_COOLING);
    EXPECT_EQ(range.lowerLimit, 0);
    EXPECT_EQ(range.upperLimit, 45);

    // Test for MED_ACTIVE_COOLING
    range = getTemperatureRange(MED_ACTIVE_COOLING);
    EXPECT_EQ(range.lowerLimit, 0);
    EXPECT_EQ(range.upperLimit, 40);

    // Test for invalid cooling type
    TemperatureRange invalidRange = getTemperatureRange(static_cast<CoolingType>(999)); // out of bounds
    EXPECT_EQ(invalidRange.lowerLimit, 0); // Expecting default value
    EXPECT_EQ(invalidRange.upperLimit, 0); // Expecting default value
}

// Test for inferBreach function
TEST(TypeWiseAlertTests, InferBreach) {
    TemperatureRange range = {0, 35};

    EXPECT_EQ(inferBreach(-1, range), TOO_LOW);
    EXPECT_EQ(inferBreach(36, range), TOO_HIGH);
    EXPECT_EQ(inferBreach(20, range), NORMAL);
}

// Test for classifyTemperatureBreach function
TEST(TypeWiseAlertTests, ClassifyTemperatureBreach) {
    // PASSIVE_COOLING range: 0 to 35
    EXPECT_EQ(classifyTemperatureBreach(PASSIVE_COOLING, -1), TOO_LOW);
    EXPECT_EQ(classifyTemperatureBreach(PASSIVE_COOLING, 0), NORMAL);
    EXPECT_EQ(classifyTemperatureBreach(PASSIVE_COOLING, 35), NORMAL);
    EXPECT_EQ(classifyTemperatureBreach(PASSIVE_COOLING, 36), TOO_HIGH);

    // HI_ACTIVE_COOLING range: 0 to 45
    EXPECT_EQ(classifyTemperatureBreach(HI_ACTIVE_COOLING, -1), TOO_LOW);
    EXPECT_EQ(classifyTemperatureBreach(HI_ACTIVE_COOLING, 0), NORMAL);
    EXPECT_EQ(classifyTemperatureBreach(HI_ACTIVE_COOLING, 45), NORMAL);
    EXPECT_EQ(classifyTemperatureBreach(HI_ACTIVE_COOLING, 46), TOO_HIGH);

    // MED_ACTIVE_COOLING range: 0 to 40
    EXPECT_EQ(classifyTemperatureBreach(MED_ACTIVE_COOLING, -1), TOO_LOW);
    EXPECT_EQ(classifyTemperatureBreach(MED_ACTIVE_COOLING, 0), NORMAL);
    EXPECT_EQ(classifyTemperatureBreach(MED_ACTIVE_COOLING, 40), NORMAL);
    EXPECT_EQ(classifyTemperatureBreach(MED_ACTIVE_COOLING, 41), TOO_HIGH);
}

// Test for sendToController function
TEST(TypeWiseAlertTests, SendToController) {
    // Test TOO_LOW
    std::string output = captureStdout([]() { sendToController(TOO_LOW); });
    EXPECT_EQ(output, "feed : 1\n");

    // Test TOO_HIGH
    output = captureStdout([]() { sendToController(TOO_HIGH); });
    EXPECT_EQ(output, "feed : 2\n");

    // Test NORMAL
    output = captureStdout([]() { sendToController(NORMAL); });
    EXPECT_EQ(output, "feed : 0\n");
}

// Test for sendToEmail function
TEST(TypeWiseAlertTests, SendToEmail) {
    // Test TOO_LOW
    std::string output = captureStdout([]() { sendToEmail(TOO_LOW); });
    EXPECT_EQ(output, "To: a.b@c.com\nHi, the temperature is too low\n");

    // Test TOO_HIGH
    output = captureStdout([]() { sendToEmail(TOO_HIGH); });
    EXPECT_EQ(output, "To: a.b@c.com\nHi, the temperature is too high\n");

    // Test NORMAL
    output = captureStdout([]() { sendToEmail(NORMAL); });
    EXPECT_EQ(output, "To: a.b@c.com\nHi, the temperature is normal\n");
}

// Test for checkAndAlert function
TEST(TypeWiseAlertTests, CheckAndAlert) {
    BatteryCharacter batteryChar = {PASSIVE_COOLING, "BrandX"};

    // Test TO_CONTROLLER alerts
    std::string output = captureStdout([&]() { checkAndAlert(TO_CONTROLLER, batteryChar, 36); });
    EXPECT_EQ(output, "feed : 2\n");

    output = captureStdout([&]() { checkAndAlert(TO_CONTROLLER, batteryChar, -1); });
    EXPECT_EQ(output, "feed : 1\n");

    output = captureStdout([&]() { checkAndAlert(TO_CONTROLLER, batteryChar, 30); });
    EXPECT_EQ(output, "feed : 0\n");

    // Test TO_EMAIL alerts
    output = captureStdout([&]() { checkAndAlert(TO_EMAIL, batteryChar, -5); });
    EXPECT_EQ(output, "To: a.b@c.com\nHi, the temperature is too low\n");

    output = captureStdout([&]() { checkAndAlert(TO_EMAIL, batteryChar, 50); });
    EXPECT_EQ(output, "To: a.b@c.com\nHi, the temperature is too high\n");

    output = captureStdout([&]() { checkAndAlert(TO_EMAIL, batteryChar, 30); });
    EXPECT_EQ(output, "To: a.b@c.com\nHi, the temperature is normal\n");

    // Test invalid alert target
    output = captureStdout([&]() { checkAndAlert(static_cast<AlertTarget>(999), batteryChar, 50); });
    EXPECT_EQ(output, "Unknown alert target\n");
}
