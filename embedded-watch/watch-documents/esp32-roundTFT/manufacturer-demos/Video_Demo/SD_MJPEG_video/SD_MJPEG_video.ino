#define MJPEG_FILENAME "/240_30fps.mjpeg"
#define MJPEG_BUFFER_SIZE (240 * 240 * 2 / 4)
#include <WiFi.h>
#include <FS.h>
#include <SD.h>
#include <SD_MMC.h>

#include <Arduino_GFX_Library.h>
#define TFT_BRIGHTNESS 128



#define MISO     2
#define SCK    14  //SCL
#define MOSI    15  //SDA
#define SD_CS   13

#define TFT_CS   5
#define TFT_BLK 22
#define TFT_DC  27
#define TFT_RST 33


Arduino_DataBus *bus = new Arduino_ESP32SPI(27 /* DC */, 5 /* CS */, SCK, MOSI, MISO, VSPI);
Arduino_GC9A01  *gfx = new Arduino_GC9A01(bus, 33, 0, true);


#include "MjpegClass.h"

void setup()
{
  static MjpegClass mjpeg;
  WiFi.mode(WIFI_OFF);
  Serial.begin(115200);

  //pinMode(35);

  // Init Video
  gfx->begin();
  gfx->fillScreen(BLACK);

#ifdef TFT_BL
  ledcAttachPin(TFT_BL, 1);     // assign TFT_BL pin to channel 1
  ledcSetup(1, 12000, 8);       // 12 kHz PWM, 8-bit resolution
  ledcWrite(1, TFT_BRIGHTNESS); // brightness 0 - 255
#endif

  // Init SD card
  SPI.begin(SCK, MISO, MOSI, SD_CS);
  if( !SD.begin(SD_CS) )
  
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
          //Serial.println(analogRead(35)/4096.00*3.48*2);
          //Serial.println(analogReadMilliVolts(35));
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

  
void loop()
{
}
