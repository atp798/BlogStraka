#include "nRF24L01.h"
#include "RF24_config.h"
#include "RF24.h"

RF24::RF24(uint8_t _cepin, uint8_t _cspin):
  ce_pin(_cepin), csn_pin(_cspin), wide_band(true), p_variant(false), 
  payload_size(20), ack_payload_available(false), dynamic_payloads_enabled(false),
  pipe0_reading_address(0)
{
  // Minimum ideal SPI bus speed is 2x data rate
  // If we assume 2Mbs data rate and 16Mhz clock, a
  // divider of 4 is the minimum we want.
  // CLK:BUS 8Mhz:2Mhz, 16Mhz:4Mhz, or 20Mhz:5Mhz
#ifdef ARDUINO
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV4);
#endif
  pinMode(ce_pin,OUTPUT);
  pinMode(csn_pin,OUTPUT);
  SPI.begin();
  //TX_ADDRESS[TX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01}; 
  RX_ADDRESS[0]= 0x34;
  RX_ADDRESS[1]= 0x43;
  RX_ADDRESS[2]= 0x10;
  RX_ADDRESS[3]= 0x10;
  RX_ADDRESS[4]= 0x01;
  TX_ADDRESS[0]= 0x34;
  TX_ADDRESS[1]= 0x43;
  TX_ADDRESS[2]= 0x10;
  TX_ADDRESS[3]= 0x10;
  TX_ADDRESS[4]= 0x01;
}

void RF24::init_NRF24L01(void)
{
   ce(LOW);
  csn(HIGH);
  //sck(LOW); 
  SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // Ð´±¾µØµØÖ·  
  SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); // Ð´½ÓÊÕ¶ËµØÖ·
  SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);      //  ÆµµÀ0×Ô¶¯ ACKÓ¦´ðÔÊÐí 
  SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  //  ÔÊÐí½ÓÊÕµØÖ·Ö»ÓÐÆµµÀ0£¬Èç¹ûÐèÒª¶àÆµµÀ¿ÉÒÔ²Î¿¼Page21  
  SPI_RW_Reg(WRITE_REG + RF_CH, 0x6e);        //   ÉèÖÃÐÅµÀ¹¤×÷Îª2.4GHZ£¬ÊÕ·¢±ØÐëÒ»ÖÂ
  SPI_RW_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //ÉèÖÃ½ÓÊÕÊý¾Ý³¤¶È£¬±¾´ÎÉèÖÃÎª32×Ö½Ú
  SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x27);       //ÉèÖÃ·¢ÉäËÙÂÊÎª1MB/S£¬·¢Éä¹¦ÂÊÎª×î´óÖµ+7dB£¬ÓÉÓÚÓÐX2401L¹¦·Å£¬Êµ¼Ê+21dbmÊä³ö
}

void RF24::csn(int mode)
{
  digitalWrite(csn_pin,mode);
}

void RF24::ce(int level)
{
  digitalWrite(ce_pin,level);
}

uint8_t RF24::SPI_RW(uint8_t dat){
    return SPI.transfer(dat);
  }

uint8_t  RF24::SPI_READ(uint8_t reg){
    uint8_t reg_val ;
    csn(LOW);
    delayMicroseconds(12);
    SPI.transfer(reg);
    delayMicroseconds(12);
    reg_val = SPI.transfer(0);
    delayMicroseconds(12);
    csn(HIGH);
    return reg_val;
  }

uint8_t  RF24::SPI_RW_Reg(uint8_t reg, uint8_t value)
{
  uint8_t sta;  
  csn(LOW);                   // CSN low, init SPI transaction
  sta = SPI.transfer(reg);      // select register
  SPI.transfer(value);             // ..and write value to it..
  csn(HIGH);                   // CSN high again
  
  return(sta);            // return nRF24L01 status
}

uint8_t  RF24::SPI_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars)
{
  uint8_t sta,uchar_ctr;
  
  csn(LOW);            //SPIÊ¹ÄÜ       
  sta = SPI.transfer(reg);   
  for(uchar_ctr=0; uchar_ctr<uchars; uchar_ctr++) //
    pBuf[uchar_ctr]= SPI.transfer(0x0);
  csn(HIGH);           //¹Ø±ÕSPI
  return(sta);    // 
}

uint8_t  RF24::SPI_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars)
{
  uint8_t sta,uchar_ctr;
  
  csn(LOW);            //SPIÊ¹ÄÜ       
  sta = SPI.transfer(reg);   
  for(uchar_ctr=0; uchar_ctr<uchars; uchar_ctr++) //
  SPI.transfer(*pBuf++);
  csn(HIGH);           //¹Ø±ÕSPI
  return(sta);    // 
}

void RF24::SetRX_Mode(void)
{
  ce(LOW);
  SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);      // IRQÊÕ·¢Íê³ÉÖÐ¶ÏÏìÓ¦£¬16Î»CRC £¬Ö÷½ÓÊÕ
  ce(HIGH);
}

void RF24::SetTX_Mode(void)
{
  ce(LOW);
  SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);      // IRQÊÕ·¢Íê³ÉÖÐ¶ÏÏìÓ¦£¬16Î»CRC £¬Ö÷½ÓÊÕ
  ce(HIGH);
}

bool RF24::nRF24L01_RxPacket( uint8_t * rx_buf, uint8_t len )
{
  bool revale = false;
  uint8_t sta=0x00;
  sta = SPI_READ(STATUS);
  ce(LOW);
  if(sta & _BV(RX_DR)){
    //write_register(STATUS,_BV(RX_DR) );
      SPI_Read_Buf(R_RX_PAYLOAD,rx_buf,20);
      revale =true;
    }
 SPI_RW_Reg(WRITE_REG+STATUS,sta);
 delayMicroseconds(30);
 SPI_RW_Reg(0xE2,0xff);
 ce(HIGH);
 
 return revale;
}

bool RF24::nRF24L01_TxPacket( uint8_t * tx_buf)
{
  ce(LOW);
  SPI_RW_Reg(0xE1,0xff);  
  SPI_Write_Buf(W_TX_PAYLOAD, tx_buf,payload_size);            
  SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);    
  SPI_RW_Reg(WRITE_REG+STATUS,0xff);
 ce(HIGH);   
   delayMicroseconds(12);
}

void RF24::SPI_CLR_Reg(uint8_t R_T)
{
    csn(LOW); 
  if(R_T==1)                  // CSN low, init SPI transaction
    SPI_RW(FLUSH_TX);             // ..and write value to it..
  else
  SPI_RW(FLUSH_RX);             // ..and write value to it..
    csn(HIGH);                  // CSN high again
}

void RF24::ifnnrf_CLERN_ALL()
{
  SPI_CLR_Reg(0);
  SPI_CLR_Reg(1);
  SPI_RW_Reg(WRITE_REG+STATUS,0xff);
  //IRQ=1;
}
