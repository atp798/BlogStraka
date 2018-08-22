#include <SPI.h>
#include "../nRF24L01.h"
#include "../RF24.h"

//const uint64_t pipe =  0x3443101001LL;
RF24 radio(9,10);

void setup(void)
{
  Serial.begin(9600);
  radio.init_NRF24L01();
}

void loop(void)
{
  uint8_t command[20];
  bool done = false;
  char i=0;
  radio.SetRX_Mode();  //设置读模式
  delay(100);
  if(radio.nRF24L01_RxPacket(command,20)){  //接受数据包
     for(i=0;i<20;i++){ 
         Serial.print(command[i]);
         Serial.print('-');
     }
     Serial.println("*");
     radio.ifnnrf_CLERN_ALL();  //判断未读完
   }
}
