#line 1 "C:\\Users\\bradw\\OneDrive\\Documents\\Arduino\\ValveTimer\\MENU.cpp"
#include <MENU.h>

Menu::Menu(uint8_t screenWidth, uint8_t screenHeight, int8_t reset, uint8_t addr, uint8_t sda, uint8_t scl)
  : display(screenWidth, screenHeight, &Wire, reset), 
    canvas(screenWidth, 16), 
    canvas2(screenWidth / 2, screenHeight - 16), 
    canvas3(screenWidth / 2, screenHeight - 16) {
  SCREEN_WIDTH = screenWidth;
  SCREEN_HEIGHT = screenHeight;
  OLED_RESET = reset;
  OLED_ADDR = addr;
  SDA_PIN = sda;
  SCL_PIN = scl;
  displayColumns = SCREEN_WIDTH / 6; // Assuming 6 pixel width per character
  displayRows = SCREEN_HEIGHT / 8;   // Assuming 8 pixel height per character
  setupCanvas();
}

Menu::Menu(String items[], uint8_t itemCount)
  : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET), 
    canvas(SCREEN_WIDTH, 16), 
    canvas2(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 16), 
    canvas3(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 16) {
  menuItems = new String[itemCount];
  for (uint8_t i = 0; i < itemCount; i++) {
    menuItems[i] = items[i];
  }
  numberOfItems = itemCount;
  displayColumns = SCREEN_WIDTH / 6; // Assuming 6 pixel width per character
  displayRows = SCREEN_HEIGHT / 8;   // Assuming 8 pixel height per character
  setupCanvas();
}

Menu::~Menu() {
  // Destructor logic if needed
}
void Menu::setupCanvas() {
  // Initialize canvases if needed
  canvas.height = 16;
  canvas2.height = SCREEN_HEIGHT - 16;
  canvas3.height = SCREEN_HEIGHT - 16;

  canvas.width = SCREEN_WIDTH;
  canvas2.width = SCREEN_WIDTH / 2;
  canvas3.width = SCREEN_WIDTH / 2;
}
uint8_t Menu::calculateCharactersPerColumn() {
  return displayColumns / menuColumns;
}

uint8_t Menu::getMaxItemsPerPage() {
  return menuRows * menuColumns;
}

uint8_t Menu::getTotalPages() {
  uint8_t maxItemsPerPage = getMaxItemsPerPage();
  return (numberOfItems + maxItemsPerPage - 1) / maxItemsPerPage; // Ceiling division
}

uint8_t Menu::getCurrentPageIndex() {
  uint8_t maxItemsPerPage = getMaxItemsPerPage();
  return currentItemIndex / maxItemsPerPage;
}

uint8_t Menu::getItemIndexInPage(uint8_t pageIndex, uint8_t itemPosition) {
  uint8_t maxItemsPerPage = getMaxItemsPerPage();
  uint8_t itemIndex = pageIndex * maxItemsPerPage + itemPosition;
  if (itemIndex >= numberOfItems) {
    return numberOfItems - 1; // Return last item if out of bounds
  }
  return itemIndex;
}

uint8_t Menu::getPageStartIndex(uint8_t pageIndex) {
  uint8_t maxItemsPerPage = getMaxItemsPerPage();
  return pageIndex * maxItemsPerPage;
}

uint8_t Menu::getPageEndIndex(uint8_t pageIndex) {
  uint8_t maxItemsPerPage = getMaxItemsPerPage();
  uint8_t endIndex = (pageIndex + 1) * maxItemsPerPage - 1;
  if (endIndex >= numberOfItems) {
    return numberOfItems - 1; // Return last item if out of bounds
  }
  return endIndex;
}

uint8_t Menu::getMenuItemRow(uint8_t itemIndex) {
  uint8_t maxItemsPerPage = getMaxItemsPerPage();
  uint8_t indexInPage = itemIndex % maxItemsPerPage;
  return indexInPage / menuColumns;
}

uint8_t Menu::getMenuItemColumn(uint8_t itemIndex) {
  uint8_t maxItemsPerPage = getMaxItemsPerPage();
  uint8_t indexInPage = itemIndex % maxItemsPerPage;
  return indexInPage % menuColumns;
}

uint8_t Menu::getMenuItemIndex(uint8_t row, uint8_t column) {
  uint8_t maxItemsPerPage = getMaxItemsPerPage();
  uint8_t pageIndex = getCurrentPageIndex();
  uint8_t itemIndexInPage = row * menuColumns + column;
  uint8_t itemIndex = pageIndex * maxItemsPerPage + itemIndexInPage;
  if (itemIndex >= numberOfItems) {
    return numberOfItems - 1; // Return last item if out of bounds
  }
  return itemIndex;
}

uint8_t Menu::getTotalMenuRows() {
  return menuRows;
}

uint8_t Menu::getTotalMenuColumns() {
  return menuColumns;
}

uint8_t Menu::getVisibleItemsCount() {
  uint8_t maxItemsPerPage = getMaxItemsPerPage();
  uint8_t currentPageIndex = getCurrentPageIndex();
  uint8_t startIndex = getPageStartIndex(currentPageIndex);
  uint8_t endIndex = getPageEndIndex(currentPageIndex);
  return endIndex - startIndex + 1;
}

void Menu::displayPage(uint8_t pageIndex) {
  // Logic to display the specified page on the OLED
}

uint8_t Menu::calculateAlignmentOffset(String text, uint8_t alignment) {
  uint8_t textLength = text.length();
  uint8_t offset = 0;
  switch (alignment) {
    case 0: // Left
      offset = 0;
      break;
    case 1: // Center
      offset = (displayColumns - textLength) / 2;
      break;
    case 2: // Right
      offset = displayColumns - textLength;
      break;
    default:
      offset = 0;
      break;
  }
  return offset;
}

Menu::setColumnNumberOfCharacters(uint8_t numChars) {
  columnNumberOfCharacters = numChars;
}

Menu::setMenuItemScrolling(boolean enable) {
  menuItemScrolling = enable;
}

Menu::setCurrentItemIndex(uint8_t index) {
  if (index < numberOfItems) {
    currentItemIndex = index;
  }
}

Menu::setTitleAlignment(uint8_t alignment) {
  titleAlignment = alignment;
}

Menu::setSubtitleAlignment(uint8_t alignment) {
  subtitleAlignment = alignment;
}

Menu::setScreenWidth(uint8_t width) {
  SCREEN_WIDTH = width;
  displayColumns = SCREEN_WIDTH / 6; // Update columns based on new width
}

Menu::setScreenHeight(uint8_t height) {
  SCREEN_HEIGHT = height;
  displayRows = SCREEN_HEIGHT / 8; // Update rows based on new height
}

Menu::setMenuColumns(uint8_t columns) {
  menuColumns = columns;
}

Menu::setMenuRows(uint8_t rows) {
  menuRows = rows;
}

Menu::setMenuItems(String items[], uint8_t itemCount) {
  delete[] menuItems; // Free existing memory
  menuItems = new String[itemCount];
  for (uint8_t i = 0; i < itemCount; i++) {
    menuItems[i] = items[i];
  }
  numberOfItems = itemCount;
}

Menu::clearMenu() {
  delete[] menuItems;
  menuItems = nullptr;
  numberOfItems = 0;
  currentItemIndex = 0;
}

Menu::refreshMenu() {
  showMenu();
}

Menu::updateMenuDisplay() {
  displayMenuItems();
  highlightCurrentItem();
}

Menu::displayTitleAndSubtitle() {
  // Logic to display title and subtitle on the OLED
}

Menu::displayMenuItems() {
  // Logic to display menu items on the OLED
}

Menu::highlightCurrentItem() {
  // Logic to highlight the current menu item on the OLED
}

uint8_t Menu::getCurrentItemIndex() {
  return currentItemIndex;
}

uint8_t Menu::getScreenWidth() {
  return SCREEN_WIDTH;
}

uint8_t Menu::getScreenHeight() {
  return SCREEN_HEIGHT;
}

uint8_t Menu::getMenuColumns() {
  return menuColumns;
}

String Menu::getCurrentItem() {
  if (currentItemIndex < numberOfItems) {
    return menuItems[currentItemIndex];
  }
  return String("");
}

uint8_t Menu::getMenuRows() {
  return menuRows;
}

void Menu::nextItem() {
  if (currentItemIndex < numberOfItems - 1) {
    currentItemIndex++;
  } else if (menuItemScrolling) {
    currentItemIndex = 0; // Wrap around to first item
  }
}

void Menu::previousItem() {
  if (currentItemIndex > 0) {
    currentItemIndex--;
  } else if (menuItemScrolling) {
    currentItemIndex = numberOfItems - 1; // Wrap around to last item
  }
}

void Menu::setMenuTitle(String title, uint8_t alignment) {
  menuTitle = title;
  titleAlignment = alignment;
}

void Menu::setMenuSubtitle(String subtitle, uint8_t alignment) {
  menuSubtitle = subtitle;
  subtitleAlignment = alignment;
}

void Menu::setupDisplay(uint8_t width, uint8_t height, int8_t reset, uint8_t addr, uint8_t sda, uint8_t scl) {
  SCREEN_WIDTH = width;
  SCREEN_HEIGHT = height;
  OLED_RESET = reset;
  OLED_ADDR = addr;
  SDA_PIN = sda;
  SCL_PIN = scl;
  displayColumns = SCREEN_WIDTH / 6; // Assuming 6 pixel width per character
  displayRows = SCREEN_HEIGHT / 8;   // Assuming 8 pixel height per character
  setupCanvas();
}

void Menu::initializeDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    error = true;
    errorString = "SSD1306 allocation failed";
    return;
  }
  initialized = true;
  display.clearDisplay();
  display.display();
}

boolean Menu::isDisplayInitialized() {
  return initialized;
}

boolean Menu::displayHasError() {
  return error;
}

String Menu::getDisplayError() {
  return errorString;
}

void Menu::showMenu() {
  if (!initialized || error) {
    return;
  }
  display.clearDisplay();
  displayTitleAndSubtitle();
  displayMenuItems();
  highlightCurrentItem();
  display.display();
}

Menu::updateDisplayTitleAndSubtitle() {
  // Logic to update title and subtitle display
}

Menu::updateDisplayMenuItems() {
  // Logic to update menu items display
}

Menu::updateDisplayHighlightCurrentItem() {
  // Logic to update highlight of current menu item
}

Menu::refreshDisplay() {
  if (!initialized || error) {
    return;
  }
  display.clearDisplay();
  updateDisplayTitleAndSubtitle();
  updateDisplayMenuItems();
  updateDisplayHighlightCurrentItem();
  display.display();
}

Menu::setSDA_PIN(uint8_t sda) {
  SDA_PIN = sda;
}

Menu::setSCL_PIN(uint8_t scl) {
  SCL_PIN = scl;
}

Menu::setOLED_ADDR(uint8_t addr) {
  OLED_ADDR = addr;
}

Menu::setOLED_RESET(int8_t reset) {
  OLED_RESET = reset;
}

Menu::setSCREEN_WIDTH(uint8_t width) {
  SCREEN_WIDTH = width;
  displayColumns = SCREEN_WIDTH / 6; // Update columns based on new width
}

Menu::setSCREEN_HEIGHT(uint8_t height) {
  SCREEN_HEIGHT = height;
  displayRows = SCREEN_HEIGHT / 8; // Update rows based on new height
}

Menu::setMENU_BG_COLOR(uint16_t color) {
  MENU_BG_COLOR = color;
}

Menu::setMENU_FG_COLOR(uint16_t color) {
  MENU_FG_COLOR = color;
}

Menu::setMENU_SELECT_ARROW(uint16_t arrowCode) {
  selectArrow = arrowCode;
}

Menu::clearDisplay() {
  if (initialized && !error) {
    display.clearDisplay();
    display.display();
  }
}

Menu::updateDisplay(GFXcanvas1& canvas, uint16_t x = 0, uint16_t y = 0) {
  if (initialized && !error) {
    display.drawBitmap(x, y, canvas.getBuffer(), canvas.width(), canvas.height(), MENU_FG_COLOR);
    display.display();
  }
}

Menu::updateDisplay() {
  if (initialized && !error) {
    display.display();
  }
}