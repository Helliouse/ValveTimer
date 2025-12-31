#include <Valve.h>

Valve::Valve(uint16_t openTimeMinutes, uint16_t closedTimeMinutes, uint16_t cycleTimeMillis) {
    this->openTime = openTimeMinutes * 60000;   // Convert minutes to milliseconds
    this->closedTime = closedTimeMinutes * 60000; // Convert minutes to milliseconds
    //this->valveCycleTime = 3500; // Default valve cycle time in milliseconds for US Solid Model: USS-MSV00002
    this->valveCycleTime = valveCycleTime; // in milliseconds
    this->lastToggleTime = millis();
    this->isOpen = false; // Start with valve closed
    this->vavleInTransition = false;
    this->dwellTimeStart = 0;
}

Valve::Valve(uint16_t openTimeMinutes, uint16_t closedTimeMinutes, uint16_t cycleTimeMillis, uint8_t openPin, uint8_t closePin, uint8_t ledPin) : Valve(openTimeMinutes, closedTimeMinutes, cycleTimeMinutes) {
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

Valve::~Valve() {
    // Destructor logic if needed
}

void Valve::update() {
    unsigned long currentTime = millis();
    if (isOpen) {
        // Valve is currently open
        if ((currentTime - lastToggleTime >= openTime) && !vavleInTransition) {
            // Time to close the valve
            digitalWrite(valveClosePin, HIGH);
            //delay(valveCycleTime);
            valveDelay(currentTime);
            digitalWrite(valveClosePin, LOW);
            isOpen = false;
            lastToggleTime = currentTime;
            digitalWrite(valveLEDStatePin, LOW); // LED OFF when valve is closed
        }
    } else {
        // Valve is currently closed
        if ((currentTime - lastToggleTime >= closedTime) && !vavleInTransition) {
            // Time to open the valve
            digitalWrite(valveOpenPin, HIGH);
            //delay(valveCycleTime);
            valveDelay(currentTime);
            digitalWrite(valveOpenPin, LOW);
            isOpen = true;
            lastToggleTime = currentTime;
            digitalWrite(valveLEDStatePin, HIGH); // LED ON when valve is open
        }
    }
    valveDelay(); // Check and update transition state
}

bool Valve::getState() {
    return isOpen;
}

uint32_t Valve::getCurrentCycleTime() {
    unsigned long currentTime = millis();
    if (isOpen) {
        return (currentTime - lastToggleTime) / 60000; // Time since valve opened
    } else {
        return (currentTime - lastToggleTime) / 60000; // Time since valve closed
    }
}

void Valve::setOpenTime(uint16_t openTimeMinutes) {
    this->openTime = openTimeMinutes * 60000; // Convert minutes to milliseconds
}

void Valve::setClosedTime(uint16_t closedTimeMinutes) {
    this->closedTime = closedTimeMinutes * 60000; // Convert minutes to milliseconds
}

void Valve::setCycleTime(uint16_t cycleTime) {
    this->valveCycleTime = cycleTime; // in milliseconds
}

uint16_t Valve::getOpenTime() {
    return this->openTime / 60000; // Convert milliseconds to minutes
}

uint16_t Valve::getClosedTime() {
    return this->closedTime / 60000; // Convert milliseconds to minutes
}

uint16_t Valve::getCycleTime() {
    return this->valveCycleTime; // in milliseconds
}

private:

void Valve::valveDelay(uint32_t startTime) {
    //delay(this->valveCycleTime);
    this->dwellTimeStart = startTime;
    valveDelay();
}

void Valve::valveDelay() {
    //delay(this->valveCycleTime);
    if (!this->vavleInTransition && this->dwellTimeStart == 0) {
        this->vavleInTransition = true;
        this->dwellTimeStart = millis();
    }
    
    if (this->dwellTimeStart + this->valveCycleTime >= millis()) {
        vavleInTransition = false;
        this->dwellTimeStart = 0;
    }
}