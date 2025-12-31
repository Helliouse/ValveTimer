
#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/**
 * Mono-only menu class for SSD1306 displays (ESP32/Arduino).
 * - Multi-canvas layout (title, body left/right, optional status) to reduce flicker.
 * - Per-row pixel-smooth marquee (horizontal) whenever text overflows its column.
 * - Pixel-smooth vertical scroll when moving within page rows.
 * - Page transition animations: slide (left/right) and temporal fade.
 * - NEW: Inverted selected item (white row, black text).
 * - RAII: predictable memory use, no raw new/delete for display/canvases.
 */
class Menu {
public:
  // Construct a menu for SSD1306; defaults match 128x64 panels on ESP32.
  Menu(uint16_t screenWidth = 128,
       uint16_t screenHeight = 64,
       int8_t  reset        = -1,
       uint8_t addr         = 0x3C,
       uint8_t sda          = 21,
       uint8_t scl          = 22,
       bool    useStatusBar = false);

  ~Menu();

  // --- Display lifecycle ---
  void   initializeDisplay();        // Wire.begin(SDA,SCL), display.begin(...)
  bool   isDisplayInitialized() const;
  bool   displayHasError()    const;
  String getDisplayError()    const;

  // --- Menu content (two modes: const char* or String) ---
  void setMenuItems(const char* const items[], uint8_t itemCount); // preferred (no heap churn)
  void setMenuItems(const String items[],       uint8_t itemCount); // optional (dynamic text)
  void clearMenu();

  // --- Title / subtitle ---
  void setMenuTitle(const String& title,    uint8_t alignment = 0); // 0=left,1=center,2=right
  void setMenuSubtitle(const String& subtitle, uint8_t alignment = 0);

  // --- Layout ---
  void setMenuColumns(uint8_t columns);           // 1 or 2
  void setMenuRows(uint8_t rows);                 // rows per column area
  void setColumnNumberOfCharacters(uint8_t charsPerColumn); // clipping width in chars
  void setMenuItemScrolling(bool enable);         // wrap at list ends

  // --- NEW: inverted selected row ---
  void setSelectedItemInverted(bool enable);      // white background + black text for selected row

  // --- Marquee controls ---
  enum class MarqueeMode : uint8_t { SelectedOnly, AllOverflow };
  void setMarqueeEnabled(bool enable);
  void setMarqueeMode(MarqueeMode mode);
  void setMarqueeSpeed(uint16_t pxPerSec);        // default 30 px/s
  void setMarqueeEdgePauseMs(uint16_t ms);        // default 600 ms pause at edges
  void setSelectedMarqueeEdgePauseMs(uint16_t ms);  // NEW: longer pause for selected row
  void setResetMarqueeOnIntraPageNav(bool enable);

  // --- Vertical smooth scroll controls ---
  void setSmoothScrollEnabled(bool enable);       // vertical scroll between rows
  void setScrollSpeed(uint16_t pxPerSec);         // default 120 px/s (row = 8 px)

  // --- Page transition animation ---
  enum class TransitionType : uint8_t { None, Slide, Fade };
  void setPageTransition(TransitionType type, uint16_t durationMs = 300);

  // --- Navigation ---
  void     nextItem();                            // advances selection (animates if enabled)
  void     previousItem();
  void     setCurrentItemIndex(uint8_t index);
  uint8_t  getCurrentItemIndex() const;
  const char* getCurrentItemC() const;            // returns "" if const-char mode inactive
  String   getCurrentItemS() const;               // returns "" if String mode inactive

  // --- Drawing / flicker control ---
  void markTitleDirty();
  void markBodyDirty();
  void markStatusDirty();

  void showMenu();        // full redraw (marks and repaints all)
  void refreshMenu();     // repaint only dirty canvases and blit them
  void clearDisplay();
  void updateDisplay();   // display.display()

  // --- Animation tick (call in loop) ---
  void tick();            // advances marquee, vertical scroll, page transitions

  // --- Hardware getters/setters ---
  void setSDA_PIN(uint8_t sda);
  void setSCL_PIN(uint8_t scl);
  void setOLED_ADDR(uint8_t addr);
  void setOLED_RESET(int8_t reset);
  void setSCREEN_WIDTH(uint8_t width);
  void setSCREEN_HEIGHT(uint8_t height);

  uint8_t getSDA_PIN()       const;
  uint8_t getSCL_PIN()       const;
  uint8_t getOLED_ADDR()     const;
  int8_t  getOLED_RESET()    const;
  uint8_t getSCREEN_WIDTH()  const;
  uint8_t getSCREEN_HEIGHT() const;

  void setMENU_BG_COLOR(uint16_t color); // 0 or 1 (mono)
  void setMENU_FG_COLOR(uint16_t color); // 0 or 1 (mono)

private:
  // --- Hardware / display ---
  Adafruit_SSD1306 display;
  uint8_t SCREEN_WIDTH;
  uint8_t SCREEN_HEIGHT;
  int8_t  OLED_RESET;
  uint8_t OLED_ADDR;
  uint8_t SDA_PIN;
  uint8_t SCL_PIN;

  bool   initialized = false;
  bool   error       = false;
  String errorString = "";

  // --- Layout metrics ---
  uint8_t  displayColumns;             // SCREEN_WIDTH / 6
  uint8_t  displayRows;                // SCREEN_HEIGHT / 8
  uint16_t MENU_BG_COLOR = BLACK;          // BLACK
  uint16_t MENU_FG_COLOR = WHITE;          // WHITE

  // --- Canvases (current page) ---
  GFXcanvas1 titleCanvas;              // (SCREEN_WIDTH x 16)
  GFXcanvas1 bodyLeftCanvas;           // (SCREEN_WIDTH/2 x SCREEN_HEIGHT-16)
  GFXcanvas1 bodyRightCanvas;          // same size; used only if columns==2
  bool       useStatusBar;
  GFXcanvas1 statusCanvas;             // (SCREEN_WIDTH x 8), optional

  // --- Canvases (previous page snapshot for transitions) ---
  GFXcanvas1 prevBodyLeftCanvas;       // same sizes as body canvases
  GFXcanvas1 prevBodyRightCanvas;

  // Dirty flags
  bool dirtyTitle  = true;
  bool dirtyBodyL  = true;
  bool dirtyBodyR  = true;
  bool dirtyStatus = false;

  // --- Menu data (two modes) ---
  const char** itemsC = nullptr;       // const char* mode
  String*      itemsS = nullptr;       // String mode
  bool         useStringItems = false;
  uint8_t      numberOfItems = 0;
  uint8_t      currentItemIndex = 0;

  // Layout/behavior
  uint8_t menuColumns = 1;             // 1 or 2
  uint8_t menuRows    = 6;             // rows per column
  uint8_t charsPerCol = 16;            // clipping width
  bool    menuItemScrolling = false;

  String  menuTitle;
  String  menuSubtitle;
  uint8_t titleAlignment    = 0;
  uint8_t subtitleAlignment = 0;

  // --- NEW: inverted selected row flag ---
  bool    selectedItemInverted = false;

  // --- Per-row marquee states (for visible items on current page) ---
  struct RowMarquee {
    int16_t   offsetPx = 0;
    int8_t    dir      = -1;           // -1 left, +1 right
    uint32_t  lastMs   = 0;
    uint32_t  holdMs   = 0;            // remaining pause at edges
  };
  RowMarquee* rowMarqueeStates = nullptr;
  uint8_t marqueeStateCount = 0;   // = getMaxItemsPerPage()
  bool marqueeEnabled = true;
  MarqueeMode marqueeMode = MarqueeMode::SelectedOnly;
  uint16_t marqueeSpeedPxSec = 30;
  uint16_t marqueeEdgePauseMs = 600;
  uint16_t selectedMarqueeEdgePauseMs = 900;        // NEW: default longer pause for selected row
  bool resetMarqueeOnIntraPageNav = false; // default: don't reset on same-page moves

  // --- Vertical smooth scroll between rows ---
  bool     smoothScrollEnabled = true;
  uint16_t scrollSpeedPxSec    = 120;  // pixels per second
  int16_t  bodyYOffsetPx       = 0;    // current animation offset
  int8_t   bodyScrollDir       = 0;    // -1 up (previous), +1 down (next), 0 idle
  uint32_t lastScrollMs        = 0;

  // --- Page transition animation ---
  TransitionType pageTransitionType   = TransitionType::Slide;
  uint16_t       pageTransitionDurationMs = 300;
  bool           transitionActive      = false;
  int8_t         transitionDir         = +1;  // +1 = next (slide left), -1 = prev (slide right)
  uint32_t       transitionStartMs     = 0;

  // --- helpers: drawing ---
  void drawTitle();
  void drawBody();          // draws current page into body canvases
  void drawStatus();

  void blitTitle();
  void blitBodyLeft();
  void blitBodyRight();
  void blitStatus();

  // Transition frame renderer
  void renderTransitionFrame(uint32_t now);

  // --- helpers: layout math ---
  uint8_t calculateAlignmentOffset(const String& text, uint8_t alignment) const;
  uint8_t getMaxItemsPerPage() const;
  uint8_t getTotalPages() const;
  uint8_t getCurrentPageIndex() const;
  uint8_t getPageStartIndex(uint8_t pageIndex) const;
  uint8_t getPageEndIndex(uint8_t pageIndex) const;
  uint8_t getVisibleItemsCount() const;

  const char* itemAtC(uint8_t idx) const;
  String      itemAtS(uint8_t idx) const;

  // --- helpers: marquee core ---
  void ensureMarqueeStateCapacity();
  void resetPageMarqueeStates();
  bool stepMarquee(RowMarquee& st, uint16_t textWidth, uint16_t colWidthPx, uint32_t now, uint16_t edgePauseMs);

  // --- helpers: vertical smooth scroll ---
  void startVerticalScroll(int8_t dir);   // -1 up, +1 down
  bool stepVerticalScroll(uint32_t now);  // returns true while animating

  // --- helpers: transitions (setup) ---
  void startPageTransition(int8_t dir, uint8_t newIndex); // captures prev canvases, sets new page

  // non-copyable
  Menu(const Menu&) = delete;
  Menu& operator=(const Menu&) = delete;
};

#endif // MENU_H
