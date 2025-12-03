
#include "oledDisplay.h"

// ---------------- I2C Implementation ----------------
template<typename DisplayT, typename BeginPolicy>
OledDisplayI2C<DisplayT, BeginPolicy>::OledDisplayI2C(uint16_t w, uint16_t h, int8_t r, uint8_t a, uint8_t sdaPin, uint8_t sclPin)
    : width(w), height(h), reset(r), addr(a), sda(sdaPin), scl(sclPin), display(w, h, &Wire, r) {
    for (uint8_t i = 0; i < MAX_LINES; i++) { lines[i][0] = '\0'; dirty[i] = true; }
}

template<typename DisplayT, typename BeginPolicy>
bool OledDisplayI2C<DisplayT, BeginPolicy>::begin() {
    Wire.begin(sda, scl);
    if (!BeginPolicy::begin(display, addr)) return false;
    display.clearDisplay();
    return true;
}

template<typename DisplayT, typename BeginPolicy>
void OledDisplayI2C<DisplayT, BeginPolicy>::setLine(uint8_t i, const char* text) {
    if (i >= MAX_LINES) return;
    strncpy(lines[i], text ? text : "", LINE_CHARS);
    lines[i][LINE_CHARS] = '\0';
    dirty[i] = true;
}

template<typename DisplayT, typename BeginPolicy>
void OledDisplayI2C<DisplayT, BeginPolicy>::updateDisplay() {
    display.clearDisplay();
    display.setTextSize(1);
    for (uint8_t i = 0; i < MAX_LINES; i++) {
        display.setCursor(0, i * 8);
        display.print(lines[i]);
        dirty[i] = false;
    }
    display.display();
}

template<typename DisplayT, typename BeginPolicy>
void OledDisplayI2C<DisplayT, BeginPolicy>::updateDirtyLines() {
    for (uint8_t i = 0; i < MAX_LINES; i++) {
        if (!dirty[i]) continue;
        display.fillRect(0, i * 8, width, 8, 0);
        display.setCursor(0, i * 8);
        display.print(lines[i]);
        dirty[i] = false;
    }
    display.display();
}

template<typename DisplayT, typename BeginPolicy>
void OledDisplayI2C<DisplayT, BeginPolicy>::scrollUp(const char* newBottom) {
    for (uint8_t i = 0; i < MAX_LINES - 1; i++) {
        strcpy(lines[i], lines[i + 1]);
        dirty[i] = true;
    }
    setLine(MAX_LINES - 1, newBottom);
    updateDirtyLines();
}

template<typename DisplayT, typename BeginPolicy>
void OledDisplayI2C<DisplayT, BeginPolicy>::scrollDown(const char* newTop) {
    for (int8_t i = MAX_LINES - 1; i > 0; i--) {
        strcpy(lines[i], lines[i - 1]);
        dirty[i] = true;
    }
    setLine(0, newTop);
    updateDirtyLines();
}

// ---------------- SPI Implementation ----------------
template<typename DisplayT, typename BeginPolicy>
OledDisplaySPI<DisplayT, BeginPolicy>::OledDisplaySPI(uint16_t w, uint16_t h, int8_t csPin, int8_t dcPin, int8_t rstPin)
    : width(w), height(h), cs(csPin), dc(dcPin), rst(rstPin), customPins(false),
      display(&SPI, dcPin, csPin, rstPin),
      canvasTop1(w, 32), canvasBottom1(w, h - 32),
      canvasTop16(w, 32), canvasBottom16(w, h - 32),
      fgColor(ILI9341_WHITE), bgColor(ILI9341_BLACK) {
    for (uint8_t i = 0; i < MAX_LINES; i++) { lines[i][0] = '\0'; dirty[i] = true; }
}

template<typename DisplayT, typename BeginPolicy>
bool OledDisplaySPI<DisplayT, BeginPolicy>::begin(uint32_t freqHz, bool enableTransactions) {
    SPI.begin();
    spiSettings = SPISettings(freqHz, MSBFIRST, SPI_MODE0);
    useTransactions = enableTransactions;
    if (!BeginPolicy::begin(display, 0, freqHz)) return false;
    display.fillScreen(bgColor);
    return true;
}

template<typename DisplayT, typename BeginPolicy>
void OledDisplaySPI<DisplayT, BeginPolicy>::setLine(uint8_t i, const char* text) {
    if (i >= MAX_LINES) return;
    strncpy(lines[i], text ? text : "", LINE_CHARS);
    lines[i][LINE_CHARS] = '\0';
    dirty[i] = true;
}

template<typename DisplayT, typename BeginPolicy>
void OledDisplaySPI<DisplayT, BeginPolicy>::updateDisplay() {
    display.fillScreen(bgColor);
    display.setTextSize(2);
    display.setTextColor(fgColor);
    for (uint8_t i = 0; i < MAX_LINES; i++) {
        display.setCursor(4, 16 + i * 20);
        display.print(lines[i]);
        dirty[i] = false;
    }
}

template<typename DisplayT, typename BeginPolicy>
void OledDisplaySPI<DisplayT, BeginPolicy>::updateDirtyLines() {
    for (uint8_t i = 0; i < MAX_LINES; i++) {
        if (!dirty[i]) continue;
        display.fillRect(0, 16 + i * 20, width, 20, bgColor);
        display.setCursor(4, 16 + i * 20);
        display.print(lines[i]);
        dirty[i] = false;
    }
}

// Canvas16 methods
template<typename DisplayT, typename BeginPolicy>
void OledDisplaySPI<DisplayT, BeginPolicy>::clearCanvasTop16() { canvasTop16.fillScreen(bgColor); }

template<typename DisplayT, typename BeginPolicy>
void OledDisplaySPI<DisplayT, BeginPolicy>::canvasTop16Draw(const char* line1, const char* line2) {
    clearCanvasTop16();
    canvasTop16.setTextSize(2);
    canvasTop16.setTextColor(fgColor);
    canvasTop16.setCursor(0, 16);
    canvasTop16.print(line1);
    canvasTop16.setCursor(0, 32);
    canvasTop16.print(line2);
}

template<typename DisplayT, typename BeginPolicy>
void OledDisplaySPI<DisplayT, BeginPolicy>::updateDisplayCanvas16() {
    display.drawRGBBitmap(0, 0, canvasTop16.getBuffer(), width, 32);
    display.drawRGBBitmap(0, 32, canvasBottom16.getBuffer(), width, height - 32);
}

// Gradient helpers
template<typename DisplayT, typename BeginPolicy>
void OledDisplaySPI<DisplayT, BeginPolicy>::drawVerticalGradient(GFXcanvas16& canvas, uint16_t colorStart, uint16_t colorEnd) {
    for (int y = 0; y < canvas.height(); y++) {
        float ratio = (float)y / canvas.height();
        uint16_t color = canvas.color565(
            ((colorStart >> 11) & 0x1F) * (1 - ratio) + ((colorEnd >> 11) & 0x1F) * ratio,
            ((colorStart >> 5) & 0x3F) * (1 - ratio) + ((colorEnd >> 5) & 0x3F) * ratio,
            (colorStart & 0x1F) * (1 - ratio) + (colorEnd & 0x1F) * ratio
        );
        for (int x = 0; x < canvas.width(); x++) canvas.drawPixel(x, y, color);
    }
}

template<typename DisplayT, typename BeginPolicy>
void OledDisplaySPI<DisplayT, BeginPolicy>::blitImage(const uint16_t* img, uint16_t imgW, uint16_t imgH, int16_t x, int16_t y) {
    display.drawRGBBitmap(x, y, img, imgW, imgH);
}
