#line 1 "C:\\Users\\bradw\\OneDrive\\Documents\\Arduino\\ValveTimer\\Valve.cpp"
#include <Valve.h>

VALVE::Valve(uint16_t openTimeMinutes, uint16_t closedTimeMinutes, uint16_t cycleTimeMinutes) {
    this->openTime = openTimeMinutes * 60000;   // Convert minutes to milliseconds
    this->closedTime = closedTimeMinutes * 60000; // Convert minutes to milliseconds
    //this->valveCycleTime = 3500; // Default valve cycle time in milliseconds for US Solid Model: USS-MSV00002
    this->valveCycleTime = valveCycleTime; // in milliseconds
    this->lastToggleTime = millis();
    this->isOpen = false; // Start with valve closed
}

VALVE::Valve(uint16_t openTimeMinutes, uint16_t closedTimeMinutes, uint16_t cycleTimeMinutes, uint8_t openPin, uint8_t closePin, uint8_t ledPin) : VALVE(openTimeMinutes, closedTimeMinutes, cycleTimeMinutes) {
    this->valveOpenPin = openPin;
    this->valveClosePin = closePin;
    this->valveLEDStatePin = ledPin;
    pinMode(this->valveOpenPin, OUTPUT);
    pinMode(this->valveClosePin, OUTPUT);
    pinMode(this->valveLEDStatePin, OUTPUT);
    digitalWrite(this->valveOpenPin, LOW);
    digitalWrite(this->valveClosePin, LOW);
    digitalWrite(this->valveLEDStatePin, LOW); // LED OFF when valve is closed
}

VALVE::~VALVE() {
    // Destructor logic if needed
}

VALVE::update() {
    unsigned long currentTime = millis();
    if (isOpen) {
        // Valve is currently open
        if (currentTime - lastToggleTime >= openTime) {
            // Time to close the valve
            digitalWrite(valveClosePin, HIGH);
            delay(valveCycleTime);
            digitalWrite(valveClosePin, LOW);
            isOpen = false;
            lastToggleTime = currentTime;
            digitalWrite(valveLEDStatePin, LOW); // LED OFF when valve is closed
        }
    } else {
        // Valve is currently closed
        if (currentTime - lastToggleTime >= closedTime) {
            // Time to open the valve
            digitalWrite(valveOpenPin, HIGH);
            delay(valveCycleTime);
            digitalWrite(valveOpenPin, LOW);
            isOpen = true;
            lastToggleTime = currentTime;
            digitalWrite(valveLEDStatePin, HIGH); // LED ON when valve is open
        }
    }
}

bool VALVE::getState() {
    return isOpen;
}

void VALVE::setOpenTime(uint16_t openTimeMinutes) {
    this->openTime = openTimeMinutes * 60000; // Convert minutes to milliseconds
}

void VALVE::setClosedTime(uint16_t closedTimeMinutes) {
    this->closedTime = closedTimeMinutes * 60000; // Convert minutes to milliseconds
}

void VALVE::setCycleTime(uint16_t cycleTime) {
    this->valveCycleTime = cycleTime; // in milliseconds
}

uint16_t VALVE::getOpenTime() {
    return this->openTime / 60000; // Convert milliseconds to minutes
}

uint16_t VALVE::getClosedTime() {
    return this->closedTime / 60000; // Convert milliseconds to minutes
}

uint16_t VALVE::getCycleTime() {
    return this->valveCycleTime; // in milliseconds
}

