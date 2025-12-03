
#ifndef OLEDDISPLAY_H
#define OLEDDISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_ILI9341.h>

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

#endif
