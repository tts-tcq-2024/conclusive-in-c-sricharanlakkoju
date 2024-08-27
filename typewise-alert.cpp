#include "typewise-alert.h"
#include <stdio.h>

// Define temperature ranges based on cooling type
TemperatureRange getTemperatureRange(CoolingType coolingType) {
    // Define temperature ranges for each cooling type
    TemperatureRange ranges[] = {
        {0, 35}, // PASSIVE_COOLING
        {0, 45}, // HI_ACTIVE_COOLING
        {0, 40}  // MED_ACTIVE_COOLING
    };
    return ranges[coolingType];
}

// Determine the breach type based on temperature and limits
BreachType inferBreach(double value, TemperatureRange range) {
    if (value < range.lowerLimit) {
        return TOO_LOW;
    }
    if (value > range.upperLimit) {
        return TOO_HIGH;
    }
    return NORMAL;
}

// Classify temperature breach based on cooling type and temperature
BreachType classifyTemperatureBreach(CoolingType coolingType, double temperatureInC) {
    TemperatureRange range = getTemperatureRange(coolingType);
    return inferBreach(temperatureInC, range);
}

// Handle alert based on target and temperature
void checkAndAlert(AlertTarget alertTarget, BatteryCharacter batteryChar, double temperatureInC) {
    BreachType breachType = classifyTemperatureBreach(batteryChar.coolingType, temperatureInC);

    switch (alertTarget) {
        case TO_CONTROLLER:
            sendToController(breachType);
            break;
        case TO_EMAIL:
            sendToEmail(breachType);
            break;
        default:
            printf("Unknown alert target\n");
            break;
    }
}

// Send alert to controller with specific format
void sendToController(BreachType breachType) {
    const unsigned short header = 0xfeed;
    printf("%04x : %d\n", header, breachType);
}

// Send alert to email based on breach type
void sendToEmail(BreachType breachType) {
    const char* recipient = "a.b@c.com";

    printf("To: %s\n", recipient);

    if (breachType == TOO_LOW) {
        printf("Hi, the temperature is too low\n");
    } else if (breachType == TOO_HIGH) {
        printf("Hi, the temperature is too high\n");
    } else {
        printf("Hi, the temperature is normal\n");
    }
}
