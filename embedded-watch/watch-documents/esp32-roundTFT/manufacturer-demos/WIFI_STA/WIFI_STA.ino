#include<WiFi.h>

const char *ssid = "PADAVAN";
const char *password = "122232542";

void WiFi_state_judgment()//判断wifi当前状态,串口返回
{
  int Networking_state;
  Networking_state=WiFi.status();
  Serial.print("WiFi状态：");
  if(Networking_state==WL_IDLE_STATUS)
  {
    Serial.println("正在WiFi工作模式间切换");
    }
    else if(Networking_state==WL_NO_SSID_AVAIL)
  {
    Serial.println("无法访问设置的SSID网络");
    }
  else if(Networking_state==WL_SCAN_COMPLETED)
  {
    Serial.println("扫描完成");
    }
    else if(Networking_state==WL_CONNECTED)
  {
    Serial.println("连接成功");
    }
    else if(Networking_state==WL_CONNECT_FAILED)
  {
    Serial.println("连接失败");
    }
    else if(Networking_state==WL_CONNECTION_LOST)
  {
    Serial.println("丢失连接");
    }
    else if(Networking_state==WL_DISCONNECTED)
  {
    Serial.println("断开连接");
    }
  }

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  Serial.println("** Starting STA");
  bool setHostname("JXL_ESP32");
  WiFi.begin(ssid,password);//设置需要连接的WIFI的名称和密码

  while (WiFi.status() != WL_CONNECTED) //等待网络连接成功,WL_CONNECTED为连接成功
  {
    delay(500);
    Serial.print("未连接");
  }
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); //打印模块IP
}

void loop() {
  // put your main code here, to run repeatedly:
  WiFi_state_judgment();
  delay(1000);
}
