
#include <Wire.h>
#include "oledDisplay.h"

// Alias for SSD1306 using our template
using MyOLED = OledDisplayI2C<Adafruit_SSD1306, SSD1306BeginPolicy>;

// Create display object (128x64 OLED)
MyOLED oled(128, 64, -1, 0x3C, SDA, SCL);

void setup() {
  Serial.begin(115200);
  delay(100);

  if (!oled.begin()) {
    Serial.println("SSD1306 init failed!");
    while (true) {}
  }

  Serial.println("SSD1306 initialized.");

  // Set initial lines
  const char* lines[] = {
    "Line 1: Hello",
    "Line 2: OLED",
    "Line 3: Demo",
    "Line 4: Scroll",
    "Line 5: Partial",
    "Line 6: Update",
    "Line 7: Enjoy!"
  };
  oled.updateDisplay(lines, 7);

  // Start marquee on top canvas
  oled.startMarqueeTop("Marquee scrolling text!", 2);
}

void loop() {
  static unsigned long lastScroll = 0;
  static unsigned long lastMarquee = 0;

  unsigned long now = millis();

  // Scroll up every 2 seconds
  if (now - lastScroll > 2000) {
    oled.scrollUp("New line added!");
    lastScroll = now;
  }

  // Update marquee every 50 ms
  if (now - lastMarquee > 50) {
    oled.tickMarqueeTop();
    lastMarquee = now;
  }
}
