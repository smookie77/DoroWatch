#include "Wire.h"

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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);

  pinMode(32, OUTPUT);
  digitalWrite(32, LOW);

  Wire.begin(21,22);   //初始化IIC，设置引脚 SCL:21  SDA:22
  Wire.setClock(100000);  //设置频率100KHZ
  delay(100);
  
  /*触摸复位*/
  pinMode(25, OUTPUT);
  digitalWrite(25, LOW);
  delay(10);
  digitalWrite(25, HIGH);
  delay(50);

  Wire.beginTransmission(TouchI2CAddr);   //开始传输
  Wire.write(ChipIdRegister);
  Wire.endTransmission(false);  //发送前面的指令，但不释放总线不发送结束
  Wire.beginTransmission(TouchI2CAddr);   //开始传输
  Wire.requestFrom(TouchI2CAddr, 1 ,true);   //参数为ture请求完数据会发送结束
  ChipID = Wire.read();

  Serial.printf("TouchChipID：0x%02X",ChipID);
  if(ChipID == CST716ChipId) Serial.println(",Touch chip model :CST716");
  else if(ChipID == CST816SChipId) Serial.println(",Touch chip model :CST816S");
  else if(ChipID == CST816TChipId) Serial.println(",Touch chip model :CST816T");
  else if(ChipID == CST816DChipId) Serial.println(",Touch chip model :CST816D");
  else if(ChipID == CST826ChipId) Serial.println(",Touch chip model :CST826");
  else if(ChipID == CST830ChipId) Serial.println(",Touch chip model :CST830");
  else if(ChipID == CST836UChipId) Serial.println(",Touch chip model :CST836U");
  else Serial.println(",error!");

/*
  pinMode(4,INPUT_PULLUP);
  pinMode(18,INPUT_PULLUP);
  pinMode(19,INPUT_PULLUP);
  pinMode(23,INPUT_PULLUP);
*/
  pinMode(26,INPUT_PULLUP);

  /*触摸初始化*/


}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned int X_H4 = 0;
  unsigned int X_L8 = 0;
  unsigned int Y_H4 = 0;
  unsigned int Y_L8 = 0;
  unsigned int X_Point = 0;
  unsigned int Y_Point = 0;


   if(!digitalRead(26))
  {
  Serial.println("有触摸！");
  
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

  X_Point = (X_H4 << 8 | X_L8)&0X0FFF;
  Y_Point = (Y_H4 << 8 | Y_L8)&0X0FFF;

  Serial.printf("Touch Point:%02X , %02X \r\n",X_Point,Y_Point);
    }
    delay(9);

/*
  digitalWrite(21,HIGH);
  digitalWrite(22,LOW);
  delay(500); // 1 second
  digitalWrite(21,LOW);
  digitalWrite(22,HIGH);
  delay(500); // 1 second
  */
}
