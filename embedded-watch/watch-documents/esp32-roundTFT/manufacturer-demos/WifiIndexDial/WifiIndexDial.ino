#include<WiFi.h>
#include<WiFiMulti.h>  //连接多个WiFi用的库
#include<ArduinoJson.h>  //解析JSON数据需要的库

#include <Arduino_GFX_Library.h>
#include <lvgl.h>
#include "gui_guider.h"
#include "events_init.h"
#include "custom.h"
//#include "lv_analogclock.h"

WiFiMulti WiFiMulti;  //声明WiFiMulti的对象

#define TFT_BRIGHTNESS 128


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

/* 显示填充 与LCD驱动关联 */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
   uint32_t w = (area->x2 - area->x1 + 1);
   uint32_t h = (area->y2 - area->y1 + 1);

   gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);

   lv_disp_flush_ready(disp);
}


/*定义获取北京时间需要访问的IP地址、端口以及网址子页或参数*/
const char* TimeHost = "api.m.taobao.com";
uint16_t TimePort=80;
const char* GetSysTime = "/rest/api3.do?api=mtop.common.getTimestamp";


/*存放最终公网IP,解析IP与UTC时间信息*/
int TimeInformation;


/*存储多个WiFi的连接信息*/
//String WifiSSID="One Plus-JXL";   //串口进行配置参数获取
//String WifiPassword="122232542";   //串口进行配置参数获取

String WifiSSID="PADAVAN";   //串口进行配置参数获取
String WifiPassword="122232542";   //串口进行配置参数获取

//String WifiSSID="JXLMate 60";   //串口进行配置参数获取
//String WifiPassword="122232542";   //串口进行配置参数获取



void WifiConfiguration(void);   //wifi初始化并连接
String HTTPRequestTime(void);  //请求UTC时间
int TimeJSONAnalysis(String JsonData);  //时间 Json解析函数


void setup() {
  // put your setup code here, to run once:
  delay(500);
  Serial.begin(115200);


  
  gfx->begin();   //初始化LCD
  /*背光设置*/
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
  delay(100);
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

  blueCounter_analog_clock_1_hour_value = 6;
  blueCounter_analog_clock_1_min_value = 30;
  blueCounter_analog_clock_1_sec_value = 30;
  
  delay(500);
  
  WifiConfiguration();

  setup_ui(&guider_ui);//初始化UI界面
  events_init(&guider_ui);//事件初始化
  custom_init(&guider_ui);//运行自定义代码，例如将实际输出跟gui显示的数值关联起来

  delay(1000);
  xTaskCreatePinnedToCore(Task_one/*任务函数*/,"task_one"/*任务名称字符串*/,10000/*堆栈大小（字节）*/,NULL/*参数作为任务输入传递*/,1/*任务优先级*/,NULL/*任务句柄*/,1/*运行在核心1,大核*/);
  xTaskCreatePinnedToCore(Task_two, "task_two", 10000, NULL, 2, NULL, 0/*小核*/);
}



void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
}

void Task_one(void *parameter)
{
  while(1)
  {
    lv_timer_handler(); /* let the GUI do its work */
    vTaskDelay(5);
  }
  vTaskDelete( NULL );//任务内调用TaskDelete函数可删除任务本身，输入的参数为任务的任务句柄
}


void Task_two(void *parameter)   //小核
{
  while(1)
  {
    int Circle;
    AgainStart:if(WiFi.status() == WL_CONNECTED)   //如果wifi连接成功才执行一下操作
    {
      /*HTTP请求北京时间函数并返回值给JSON函数进行解析,JSON函数取出来的时间信息返回到 变量 TimeInformation 中*/
      TimeInformation = TimeJSONAnalysis(HTTPRequestTime());

      blueCounter_analog_clock_1_hour_value = (((TimeInformation/10000)+8)%12);
      blueCounter_analog_clock_1_min_value = TimeInformation%10000/100;
      blueCounter_analog_clock_1_sec_value = TimeInformation%100;
      
      Serial.print("\r\nUTC time:");
      Serial.print(((TimeInformation/10000)+8)%24);
      Serial.print(":");
      Serial.print(blueCounter_analog_clock_1_min_value);
      Serial.print(":");
      Serial.print(blueCounter_analog_clock_1_sec_value);
    }
    vTaskDelay(50000);
  }
  vTaskDelete( NULL );
}


void WifiConfiguration(void)
{
  WiFiMulti.addAP(WifiSSID.c_str(), WifiPassword.c_str());
  Serial.print("\r\nwifi connecting.");
  while (WiFiMulti.run() != WL_CONNECTED) //连接wifi,连接失败则一直输出 "." ，否则跳出循环输出已连接到wifi：xxx
  {
    Serial.print(".");
    delay(3000);
  }
  Serial.print("\r\nconnected wifi:");
  Serial.print(WiFi.SSID());
}

String HTTPRequestTime(void)  //请求UTC时间
{
  WiFiClient wifiClient;  //实例化对象
  
  String TimeReceiveData = "";
  
  /*GET请求数据*/
  String TimeRequest=String("GET ")+GetSysTime+" HTTP/1.1\r\n"+
                 "Host: "+TimeHost+"\r\n"+
                 "Connection: Close\r\n"+
                 "\r\n";

  if(wifiClient.connect(TimeHost,TimePort))  //连接到指定的host和端口
  {
    wifiClient.print(TimeRequest);  //向连接到的host的端口发送数据
    while(wifiClient.connected()||wifiClient.available())  //如果连接成功或者有数据接收进入下面循环
    {
      if(wifiClient.available())
      {
        wifiClient.find("\r\n\r\n");  //跳过HTTP响应头的信息,只需要解析主体信息即可
        TimeReceiveData = wifiClient.readString();  //存储从服务器请求到的数据
        }
      }
    }
  else
  {
  }
  wifiClient.stop();  //断开连接

  return TimeReceiveData;
}

int TimeJSONAnalysis(String JsonData)  //时间 Json解析函数
{
  /*从JSON数据中提取时间数据，但是是JSON格式的，需要转换成data time格式的，JSON格式时间是从1970年到现在的时间毫秒数*/
  String JSONTime;
  int DateTime;
  unsigned int Year,Month,Day,Hour,Minute,Second,MonthSecond,DaySecond,JSONTime_1_6,JSONTime_7_13;  //Month存储第几月的时间，MonthSecond存储当年度过所有月的秒数，其他类似
  bool LeapYearFlag = 0;   //闰年标志位，0为平年，1位闰年，默认参数为0
  const size_t capacity = JSON_OBJECT_SIZE(4) + JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 130;
  DynamicJsonDocument JsonDataObject(capacity);
  deserializeJson(JsonDataObject,JsonData);
  JSONTime = JsonDataObject["data"]["t"].as<String>();

  /*
    unsigned int型无法存取这么大的数字，将string型拆分之后再转换为unsigned int型
    substring(N,M) 函数可以将第N个开始到第M个的字符串截取出来，之后再用toInt()转换成整形
  */
  JSONTime_1_6 = (JSONTime.substring(0,6)).toInt()*10000;  //从第1个数到第6个截取出来，第一个数的标号是0，最后一个不截取，并转化为unsigned int型数据，顺便*10000，以便节约后续计算资源
  JSONTime_7_13 = (JSONTime.substring(6,13)).toInt()/1000;   //从第7个数到第13个截取出来，第一个数的标号是0，最后一个不截取，并转化为unsigned int型数据，顺便/1000把单位转换成秒，以便节约后续计算资源
  
  /*
    年计算
    计算出是否为闰年最后一天，因为闰年多一天，按照平年计算的话会算成下一年，因此应该推算出是否为闰年最后一天并按照平年计算方法减去1天
    判断是否为闰年，不足一个周期剩余的年数为2时一定是闰年  或者  年数等于3时但开始时间不大于1天

    4年为一个周期(平年、平年、闰年、平年，1972年是闰年，闰年有366天，多在2月有29号)，
    一个周期秒数为126230400秒，一个平年有31536000秒，一个闰年有31622400秒，一天有86400秒
    ，下面依此计算
  */
  if((((JSONTime_1_6 + JSONTime_7_13)%126230400/31536000) == 2)||((((JSONTime_1_6 + JSONTime_7_13)%126230400/31536000) == 3)&&(((JSONTime_1_6 + JSONTime_7_13)%126230400%31536000/86400) == 0)))   //判断是否为闰年
  {
    LeapYearFlag = 1;   //闰年标志
    if((((JSONTime_1_6 + JSONTime_7_13)%126230400%31536000/86400) == 0))   //判断是否为闰年最后一天
    {
      Year = (JSONTime_1_6 + JSONTime_7_13)/126230400*4 + (JSONTime_1_6 + JSONTime_7_13)%126230400/31536000-1 + 1970;
    }
    else
    {
      Year = (JSONTime_1_6 + JSONTime_7_13)/126230400*4 + (JSONTime_1_6 + JSONTime_7_13)%126230400/31536000 + 1970;
    }
  }
  else
  {
    LeapYearFlag = 0;   //平年标志
    Year = (JSONTime_1_6 + JSONTime_7_13)/126230400*4 + (JSONTime_1_6 + JSONTime_7_13)%126230400/31536000 + 1970;
  }

  /*
    月份计算
    闰年一定在一个周期中的第三年，计算第三年度过的时间去计算月份即可
    每个月的秒数
      1月   2678400
      2月   平年 2419200   闰年 2505600   5097600   5184000
      3月   2678400   7776000   7862400
      4月   2592000   10368000  10454400
      5月   2678400   13046400  13132800
      6月   2592000   15638400  15724800
      7月   2678400   18316800  18403200
      8月   2678400   20995200  21081600
      9月   2592000   23587200  23673600
      10月  2678400   26265600  26352000
      11月  2592000   28857600  28944000
      12月  2678400   31536000  31622400
  */
  if(LeapYearFlag)  //闰年
  {
    MonthSecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000;
    if(MonthSecond > 28944000) Month = 12;
    else if(MonthSecond > 26352000) Month = 11;
    else if(MonthSecond > 23673600) Month = 10;
    else if(MonthSecond > 21081600) Month = 9;
    else if(MonthSecond > 18403200) Month = 8;
    else if(MonthSecond > 15724800) Month = 7;
    else if(MonthSecond > 13132800) Month = 6;
    else if(MonthSecond > 10454400) Month = 5;
    else if(MonthSecond > 7862400) Month = 4;
    else if(MonthSecond > 518400) Month = 3;
    else if(MonthSecond > 2678400) Month = 2;
    else Month = 1;
  }
  else //分为这个周期度过的时间小于三个平年和大于三个平年的情况，也只有这两种情况
  {
    if((JSONTime_1_6 + JSONTime_7_13)%126230400 < 94608000)
    {
      MonthSecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000;
    }
    else
    {
      MonthSecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000 - 86400;
    }
    if(MonthSecond > 28857600) Month = 12;
    else if(MonthSecond > 26265600) Month = 11;
    else if(MonthSecond > 23587200) Month = 10;
    else if(MonthSecond > 20995200) Month = 9;
    else if(MonthSecond > 18316800) Month = 8;
    else if(MonthSecond > 15638400) Month = 7;
    else if(MonthSecond > 13046400) Month = 6;
    else if(MonthSecond > 10368000) Month = 5;
    else if(MonthSecond > 7776000) Month = 4;
    else if(MonthSecond > 5097600) Month = 3;
    else if(MonthSecond > 2678400) Month = 2;
    else Month = 1;
    }
  /*
    日计算
    把每个月都分开来进行计算
    每个月的秒数
      1月   2678400
      2月   平年 2419200   闰年 2505600   5097600   5184000
      3月   2678400   7776000   7862400
      4月   2592000   10368000  10454400
      5月   2678400   13046400  13132800
      6月   2592000   15638400  15724800
      7月   2678400   18316800  18403200
      8月   2678400   20995200  21081600
      9月   2592000   23587200  23673600
      10月  2678400   26265600  26352000
      11月  2592000   28857600  28944000
      12月  2678400   31536000  31622400
  */
  if(LeapYearFlag)   //闰年
  {
    if(Month == 1) DaySecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000;
    else if(Month == 2) DaySecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000 - 2678400;
    else if(Month == 3) DaySecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000 - 5184000;
    else if(Month == 4) DaySecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000 - 7862400;
    else if(Month == 5) DaySecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000 - 10454400;
    else if(Month == 6) DaySecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000 - 13132800;
    else if(Month == 7) DaySecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000 - 15724800;
    else if(Month == 8) DaySecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000 - 18403200;
    else if(Month == 9) DaySecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000 - 21081600;
    else if(Month == 10) DaySecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000 - 23673600;
    else if(Month == 11) DaySecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000 - 26352000;
    else DaySecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000 - 28944000;
    Day = DaySecond/86400 + 1;
  }
  else
  {
    if(Month == 1) DaySecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000;
    else if(Month == 2) DaySecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000 - 2678400;
    else if(Month == 3) DaySecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000 - 5097600;
    else if(Month == 4) DaySecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000 - 7776000;
    else if(Month == 5) DaySecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000 - 10368000;
    else if(Month == 6) DaySecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000 - 13046400;
    else if(Month == 7) DaySecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000 - 15638400;
    else if(Month == 8) DaySecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000 - 18316800;
    else if(Month == 9) DaySecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000 - 20995200;
    else if(Month == 10) DaySecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000 - 23587200;
    else if(Month == 11) DaySecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000 - 26265600;
    else DaySecond = (JSONTime_1_6 + JSONTime_7_13)%126230400%31536000 - 28857600;
    Day = DaySecond/86400 + 1;
  }
  /*小时计算，每天的小时分钟和秒都是一样的，默认为UTC时间，+8:00才能换算成北京时间*/
  Hour = DaySecond%86400/3600;
  /*分钟计算，每天的小时分钟和秒都是一样的*/
  Minute = DaySecond%86400%3600/60;
  /*秒钟计算，每天的小时分钟和秒都是一样的*/
  Second = DaySecond%86400%3600%60;

  DateTime = Hour*10000+Minute*100+Second;
  return DateTime;   //返回的时间为DateTime格式的UTC时间
}
