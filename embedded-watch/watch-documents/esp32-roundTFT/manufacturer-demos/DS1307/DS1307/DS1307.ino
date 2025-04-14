#include "Wire.h"

#define Ds1307I2CAddr 0x68

#define Ds1307_SecondsRegister 0x00
#define Ds1307_MinutesRegister 0x01
#define Ds1307_HoursRegister 0x02
#define Ds1307_DayRegister 0x03
#define Ds1307_DateRegister 0x04
#define Ds1307_MonthRegister 0x05
#define Ds1307_YearRegister 0x06
#define Ds1307_ControlRegister 0x07

int Ds1307_SecondsVlue = 0x00;
int Ds1307_MinutesVlue = 0x00;
int Ds1307_HoursVlue = 0x00;
int Ds1307_DayVlue = 0x00;
int Ds1307_DateVlue = 0x00;
int Ds1307_MonthVlue = 0x00;
int Ds1307_YearVlue = 0x00;
int Ds1307_ControlVlue = 0x00;

int Seconds = 0x00;
int Minutes = 0x00;
int Hours = 0x00;
int Day = 0x00;
int Date = 0x00;
int Month = 0x00;
int Year = 0x00;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);

  Wire.begin(21,22);   //初始化IIC，设置引脚 SCL:21  SDA:22
  Wire.setClock(100000);  //设置频率100KHZ
  delay(100);

  /*DS1307初始化*/
  //时钟允许
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.write(Ds1307_SecondsRegister);
  Wire.endTransmission(false);  //发送前面的指令，但不释放总线不发送结束
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.requestFrom(Ds1307I2CAddr, 1 ,true);   //参数为ture请求完数据会发送结束
  Ds1307_SecondsVlue = Wire.read();

  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.write(Ds1307_SecondsRegister);
  Wire.write(0x7F & Ds1307_SecondsVlue);
  Wire.endTransmission(true);  //发送前面的指令，但不释放总线不发送结束

  //设置24小时制
    Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.write(Ds1307_HoursRegister);
  Wire.endTransmission(false);  //发送前面的指令，但不释放总线不发送结束
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.requestFrom(Ds1307I2CAddr, 1 ,true);   //参数为ture请求完数据会发送结束
  Ds1307_HoursVlue = Wire.read();

  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.write(Ds1307_HoursRegister);
  Wire.write(0xBF & Ds1307_HoursVlue);
  Wire.endTransmission(true);  //发送前面的指令，但不释放总线不发送结束

  #if 0
  //设置时间
  Seconds = 30;
  Minutes = 10;
  Hours = 20;

  Day = 4;
  Date = 4;
  Month = 5;
  Year = 23;
  /*
  //设置小时
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.write(Ds1307_HoursRegister);
  Wire.endTransmission(false);  //发送前面的指令，但不释放总线不发送结束
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.requestFrom(Ds1307I2CAddr, 1 ,true);   //参数为ture请求完数据会发送结束
  Ds1307_HoursVlue = Wire.read();
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.write(Ds1307_HoursRegister);
  Wire.write((Ds1307_HoursVlue & 0xC0) | (Hours / 10) << 4 | (Hours % 10));
  Wire.endTransmission(true);  //发送前面的指令，但不释放总线不发送结束
  //设置分钟
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.write(Ds1307_MinutesRegister);
  Wire.endTransmission(false);  //发送前面的指令，但不释放总线不发送结束
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.requestFrom(Ds1307I2CAddr, 1 ,true);   //参数为ture请求完数据会发送结束
  Ds1307_MinutesVlue = Wire.read();
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.write(Ds1307_MinutesRegister);
  Wire.write((Ds1307_MinutesVlue & 0x80) | (Minutes / 10) << 4 | (Minutes % 10));
  Wire.endTransmission(true);  //发送前面的指令，但不释放总线不发送结束
  //设置秒钟
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.write(Ds1307_SecondsRegister);
  Wire.endTransmission(false);  //发送前面的指令，但不释放总线不发送结束
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.requestFrom(Ds1307I2CAddr, 1 ,true);   //参数为ture请求完数据会发送结束
  Ds1307_SecondsVlue = Wire.read();
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.write(Ds1307_SecondsRegister);
  Wire.write((Ds1307_SecondsVlue & 0x80) | (Seconds / 10) << 4 | (Seconds % 10));
  Wire.endTransmission(true);  //发送前面的指令，但不释放总线不发送结束
  */
  //设置星期几
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.write(Ds1307_DayRegister);
  Wire.write(Day & 0x07);
  Wire.endTransmission(true);  //发送前面的指令，但不释放总线不发送结束
  //设置几号
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.write(Ds1307_DateRegister);
  Wire.write(((Date/10) << 4 | (Date%10)) & 0x3F);
  Wire.endTransmission(true);  //发送前面的指令，但不释放总线不发送结束
  //设置几月
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.write(Ds1307_MonthRegister);
  Wire.write(((Month/10) << 4 | (Month%10)) & 0x1F);
  Wire.endTransmission(true);  //发送前面的指令，但不释放总线不发送结束
  //设置年
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.write(Ds1307_YearRegister);
  Wire.write(((Year/10) << 4 | (Year%10)) & 0xFF);
  Wire.endTransmission(true);  //发送前面的指令，但不释放总线不发送结束

  #endif
}

void loop() {
  // put your main code here, to run repeatedly:
  //获取秒
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.write(Ds1307_SecondsRegister);
  Wire.endTransmission(false);  //发送前面的指令，但不释放总线不发送结束
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.requestFrom(Ds1307I2CAddr, 1 ,true);   //参数为ture请求完数据会发送结束
  Ds1307_SecondsVlue = Wire.read();
  //获取分
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.write(Ds1307_MinutesRegister);
  Wire.endTransmission(false);  //发送前面的指令，但不释放总线不发送结束
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.requestFrom(Ds1307I2CAddr, 1 ,true);   //参数为ture请求完数据会发送结束
  Ds1307_MinutesVlue = Wire.read();
  //获取小时
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.write(Ds1307_HoursRegister);
  Wire.endTransmission(false);  //发送前面的指令，但不释放总线不发送结束
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.requestFrom(Ds1307I2CAddr, 1 ,true);   //参数为ture请求完数据会发送结束
  Ds1307_HoursVlue = Wire.read();
  //获取星期
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.write(Ds1307_DayRegister);
  Wire.endTransmission(false);  //发送前面的指令，但不释放总线不发送结束
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.requestFrom(Ds1307I2CAddr, 1 ,true);   //参数为ture请求完数据会发送结束
  Ds1307_DayVlue = Wire.read();
  //获取日
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.write(Ds1307_DateRegister);
  Wire.endTransmission(false);  //发送前面的指令，但不释放总线不发送结束
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.requestFrom(Ds1307I2CAddr, 1 ,true);   //参数为ture请求完数据会发送结束
  Ds1307_DateVlue = Wire.read();
  //获取月
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.write(Ds1307_MonthRegister);
  Wire.endTransmission(false);  //发送前面的指令，但不释放总线不发送结束
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.requestFrom(Ds1307I2CAddr, 1 ,true);   //参数为ture请求完数据会发送结束
  Ds1307_MonthVlue = Wire.read();
  //获取年
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.write(Ds1307_YearRegister);
  Wire.endTransmission(false);  //发送前面的指令，但不释放总线不发送结束
  Wire.beginTransmission(Ds1307I2CAddr);   //开始传输
  Wire.requestFrom(Ds1307I2CAddr, 1 ,true);   //参数为ture请求完数据会发送结束
  Ds1307_YearVlue = Wire.read();



  Seconds = ((Ds1307_SecondsVlue & 0x70) >> 4)*10 + (Ds1307_SecondsVlue & 0x0F);
  Minutes = ((Ds1307_MinutesVlue & 0x70) >> 4)*10 + (Ds1307_MinutesVlue & 0x0F);
  Hours = ((Ds1307_HoursVlue & 0x30) >> 4)*10 + (Ds1307_HoursVlue & 0x0F);
  Day = Ds1307_DayVlue;
  Date = ((Ds1307_DateVlue & 0x30) >> 4)*10 + (Ds1307_DateVlue & 0x0F);
  Month = ((Ds1307_MonthVlue & 0x10) >> 4)*10 + (Ds1307_MonthVlue & 0x0F);
  Year = ((Ds1307_YearVlue & 0xF0) >> 4)*10 + (Ds1307_YearVlue & 0x0F);


  Serial.printf("%2d-%2d-%2d   %2d:%2d:%2d  +week:%2d\r\n",Year,Month,Date,Hours,Minutes,Seconds,Day);

  delay(1000);
}
