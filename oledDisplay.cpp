#include <oledDisplay.h>

OledDisplay::OledDisplay() {
    // Constructor can initialize display if needed
    this->display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
    Wire.begin(SDA_PIN, SCL_PIN); // SDA, SCL
    if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)){
        //Serial.println(F("SSH1306 allocation failed"));
        this->error = true;
        this->errorString = "SSD1306 allocation failed";
        this->initialized = false;
    }
    //display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
    this->display.clearDisplay();
}

OledDisplay::OledDisplay(uint8_t width, uint8_t height, int8_t reset, uint8_t addr, uint8_t sda, uint8_t scl) {
    this->SCREEN_WIDTH = width;
    this->SCREEN_HEIGHT = height;
    this->OLED_RESET = reset;
    this->OLED_ADDR = addr;
    this->SDA_PIN = sda;
    this->SCL_PIN = scl;
    this->display(this->SCREEN_WIDTH, this->SCREEN_HEIGHT, &Wire, this->OLED_RESET);
    Wire.begin(SDA_PIN, SCL_PIN); // SDA, SCL
    if(!this->display.begin(SSD1306_SWITCHCAPVCC, this->OLED_ADDR)){
        //Serial.println(F("SSH1306 allocation failed"));
        this->error = true;
        this->errorString = "SSD1306 allocation failed";
        this->initialized = false;
    }
    //display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
    this->display.clearDisplay();
}

OledDisplay::~OledDisplay() {
    // Destructor can clean up resources if needed
}

void OledDisplay::updateDisplay() {
    // Update the display with current displayLines content
    this->clearDisplay();
    for (uint8_t i = 0; i < 7; i++) {
        this->display.setCursor(0, i * 8);
        this->display.println(this->displayLines[i]);
    }
    this->display.display();
}

void OledDisplay::updateDisplay(String line1, String line2, String line3, String line4, String line5, String line6, String line7, String line8) {
    this->displayLines[0] = line1;
    this->displayLines[1] = line2;
    this->displayLines[2] = line3;
    this->displayLines[3] = line4;
    this->displayLines[4] = line5;
    this->displayLines[5] = line6;
    this->displayLines[6] = line7;
    this->updateDisplay();
}

void OledDisplay::updateDisplay(String lines[], uint8_t numberOfLines) {
    for (uint8_t i = 0; i < numberOfLines && i < 7; i++) {
        this->displayLines[i] = lines[i];
    }
    this->updateDisplay();
}

void OledDisplay::displayTop(String line1, String line2) {
    this->display.setCursor(0, 0);
    this->display.println(line1);
    this->display.setCursor(0, 8);
    this->display.println(line2);
    this->display.display();
}

void OledDisplay::displayBottom(uint8_t lineNumber, String line) {
    if (lineNumber >= 3 && lineNumber <= 7) {
        this->display.setCursor(0, (lineNumber - 1) * 8);
        this->display.println(line);
        this->display.display();
    }
}

void OledDisplay::displayBottom(String lines[], uint8_t numberOfLines) {
    for (uint8_t i = 0; i < numberOfLines && (i + 3) <= 7; i++) {
        this->display.setCursor(0, (i + 2) * 8);
        this->display.println(lines[i]);
    }
    this->display.display();
}

void OledDisplay::clearDisplay() {
    this->display.clearDisplay();
}

boolean OledDisplay::isInitialized() {
    return this->initialized;
}

boolean OledDisplay::hasError() {
    return this->error;
}

String OledDisplay::getError() {
    return this->errorString;
}

void OledDisplay::setSDA_PIN(uint8_t sda) {
    this->SDA_PIN = sda;
}

void OledDisplay::setSCL_PIN(uint8_t scl) {
    this->SCL_PIN = scl;
}

void OledDisplay::setOLED_ADDR(uint8_t addr) {
    this->OLED_ADDR = addr;
}

void OledDisplay::setOLED_RESET(int8_t reset) {
    this->OLED_RESET = reset;
}

void OledDisplay::setSCREEN_WIDTH(uint8_t width) {
    this->SCREEN_WIDTH = width;
}

void OledDisplay::setSCREEN_HEIGHT(uint8_t height) {
    this->SCREEN_HEIGHT = height;
}

uint8_t OledDisplay::getSDA_PIN() {
    return this->SDA_PIN;
}

uint8_t OledDisplay::getSCL_PIN() {
    return this->SCL_PIN;
}

uint8_t OledDisplay::getOLED_ADDR() {
    return this->OLED_ADDR;
}

int8_t OledDisplay::getOLED_RESET() {
    return this->OLED_RESET;
}

uint8_t OledDisplay::getSCREEN_WIDTH() {
    return this->SCREEN_WIDTH;
}

uint8_t OledDisplay::getSCREEN_HEIGHT() {
    return this->SCREEN_HEIGHT;
}