#ifndef MENU_H
#define MENU_H
#include <Arduino.h>
//#include <OledDisplay.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> 

class Menu {
  private:
    //OledDisplay* display;
    String* menuItems;
    String menuTitle;
    String menuSubtitle;
    uint8_t menuColumns;
    uint8_t columnNumberOfCharacters;
    uint8_t menuRows;
    uint8_t numberOfItems;
    uint8_t currentItemIndex;
    uint8_t titleAlignment;    // 0 = left, 1 = center, 2 = right
    uint8_t subtitleAlignment; // 0 = left, 1 = center, 2 = right
    uint8_t numberOfPages;
    boolean menuItemScrolling;
    
    uint8_t calculateAlignmentOffset(String text, uint8_t alignment);
    uint8_t getMaxItemsPerPage();
    uint8_t getTotalPages();
    uint8_t getCurrentPageIndex();
    uint8_t getItemIndexInPage(uint8_t pageIndex, uint8_t itemPosition);
    uint8_t getPageStartIndex(uint8_t pageIndex);
    uint8_t getPageEndIndex(uint8_t pageIndex);
    uint8_t getMenuItemRow(uint8_t itemIndex);
    uint8_t getMenuItemColumn(uint8_t itemIndex);
    uint8_t getMenuItemIndex(uint8_t row, uint8_t column);
    uint8_t getTotalMenuRows();
    uint8_t getTotalMenuColumns();
    uint8_t getVisibleItemsCount();
    uint8_t calculateCharactersPerColumn();
    void displayPage(uint8_t pageIndex);

    // Setup the screen dimensions
    uint8_t displayColumns; // columns for display @ 6 pixel width
    uint8_t displayRows; // rows for display @ 8 pixel height
    uint8_t SCREEN_WIDTH = 128;
    uint8_t SCREEN_HEIGHT = 64;
    uint8_t OLED_RESET = -1;
    uint8_t OLED_ADDR = 0x3C;
    uint8_t SDA_PIN = 21;
    uint8_t SCL_PIN = 22;
    uint16_t MENU_BG_COLOR = 1; // Black
    uint16_t MENU_FG_COLOR = 0; // White
    uint16_t selectArrow = 0x10; // Right arrow character code
    Adafruit_SSD1306* display;
    boolean initialized = false;
    boolean error = false;
    String errorString = "";
    
    GFXcanvas1* canvas; // Title area
    uint16_t titleXOffset = 0;
    uint16_t titleYOffset = 0;
    GFXcanvas1* canvas2; // Column 1 area
    uint16_t column1XOffset = 0;
    uint16_t column1YOffset = 17;
    GFXcanvas1* canvas3; // Column 2 area
    uint16_t column2XOffset = 0;
    uint16_t column2YOffset = 17;

    void setupCanvas();

  public:
    //Menu(OledDisplay* oledDisplay, String items[], uint8_t itemCount);
    Menu(uint8_t screenWidth, uint8_t screnHeight, int8_t reset, uint8_t addr, uint8_t sda, uint8_t scl);
    Menu(String items[], uint8_t itemCount);
    ~Menu();
    void setupDisplay(uint8_t width, uint8_t height, int8_t reset, uint8_t addr, uint8_t sda, uint8_t scl);
    void initializeDisplay();
    boolean isDisplayInitialized();
    boolean displayHasError();
    String getDisplayError();
    void setMenuTitle(String title, uint8_t alignment = 0); // 0 = left, 1 = center, 2 = right
    void setMenuSubtitle(String subtitle, uint8_t alignment = 0); // 0 = left, 1 = center, 2 = right
    void showMenu();
    void nextItem();
    void previousItem();
    String getCurrentItem();

    void setScreenWidth(uint8_t width);
    void setScreenHeight(uint8_t height);
    void setMenuColumns(uint8_t columns);
    void setMenuRows(uint8_t rows);
    void setMenuItems(String items[], uint8_t itemCount);
    void setCurrentItemIndex(uint8_t index);
    void setTitleAlignment(uint8_t alignment);
    void setSubtitleAlignment(uint8_t alignment);
    void clearMenu();
    void refreshMenu();
    void updateMenuDisplay();
    void displayTitleAndSubtitle();
    void displayMenuItems();
    void highlightCurrentItem();
    void setColumnNumberOfCharacters(uint8_t numChars);
    void setMenuItemScrolling(boolean enable);
    
    uint8_t getCurrentItemIndex();
    uint8_t getScreenWidth();
    uint8_t getScreenHeight();
    uint8_t getMenuColumns();
    uint8_t getMenuRows();

    // display settings and setup
    void setSDA_PIN(uint8_t sda);
    void setSCL_PIN(uint8_t scl);
    void setOLED_ADDR(uint8_t addr);
    void setOLED_RESET(int8_t reset);
    void setSCREEN_WIDTH(uint8_t width);
    void setSCREEN_HEIGHT(uint8_t height);
    uint8_t getSDA_PIN();
    uint8_t getSCL_PIN();
    uint8_t getOLED_ADDR();
    uint8_t getOLED_RESET();
    uint8_t getSCREEN_WIDTH();
    uint8_t getSCREEN_HEIGHT();
    void setMENU_BG_COLOR(uint16_t color);
    void setMENU_FG_COLOR(uint16_t color);
    void setMENU_SELECT_ARROW(uint16_t arrowCode);
    void clearDisplay();
    void updateDisplay();
    void updateDisplay(GFXcanvas1& canvas, uint16_t x = 0, uint16_t y = 0);
};

#endif // MENU_H