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

#include <Arduino_GFX_Library.h>
#include <lvgl.h>
#include "gui_guider.h"
#include "events_init.h"
#include "custom.h"
//#include "lv_analogclock.h"

#include <DHT11.h>

DHT11 DHT(23);   //使用23号引脚与DHT11进行连接

/*定义ESP32的SPI使用的引脚*/
#define GFX_BL 32
Arduino_DataBus *bus = new Arduino_ESP32SPI(27 /* DC */, 5 /* CS */, 14 /* SCK */, 15 /* MOSI */, -1 /* MISO */, VSPI /* spi_num */);
Arduino_GFX *gfx = new Arduino_GC9A01(bus, 33 /* RST */, 0 /* rotation */, true /* IPS */);

/* 定义LVGL的分辨率等参数 */
static uint32_t screenWidth = 240;
static uint32_t screenHeight = 240;
//定义缓冲
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf = (lv_color_t *)malloc(sizeof(lv_color_t) * screenWidth * 10);
//初始化显示用
static lv_disp_drv_t disp_drv;

lv_ui guider_ui;//结构体包含所有屏幕与部件，必不可少，且不能放到setup里 。使用该指针可以找到程序里任何对象
lv_span_t *screen_spangroup_2_span;
lv_span_t *screen_spangroup_1_span;

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

void myDHT11(lv_timer_t * timer) {
  int DHT11Temp , DHT11RH;
  DHT.readDht11();
  DHT11Temp = DHT.getTemperature();
  DHT11RH = DHT.getHumidity();
  Serial.print("temp:");
  Serial.print(DHT11Temp);
  Serial.print("  humi:");
  Serial.println(DHT11RH);
  lv_span_set_text(screen_spangroup_1_span, (String(DHT11Temp)+"℃").c_str());
  lv_span_set_text(screen_spangroup_2_span, (String(DHT11RH)+"%RH").c_str());
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
  lv_init();   //lvgl系统初始化

  delay(100);
  lv_timer_create(myDHT11, 2000, NULL);   //定时2S执行一个任务

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

void loop()
{
  lv_timer_handler(); /* let the GUI do its work */
  usleep(5000);  // 休眠5毫秒，防止占用过多CPU
   //delay(5);
  //delay(1000); // 1 second
}
