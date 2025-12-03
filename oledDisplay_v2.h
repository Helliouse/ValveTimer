
#ifndef OLEDDISPLAY_H
#define OLEDDISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---- BeginPolicy for SSD1306 ----
struct SSD1306BeginPolicy {
    static bool begin(Adafruit_SSD1306& d, uint8_t addr) {
        return d.begin(SSD1306_SWITCHCAPVCC, addr);
    }
};

// ---- Template Class ----
template<typename DisplayT, typename BeginPolicy = SSD1306BeginPolicy>
class OledDisplay {
private:
    static constexpr uint8_t MAX_LINES = 7;
    static constexpr uint8_t TOP_HEIGHT = 16;
    static constexpr uint8_t LINE_CHARS = 21; // ~21 chars per line at textSize=1

    uint16_t SCREEN_WIDTH;
    uint16_t SCREEN_HEIGHT;
    int8_t OLED_RESET;
    uint8_t OLED_ADDR;
    uint8_t SDA_PIN;
    uint8_t SCL_PIN;

    DisplayT display;
    GFXcanvas1 canvasTop;
    GFXcanvas1 canvasBottom;

    bool initialized = false;
    bool error = false;
    const char* errorString = nullptr;

    char displayLines[MAX_LINES][LINE_CHARS + 1]; // Fixed buffers

public:
    // Constructor
    OledDisplay(uint16_t width = 128, uint16_t height = 64,
                int8_t reset = -1, uint8_t addr = 0x3C,
                uint8_t sda = 21, uint8_t scl = 22)
        : SCREEN_WIDTH(width), SCREEN_HEIGHT(height),
          OLED_RESET(reset), OLED_ADDR(addr),
          SDA_PIN(sda), SCL_PIN(scl),
          display(width, height, &Wire, reset),
          canvasTop(width, TOP_HEIGHT),
          canvasBottom(width, height - TOP_HEIGHT) {
        for (uint8_t i = 0; i < MAX_LINES; i++) displayLines[i][0] = '\0';
    }

    bool begin() {
        Wire.begin(SDA_PIN, SCL_PIN);
        if (!BeginPolicy::begin(display, OLED_ADDR)) {
            error = true;
            errorString = "Display begin() failed";
            initialized = false;
            return false;
        }
        initialized = true;
        display.clearDisplay();
        return true;
    }

    bool isInitialized() const { return initialized; }
    bool hasError() const { return error; }
    const char* getError() const { return errorString; }

    void clearDisplay() { display.clearDisplay(); }

    void updateDisplay() {
        clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        for (uint8_t i = 0; i < MAX_LINES; i++) {
            display.setCursor(0, i * 8);
            display.print(displayLines[i]);
        }
        display.display();
    }

    void updateDisplay(const char* lines[], uint8_t numberOfLines = MAX_LINES) {
        uint8_t n = (numberOfLines > MAX_LINES) ? MAX_LINES : numberOfLines;
        for (uint8_t i = 0; i < n; i++) setLine(i, lines[i]);
        updateDisplay();
    }

    void setLine(uint8_t i, const char* text) {
        if (i >= MAX_LINES) return;
        strncpy(displayLines[i], text ? text : "", LINE_CHARS);
        displayLines[i][LINE_CHARS] = '\0';
    }

    // Canvas operations
    void clearCanvasTop() { canvasTop.fillScreen(0); }
    void clearCanvasBottom() { canvasBottom.fillScreen(0); }

    void canvasTopDraw(const char* line1, const char* line2) {
        clearCanvasTop();
        canvasTop.setTextSize(1);
        canvasTop.setTextColor(SSD1306_WHITE);
        canvasTop.setCursor(0, 8); canvasTop.print(line1);
        canvasTop.setCursor(0, 16); canvasTop.print(line2);
    }

    void canvasBottomDraw(const char* lines[], uint8_t count) {
        clearCanvasBottom();
        canvasBottom.setTextSize(1);
        canvasBottom.setTextColor(SSD1306_WHITE);
        for (uint8_t i = 0; i < count; i++) {
            canvasBottom.setCursor(0, (i + 1) * 8);
            canvasBottom.print(lines[i]);
        }
    }

    void updateDisplayCanvas() {
        clearDisplay();
        display.drawBitmap(0, 0, canvasTop.getBuffer(), SCREEN_WIDTH, TOP_HEIGHT, SSD1306_WHITE);
        display.drawBitmap(0, TOP_HEIGHT, canvasBottom.getBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT - TOP_HEIGHT, SSD1306_WHITE);
        display.display();
    }
};

#endif // OLEDDISPLAY_H
