/*******************************************************************************
  ESP32         1.28 LCD_GC9A01
  GND                GND
  3V3                VIN
  18                 SCL
  23                 SDA
  33                 RES
  27                 DC
  5                  CS
  22                 BLK

  代码使用Arduino_GFX库
 ******************************************************************************/
 
#define MJPEG_FILENAME "/240_30fps.mjpeg"
#define MJPEG_BUFFER_SIZE (240 * 240 * 2 / 4)
#include <WiFi.h>
#include <FS.h>
#include <SD.h>
#include <SD_MMC.h>

#include <Arduino_GFX_Library.h>
#include <lvgl.h>
#include "gui_guider.h"
#include "events_init.h"
#include "custom.h"
//#include "lv_analogclock.h"

#define TFT_BRIGHTNESS 128

#define MISO     2
#define SCK    14  //SCL
#define MOSI    15  //SDA
#define SD_CS   13

/*定义ESP32的SPI使用的引脚*/
#define GFX_BL 32
Arduino_DataBus *bus = new Arduino_ESP32SPI(27 /* DC */, 5 /* CS */, 14 /* SCK */, 15 /* MOSI */, -1 /* MISO */, VSPI /* spi_num */);
Arduino_GC9A01 *gfx = new Arduino_GC9A01(bus, 33 /* RST */, 0 /* rotation */, true /* IPS */);

#include "MjpegClass.h"
static MjpegClass mjpeg;

/* 定义LVGL的分辨率等参数 */
static uint32_t screenWidth = 240;
static uint32_t screenHeight = 240;
//定义缓冲
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf = (lv_color_t *)malloc(sizeof(lv_color_t) * screenWidth * 10);
//初始化显示用
static lv_disp_drv_t disp_drv;

lv_ui guider_ui;//结构体包含所有屏幕与部件，必不可少，且不能放到setup里 。使用该指针可以找到程序里任何对象

/* 显示填充 与LCD驱动关联 */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
   uint32_t w = (area->x2 - area->x1 + 1);
   uint32_t h = (area->y2 - area->y1 + 1);

   gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);

   lv_disp_flush_ready(disp);
}

void btn_event_cb(lv_event_t * e)
{
  Serial.println("Clicked\n");
}


void setup(void)
{
  Serial.begin(115200);
  /*显示初始化*/
  gfx->begin();   //初始化LCD
  //gfx->fillScreen(BLACK);   //背景色黑色
  /*背光设置*/
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
  delay(100);
  
  SPI.begin(SCK, MISO, MOSI, SD_CS);

if(SD.begin(SD_CS))
{
  #ifdef GFX_BL
    ledcAttachPin(GFX_BL, 1);     // assign TFT_BL pin to channel 1
    ledcSetup(1, 12000, 8);       // 12 kHz PWM, 8-bit resolution
    ledcWrite(1, TFT_BRIGHTNESS); // brightness 0 - 255
  #endif
  
  gfx->fillScreen(BLACK);   //背景色黑色
  
  // Init SD card
  
  if( !SD.begin(SD_CS))
  //if (!SD.begin(SC, SPI, 80000000)) /* SPI bus mode */
  //if ((!SD_MMC.begin()) && (!SD_MMC.begin())) /* 4-bit SD bus mode */
  //if ((!SD_MMC.begin("/sdcard", true)) && (!SD_MMC.begin("/sdcard", true))) /* 1-bit SD bus mode */
  {
    Serial.println(F("ERROR: SD card mount failed!"));
    gfx->println(F("ERROR: SD card mount failed!"));
  }
  else
  {
    File vFile = SD.open(MJPEG_FILENAME);
    // File vFile = SD_MMC.open(MJPEG_FILENAME);
    if (!vFile || vFile.isDirectory())
    {
      Serial.println(F("ERROR: Failed to open " MJPEG_FILENAME " file for reading"));
      gfx->println(F("ERROR: Failed to open " MJPEG_FILENAME " file for reading"));
      delay(10);
    }
    else
    {
      uint8_t *mjpeg_buf = (uint8_t *)malloc(MJPEG_BUFFER_SIZE);
      delay(10);
      if (!mjpeg_buf)
      {
        Serial.println(F("mjpeg_buf malloc failed!"));
        delay(10);
      }
      else
      {
        delay(10);
        Serial.println(F("MJPEG video start"));
        delay(10);
        mjpeg.setup(vFile, mjpeg_buf, gfx, true);
        delay(10);
        Serial.println(mjpeg.readMjpegBuf());
        // Read video
        delay(10);
        
        while (mjpeg.readMjpegBuf())
        {
          // Play video
          mjpeg.drawJpg();
          delay(10);     
        }
        delay(10);
        Serial.println(mjpeg.readMjpegBuf());
        Serial.println(F("MJPEG video end"));
        vFile.close();
        Serial.println(F("Reboot"));
        esp_restart();
        delay(10);
      }
    }
  }

  #ifdef TFT_BL
    delay(60000);
    ledcDetachPin(TFT_BL);
  #endif
    gfx->displayOff();
    esp_deep_sleep_start();
}
else
{
  lv_init();   //lvgl系统初始化

  /*缓冲区*/
  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, screenWidth * 10);

  /* 初始化显示器 */
  lv_disp_drv_init(&disp_drv);
  /* 将以下行更改为您的显示分辨率 */
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /* 初始化（伪）输入设备驱动程序 */
  
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  lv_indev_drv_register(&indev_drv);

  setup_ui(&guider_ui);//初始化UI界面
  events_init(&guider_ui);//事件初始化
  custom_init(&guider_ui);//运行自定义代码，例如将实际输出跟gui显示的数值关联起来

  Serial.println("Setup done");
  }
   
  delay(1000); // 5 seconds
}

void loop()
{
  lv_timer_handler(); /* let the GUI do its work */
   delay(5);
  //delay(1000); // 1 second
}
