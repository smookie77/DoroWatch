#include <Arduino.h>
#include <Ticker.h>
#include <esp_heap_caps.h>
 
// attach a LED to pPIO 21
#define LED_PIN 21
 
Ticker blinker;
Ticker toggler;
Ticker changer;
float blinkerPace = 0.1;  //seconds
const float togglePeriod = 5; //seconds
 
 
 
void change() {
  blinkerPace = 1.5;
}
 
void blink() {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
}

void toggle() {
  static bool isBlinking = false;
  if (isBlinking) {
    blinker.detach();
    isBlinking = false;
  }
  else {
    blinker.attach(blinkerPace, blink);
    isBlinking = true;
  }
  digitalWrite(LED_PIN, LOW);  //make sure LED on on after toggling (pin LOW = led ON)
}
 
void setup() {
  pinMode(LED_PIN, OUTPUT);
  toggler.attach(togglePeriod, toggle);
  changer.once(30, change);
  Serial.begin(115200);
  Serial.println("hello esp32");
  Serial.println(ESP.getFreeHeap());
 
  Serial.print("befor alloc:");
  Serial.println(heap_caps_get_free_size( MALLOC_CAP_SPIRAM ));
  void* ptrVal = NULL;
  ptrVal = heap_caps_malloc(1000, MALLOC_CAP_SPIRAM);
  char* sz_ptr = (char*)ptrVal;
  sprintf(sz_ptr, "%s", "hello spi ram..................\n");
 
  Serial.print(sz_ptr);
 
  Serial.print("after alloc:");
  Serial.println(heap_caps_get_free_size( MALLOC_CAP_SPIRAM ));
 
  heap_caps_free(ptrVal);
  sz_ptr = NULL;
 
  Serial.print("after release:");
  Serial.println(heap_caps_get_free_size( MALLOC_CAP_SPIRAM ));
}
 
void loop() {
  
}
