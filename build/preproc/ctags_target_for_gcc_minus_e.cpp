# 1 "C:\\Users\\bradw\\OneDrive\\Documents\\Arduino\\ValveTimer\\ValveTimer.ino"
# 2 "C:\\Users\\bradw\\OneDrive\\Documents\\Arduino\\ValveTimer\\ValveTimer.ino" 2
# 3 "C:\\Users\\bradw\\OneDrive\\Documents\\Arduino\\ValveTimer\\ValveTimer.ino" 2
//#include <Valve.h>
//#include <MENU.h>
# 23 "C:\\Users\\bradw\\OneDrive\\Documents\\Arduino\\ValveTimer\\ValveTimer.ino"
ezButton btnSelect(32);
ezButton btnEnter(33);
ezButton btnMinus(34);
/*

Display function for updating he OLED display

*/
# 29 "C:\\Users\\bradw\\OneDrive\\Documents\\Arduino\\ValveTimer\\ValveTimer.ino"
OledDisplay display(128, 64, -1, 0x3C, 21, 22);

uint8_t stateValve2Open = 0x0;
uint8_t stateValve2Close = 0x0;
uint8_t stateValve2LED = 0x0;
uint16_t minute = 60;
uint16_t second = 1000;
uint16_t hour = 60;
uint16_t valveDelay = 3500; // Delay for valve operation in milliseconds
uint16_t valveOpenTime = 10; // Time to keep valve open in minutes
uint16_t valveClosedTime = 5; // Time to keep valve closed in minutes

// Create VALVE objects
//VALVE valve(valveOpenTime, valveClosedTime, valveDelay, VALVE_OPEN_PIN, VALVE_CLOSE_PIN, VALVE_LED_PIN); // Example pins for valve control
//VALVE valve2(valveOpenTime, valveClosedTime, valveDelay, VALVE2_OPEN_PIN, VALVE2_CLOSE_PIN, VALVE2_LED_PIN); // Example pins for second valve control

void setup() {


  pinMode(13, 0x03);
  pinMode(12, 0x03);
  pinMode(14, 0x03);
  digitalWrite(13, stateValve2Open);
  digitalWrite(12, stateValve2Close);
  digitalWrite(14, stateValve2LED);
  Serial0.begin(115200);

  button_1.setDebounceTime(50); // set debounce time to 50 milliseconds
  button_2.setDebounceTime(50); // set debounce time to 50 milliseconds
  button_3.setDebounceTime(50); // set debounce time to 50 milliseconds

  if(!display.isInitialized()){
    Serial0.println("OLED Display not initialized!");
    Serial0.println(display.getError());
  }
  //updatedisplay("Two LEDs Example", "Press BUTTON_1", "to toggle LED_1", "");
  //display.displayTop("Valve Timer", "Press SELECT/ENTER(+) for Menu items");
  display.canvaseTopDraw("Valve Timer", "Press SELECT/ENTER(+) for Menu items");
  //display.displayBottom(3, "to toggle LED_1");
  //display.displayBottom(3, "");
  display.canvaseBottomDraw(3, "Testing Bottom Line");
  //display.updateDisplay();
  display.displayCanvas();
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
      if (stateLED_1 == 0x1){
        stateValve2Open = 0x0;
        stateValve2LED = 0x0;
        //updatedisplay("Two LEDs Example", "BUTTON_1 Pressed!", "LED_1 OFF", "");
        //display.displayTop("Two LEDs Example", "BUTTON_1 Pressed!");
        //display.displayBottom(3, "LED_1 OFF");
        //display.updateDisplay();
        display.canvaseBottom(3, "Button 1 Pressed!");
        display.canvaseBottomDraw(4, "LED_1 OFF");
        display.updateDisplayCanvasBottom();
        digitalWrite(13, stateValve2Open);
        digitalWrite(14, stateValve2LED);
      }else{
        //updatedisplay("Two LEDs Example", "BUTTON_1 Pressed!", "LED_1 ON", "");
        //display.displayTop("Two LEDs Example", "BUTTON_1 Pressed!");
        //display.displayBottom(3, "LED_1 ON");
        display.canvaseBottomDraw(3, "BUTTON_1 Pressed!");
        display.canvaseBottomDraw(4, "LED_1 ON");
        display.updateDisplayCanvasBottom();
        stateValve2Open = 0x1;
        stateValve2LED = 0x1;
        digitalWrite(13 stateValve2Open);
        digitalWrite(14, stateValve2LED);
      }
    }
    if(button_2.isPressed()){
      if (stateLED_2 == 0x1){
        stateValve2Close = 0x0;
        stateValve2LED = 0x1;
        //updatedisplay("Two LEDs Example", "BUTTON_2 Pressed!", "LED_2 OFF", "");
        //display.displayTop("Two LEDs Example", "BUTTON_2 Pressed!");
        //display.displayBottom(4, "LED_2 OFF");
        //display.updateDisplay();
        display.canvaseBottomDraw(3, "BUTTON_2 Pressed!");
        display.canvaseBottomDraw(4, "LED_2 OFF");
        display.updateDisplayCanvasBottom();
        digitalWrite(12, stateValve2Close);
        digitalWrite(14, stateValve2LED);
      }else{
        //updatedisplay("Two LEDs Example", "BUTTON_2 Pressed!", "LED_2 ON", "");
        //display.displayTop("Two LEDs Example", "BUTTON_2 Pressed!");
        //display.displayBottom(4, "LED_2 ON");
        //display.updateDisplay();
        display.canvaseBottomDraw(3, "BUTTON_2 Pressed!");
        display.canvaseBottomDraw(4, "LED_2 ON");
        display.updateDisplayCanvasBottom();
        stateValve2Close = 0x1;
        stateValve2LED = 0x0;
        digitalWrite(12, stateValve2Close);
        digitalWrite(14, stateValve2LED);
      }
    }
    if(button_3.isPressed()){
      stateValve2Open = 0x0;
      stateValve2Close = 0x0;
      stateValve2LED = 0x0;
      //digitalWrite(LED_1, stateLED_1);
      //digitalWrite(LED_2, stateLED_2);
      //updatedisplay("Two LEDs Example", "BUTTON_3 Pressed!", "Both LEDs OFF", "");
      //display.displayTop("Two LEDs Example", "BUTTON_3 Pressed!");
      //display.displayBottom(5, "Both LEDs OFF");
      //display.updateDisplay();
      display.canvaseBottomDraw(3, "BUTTON_3 Pressed!");
      display.canvaseBottomDraw(4, "Both LEDs OFF");
      display.updateDisplayCanvasBottom();
    }
}
