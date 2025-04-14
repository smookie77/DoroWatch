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
#include "Wire.h"


#define TouchInt 19
#define TouchRST 4
#define SCL 22
#define SDA 21

#define TouchI2CAddr 0x15

#define ChipIdRegister 0xA7
#define CST716ChipId 0X20
#define CST816SChipId 0XB4
#define CST816TChipId 0XB5
#define CST816DChipId 0XB6
#define CST826ChipId 0X11
#define CST830ChipId 0X12
#define CST836UChipId 0X13

unsigned char ChipID = 0x00;

volatile bool TouchEventFlag = false;

/*定义ESP32的SPI使用的引脚*/
#define GFX_BL 32
#define BL_Freq 5000
unsigned int BL_Brightness = 1000;
Arduino_DataBus *bus = new Arduino_ESP32SPI(27 /* DC */, 5 /* CS */, 14 /* SCK */, 15 /* MOSI */, -1 /* MISO */, HSPI /* spi_num */);
Arduino_GFX *gfx = new Arduino_GC9A01(bus, 33 /* RST */, 0 /* rotation */, true /* IPS */);

/* 定义LVGL的分辨率等参数 */
static uint32_t screenWidth = 240;
static uint32_t screenHeight = 240;
//定义缓冲
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf = (lv_color_t *)malloc(sizeof(lv_color_t) * screenWidth * 10);
//初始化显示用
static lv_disp_drv_t disp_drv;
//初始化触摸用
static lv_indev_drv_t indev_drv;

lv_ui guider_ui;//结构体包含所有屏幕与部件，必不可少，且不能放到setup里 。使用该指针可以找到程序里任何对象

/* 显示填充 与LCD驱动关联 */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
   uint32_t w = (area->x2 - area->x1 + 1);
   uint32_t h = (area->y2 - area->y1 + 1);

   gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);

   lv_disp_flush_ready(disp);
}


void my_touch_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
  //存储按下的坐标和状态
  lv_coord_t last_x = 0;
  lv_coord_t last_y = 0;
  unsigned int X_H4 = 0;
  unsigned int X_L8 = 0;
  unsigned int Y_H4 = 0;
  unsigned int Y_L8 = 0;

  //有触摸为ture  平时为false
  if(TouchEventFlag) {
      //touchpad_get_xy(&last_x, &last_y);
      //Serial.println("有触摸！");
  
      Wire.beginTransmission(TouchI2CAddr);   //开始传输
      Wire.write(0x03);
      Wire.endTransmission(false);  //发送前面的指令，但不释放总线不发送结束
      Wire.beginTransmission(TouchI2CAddr);   //开始传输
      Wire.requestFrom(TouchI2CAddr, 1 ,true);   //参数为ture请求完数据会发送结束
      X_H4 = Wire.read();

      Wire.beginTransmission(TouchI2CAddr);   //开始传输
      Wire.write(0x04);
      Wire.endTransmission(false);  //发送前面的指令，但不释放总线不发送结束
      Wire.beginTransmission(TouchI2CAddr);   //开始传输
      Wire.requestFrom(TouchI2CAddr, 1 ,true);   //参数为ture请求完数据会发送结束
      X_L8 = Wire.read();

        Wire.beginTransmission(TouchI2CAddr);   //开始传输
      Wire.write(0x05);
      Wire.endTransmission(false);  //发送前面的指令，但不释放总线不发送结束
      Wire.beginTransmission(TouchI2CAddr);   //开始传输
      Wire.requestFrom(TouchI2CAddr, 1 ,true);   //参数为ture请求完数据会发送结束
      Y_H4 = Wire.read();

      Wire.beginTransmission(TouchI2CAddr);   //开始传输
      Wire.write(0x06);
      Wire.endTransmission(false);  //发送前面的指令，但不释放总线不发送结束
      Wire.beginTransmission(TouchI2CAddr);   //开始传输
      Wire.requestFrom(TouchI2CAddr, 1 ,true);   //参数为ture请求完数据会发送结束
      Y_L8 = Wire.read();

      last_x = (X_H4 << 8 | X_L8)&0X0FFF;
      last_y = (Y_H4 << 8 | Y_L8)&0X0FFF;

      data->point.x = last_x;
      data->point.y = last_y;

      TouchEventFlag = false;

      //Serial.printf("Touch Point:%02X , %02X \r\n",last_x,last_y);
      data->state = LV_INDEV_STATE_PR;
  }
  else {
      data->state = LV_INDEV_STATE_REL;
  }

}


void TouchInit()
{
  //初始化触摸
  pinMode(TouchRST, OUTPUT);
  pinMode(TouchInt,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TouchInt), TouchHandleInterrupt, RISING);
  //触摸复位
  digitalWrite(TouchRST, LOW);
  delay(10);
  digitalWrite(TouchRST, HIGH);
  delay(50);

  Wire.begin(SDA,SCL);   //初始化IIC，设置引脚 SCL:22  SDA:21
  Wire.setClock(400000);  //设置频率400KHZ
  delay(10);

  Wire.beginTransmission(TouchI2CAddr);   //开始传输
  Wire.write(ChipIdRegister);
  Wire.endTransmission(false);  //发送前面的指令，但不释放总线不发送结束
  Wire.beginTransmission(TouchI2CAddr);   //开始传输
  Wire.requestFrom(TouchI2CAddr, 1 ,true);   //参数为ture请求完数据会发送结束
  ChipID = Wire.read();

  Serial.printf("TouchChipID: 0x%02X",ChipID);
  if(ChipID == CST716ChipId) Serial.println(",Touch chip model :CST716");
  else if(ChipID == CST816SChipId) Serial.println(",Touch chip model :CST816S");
  else if(ChipID == CST816TChipId) Serial.println(",Touch chip model :CST816T");
  else if(ChipID == CST816DChipId) Serial.println(",Touch chip model :CST816D");
  else if(ChipID == CST826ChipId) Serial.println(",Touch chip model :CST826");
  else if(ChipID == CST830ChipId) Serial.println(",Touch chip model :CST830");
  else if(ChipID == CST836UChipId) Serial.println(",Touch chip model :CST836U");
  else Serial.println(",error!");
}

void TouchHandleInterrupt(void)
{
  TouchEventFlag = true;
  }

void setup(void)
{
  Serial.begin(115200);
  /*显示初始化*/
  gfx->begin();   //初始化LCD
  /*背光设置*/
  ledcSetup(0, BL_Freq, 10);  //初始化电机PWM，参数为通道，频率，分辨率
  ledcAttachPin(GFX_BL, 0);  //将电机PWM通道绑定到引脚上
  ledcWrite(0, BL_Brightness);  //配置电机PWM通道占空比
  delay(100);
  
  /*触摸初始化*/
  TouchInit();
  /*注册触摸中断服务函数*/
  //attachInterrupt(digitalPinToInterrupt(TouchInt), my_touch_read, FALLING);

  /*按键初始化*/
  pinMode(18,INPUT_PULLUP);
  pinMode(23,INPUT_PULLUP);

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
  lv_indev_drv_init(&indev_drv);   //使用默认值初始化一个触摸设备
  indev_drv.type = LV_INDEV_TYPE_POINTER;   //选择输入设备的类型为触摸
  indev_drv.read_cb = my_touch_read;   //添加回调函数
  lv_indev_drv_register(&indev_drv);   // 注册触摸设备


  setup_ui(&guider_ui);//初始化UI界面
  events_init(&guider_ui);//事件初始化
  custom_init(&guider_ui);//运行自定义代码，例如将实际输出跟gui显示的数值关联起来

  Serial.println("Setup done");
   
  delay(1000); // 5 seconds

  
}

void loop()
{
  lv_timer_handler(); /* let the GUI do its work */
  
   delay(5);
}
