/*******************************************************************************
 * U8g2 Chinese font example
 * Please note this font is 1,024,137 in size and cannot fit in many platform.
 * This font is generated by U8g2 tools:
 * u8g2/tools/font/bdfconv/bdfconv -v -f 1 -b 1 -m "32-127,11904-12351,19968-40959,63744-64255,65280-65376" unifont_jp-14.0.02.bdf -o u8g2_font_unifont_t_chinese.h -n u8g2_font_unifont_t_chinese
 ******************************************************************************/

/*******************************************************************************
 * Start of Arduino_GFX setting
 *
 * Arduino_GFX try to find the settings depends on selected board in Arduino IDE
 * Or you can define the display dev kit not in the board list
 * Defalult pin list for non display dev kit:
 * Arduino Nano, Micro and more: CS:  9, DC:  8, RST:  7, BL:  6, SCK: 13, MOSI: 11, MISO: 12
 * ESP32 various dev board     : CS:  5, DC: 27, RST: 33, BL: 22, SCK: 18, MOSI: 23, MISO: nil
 * ESP32-C3 various dev board  : CS:  7, DC:  2, RST:  1, BL:  3, SCK:  4, MOSI:  6, MISO: nil
 * ESP32-S2 various dev board  : CS: 34, DC: 38, RST: 33, BL: 21, SCK: 36, MOSI: 35, MISO: nil
 * ESP32-S3 various dev board  : CS: 40, DC: 41, RST: 42, BL: 48, SCK: 36, MOSI: 35, MISO: nil
 * ESP8266 various dev board   : CS: 15, DC:  4, RST:  2, BL:  5, SCK: 14, MOSI: 13, MISO: 12
 * Raspberry Pi Pico dev board : CS: 17, DC: 27, RST: 26, BL: 28, SCK: 18, MOSI: 19, MISO: 16
 * RTL8720 BW16 old patch core : CS: 18, DC: 17, RST:  2, BL: 23, SCK: 19, MOSI: 21, MISO: 20
 * RTL8720_BW16 Official core  : CS:  9, DC:  8, RST:  6, BL:  3, SCK: 10, MOSI: 12, MISO: 11
 * RTL8722 dev board           : CS: 18, DC: 17, RST: 22, BL: 23, SCK: 13, MOSI: 11, MISO: 12
 * RTL8722_mini dev board      : CS: 12, DC: 14, RST: 15, BL: 13, SCK: 11, MOSI:  9, MISO: 10
 * Seeeduino XIAO dev board    : CS:  3, DC:  2, RST:  1, BL:  0, SCK:  8, MOSI: 10, MISO:  9
 * Teensy 4.1 dev board        : CS: 39, DC: 41, RST: 40, BL: 22, SCK: 13, MOSI: 11, MISO: 12
 ******************************************************************************/
#include <U8g2lib.h>
#include <Arduino_GFX_Library.h>

#define GFX_BL DF_GFX_BL // default backlight pin, you may replace DF_GFX_BL to actual backlight pin

/* More dev device declaration: https://github.com/moononournation/Arduino_GFX/wiki/Dev-Device-Declaration */
#if defined(DISPLAY_DEV_KIT)
Arduino_GFX *gfx = create_default_Arduino_GFX();
#else /* !defined(DISPLAY_DEV_KIT) */

/* More data bus class: https://github.com/moononournation/Arduino_GFX/wiki/Data-Bus-Class */
Arduino_DataBus *bus = create_default_Arduino_DataBus();

/* More display class: https://github.com/moononournation/Arduino_GFX/wiki/Display-Class */
Arduino_GFX *gfx = new Arduino_ILI9341(bus, DF_GFX_RST, 0 /* rotation */, false /* IPS */);

#endif /* !defined(DISPLAY_DEV_KIT) */
/*******************************************************************************
 * End of Arduino_GFX setting
 ******************************************************************************/

/* more fonts at: https://github.com/moononournation/ArduinoFreeFontFile.git */

void setup(void)
{
#ifdef DEV_DEVICE_INIT
  DEV_DEVICE_INIT();
#endif

  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  // while(!Serial);
  Serial.println("Arduino_GFX U8g2 Font UTF8 Chinese example");

  // Init Display
  if (!gfx->begin())
  {
    Serial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(RGB565_BLACK);
  gfx->setUTF8Print(true); // enable UTF8 support for the Arduino print() function

#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif

  gfx->setFont(u8g2_font_unifont_t_chinese);
  gfx->setTextColor(RGB565_WHITE);
  gfx->setCursor(0, 16);
  gfx->println("Arduino 是一個開源嵌入式硬體平台，用來供用戶製作可互動式的嵌入式專案。此外 Arduino 作為一個開源硬體和開源軟件的公司，同時兼有專案和用戶社群。該公司負責設計和製造Arduino電路板及相關附件。這些產品按照GNU寬通用公共許可證（LGPL）或GNU通用公共許可證（GPL）[1]許可的開源硬體和軟件分發的，Arduino 允許任何人製造 Arduino 板和軟件分發。 Arduino 板可以以預裝的形式商業銷售，也可以作為DIY套件購買。");
  gfx->println("Arduino 专案始于2003年，作为意大利伊夫雷亚地区伊夫雷亚互动设计研究所的学生专案，目的是为新手和专业人员提供一种低成本且简单的方法，以建立使用感测器与环境相互作用的装置执行器。适用于初学者爱好者的此类装置的常见范例包括感测器、简单机械人、恒温器和运动检测器。");
}

void loop()
{
}
