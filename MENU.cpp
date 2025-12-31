
#include "MENU.h"
#include <string.h> // for memcpy

// --- ctor/dtor ---------------------------------------------------------------

Menu::Menu(uint16_t screenWidth, uint16_t screenHeight, int8_t reset, uint8_t addr, uint8_t sda, uint8_t scl, bool enableStatus)
  : display(screenWidth, screenHeight, &Wire, reset),
    SCREEN_WIDTH(screenWidth),
    SCREEN_HEIGHT(screenHeight),
    OLED_RESET(reset),
    OLED_ADDR(addr),
    SDA_PIN(sda),
    SCL_PIN(scl),
    displayColumns(screenWidth / 6),
    displayRows(screenHeight / 8),
    titleCanvas(screenWidth, 16),
    bodyLeftCanvas(screenWidth / 2, screenHeight - 16),
    bodyRightCanvas(screenWidth / 2, screenHeight - 16),
    useStatusBar(enableStatus),
    statusCanvas(screenWidth, 8),
    prevBodyLeftCanvas(screenWidth / 2, screenHeight - 16),
    prevBodyRightCanvas(screenWidth / 2, screenHeight - 16)
    {
      titleCanvas.setTextWrap(false);
      bodyLeftCanvas.setTextWrap(false);
      bodyRightCanvas.setTextWrap(false);
      statusCanvas.setTextWrap(false);
      // Set default font
      titleCanvas.setFont(NULL);
      bodyLeftCanvas.setFont(NULL);
      bodyRightCanvas.setFont(NULL);
      statusCanvas.setFont(NULL);
    }

Menu::~Menu() {
  if (itemsC) { delete[] itemsC; itemsC = nullptr; }
  if (itemsS) { delete[] itemsS; itemsS = nullptr; }
  if (rowMarqueeStates) { delete[] rowMarqueeStates; rowMarqueeStates = nullptr; }
}

// --- display lifecycle -------------------------------------------------------

void Menu::initializeDisplay() {
  Wire.begin(SDA_PIN, SCL_PIN);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    error = true;
    errorString = F("SSD1306 init failed");
    initialized = false;
    return;
  }
  initialized = true;
  error = false;
  errorString = "";
  display.clearDisplay();
  display.display();
}

bool Menu::isDisplayInitialized() const { return initialized && !error; }
bool Menu::displayHasError()     const { return error; }
String Menu::getDisplayError()   const { return errorString; }

// --- content ---------------------------------------------------------------

void Menu::setMenuItems(const char* const items[], uint8_t itemCount) {
  if (itemsS) { delete[] itemsS; itemsS = nullptr; }
  if (itemsC) { delete[] itemsC; itemsC = nullptr; }

  useStringItems = false;
  numberOfItems = itemCount;
  currentItemIndex = 0;

  if (itemCount == 0) { markBodyDirty(); return; }

  itemsC = new const char*[itemCount];
  for (uint8_t i = 0; i < itemCount; ++i) itemsC[i] = items[i];

  ensureMarqueeStateCapacity();
  resetPageMarqueeStates();
  markBodyDirty();
}

void Menu::setMenuItems(const String items[], uint8_t itemCount) {
  if (itemsC) { delete[] itemsC; itemsC = nullptr; }
  if (itemsS) { delete[] itemsS; itemsS = nullptr; }

  useStringItems = true;
  numberOfItems = itemCount;
  currentItemIndex = 0;

  if (itemCount == 0) { markBodyDirty(); return; }

  itemsS = new String[itemCount];
  for (uint8_t i = 0; i < itemCount; ++i) itemsS[i] = items[i];

  ensureMarqueeStateCapacity();
  resetPageMarqueeStates();
  markBodyDirty();
}

void Menu::clearMenu() {
  if (itemsC) { delete[] itemsC; itemsC = nullptr; }
  if (itemsS) { delete[] itemsS; itemsS = nullptr; }
  numberOfItems = 0;
  currentItemIndex = 0;
  resetPageMarqueeStates();
  markBodyDirty();
}

// --- titles / layout --------------------------------------------------------

void Menu::setMenuTitle(const String& title, uint8_t alignment) {
  menuTitle = title;
  titleAlignment = alignment;
  markTitleDirty();
}

void Menu::setMenuSubtitle(const String& subtitle, uint8_t alignment) {
  menuSubtitle = subtitle;
  subtitleAlignment = alignment;
  markTitleDirty();
}

void Menu::setMenuColumns(uint8_t columns) {
  menuColumns = (columns == 2) ? 2 : 1;
  ensureMarqueeStateCapacity();
  resetPageMarqueeStates();
  markBodyDirty();
}

void Menu::setMenuRows(uint8_t rows) {
  menuRows = rows ? rows : 1;
  ensureMarqueeStateCapacity();
  resetPageMarqueeStates();
  markBodyDirty();
}

void Menu::setColumnNumberOfCharacters(uint8_t charsPerColumn) {
  charsPerCol = charsPerColumn ? charsPerColumn : 1;
  markBodyDirty();
}

void Menu::setMenuItemScrolling(bool enable) {
  menuItemScrolling = enable;
}

// --- NEW: inverted selected row --------------------------------------------

void Menu::setSelectedItemInverted(bool enable) {
  selectedItemInverted = enable;
  markBodyDirty();
}

// --- marquee controls -------------------------------------------------------

void Menu::setMarqueeEnabled(bool enable)       { marqueeEnabled = enable; }
void Menu::setMarqueeMode(MarqueeMode mode)     { marqueeMode = mode; }
void Menu::setMarqueeSpeed(uint16_t pxPerSec)   { marqueeSpeedPxSec = pxPerSec ? pxPerSec : 1; }
void Menu::setMarqueeEdgePauseMs(uint16_t ms)   { marqueeEdgePauseMs = ms; }
void Menu::setSelectedMarqueeEdgePauseMs(uint16_t ms) { selectedMarqueeEdgePauseMs = ms; }
void Menu::setResetMarqueeOnIntraPageNav(bool enable) { resetMarqueeOnIntraPageNav = enable; }

// --- vertical scroll controls ----------------------------------------------

void Menu::setSmoothScrollEnabled(bool enable)  { smoothScrollEnabled = enable; }
void Menu::setScrollSpeed(uint16_t pxPerSec)    { scrollSpeedPxSec = pxPerSec ? pxPerSec : 1; }

// --- page transition controls ----------------------------------------------

void Menu::setPageTransition(TransitionType type, uint16_t durationMs) {
  pageTransitionType = type;
  pageTransitionDurationMs = durationMs ? durationMs : 1;
}

// --- navigation -------------------------------------------------------------

void Menu::nextItem() {
  if (!numberOfItems) return;
  
  uint8_t oldPage = getCurrentPageIndex();          // NEW
  uint8_t curPage = getCurrentPageIndex();
  uint8_t endIdx  = getPageEndIndex(curPage);
  bool staysInPage = (currentItemIndex < endIdx);

  if (currentItemIndex < numberOfItems - 1) {
    if (smoothScrollEnabled && staysInPage && pageTransitionType == TransitionType::None) {
      startVerticalScroll(+1);
    } else if (!staysInPage && pageTransitionType != TransitionType::None) {
      startPageTransition(+1, currentItemIndex + 1);  // keeps reset call inside transition
    } else {
      currentItemIndex++;
      // RESET ONLY IF PAGE CHANGED
      uint8_t newPage = getCurrentPageIndex();       // NEW
      if (resetMarqueeOnIntraPageNav || (newPage != oldPage)) resetPageMarqueeStates(); // NEW
      //resetPageMarqueeStates();
      markBodyDirty();
    }
  } else if (menuItemScrolling) {
    // wrap to first item (page transition optional)
    if (pageTransitionType != TransitionType::None) {
      startPageTransition(+1, 0);
    } else {
      currentItemIndex = 0;
      resetPageMarqueeStates();  // wrap ⇒ page likely changed; OK to reset
      markBodyDirty();
    }
  }
}

void Menu::previousItem() {
  if (!numberOfItems) return;
  
  uint8_t oldPage = getCurrentPageIndex();          // NEW
  uint8_t curPage = getCurrentPageIndex();
  uint8_t startIdx = getPageStartIndex(curPage);
  bool staysInPage = (currentItemIndex > startIdx);

  if (currentItemIndex > 0) {
    if (smoothScrollEnabled && staysInPage && pageTransitionType == TransitionType::None) {
      startVerticalScroll(-1);
    } else if (!staysInPage && pageTransitionType != TransitionType::None) {
      startPageTransition(-1, currentItemIndex - 1);
    } else {
      currentItemIndex--;
      // RESET ONLY IF PAGE CHANGED
      uint8_t newPage = getCurrentPageIndex();      // NEW
      if (resetMarqueeOnIntraPageNav || (newPage != oldPage)) resetPageMarqueeStates();
      markBodyDirty();
    }
  } else if (menuItemScrolling) {
    // wrap to last item
    uint8_t last = numberOfItems ? (numberOfItems - 1) : 0;
    if (pageTransitionType != TransitionType::None) {
      startPageTransition(-1, last);
    } else {
      currentItemIndex = last;
      resetPageMarqueeStates(); // wrap ⇒ page likely changed; OK to reset
      markBodyDirty();
    }
  }
}

void Menu::setCurrentItemIndex(uint8_t index) {
  if (index < numberOfItems) {
    uint8_t oldPage = getCurrentPageIndex();        // NEW
    currentItemIndex = index;
    uint8_t newPage = getCurrentPageIndex();        // NEW
    if (resetMarqueeOnIntraPageNav || (newPage != oldPage)) resetPageMarqueeStates();
    markBodyDirty();
  }
}

uint8_t Menu::getCurrentItemIndex() const { return currentItemIndex; }

const char* Menu::getCurrentItemC() const {
  return itemAtC(currentItemIndex);
}
String Menu::getCurrentItemS() const {
  return itemAtS(currentItemIndex);
}

// --- dirty flags ------------------------------------------------------------

void Menu::markTitleDirty()  { dirtyTitle  = true; }
void Menu::markBodyDirty()   { dirtyBodyL  = true; dirtyBodyR = (menuColumns == 2); }
void Menu::markStatusDirty() { if (useStatusBar) dirtyStatus = true; }

// --- redraw / blit ----------------------------------------------------------

void Menu::showMenu() {
  dirtyTitle = dirtyBodyL = true;
  dirtyBodyR = (menuColumns == 2);
  dirtyStatus = useStatusBar;
  refreshMenu();
}

void Menu::refreshMenu() {
  if (!initialized || error) return;

  uint32_t now = millis();

  if (dirtyTitle && !transitionActive)  { drawTitle();  blitTitle();      dirtyTitle = false; }

  // During transitions, body is rendered via renderTransitionFrame()
  if (transitionActive) {
    renderTransitionFrame(now);
  } else {
    if (dirtyBodyL)  { drawBody();   blitBodyLeft();   dirtyBodyL = false; }
    if (menuColumns == 2 && dirtyBodyR) {
      blitBodyRight(); dirtyBodyR = false;
    }
  }

  if (useStatusBar && dirtyStatus && !transitionActive) { drawStatus(); blitStatus(); dirtyStatus = false; }

  updateDisplay();
}

void Menu::clearDisplay() {
  if (!initialized || error) return;
  display.clearDisplay();
  display.display();
}

void Menu::updateDisplay() {
  if (!initialized || error) return;
  display.display();
}

// --- tick (animations) ------------------------------------------------------

void Menu::tick() {
  if (!initialized || error) return;

  uint32_t now = millis();
  bool needsRedraw = false;

  // Vertical smooth scroll (within page)
  if (bodyScrollDir != 0) {
    if (stepVerticalScroll(now)) {
      needsRedraw = true;
    } else {
      // commit index after finishing 1 row
      currentItemIndex += (bodyScrollDir > 0 ? 1 : -1);
      bodyScrollDir = 0;
      bodyYOffsetPx = 0;
      resetPageMarqueeStates();
      needsRedraw = true;
    }
  }

  // Page transition active? We just mark needsRedraw; rendering happens in refreshMenu()
  if (transitionActive) {
    needsRedraw = true;
  }

  // Per-row marquee: step each visible row if enabled
  if (marqueeEnabled && numberOfItems) {
    const uint8_t pi = getCurrentPageIndex();
    const uint8_t s  = getPageStartIndex(pi);
    const uint8_t e  = getPageEndIndex(pi);
    const uint16_t leftWidthPx  = min<uint16_t>(charsPerCol * 6, bodyLeftCanvas.width());
    const uint16_t rightWidthPx = min<uint16_t>(charsPerCol * 6, bodyRightCanvas.width());

    for (uint8_t i = s; i <= e; ++i) {
      uint8_t ip = i - s;               // index within current page
      // Skip if SelectedOnly and not selected
      if (marqueeMode == MarqueeMode::SelectedOnly && i != currentItemIndex) {
        // ensure offset reset so text aligns cleanly
        if (rowMarqueeStates && rowMarqueeStates[ip].offsetPx != 0) {
          rowMarqueeStates[ip].offsetPx = 0;
          rowMarqueeStates[ip].dir = -1;
          rowMarqueeStates[ip].lastMs = now;
          rowMarqueeStates[ip].holdMs = 0;
          needsRedraw = true;
        }
        continue;
      }

      String text = useStringItems ? itemAtS(i) : String(itemAtC(i));
      // Measure on appropriate canvas for correct width (left or right)
      bool isLeft = ((ip % menuColumns) == 0);
      GFXcanvas1& canvas = isLeft ? bodyLeftCanvas : bodyRightCanvas;
      int16_t x1, y1; uint16_t tw, th;
      canvas.getTextBounds(text.c_str(), 0, 0, &x1, &y1, &tw, &th);
      uint16_t colWidthPx = isLeft ? leftWidthPx : rightWidthPx;

      if (!Serial) { /* optional wait */ }
      //Serial.print(F("Row ")); 
      //Serial.print(ip);
      //Serial.print(F(" tw="));
      //Serial.print(tw);
      //Serial.print(F(" colW="));
      //Serial.println(colWidthPx);

      if (tw > colWidthPx && rowMarqueeStates) {
        if (rowMarqueeStates[ip].lastMs == 0) rowMarqueeStates[ip].lastMs = now - 16;

        // NEW: choose edge pause per row
        const bool isSelectedRow = (i == currentItemIndex);
        const uint16_t edgePause = isSelectedRow ? selectedMarqueeEdgePauseMs : marqueeEdgePauseMs;
        if (stepMarquee(rowMarqueeStates[ip], tw, colWidthPx, now, edgePause)) { // CHANGED signature
          needsRedraw = true;
          //Serial.print(F(" ip="));
          //Serial.print(ip);
          //Serial.print(F(" offset="));
          //Serial.print(rowMarqueeStates[ip].offsetPx);
          //Serial.print(F(" dir="));
          //Serial.print(rowMarqueeStates[ip].dir);
          //Serial.print(F(" hold="));
          //Serial.println(rowMarqueeStates[ip].holdMs);
        }
      } else {
        // fits: reset state if needed
        if (rowMarqueeStates && (rowMarqueeStates[ip].offsetPx != 0 || rowMarqueeStates[ip].holdMs != 0)) {
          rowMarqueeStates[ip].offsetPx = 0;
          rowMarqueeStates[ip].dir = -1;
          rowMarqueeStates[ip].holdMs = 0;
          rowMarqueeStates[ip].lastMs = now;
          needsRedraw = true;
        }
      }
    }
  }

  if (needsRedraw) markBodyDirty();
}

// --- drawing routines -------------------------------------------------------

void Menu::drawTitle() {
  titleCanvas.fillScreen(MENU_BG_COLOR);

  titleCanvas.setTextColor(MENU_FG_COLOR);
  titleCanvas.setTextSize(1);

  // Title line (y=0)
  const uint8_t offTitle = calculateAlignmentOffset(menuTitle, titleAlignment);
  titleCanvas.setCursor(offTitle * 6, 0);
  titleCanvas.print(menuTitle);

  // Subtitle line (y=8)
  const uint8_t offSub = calculateAlignmentOffset(menuSubtitle, subtitleAlignment);
  titleCanvas.setCursor(offSub * 6, 8);
  titleCanvas.print(menuSubtitle);
}

void Menu::drawBody() {
  // Clear body canvases
  bodyLeftCanvas.fillScreen(MENU_BG_COLOR);
  if (menuColumns == 2) bodyRightCanvas.fillScreen(MENU_BG_COLOR);

  const uint8_t pi = getCurrentPageIndex();
  const uint8_t s  = getPageStartIndex(pi);
  const uint8_t e  = getPageEndIndex(pi);

  // Text settings
  bodyLeftCanvas.setTextColor(MENU_FG_COLOR);
  bodyLeftCanvas.setTextSize(1);
  if (menuColumns == 2) {
    bodyRightCanvas.setTextColor(MENU_FG_COLOR);
    bodyRightCanvas.setTextSize(1);
  }

  const uint16_t leftColWidthPx  = min<uint16_t>(charsPerCol * 6, bodyLeftCanvas.width());
  const uint16_t rightColWidthPx = min<uint16_t>(charsPerCol * 6, bodyRightCanvas.width());

  // Lay out items across columns; apply vertical animation offset
  uint8_t row = 0, col = 0;
  for (uint8_t i = s; i <= e; ++i) {
    const uint16_t baseY = row * 8;
    const int16_t  y     = baseY + bodyYOffsetPx; // smooth vertical scroll
    String text = useStringItems ? itemAtS(i) : String(itemAtC(i));

    // Clip (static fallback)
    String clip = text;
    if (clip.length() > charsPerCol) clip.remove(charsPerCol);

    const uint8_t ip = i - s;   // per-page index
    const bool isSelected = (i == currentItemIndex);
    const bool useMarqueeForThisRow =
      marqueeEnabled &&
      ((marqueeMode == MarqueeMode::AllOverflow) || (marqueeMode == MarqueeMode::SelectedOnly && isSelected));

    // Choose target canvas and width for current column
    GFXcanvas1& target = (col == 0) ? bodyLeftCanvas :
                         (menuColumns == 2 ? bodyRightCanvas : bodyLeftCanvas);
    const uint16_t colWidthPx = (col == 0) ? leftColWidthPx :
                                (menuColumns == 2 ? rightColWidthPx : leftColWidthPx);


    // NEW: make sure this canvas won’t wrap long lines into the next row
    target.setTextWrap(false);
    target.setFont(NULL);

    if (isSelected && selectedItemInverted) {
      // Inverted row: MENU_FG_COLOR background, MENU_BG_COLOR text (mono)
      target.fillRect(0, baseY, colWidthPx, 8, MENU_FG_COLOR);   // MENU_FG_COLOR row
      target.setTextColor(MENU_BG_COLOR);                         // MENU_BG_COLOR text
      //target.setCursor(rowMarqueeStates[ip].offsetPx, y);
      //target.print(text);

      if (useMarqueeForThisRow && rowMarqueeStates) {
        int16_t x1, y1; uint16_t tw, th;
        target.getTextBounds(text.c_str(), 0, 0, &x1, &y1, &tw, &th);
        if (tw > colWidthPx) {
          target.setCursor(rowMarqueeStates[ip].offsetPx, y);
          target.print(text);
          
        //Serial.print(F(" ip="));
        //Serial.print(ip);
        //Serial.print(F(" offset="));
        //Serial.println(rowMarqueeStates[ip].offsetPx);

        } else {
          target.setCursor(0, y);
          target.print(clip);
        }
      } else {
        target.setCursor(0, y);
        target.print(clip);
      }
      
      //Serial.print(F(" ip="));
      //Serial.print(ip);
      //Serial.print(F(" offset="));
      //Serial.println(rowMarqueeStates[ip].offsetPx);

      // Optional MENU_BG_COLOR outline around the MENU_FG_COLOR selected row
      target.drawRect(0, y, colWidthPx, 8, MENU_BG_COLOR);
    } else {
      // Normal row: MENU_BG_COLOR background (already), MENU_FG_COLOR text
      target.setTextColor(MENU_FG_COLOR);                         // MENU_FG_COLOR text

      if (useMarqueeForThisRow && rowMarqueeStates) {
        int16_t x1, y1; uint16_t tw, th;
        target.getTextBounds(text.c_str(), 0, 0, &x1, &y1, &tw, &th);
        //Serial.print(F(" ip="));
        //Serial.print(ip);
        //Serial.print(F(" offset="));
        //Serial.println(rowMarqueeStates[ip].offsetPx);
        if (tw > colWidthPx) {
          // ensure clean row area for marquee
          target.fillRect(0, baseY, colWidthPx, 8, MENU_BG_COLOR);   // MENU_BG_COLOR row clear
          target.setCursor(rowMarqueeStates[ip].offsetPx, y);
          target.print(text);
        } else {
          target.setCursor(0, y);
          target.print(clip);
        }
      } else {
        target.setCursor(0, y);
        target.print(clip);
      }

      // If you still want a selection frame when not inverted:
      if (isSelected) {
        target.drawRect(0, y, colWidthPx, 8, MENU_FG_COLOR);     // MENU_FG_COLOR outline
      }
    }

    // Next position
    ++col;
    if (col >= menuColumns) { col = 0; ++row; }
  }
}

void Menu::drawStatus() {
  statusCanvas.fillScreen(MENU_BG_COLOR);
  statusCanvas.setTextColor(MENU_FG_COLOR);
  statusCanvas.setTextSize(1);
  statusCanvas.setCursor(0, 0);
  statusCanvas.print(F("↑/↓ navigate   OK=Select"));
}

// --- blits ------------------------------------------------------------------

void Menu::blitTitle() {
  display.drawBitmap(0, 0, titleCanvas.getBuffer(), titleCanvas.width(), titleCanvas.height(), MENU_FG_COLOR, MENU_BG_COLOR);
}
void Menu::blitBodyLeft() {
  display.drawBitmap(0, 16, bodyLeftCanvas.getBuffer(), bodyLeftCanvas.width(), bodyLeftCanvas.height(), MENU_FG_COLOR, MENU_BG_COLOR);
}
void Menu::blitBodyRight() {
  if (menuColumns == 2) {
    display.drawBitmap(SCREEN_WIDTH / 2, 16, bodyRightCanvas.getBuffer(), bodyRightCanvas.width(), bodyRightCanvas.height(), MENU_FG_COLOR, MENU_BG_COLOR);
  }
}
void Menu::blitStatus() {
  if (useStatusBar) {
    display.drawBitmap(0, SCREEN_HEIGHT - 8, statusCanvas.getBuffer(), statusCanvas.width(), statusCanvas.height(), MENU_FG_COLOR, MENU_BG_COLOR);
  }
}

// --- transition frame renderer ---------------------------------------------

void Menu::renderTransitionFrame(uint32_t now) {
  // Progress [0..1]
  uint32_t elapsed = now - transitionStartMs;
  float r = (elapsed >= pageTransitionDurationMs) ? 1.0f
                                                  : (float)elapsed / (float)pageTransitionDurationMs;

  // Clear body area first (to avoid stale pixels)
  display.fillRect(0, 16, SCREEN_WIDTH, SCREEN_HEIGHT - 16 - (useStatusBar ? 8 : 0), MENU_BG_COLOR);

  if (pageTransitionType == TransitionType::Slide) {
    // Slide: old page moves out, new moves in
    int16_t halfW = SCREEN_WIDTH / 2;
    int16_t progressPx = (int16_t)(r * halfW);

    // Old page offsets
    int16_t oldOffX = (transitionDir > 0) ? -progressPx : +progressPx;
    // New page offsets (start off-screen)
    int16_t newOffX = (transitionDir > 0) ? (halfW - progressPx) : (-halfW + progressPx);

    // Blit previous left/right
    display.drawBitmap(oldOffX + 0, 16, prevBodyLeftCanvas.getBuffer(), prevBodyLeftCanvas.width(), prevBodyLeftCanvas.height(), MENU_FG_COLOR, MENU_BG_COLOR);
    display.drawBitmap(oldOffX + halfW, 16, prevBodyRightCanvas.getBuffer(), prevBodyRightCanvas.width(), prevBodyRightCanvas.height(), MENU_FG_COLOR, MENU_BG_COLOR);

    // Blit new left/right
    display.drawBitmap(newOffX + 0, 16, bodyLeftCanvas.getBuffer(), bodyLeftCanvas.width(), bodyLeftCanvas.height(), MENU_FG_COLOR, MENU_BG_COLOR);
    display.drawBitmap(newOffX + halfW, 16, bodyRightCanvas.getBuffer(), bodyRightCanvas.width(), bodyRightCanvas.height(), MENU_FG_COLOR, MENU_BG_COLOR);

  } else if (pageTransitionType == TransitionType::Fade) {
    // Temporal crossfade: switch between old/new based on duty
    const uint16_t periodMs = 30; // ~33fps cadence
    uint16_t phase = (elapsed % periodMs);
    uint16_t threshold = (uint16_t)(r * periodMs);

    bool showNew = (phase < threshold);

    if (!showNew) {
      // Show previous page
      display.drawBitmap(0, 16, prevBodyLeftCanvas.getBuffer(), prevBodyLeftCanvas.width(), prevBodyLeftCanvas.height(), MENU_FG_COLOR, MENU_BG_COLOR);
      display.drawBitmap(SCREEN_WIDTH / 2, 16, prevBodyRightCanvas.getBuffer(), prevBodyRightCanvas.width(), prevBodyRightCanvas.height(), MENU_FG_COLOR, MENU_BG_COLOR);
    } else {
      // Show new page
      display.drawBitmap(0, 16, bodyLeftCanvas.getBuffer(), bodyLeftCanvas.width(), bodyLeftCanvas.height(), MENU_FG_COLOR, MENU_BG_COLOR);
      display.drawBitmap(SCREEN_WIDTH / 2, 16, bodyRightCanvas.getBuffer(), bodyRightCanvas.width(), bodyRightCanvas.height(), MENU_FG_COLOR, MENU_BG_COLOR);
    }
  }

  if (elapsed >= pageTransitionDurationMs) {
    // Transition completed
    transitionActive = false;
    // Finalize: mark body dirty so the steady state blit happens next refresh
    markBodyDirty();
  }
}

// --- math / helpers ---------------------------------------------------------

uint8_t Menu::calculateAlignmentOffset(const String& text, uint8_t alignment) const {
  const int16_t textLen = (int16_t)text.length();
  const int16_t cols    = (int16_t)displayColumns;
  int16_t offset = 0;
  switch (alignment) {
    case 0: offset = 0; break;
    case 1: offset = (cols - textLen) / 2; break;
    case 2: offset = (cols - textLen); break;
    default: offset = 0; break;
  }
  if (offset < 0) offset = 0;
  if (offset > cols) offset = cols;
  return (uint8_t)offset;
}

uint8_t Menu::getMaxItemsPerPage() const { return uint8_t(menuRows * menuColumns); }

uint8_t Menu::getTotalPages() const {
  const uint8_t mpp = getMaxItemsPerPage();
  return mpp ? uint8_t((numberOfItems + mpp - 1) / mpp) : 0;
}

uint8_t Menu::getCurrentPageIndex() const {
  const uint8_t mpp = getMaxItemsPerPage();
  return mpp ? uint8_t(currentItemIndex / mpp) : 0;
}

uint8_t Menu::getPageStartIndex(uint8_t pageIndex) const { return uint8_t(pageIndex * getMaxItemsPerPage()); }

uint8_t Menu::getPageEndIndex(uint8_t pageIndex) const {
  const uint8_t mpp = getMaxItemsPerPage();
  if (!mpp || !numberOfItems) return 0;
  const uint8_t end = uint8_t((pageIndex + 1) * mpp - 1);
  return (end >= numberOfItems) ? uint8_t(numberOfItems - 1) : end;
}

uint8_t Menu::getVisibleItemsCount() const {
  const uint8_t pi = getCurrentPageIndex();
  const uint8_t s  = getPageStartIndex(pi);
  const uint8_t e  = getPageEndIndex(pi);
  return (e >= s) ? uint8_t(e - s + 1) : 0;
}

const char* Menu::itemAtC(uint8_t idx) const {
  if (!itemsC || idx >= numberOfItems) return "";
  return itemsC[idx];
}
String Menu::itemAtS(uint8_t idx) const {
  if (!itemsS || idx >= numberOfItems) return String("");
  return itemsS[idx];
}

// --- marquee core -----------------------------------------------------------

void Menu::ensureMarqueeStateCapacity() {
  uint8_t needed = getMaxItemsPerPage();
  if (needed == marqueeStateCount) return;

  if (rowMarqueeStates) { delete[] rowMarqueeStates; rowMarqueeStates = nullptr; }
  marqueeStateCount = needed;
  if (needed > 0) rowMarqueeStates = new RowMarquee[needed];
}

void Menu::resetPageMarqueeStates() {
  if (!rowMarqueeStates) return;
  uint8_t count = marqueeStateCount;
  uint32_t now = millis();
  for (uint8_t i = 0; i < count; ++i) {
    rowMarqueeStates[i].offsetPx = 0;
    rowMarqueeStates[i].dir = -1;
    rowMarqueeStates[i].holdMs = 0;
    rowMarqueeStates[i].lastMs = now;
  }
}

bool Menu::stepMarquee(RowMarquee& st, uint16_t textWidth, uint16_t colWidthPx, uint32_t now, uint16_t edgePauseMs) {
  // Edge pause
  if (st.holdMs > 0) {
    uint32_t dt = now - st.lastMs;
    st.lastMs = now;
    if (dt >= st.holdMs) st.holdMs = 0;
    else st.holdMs -= dt;
    //return (dt > 0);
    return true;
  }

  uint32_t dt = now - st.lastMs;
  if (dt == 0) return false;
  st.lastMs = now;

  float step = (marqueeSpeedPxSec / 1000.0f) * dt;
  int16_t minOffset = -int16_t(textWidth - colWidthPx); // far left (negative)
  int16_t maxOffset = 0;                                // far right

  st.offsetPx += (int16_t)(st.dir * step);

  if (st.offsetPx <= minOffset) {
    st.offsetPx = minOffset;
    st.dir = +1;
    st.holdMs = edgePauseMs;
  } else if (st.offsetPx >= maxOffset) {
    st.offsetPx = maxOffset;
    st.dir = -1;
    st.holdMs = edgePauseMs;
  }
  return true;
}

// --- vertical smooth scroll -------------------------------------------------

void Menu::startVerticalScroll(int8_t dir) {
  if (dir == 0) return;
  bodyScrollDir  = (dir < 0) ? -1 : +1;
  lastScrollMs   = millis();
}

bool Menu::stepVerticalScroll(uint32_t now) {
  uint32_t dt = now - lastScrollMs;
  if (dt == 0) return true;
  lastScrollMs = now;

  float step = (scrollSpeedPxSec / 1000.0f) * dt; // pixels per tick
  bodyYOffsetPx += (int16_t)(bodyScrollDir * step);

  // A row is 8 px tall; stop when we moved a full row
  if (abs(bodyYOffsetPx) >= 8) {
    return false; // animation finished; commit in tick()
  }
  return true;   // continue animating
}

// --- transitions (setup) ----------------------------------------------------

void Menu::startPageTransition(int8_t dir, uint8_t newIndex) {
  // Capture previous body canvases
  size_t bytesLeft  = (bodyLeftCanvas.width()  * bodyLeftCanvas.height())  / 8;
  size_t bytesRight = (bodyRightCanvas.width() * bodyRightCanvas.height()) / 8;
  memcpy(prevBodyLeftCanvas.getBuffer(),  bodyLeftCanvas.getBuffer(),  bytesLeft);
  memcpy(prevBodyRightCanvas.getBuffer(), bodyRightCanvas.getBuffer(), bytesRight);

  // Commit new index and draw new page into current body canvases
  currentItemIndex = newIndex;
  resetPageMarqueeStates();
  drawBody(); // prepare new canvases immediately

  // Activate transition
  transitionActive   = true;
  transitionDir      = (dir < 0) ? -1 : +1;
  transitionStartMs  = millis();
  // Title/status remain steady; redraw body only via renderTransitionFrame()
}

// --- hardware setters/getters ----------------------------------------------

void Menu::setSDA_PIN(uint8_t sda)     { SDA_PIN = sda; }
void Menu::setSCL_PIN(uint8_t scl)     { SCL_PIN = scl; }
void Menu::setOLED_ADDR(uint8_t addr)  { OLED_ADDR = addr; }
void Menu::setOLED_RESET(int8_t reset) { OLED_RESET = reset; }
void Menu::setSCREEN_WIDTH(uint8_t width) {
  SCREEN_WIDTH = width; displayColumns = SCREEN_WIDTH / 6;
}
void Menu::setSCREEN_HEIGHT(uint8_t height) {
  SCREEN_HEIGHT = height; displayRows = SCREEN_HEIGHT / 8;
}

uint8_t Menu::getSDA_PIN()       const { return SDA_PIN; }
uint8_t Menu::getSCL_PIN()       const { return SCL_PIN; }
uint8_t Menu::getOLED_ADDR()     const { return OLED_ADDR; }
int8_t  Menu::getOLED_RESET()    const { return OLED_RESET; }
uint8_t Menu::getSCREEN_WIDTH()  const { return SCREEN_WIDTH; }
uint8_t Menu::getSCREEN_HEIGHT() const { return SCREEN_HEIGHT; }

//void Menu::setMENU_BG_COLOR(uint16_t color) { MENU_BG_COLOR = color ? 1 : 0; }
void Menu::setMENU_BG_COLOR(uint16_t color) { MENU_BG_COLOR = color ? 1 : 0; }
//void Menu::setMENU_FG_COLOR(uint16_t color) { MENU_FG_COLOR = color ? 1 : 0; }
void Menu::setMENU_FG_COLOR(uint16_t color) { MENU_FG_COLOR = color ? 1 : 0; }
