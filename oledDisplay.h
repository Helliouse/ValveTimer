#ifndef OLEDDISPLAY_H
#define OLEDDISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_ILI9341.h>

/*
    OledDisplay class for managing an OLED display using the Adafruit SSD1306 library.
*/

// ---------------- Policies ----------------
struct SSD1306BeginPolicy {
    static bool begin(Adafruit_SSD1306& d, uint8_t addr) {
        return d.begin(SSD1306_SWITCHCAPVCC, addr);
    }
};

struct SH110XBeginPolicy {
    template<typename T>
    static bool begin(T& d, uint8_t addr) {
        d.begin(addr, true);
        return true;
    }
};

struct ILI9341BeginPolicy {
    static bool begin(Adafruit_ILI9341& d, uint8_t, uint32_t freqHz = 0) {
        d.begin(freqHz);
        return true;
    }
};

// ---------------- I2C Display Template ----------------
template<typename DisplayT, typename BeginPolicy>
class OledDisplayI2C {
public:
    static constexpr uint8_t MAX_LINES = 7;
    static constexpr uint8_t LINE_CHARS = 21;

private:
    uint16_t width, height;
    int8_t reset;
    uint8_t addr, sda, scl;
    DisplayT display;
    char lines[MAX_LINES][LINE_CHARS + 1];
    bool dirty[MAX_LINES];

public:
    OledDisplayI2C(uint16_t w, uint16_t h, int8_t r, uint8_t a, uint8_t sdaPin, uint8_t sclPin);
    bool begin();
    void setLine(uint8_t i, const char* text);
    void updateDisplay();
    void updateDirtyLines();
    void scrollUp(const char* newBottom = "");
    void scrollDown(const char* newTop = "");
};

// ---------------- SPI Display Template ----------------
template<typename DisplayT, typename BeginPolicy = ILI9341BeginPolicy>
class OledDisplaySPI {
public:
    static constexpr uint8_t MAX_LINES = 12;
    static constexpr uint8_t LINE_CHARS = 26;

private:
    uint16_t width, height;
    int8_t cs, dc, rst;
    bool customPins;
    int8_t sck, miso, mosi;
    DisplayT display;
    SPISettings spiSettings;
    bool useTransactions;
    uint16_t fgColor, bgColor;
    char lines[MAX_LINES][LINE_CHARS + 1];
    bool dirty[MAX_LINES];

    // Canvases
    GFXcanvas1 canvasTop1, canvasBottom1;
    GFXcanvas16 canvasTop16, canvasBottom16;

public:
    OledDisplaySPI(uint16_t w, uint16_t h, int8_t csPin, int8_t dcPin, int8_t rstPin);
    OledDisplaySPI(uint16_t w, uint16_t h, int8_t csPin, int8_t dcPin, int8_t rstPin,
                   int8_t sckPin, int8_t misoPin, int8_t mosiPin);
    bool begin(uint32_t freqHz = 40000000UL, bool enableTransactions = false);
    void setLine(uint8_t i, const char* text);
    void updateDisplay();
    void updateDirtyLines();
    void scrollUp(const char* newBottom = "");
    void scrollDown(const char* newTop = "");

    // Canvas operations
    void clearCanvasTop16();
    void clearCanvasBottom16();
    void canvasTop16Draw(const char* line1, const char* line2);
    void canvasBottom16Draw(const char* lines[], uint8_t count);
    void updateDisplayCanvas16();

    // Gradient & image helpers
    void drawVerticalGradient(GFXcanvas16& canvas, uint16_t colorStart, uint16_t colorEnd);
    void drawHorizontalGradient(GFXcanvas16& canvas, uint16_t colorStart, uint16_t colorEnd);
    void blitImage(const uint16_t* img, uint16_t imgW, uint16_t imgH, int16_t x, int16_t y);
};

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
    Adafruit_SSD1306* display;
    boolean initialized = false;
    boolean error = false;
    String errorString = "";

    GFXcanvas1* canvasTop;    // Canvas for top part of display
    GFXcanvas1* canvasBottom; // Canvas for bottom part of display

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