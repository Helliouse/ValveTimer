#ifndef VALVE_H
#define VALVE_H
#include <Arduino.h>

class VALVE {
  private:
    uint16_t openTime;    // Time the valve remains open (in milliseconds)
    uint16_t closedTime;  // Time the valve remains closed (in milliseconds)
    uint16_t valveCycleTime; // Total cycle time time it takes the valve to open or close (in milliseconds)
    unsigned long lastToggleTime; // Last time the valve state was toggled
    bool isOpen;          // Current state of the valve
    uint8_t valveOpenPin;     // Pin controlling the opening of the valve
    uint8_t valveClosePin;    // Pin controlling the closing of the valve
    uint8_t valveLEDStatePin; // Pin for valve state LED ON = OPEN, OFF = CLOSED

  public:
    Valve(uint16_t openTimeMinutes, uint16_t closedTimeMinutes, uint16_t cycleTimeMinutes);
    Valve(uint16_t openTimeMinutes, uint16_t closedTimeMinutes, uint16_t cycleTimeMinutes, uint8_t openPin, uint8_t closePin, uint8_t ledPin);
    ~Valve();
    void update();
    bool getState();
    void setOpenTime(uint16_t openTimeMinutes);
    void setClosedTime(uint16_t closedTimeMinutes);
    void setCycleTime(uint16_t cycleTime);
    uint16_t getOpenTime();
    uint16_t getClosedTime();
    uint16_t getCycleTime();
};

#endif // VALVE_H