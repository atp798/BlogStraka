#ifndef __RF24_H__
#define __RF24_H__

class RF24
{
private:
  uint8_t ce_pin; /**< "Chip Enable" pin, activates the RX or TX role */
  uint8_t csn_pin; /**< SPI Chip select */
  bool wide_band; /* 2Mbs data rate in use? */
  bool p_variant; /* False for RF24L01 and true for RF24L01P */
  uint8_t payload_size; /**< Fixed size of payloads */
  bool ack_payload_available; /**< Whether there is an ack payload waiting */
  bool dynamic_payloads_enabled; /**< Whether dynamic payloads are enabled. */ 
  uint8_t ack_payload_length; /**< Dynamic size of pending ack payload. */
  uint64_t pipe0_reading_address; /**< Last address set on pipe 0 for reading. */
uint8_t TX_ADDRESS[TX_ADR_WIDTH];
uint8_t RX_ADDRESS[RX_ADR_WIDTH];
public:
RF24(uint8_t _cepin, uint8_t _cspin);
  void csn(int mode);
  void ce(int level);
  uint8_t SPI_RW(uint8_t dat);
uint8_t  SPI_READ(uint8_t reg);
uint8_t  SPI_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars);
uint8_t  SPI_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars);
uint8_t  SPI_RW_Reg(uint8_t reg, uint8_t value);
void SetRX_Mode(void);
void SetTX_Mode(void);
bool nRF24L01_RxPacket( uint8_t * buf, uint8_t len );
bool nRF24L01_TxPacket( uint8_t * tx_buf);
void init_NRF24L01(void);void SPI_CLR_Reg(uint8_t R_T);
void ifnnrf_CLERN_ALL();


};




#endif // __RF24_H__
