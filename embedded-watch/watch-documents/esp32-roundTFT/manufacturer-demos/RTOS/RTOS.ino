void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);
  
  //xTaskCreate(Task_one/*任务函数*/,"task_one"/*任务名称字符串*/,10000/*堆栈大小（字节）*/,NULL/*参数作为任务输入传递*/,1/*任务优先级*/,NULL/*任务句柄*/,1);
  //调用xTaskCreate以创建任务Task_one

  xTaskCreatePinnedToCore(Task_one/*任务函数*/,"task_one"/*任务名称字符串*/,10000/*堆栈大小（字节）*/,NULL/*参数作为任务输入传递*/,1/*任务优先级*/,NULL/*任务句柄*/,0/*运行在核心0*/);

  xTaskCreatePinnedToCore(Task_two, "task_two", 10000, NULL, 1, NULL, 1);
  
  
  //xTaskCreate(Task_two,"task_two",10000,NULL,1,NULL);
  //调用xTaskCreate以创建任务Task_two
}



void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
}

void Task_one(void *parameter)
{
      delay(100);
      Serial.print("这是任务Task_one的执行结果！\r\n任务执行在：核心");
      Serial.println(xPortGetCoreID());
        
      Serial.println("任务Task_one的执行结束！");
      vTaskDelete( NULL );//任务内调用TaskDelete函数可删除任务本身，输入的参数为任务的任务句柄
      Serial.println("这是任务Task_one的执行结果！");//在后面再放置串口输出函数发现没有输出，说明任务已被删除
  }

void Task_two(void *parameter)
{
        delay(500);
        Serial.print("这是任务Task_two的执行结果！\r\n任务执行在：核心");
        Serial.println(xPortGetCoreID());
        

      Serial.println("任务Task_two的执行结束！");
      vTaskDelete( NULL );//任务内调用TaskDelete函数可删除任务本身，输入的参数为任务的任务句柄
  }
