#include <SPI.h>
#include "../nRF24L01.h"
#include "../RF24.h"

#define IRQ 8  //定义中断查询引脚

//const uint64_t pipe =  0x3443101001LL;
RF24 radio(9,10); //初始化类，定义引脚

void setup(void)
{
  Serial.begin(9600);
  radio.init_NRF24L01();  //初始化
  radio.SetTX_Mode();  //设置发送模式
}

void loop(void)
{
  uint8_t command[20];
  bool done = false;
  char i=0;
  for(i=0;i<20;i++){ 
    command[i]='a'+i;
  }
  radio.nRF24L01_TxPacket(command);  //发送数据包
  while(digitalRead(IRQ)==HIGH);  //等待发送完成
}
