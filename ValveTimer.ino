#include <oledDisplay.h>
#include <ezButton.h>
#include <Valve.h>

#define VALVE2_OPEN_PIN 13
#define VALVE2_CLOSE_PIN 12
#define VALVE2_LED_PIN 14
#define BUTTON_1 32
#define BUTTON_2 33
#define BUTTON_3 34
#define VALVE_OPEN_PIN 25
#define VALVE_CLOSE_PIN 26
#define VALVE_LED_PIN 27
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C
#define SDA_PIN 21
#define SCL_PIN 22
#define BUTTON_DEBOUNCE_TIME 50

ezButton btnSelect(BUTTON_1);
ezButton btnEnter(BUTTON_2);
ezButton btnMinus(BUTTON_3);
/*
Display function for updating he OLED display
*/
OledDisplay display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_RESET, OLED_ADDR, SDA_PIN, SCL_PIN);

uint8_t stateLED_1 = LOW;
uint8_t stateLED_2 = LOW;
uint16_t minute = 60;
uint16_t second = 1000;
uint16_t hour = 60;
uint16_t valveDelay = 3500; // Delay for valve operation in milliseconds
uint16_t valveOpenTime = 10; // Time to keep valve open in minutes
uint16_t valveClosedTime = 5; // Time to keep valve closed in minutes

void setup() {
  VALVE valve(valveOpenTime, valveClosedTime, valveDelay, VALVE_OPEN_PIN, VALVE_CLOSE_PIN, VALVE_LED_PIN); // Example pins for valve control
  VALVE valve2(valveOpenTime, valveClosedTime, valveDelay, VALVE2_OPEN_PIN, VALVE2_CLOSE_PIN, VALVE2_LED_PIN); // Example pins for second valve control

  //pinMode(LED_1, OUTPUT);
  //pinMode(LED_2, OUTPUT);
  //digitalWrite(LED_1, stateLED_1);
  //digitalWrite(LED_2, stateLED_2);
  Serial.begin(115200);
  
  button_1.setDebounceTime(BUTTON_DEBOUNCE_TIME); // set debounce time to 50 milliseconds
  button_2.setDebounceTime(BUTTON_DEBOUNCE_TIME); // set debounce time to 50 milliseconds
  button_3.setDebounceTime(BUTTON_DEBOUNCE_TIME); // set debounce time to 50 milliseconds

  if(!display.isInitialized()){
    Serial.println("OLED Display not initialized!");
    Serial.println(display.getError());
  }
  //updatedisplay("Two LEDs Example", "Press BUTTON_1", "to toggle LED_1", "");
  display.displayTop("Valve Timer", "Press SELECT/ENTER(+) for Menu items");
  display.displayBottom(3, ""); 
  display.updateDisplay();
}

void loop() {
  // setup the buttons
  button_1.loop();
  button_2.loop();
  button_3.loop();

  // Update the VALVE states
  valve.update();
  valve2.update();
     
    if(button_1.isPressed()){
      if (stateLED_1 == HIGH){
        //stateLED_1 = LOW;
        //updatedisplay("Two LEDs Example", "BUTTON_1 Pressed!", "LED_1 OFF", "");
        display.displayTop("Two LEDs Example", "BUTTON_1 Pressed!");
        display.displayBottom(3, "LED_1 OFF");
        display.updateDisplay();
        //digitalWrite(LED_1, stateLED_1);
      }else{
        //updatedisplay("Two LEDs Example", "BUTTON_1 Pressed!", "LED_1 ON", "");
        display.displayTop("Two LEDs Example", "BUTTON_1 Pressed!");
        display.displayBottom(3, "LED_1 ON");
        stateLED_1 = HIGH;
        //digitalWrite(LED_1, stateLED_1);
      }     
    }
    if(button_2.isPressed()){
      if (stateLED_2 == HIGH){
        //stateLED_2 = LOW;
        //updatedisplay("Two LEDs Example", "BUTTON_2 Pressed!", "LED_2 OFF", "");
        display.displayTop("Two LEDs Example", "BUTTON_2 Pressed!");
        display.displayBottom(4, "LED_2 OFF");
        display.updateDisplay();
        //digitalWrite(LED_2, stateLED_2);
      }else{
        //updatedisplay("Two LEDs Example", "BUTTON_2 Pressed!", "LED_2 ON", "");
        display.displayTop("Two LEDs Example", "BUTTON_2 Pressed!");
        display.displayBottom(4, "LED_2 ON");
        display.updateDisplay();
        //stateLED_2 = HIGH;
        //digitalWrite(LED_2, stateLED_2);
      }     
    }
    if(button_3.isPressed()){
      stateLED_1 = LOW;
      stateLED_2 = LOW;
      digitalWrite(LED_1, stateLED_1);
      digitalWrite(LED_2, stateLED_2);
      //updatedisplay("Two LEDs Example", "BUTTON_3 Pressed!", "Both LEDs OFF", "");
      display.displayTop("Two LEDs Example", "BUTTON_3 Pressed!");
      display.displayBottom(5, "Both LEDs OFF");
      display.updateDisplay();
    }
}
