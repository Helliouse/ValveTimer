#line 1 "C:\\Users\\bradw\\OneDrive\\Documents\\Arduino\\ValveTimer\\oledDisplay.h"
#ifndef OLEDDISPLAY_H
#define OLEDDISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> 

/*
    OledDisplay class for managing an OLED display using the Adafruit SSD1306 library.
*/


class OledDisplay {
private:
    // Private members can be added here if needed
    String displayLines[7];
    uint8_t SCREEN_WIDTH = 128;
    uint8_t SCREEN_HEIGHT = 64;
    uint8_t OLED_RESET = -1;
    uint8_t OLED_ADDR = 0x3C;
    uint8_t SDA_PIN = 21;
    uint8_t SCL_PIN = 22;
    Adafruit_SSD1306 display;
    boolean initialized = false;
    boolean error = false;
    String errorString = "";

    GFXcanvas1 canvasTop;    // Canvas for top part of display
    GFXcanvas1 canvasBottom; // Canvas for bottom part of display

    void setupCanvas();

public:
    OledDisplay();
    OledDisplay(uint8_t width, uint8_t height, int8_t reset, uint8_t addr, uint8_t sda, uint8_t scl);
    ~OledDisplay();
    boolean isInitialized();
    boolean hasError();
    String getError();
    void updateDisplay();
    void updateDisplay(GFXcanvas1 topCanvas, GFXcanvas1 bottomCanvas);
    void updateDisplay(String line1, String line2, String line3, String line4, String line5 = "", String line6 = "", String line7 = "", String line8 = "");
    void updateDisplay(String lines[], uint8_t numberOfLines = 7);
    void updateDisplayCanvas();
    void updateDisplayCanvasTop();
    void updateDisplayCanvasBottom();
    void displayTop(String line1, String line2);
    void canvaseTopDraw(String line1, String line2);
    void displayBottom(uint8_t lineNumber, String line);
    void canvaseBottomDraw(uint8_t lineNumber, String line);
    void displayBottom(String lines[], uint8_t numberOfLines);
    void canvaseBottomDraw(String lines[], uint8_t numberOfLines);
    void clearDisplay();
    void clearCanvasTop();
    void clearCanvasBottom();

    void setSDA_PIN(uint8_t sda);
    void setSCL_PIN(uint8_t scl);
    void setOLED_ADDR(uint8_t addr);
    void setOLED_RESET(int8_t reset);
    void setSCREEN_WIDTH(uint8_t width);
    void setSCREEN_HEIGHT(uint8_t height);

    uint8_t getSDA_PIN();
    uint8_t getSCL_PIN();
    uint8_t getOLED_ADDR();
    int8_t getOLED_RESET();
    uint8_t getSCREEN_WIDTH();
    uint8_t getSCREEN_HEIGHT();

};

#endif // OLEDDISPLAY_H