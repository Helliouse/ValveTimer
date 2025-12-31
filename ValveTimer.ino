
#include "MENU.h"
#include <Valve.h>
#include <ezButton.h>

#define VALVE2_OPEN_PIN 13
#define VALVE2_CLOSE_PIN 12
#define VALVE2_LED_PIN 14
#define BUTTON_1 32
#define BUTTON_2 33
#define BUTTON_3 34
#define VALVE_OPEN_PIN 25
#define VALVE_CLOSE_PIN 26
#define VALVE_LED_PIN 27HardWire
#define OLED_ADDR 0x3C
#define SDA_PIN 21
#define SCL_PIN 22
#define BUTTON_DEBOUNCE_TIME 50

Menu mainMenu(128, 64, -1, 0x3C, 21, 22, false);


ezButton btnSelect(BUTTON_1);
ezButton btnEnter(BUTTON_2);
ezButton btnMinus(BUTTON_3);

uint8_t stateValve2Open = LOW;
uint8_t stateValve2Close = LOW;
uint8_t stateValve2LED = LOW;
uint8_t stateLED_1 = LOW;
uint8_t stateLED_2 = LOW;
uint16_t minute = 60;
uint16_t second = 1000;
uint16_t hour = 60;
uint16_t valveDelay = 3500; // Delay for valve operation in milliseconds
uint16_t valveOpenTime = 18; // Time to keep valve open in minutes
uint16_t valveClosedTime = 5; // Time to keep valve closed in minutes

Valve valve(valveOpenTime, valveClosedTime, valveDelay, VALVE_OPEN_PIN, VALVE_CLOSE_PIN, VALVE_LED_PIN);
Valve valve2(valveOpenTime, valveClosedTime, valveDelay, VALVE2_OPEN_PIN, VALVE2_CLOSE_PIN, VALVE2_LED_PIN);

uint8_t menuItem = 0;
// Menu items
const char* items[] = {
  "Device Status",
  "Adjust time",
  "Open Valve",
  "Close Valve"
};
  const char* adjustTime[] = {
  "Add 1 minute",
  "Remove 1 minute",
  "Return to Main Menu"
};


function adjustTime() {
  mainMenu.setMenuSubtitle("Open Time: " + String(valveOpenTime) + " mins, Closed Time: " + String(valveClosedTime) + " mins.");
  // Update display with Time adustment on the bottom screen.
  mainMenu.setMenuItems(adjustTime, sizeof(adjustTime)/sizeof(adjustTime[0]));
  mainMenu.setCurrentItemIndex(0);
  if (btnSelect.isPressed()){
    mainMenu.nextItem();
  }
  if (btnEnter.isPressed()){
    menuItem = mainMenu.getCurrentItemIndex();
    if (menuItem == 0) { // Add 1 minute
      valveOpenTime += 1; // Increase open time by 1 minute
      valve.setOpenTime(valveOpenTime);
      mainMenu.setMenuSubtitle("Open Time: " + String(valveOpenTime) + " mins, Closed Time: " + String(valveClosedTime) + " mins.");
    }
    if (menuItem == 1) { // Remove 1 minute
      if (valveOpenTime > 1) {
        valveOpenTime -= 1; // Decrease open time by 1 minute
        valve.setOpenTime(valveOpenTime);
      }
      mainMenu.setMenuSubtitle("Open Time: " + String(valveOpenTime) + " mins, Closed Time: " + String(valveClosedTime) + " mins.");
    }
    if (menuItem == 2) { // Return to Main Menu
      mainMenu.setMenuItems(items, sizeof(items)/sizeof(items[0]));
      mainMenu.setMenuSubtitle("Valve Countdown.");
      mainMenu.setCurrentItemIndex(0);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  mainMenu.initializeDisplay();

  btnSelect.setDebounceTime(BUTTON_DEBOUNCE_TIME); // set debounce time to 50 milliseconds
  btnEnter.setDebounceTime(BUTTON_DEBOUNCE_TIME); // set debounce time to 50 milliseconds
  btnMinus.setDebounceTime(BUTTON_DEBOUNCE_TIME); // set debounce time to 50 milliseconds

  mainMenu.setMenuItems(items, sizeof(items)/sizeof(items[0]));
  mainMenu.setMenuTitle("Valve Timer", 1);
  mainMenu.setMenuSubtitle("Valve Countdown.", 1);

  // Layout
  mainMenu.setMenuColumns(2);            // 2 columns
  mainMenu.setMenuRows(3);               // 3 rows per column -> 6 items/page
  mainMenu.setColumnNumberOfCharacters(14);
  mainMenu.setMenuItemScrolling(true);

  // Marquee: enable per-row (all overflow) and set speed/pause
  mainMenu.setMarqueeEnabled(true);
  mainMenu.setMarqueeMode(Menu::MarqueeMode::AllOverflow);
  mainMenu.setMarqueeSpeed(60);
  mainMenu.setMarqueeEdgePauseMs(600);
  mainMenu.setSelectedMarqueeEdgePauseMs(900);

  // Smooth vertical scroll within a page
  mainMenu.setSmoothScrollEnabled(true);
  mainMenu.setScrollSpeed(120);

  // Page transition: slide (left/right)
  mainMenu.setPageTransition(Menu::TransitionType::Slide, 280);
  // To try fade instead:
  // mainMenu.setPageTransition(Menu::TransitionType::Fade, 300);

  mainMenu.showMenu();
}

uint32_t lastNav = 0;
bool goForward = true;

void loop() {
  // Drive animations
  mainMenu.tick();
  mainMenu.refreshMenu();

  // setup the buttons
  btnSelect.loop();
  btnEnter.loop();
  btnMinus.loop();

  valve.update();
  valve2.update();
  

  // Demo: change selection every 900ms
  /*if (millis() - lastNav > 2000) {
    lastNav = millis();
    if (goForward) mainMenu.nextItem();
    else           mainMenu.previousItem();
    // Toggle direction occasionally
    if ((mainMenu.getCurrentItemIndex() % 12) == 0) goForward = !goForward;
  }*/
  if (btnSelect.isPressed()){
    mainMenu.nextItem();
  }
  if (btnEnter.isPressed()){
    menuItem = mainMenu.getCurrentItemIndex();
    mainMenu.setMenuSubtitle("Menu Item Selected: " +  mainMenu.getCurrentItemS() + ".");
    
    if (menuItem == 0) { // Device Status
      mainMenu.setMenuSubtitle("Valve Open Time: " + String(valveOpenTime) + " mins, Closed Time: " + String(valveClosedTime) + " mins.");
    }
    if (menuItem == 1) { // Adjust Time
      // Simulate time adjustment
      //valveOpenTime += 5; // Increase open time by 5 minutes
      //valveClosedTime += 5; // Increase closed time by 5 minutes
      mainMenu.setMenuSubtitle("Open Time: " + String(valveOpenTime) + " mins, Closed Time: " + String(valveClosedTime) + " mins.");

    }
    if (menuItem == 2) { // Open Valve
      digitalWrite(VALVE_OPEN_PIN, HIGH);
      delay(valveDelay);
      digitalWrite(VALVE_OPEN_PIN, LOW);
      mainMenu.setMenuSubtitle("Valve Opened.");
    }
    if (menuItem == 3) { // Close Valve
      digitalWrite(VALVE_CLOSE_PIN, HIGH);
      delay(valveDelay);
      digitalWrite(VALVE_CLOSE_PIN, LOW);
      mainMenu.setMenuSubtitle("Valve Closed.");
    }

  }
  if (btnMinus.isPressed()) {
    mainMenu.previousItem();
  }
}
