#include "typewise-alert.h"
#include <stdio.h>


// Function to get the temperature range based on cooling type
TemperatureRange getTemperatureRange(CoolingType coolingType) {
  TemperatureRange ranges[] = {
    {0, 35}, // PASSIVE_COOLING
    {0, 45}, // HI_ACTIVE_COOLING
    {0, 40}  // MED_ACTIVE_COOLING
  };
  return ranges[coolingType];
}

// Function to infer breach type based on value and limits
BreachType inferBreach(double value, TemperatureRange range) {
  if (value < range.lowerLimit) {
    return TOO_LOW;
  }
  if (value > range.upperLimit) {
    return TOO_HIGH;
  }
  return NORMAL;
}

// Function to classify temperature breach based on cooling type and temperature
BreachType classifyTemperatureBreach(CoolingType coolingType, double temperatureInC) {
  TemperatureRange range = getTemperatureRange(coolingType);
  return inferBreach(temperatureInC, range);
}

void checkAndAlert(
    AlertTarget alertTarget, BatteryCharacter batteryChar, double temperatureInC) {

  BreachType breachType = classifyTemperatureBreach(
    batteryChar.coolingType, temperatureInC
  );

  switch(alertTarget) {
    case TO_CONTROLLER:
      sendToController(breachType);
      break;
    case TO_EMAIL:
      sendToEmail(breachType);
      break;
  }
}

void sendToController(BreachType breachType) {
  const unsigned short header = 0xfeed;
  printf("%x : %x\n", header, breachType);
}
void sendToEmail(BreachType breachType) {
    const char* recipient = "a.b@c.com";
    const BreachMessage breachMessages[] = {
        {TOO_LOW, "Hi, the temperature is too low"},
        {TOO_HIGH, "Hi, the temperature is too high"}
    };
    const int messageCount = sizeof(breachMessages) / sizeof(breachMessages[0]);
 
    for (int i = 0; i < messageCount; i++) {
        if (breachMessages[i].type == breachType) {
            printf("To: %s\n", recipient);
            printf("%s\n", breachMessages[i].message);
            break;
        }
    }
}
