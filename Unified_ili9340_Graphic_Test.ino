/***************************************************
  This is our GFX example for the Adafruit ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/


/*
 * Combination TFT_ILI9341 library and avr128db MCUs is not supported
 * 
 * Following functions are added in case of using TFT_ILI9341 library
 * void setScrollMargins(uint16_t top, uint16_t bottom)
 * void scrollTo(uint16_t y)
 * void drawBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h)
 * 
 * Following new tests are added for both Adafruit_ILI9341 and TFT_ILI9341 libraries:
 * unsigned long testFillScreenByPixels(void)
 * unsigned long testFillScreenByBitmaps(void)
 * unsigned long testScrollFillScreen(void)
 * 
 * There are some more changes to origibal Adafruit_ILI9341 grafic test
 * 
 * Add next 52 lines to User_Setup.h in TFT_ILI9341 library for automatically 
 * setup SPI RST, DC and CS pins for supported MCUs
 * 
*/


#if defined(__AVR_ATmega328P__)
  #define ATMega328_spi
  #define TotalFLASH 32256
  #define TotalSRAM 2048
#elif defined(__AVR_ATmega32U4__)
  #define ATMega32u4_spi
  #define TotalFLASH 28672
  #define TotalSRAM 2560
#elif defined(__AVR_AVR128DB64__) || defined(__AVR_AVR128DB48__) || defined(__AVR_AVR128DB32__)
  #define avr128db48_spi0
  #define TotalFLASH 130560
  #define TotalSRAM 16384
#elif defined(ESP32)
  #define esp32d1r32_vspi
  #define TotalFLASH 1310720
  #define TotalSRAM 327680
#elif defined(TARGET_RP2040)
  #define PiPicoW_RP2040_spi0
  #define TotalFLASH 2093056
  #define TotalSRAM 262144
#else
  #error "Unsupported device."
#endif

#ifdef avr128db48_spi0
  // These are the pins used for avr128db48 Optiboot like boards fixed for SPI0
  #define TFT_MOSI 4
  #define TFT_MISO 5
  #define TFT_CLK  6
  // flexible SPI independent
  #define TFT_RST 2
  #define TFT_DC  3
  #define TFT_CS  7
#elif defined(ATMega32u4_spi)
  // These are the pins used for ATMega32u4 Leonardo like boards
  #define TFT_MOSI 16
  #define TFT_MISO 14
  #define TFT_CLK  15
  // flexible SPI independent
  #define TFT_RST 4
  #define TFT_DC  11
  #define TFT_CS  13
#elif defined(ATMega328_spi)
  // These are the pins used for ATMega328 Arduino UNO boards
  #define TFT_MOSI 11
  #define TFT_MISO 12
  #define TFT_CLK  13
  // flexible SPI independent
  #define TFT_RST 8
  #define TFT_DC  9
  #define TFT_CS  10
#elif defined(esp32d1r32_vspi)
  // These are the pins used for ESP32-WROOM Arduino D1 R32 board
  #define TFT_MOSI 23
  #define TFT_MISO 19
  #define TFT_CLK  18
  // flexible SPI independent
  #define TFT_RST 12
  #define TFT_DC  13
  #define TFT_CS  5
#elif defined(PiPicoW_RP2040_spi0)
  // These are the pins used for ESP32-WROOM Arduino D1 R32 board
  #define TFT_MOSI 19
  #define TFT_MISO 16
  #define TFT_CLK  18
  // flexible SPI independent
  #define TFT_RST 21
  #define TFT_DC  20
  #define TFT_CS  17
#else
#endif

#include "SPI.h"

//#define LIB_Adafruit_ILI9341
//#define LIB_TFT_ILI9341
#define LIB_TFT_ESPI

#ifdef LIB_Adafruit_ILI9341
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
// Use hardware SPI
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
// If using the breakout, change pins as desired
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
#elif defined(LIB_TFT_ILI9341) && !defined(__AVR_AVR128DB64__) && \
     !defined(__AVR_AVR128DB48__) && !defined(__AVR_AVR128DB32__)

#include <TFT_ILI9341.h>

TFT_ILI9341 tft = TFT_ILI9341();

void drawBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h) {
  int16_t i, j;
  tft.fastSetup();
  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++ ) {
      tft.fastPixel(x + i, y + j, bitmap[i + j * ILI9341_TFTWIDTH]);
    }
  }
}

void setScrollMargins(uint16_t top, uint16_t bottom) {
  // TFA+VSA+BFA must equal 320
  if (top + bottom <= ILI9341_TFTHEIGHT) {
    uint16_t middle = ILI9341_TFTHEIGHT - (top + bottom);
    // Vertical scroll definition command
    tft.writecommand(ILI9341_VSCRDEF);
    tft.writedata(top >> 8);
    tft.writedata(top & 0xff);
    tft.writedata(middle >> 8);
    tft.writedata(middle & 0xff);
    tft.writedata(bottom >> 8);
    tft.writedata(bottom & 0xff);
  }
}

void scrollTo(uint16_t y) {
  // Vertical scrolling start address command
  tft.writecommand(ILI9341_VSCRSADD);
  tft.writedata(y >> 8);
  tft.writedata(y & 0xff);
}

#elif defined(LIB_TFT_ESPI) 
// && (defined(esp32d1r32_vspi) || defined(avr128db48_spi0))

#define ILI9341_TFTHEIGHT 320
#define ILI9341_TFTWIDTH  240

#include <TFT_eSPI.h>            // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

void drawBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h) {
  int16_t i, j;
  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++ ) {
      tft.drawPixel(x + i, y + j, bitmap[i + j * ILI9341_TFTWIDTH]);
    }
  }
}

void setScrollMargins(uint16_t top, uint16_t bottom) {
  // TFA+VSA+BFA must equal 320
  if (top + bottom <= ILI9341_TFTHEIGHT) {
    uint16_t middle = ILI9341_TFTHEIGHT - (top + bottom);
    // Vertical scroll definition command
    tft.writecommand(ILI9341_VSCRDEF);
    tft.writedata(top >> 8);
    tft.writedata(top & 0xff);
    tft.writedata(middle >> 8);
    tft.writedata(middle & 0xff);
    tft.writedata(bottom >> 8);
    tft.writedata(bottom & 0xff);
  }
}

void scrollTo(uint16_t y) {
  // Vertical scrolling start address command
  tft.writecommand(ILI9341_VSCRSADD);
  tft.writedata(y >> 8);
  tft.writedata(y & 0xff);
}

#else
#error "Unsupported device / library combination."
#endif

// Used in calculating free memory.
#if defined(PiPicoW_RP2040_spi0)
#elif defined(esp32d1r32_vspi)
#include <ESP.h>
#else
extern unsigned int __bss_end;
extern void *__brkval;
int freeMemory() {
  int free_memory;
  if ((int) __brkval)
    return ((int) &free_memory) - ((int) __brkval);
  return ((int) &free_memory) - ((int) &__bss_end);
}

extern "C" unsigned char _etext;
const unsigned char *rom_end = &_etext; // + (&_erelocate - &_srelocate); 
int romSize = (uintptr_t)rom_end;
#endif


char buff[16];
void printfcomma2 (unsigned long n) {
  if (n < 1000) {
    sprintf(buff, "%d", n);
    Serial.print(buff);
    return;
  }
  printfcomma2 (n/1000);
  sprintf(buff, ",%03d", n%1000);
  Serial.print(buff);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) ;
  int incomingByte = 0;
  while (incomingByte != '\r') incomingByte = Serial.read();
  
  Serial.print("Graphic test modified by "); Serial.println("ChRadev");
  Serial.print("Arduino MCU              "); 
#ifdef avr128db48_spi0
  Serial.println("avr128db48"); 
#elif defined(ATMega32u4_spi)
  Serial.println("ATMega32u4"); 
#elif defined(ATMega328_spi)
  Serial.println("ATMega328"); 
#elif defined(esp32d1r32_vspi)
  Serial.println("ESP32-WROOM"); 
#elif defined(PiPicoW_RP2040_spi0)
  Serial.println("PiPicoW-RP2040"); 
#else
#endif
  Serial.print("Library used             "); 
#ifdef LIB_Adafruit_ILI9341
  Serial.println("Adafruit_ILI9341"); 
#elif defined(LIB_TFT_ILI9341)
  Serial.println("TFT_ILI9341"); 
#elif defined(LIB_TFT_ESPI)
  Serial.println("TFT_eSPI"); 
#else
#endif

  Serial.println("Memory usage [B]");

  char buff[12];

#if defined(PiPicoW_RP2040_spi0)
  // TBD - use data from Arduino IDE
  Serial.println("Flash used:              ");
  Serial.println("SRAM used:               ");
#elif defined(esp32d1r32_vspi)
  // Not very precise - use data from Arduino IDE
  int flashUsed = ESP.getSketchSize();
  float per = 100.0 * flashUsed / TotalFLASH;
  Serial.print("Flash used:              "); printfcomma2(flashUsed); Serial.print( " of " ); 
  printfcomma2(TotalFLASH); Serial.print( " ("); Serial.print(per); Serial.println( "%)" );

#if 1
  int ramUsed = TotalSRAM - esp_get_free_heap_size();
  per = 100.0 * ramUsed / TotalSRAM;
  Serial.print("SRAM used:               "); printfcomma2(ramUsed); Serial.print( " of " ); 
  printfcomma2(TotalSRAM); Serial.print( " ("); Serial.print(per); Serial.println( "%)" );
#else
  int ramTotal = ESP.getHeapSize(); //total heap size
  int ramUsed = ramTotal - ESP.getFreeHeap(); //available heap
  per = 100.0 * ramUsed / ramTotal;
  Serial.print("SRAM used:               "); printfcomma2(ramUsed); Serial.print( " of " ); 
  printfcomma2(ramTotal); Serial.print( " ("); Serial.print(per); Serial.println( "%)" );
#endif

#else
  float per = 100.0 * romSize / TotalFLASH;
  Serial.print("Flash used:              "); printfcomma2(romSize); Serial.print( " of " ); 
  printfcomma2(TotalFLASH); Serial.print( " ("); Serial.print(per); Serial.println( "%)" );
  int ramSize = TotalSRAM - freeMemory();
  per = 100.0 * ramSize / TotalSRAM;
  Serial.print("SRAM used:               "); printfcomma2(ramSize); Serial.print( " of " ); 
  printfcomma2(TotalSRAM); Serial.print( " ("); Serial.print(per); Serial.println( "%)" );
#endif


  Serial.println("Benchmarks [us]");

//  SPI.begin(TFT_CLK, TFT_MISO, TFT_MOSI, TFT_CS);
#if defined(esp32d1r32_vspi) && defined(LIB_Adafruit_ILI9341)
  tft.begin(3000000);
#else
  tft.begin();
#endif

/* From original test
  // read diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  Serial.print("Display Power Mode:      0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDMADCTL);
  Serial.print("MADCTL Mode:             0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDPIXFMT);
  Serial.print("Pixel Format:            0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDIMGFMT);
  Serial.print("Image Format:            0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDSELFDIAG);
  Serial.print("Self Diagnostic:         0x"); Serial.println(x, HEX); 
  
  Serial.println(F("Benchmark                Time (microseconds)"));
*/

  delay(10);
  Serial.print(F("Screen fill              "));
  printfcomma2(testFillScreen()); Serial.println();
  delay(500);

  Serial.print(F("Text                     "));
  printfcomma2(testText()); Serial.println();
  delay(3000);

  Serial.print(F("Lines                    "));
  printfcomma2(testLines(ILI9341_CYAN)); Serial.println();
  delay(500);

  Serial.print(F("Horiz/Vert Lines         "));
  printfcomma2(testFastLines(ILI9341_RED, ILI9341_BLUE)); Serial.println();
  delay(500);

  Serial.print(F("Rectangles (outline)     "));
  printfcomma2(testRects(ILI9341_GREEN)); Serial.println();
  delay(500);

  Serial.print(F("Rectangles (filled)      "));
  printfcomma2(testFilledRects(ILI9341_YELLOW, ILI9341_MAGENTA)); Serial.println();
  delay(500);

  Serial.print(F("Circles (filled)         "));
  printfcomma2(testFilledCircles(10, ILI9341_MAGENTA)); Serial.println();
  delay(500);

  Serial.print(F("Circles (outline)        "));
  printfcomma2(testCircles(10, ILI9341_WHITE)); Serial.println();
  delay(500);

  Serial.print(F("Triangles (outline)      "));
  printfcomma2(testTriangles()); Serial.println();
  delay(500);

  Serial.print(F("Triangles (filled)       "));
  printfcomma2(testFilledTriangles()); Serial.println();
  delay(500);

  Serial.print(F("Rounded rects (outline)  "));
  printfcomma2(testRoundRects()); Serial.println();
  delay(500);

  Serial.print(F("Rounded rects (filled)   "));
  printfcomma2(testFilledRoundRects()); Serial.println();
  delay(500);

  Serial.print(F("Fill screen by pixels    "));
  printfcomma2(testFillScreenByPixels()); Serial.println();
  delay(500);

//return;

  Serial.print(F("Fill screen by bitmaps   "));
  printfcomma2(testFillScreenByBitmaps()); Serial.println();
  delay(500);

  Serial.print(F("Scroll and fill screen   "));
  printfcomma2(testScrollFillScreen()); Serial.println();
  delay(500);

  Serial.println(F("Done!"));

#ifdef LIB_Adafruit_ILI9341
  tft.setScrollMargins(0, 0);
#elif defined(LIB_TFT_ILI9341) || defined(LIB_TFT_ESPI)
  setScrollMargins(0, 0);
#else
#endif
}

int y = 1;

void loop() {
  uint16_t data[240];

//return;

#ifdef LIB_Adafruit_ILI9341
  tft.scrollTo(y);
  for (uint8_t x = 0; x < ILI9341_TFTWIDTH; x++) {
    data[x] = y + x << 5;
  }
  tft.drawRGBBitmap(0, y, data, ILI9341_TFTWIDTH, 1);
#elif defined(LIB_TFT_ILI9341) || defined(LIB_TFT_ESPI)
  scrollTo(y);
  for (uint8_t x = 0; x < ILI9341_TFTWIDTH; x++) {
    tft.drawPixel(x, y, y + x << 5);
  }
#else
#endif
  y++;
  if (y >= ILI9341_TFTHEIGHT) {
    // full screen is scrolled
    y = 0;
  }
/*
  // From original test
  for(uint8_t rotation=0; rotation<4; rotation++) {
    tft.setRotation(rotation);
    testText();
    delay(1000);
  }
*/
}

/*
 * Next 3 tests are new for fast screen fill and scroll
*/
unsigned long testFillScreenByPixels(void) {
  tft.fillScreen(ILI9341_BLACK);
  unsigned long start = micros();
#ifdef LIB_Adafruit_ILI9341
  for (int y = 0; y < ILI9341_TFTHEIGHT; y++) {
    for (int x = 0; x < ILI9341_TFTWIDTH; x++) {
      tft.drawPixel(x, y, y + x << 5);
    }
  }
#elif defined(LIB_TFT_ILI9341)
  tft.fastSetup();
  for (int y = 0; y < ILI9341_TFTHEIGHT; y++) {
    for (int x = 0; x < ILI9341_TFTWIDTH; x++) {
      tft.fastPixel(x, y, y + x << 5);
    }
  }
#elif defined(LIB_TFT_ESPI)
  for (int y = 0; y < ILI9341_TFTHEIGHT; y++) {
    for (int x = 0; x < ILI9341_TFTWIDTH; x++) {
      tft.drawPixel(x, y, y + x << 5);
    }
  }
#else
#endif
  return micros() - start;
}

unsigned long testFillScreenByBitmaps(void) {
  uint16_t data[240];
  tft.fillScreen(ILI9341_BLACK);
  unsigned long start = micros();
  for (int y = 0; y < ILI9341_TFTHEIGHT; y++) {
    for (uint8_t x = 0; x < ILI9341_TFTWIDTH; x++) {
      data[x] = y + x << 5;
    }
#ifdef LIB_Adafruit_ILI9341
    tft.drawRGBBitmap(0, y, data, ILI9341_TFTWIDTH, 1);
#elif defined(LIB_TFT_ILI9341) || defined(LIB_TFT_ESPI)
    drawBitmap(0, y, data, ILI9341_TFTWIDTH, 1);
#else
#endif
  }
  return micros() - start;
}

unsigned long testScrollFillScreen(void) {
  uint16_t data[240];
  tft.fillScreen(ILI9341_BLACK);
#ifdef LIB_Adafruit_ILI9341
  unsigned long start = micros();
  for (int y = 1; y < ILI9341_TFTHEIGHT; y++) {
    tft.scrollTo(y);
    for (uint8_t x = 0; x < ILI9341_TFTWIDTH; x++) {
      data[x] = y + x << 5;
    }
    tft.drawRGBBitmap(0, y, data, ILI9341_TFTWIDTH, 1);
  }
  tft.scrollTo(0);
  return micros() - start;
#elif defined(LIB_TFT_ILI9341) || defined(LIB_TFT_ESPI)
#if 1
// In case of using drawBitmap functions in case of TFT_ILI9341
  unsigned long start = micros();
  for (int y = 1; y < ILI9341_TFTHEIGHT; y++) {
    scrollTo(y);
    for (uint8_t x = 0; x < ILI9341_TFTWIDTH; x++) {
      data[x] = y + x << 5;
    }
    drawBitmap(0, y, data, ILI9341_TFTWIDTH, 1);
  }
#else
// In case of using fastPixel functions in case of TFT_ILI9341
  tft.fastSetup();
  unsigned long start = micros();
  for (int y = 1; y < ILI9341_TFTHEIGHT; y++) {
    scrollTo(y);
    for (uint8_t x = 0; x < ILI9341_TFTWIDTH; x++) {
      tft.fastPixel(x, y, y + x << 5);
    }
  }
#endif
  scrollTo(0);
  return micros() - start;
#else
#endif
}

unsigned long testFillScreen() {
  unsigned long start = micros();
  tft.fillScreen(ILI9341_BLACK);
  yield();
  tft.fillScreen(ILI9341_RED);
  yield();
  tft.fillScreen(ILI9341_GREEN);
  yield();
  tft.fillScreen(ILI9341_BLUE);
  yield();
  tft.fillScreen(ILI9341_BLACK);
  yield();
  return micros() - start;
}

unsigned long testText() {
  tft.fillScreen(ILI9341_BLACK);
  unsigned long start = micros();
#ifdef LIB_Adafruit_ILI9341
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ILI9341_YELLOW); tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(ILI9341_RED);    tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(5);
  tft.println("Groop");
  tft.setTextSize(2);
  tft.println("I implore thee,");
  tft.setTextSize(1);
  tft.println("my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles,");
  tft.println("Or I will rend thee");
  tft.println("in the gobberwarts");
  tft.println("with my blurglecruncheon,");
  tft.println("see if I don't!");
#elif defined(LIB_TFT_ILI9341) || defined(LIB_TFT_ESPI)
  // Changes because of tft.print(ln) functions lack in case of TFT_ILI9341
  char str[16];
  int x = 0, y = 0;
  tft.setTextColor(ILI9341_WHITE);                             tft.setTextSize(1);
  tft.drawString("Hello World!", x, y, 1);                y += tft.fontHeight(1);
  tft.setTextColor(ILI9341_YELLOW);                            tft.setTextSize(2);
  sprintf(str, "%.2f", 1234.56);
  tft.drawString(str, x, y, 1);                           y += tft.fontHeight(1);
  tft.setTextColor(ILI9341_RED);                               tft.setTextSize(3);
  sprintf(str, "%lX", 0xDEADBEEF);
  tft.drawString(str, x, y, 1);                           y += tft.fontHeight(1);
  tft.drawString("", x, y, 1);                            y += tft.fontHeight(1);
  tft.setTextColor(ILI9341_GREEN);                             tft.setTextSize(5);
  tft.drawString("Groop", x, y, 1);                       y += tft.fontHeight(1);
                                                               tft.setTextSize(2);
  tft.drawString("I implore thee,", x, y, 1);             y += tft.fontHeight(1);
                                                               tft.setTextSize(1);
  tft.drawString("my foonting turlingdromes.", x, y, 1);  y += tft.fontHeight(1);
  tft.drawString("And hooptiously drangle me", x, y, 1);  y += tft.fontHeight(1);
  tft.drawString("with crinkly bindlewurdles,", x, y, 1); y += tft.fontHeight(1);
  tft.drawString("Or I will rend thee", x, y, 1);         y += tft.fontHeight(1);
  tft.drawString("in the gobberwarts", x, y, 1);          y += tft.fontHeight(1);
  tft.drawString("with my blurglecruncheon,", x, y, 1);   y += tft.fontHeight(1);
  tft.drawString("see if I don't!", x, y, 1);             y += tft.fontHeight(1);
#else
#endif
  return micros() - start;
}

unsigned long testLines(uint16_t color) {
  unsigned long start, t;
  int           x1, y1, x2, y2,
                w = tft.width(),
                h = tft.height();

  tft.fillScreen(ILI9341_BLACK);
  yield();
  
  x1 = y1 = 0;
  y2    = h - 1;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t     = micros() - start; // fillScreen doesn't count against timing

  yield();
  tft.fillScreen(ILI9341_BLACK);
  yield();

  x1    = w - 1;
  y1    = 0;
  y2    = h - 1;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;

  yield();
  tft.fillScreen(ILI9341_BLACK);
  yield();

  x1    = 0;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;

  yield();
  tft.fillScreen(ILI9341_BLACK);
  yield();

  x1    = w - 1;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);

  yield();
  return micros() - start;
}

unsigned long testFastLines(uint16_t color1, uint16_t color2) {
  unsigned long start;
  int           x, y, w = tft.width(), h = tft.height();

  tft.fillScreen(ILI9341_BLACK);
  start = micros();
  for(y=0; y<h; y+=5) tft.drawFastHLine(0, y, w, color1);
  for(x=0; x<w; x+=5) tft.drawFastVLine(x, 0, h, color2);

  return micros() - start;
}

unsigned long testRects(uint16_t color) {
  unsigned long start;
  int           n, i, i2,
                cx = tft.width()  / 2,
                cy = tft.height() / 2;

  tft.fillScreen(ILI9341_BLACK);
  n     = min(tft.width(), tft.height());
  start = micros();
  for(i=2; i<n; i+=6) {
    i2 = i / 2;
    tft.drawRect(cx-i2, cy-i2, i, i, color);
  }

  return micros() - start;
}

unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
  unsigned long start, t = 0;
  int           n, i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9341_BLACK);
  n = min(tft.width(), tft.height());
  for(i=n; i>0; i-=6) {
    i2    = i / 2;
    start = micros();
    tft.fillRect(cx-i2, cy-i2, i, i, color1);
    t    += micros() - start;
    // Outlines are not included in timing results
    tft.drawRect(cx-i2, cy-i2, i, i, color2);
    yield();
  }

  return t;
}

unsigned long testFilledCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

  tft.fillScreen(ILI9341_BLACK);
  start = micros();
  for(x=radius; x<w; x+=r2) {
    for(y=radius; y<h; y+=r2) {
      tft.fillCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

unsigned long testCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int           x, y, r2 = radius * 2,
                w = tft.width()  + radius,
                h = tft.height() + radius;

  // Screen is not cleared for this one -- this is
  // intentional and does not affect the reported time.
  start = micros();
  for(x=0; x<w; x+=r2) {
    for(y=0; y<h; y+=r2) {
      tft.drawCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

unsigned long testTriangles() {
  unsigned long start;
  int           n, i, cx = tft.width()  / 2 - 1,
                      cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9341_BLACK);
  n     = min(cx, cy);
  start = micros();
  for(i=0; i<n; i+=5) {
    tft.drawTriangle(
      cx    , cy - i, // peak
      cx - i, cy + i, // bottom left
      cx + i, cy + i, // bottom right
      tft.color565(i, i, i));
  }

  return micros() - start;
}

unsigned long testFilledTriangles() {
  unsigned long start, t = 0;
  int           i, cx = tft.width()  / 2 - 1,
                   cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9341_BLACK);
  start = micros();
  for(i=min(cx,cy); i>10; i-=5) {
    start = micros();
    tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      tft.color565(0, i*10, i*10));
    t += micros() - start;
    tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      tft.color565(i*10, i*10, 0));
    yield();
  }

  return t;
}

unsigned long testRoundRects() {
  unsigned long start;
  int           w, i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9341_BLACK);
  w     = min(tft.width(), tft.height());
  start = micros();
  for(i=0; i<w; i+=6) {
    i2 = i / 2;
    tft.drawRoundRect(cx-i2, cy-i2, i, i, i/8, tft.color565(i, 0, 0));
  }

  return micros() - start;
}

unsigned long testFilledRoundRects() {
  unsigned long start;
  int           i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9341_BLACK);
  start = micros();
  for(i=min(tft.width(), tft.height()); i>20; i-=6) {
    i2 = i / 2;
    tft.fillRoundRect(cx-i2, cy-i2, i, i, i/8, tft.color565(0, i, 0));
    yield();
  }

  return micros() - start;
}
