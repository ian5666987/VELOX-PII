//------------------------------------------------------------------------------------
// Luxp_sd.c
//------------------------------------------------------------------------------------
// Copyright (C) 2013 Nanyang Technological University.
//
// AUTH: LEE ZHONG ZHENG
//		 IAN
//		 CHEW YAN CHONG
// DATE: 03 APR 2013
//
// The program Luxp_sd.c will be downloaded to the Pumpkin Motherboard with 
// C8051F120 uC as part of the VELOX-P Pico-Satellite Flight Software. 
//
// This file contains checking functions of the Flight Software
//
//------------------------------------------------------------------------------------
// Overall functional test status
//------------------------------------------------------------------------------------
// Completely untested
//
//------------------------------------------------------------------------------------
// Overall documentation status
//------------------------------------------------------------------------------------
// Minimally documented
//------------------------------------------------------------------------------------

#include <luxp_sd.h>

#if (STORING_ENABLE == 1)
short SPI0_failures = 0;
short SPI0_timeout = 500; // default timeout
unsigned char sd_crc7_ply = 0x12; // default value without initialization is chosen to be 0x12

#define INIT_DELAY_10us_UNIT  1 //Delkin: 1 | SANDisk: 7000
#define RNW_DELAY_10us_UNIT	  1 //Delkin: 1 | SANDisk: 20
#define STORE_DELAY_10us_UNIT 150 //Delkin: 150 | SANDisk: 200

// private function, use SPI0_getPutChars instead ! warning this page does not need to set SFRPAGE = SPI0_PAGE;
// Min-max time: 0.005-0.05ms with SPI0_timeout = 25
char SPI0_getPut(char* value) {
  short delay;
  // ---
  delay = 0;
  while (SPI0CFG & 0x80) // busy
    if (SPI0_timeout < ++delay) {
      ++SPI0_failures;
      return 0;
      break;
    }
  // ---
  SPI0DAT = *value; // send
  // ---
  delay = 0;
  while (!SPIF)
    if (SPI0_timeout < ++delay) {
      ++SPI0_failures;
      break;
    }
  // ---
  *value = SPI0DAT; // recv
  SPIF = 0;
  return 1;
}

// function writes length chars from ptr to SPI and simultaneously writes reply into ptr
void SPI0_putGetChars(void* buffer, short length) {
  short count;
  char* message = (char*) buffer;
  char SFRPAGE_SAVE = SFRPAGE;
  SFRPAGE = SPI0_PAGE;
  for (count = 0; count < length; ++count)
    SPI0_getPut(&message[count]); // return value is ignored
  SFRPAGE = SFRPAGE_SAVE;
}

// function writes length chars from ptr to SPI
// the reply is dropped on purpose
void SPI0_putChars(void* buffer, short length) {
  short count;
  char* message = (char*) buffer;
  char temp;
  char SFRPAGE_SAVE = SFRPAGE;
  SFRPAGE = SPI0_PAGE;
  for (count = 0; count < length; ++count) {
    temp = message[count];
    SPI0_getPut(&temp); // reply is discarded, return value is ignored
  }
  SFRPAGE = SFRPAGE_SAVE;
}

char SD_spiWriteByte(char value) {
  SPI0_putGetChars(&value, 1);
  return value;
}

unsigned char SD_spiReadByte() {
  unsigned char l_uc_spi_received_buffer = 0xff;
  SPI0_putGetChars(&l_uc_spi_received_buffer, 1);
  return l_uc_spi_received_buffer;
}

//Private function, only visible to SD data
unsigned char SD_crc7Rotate(unsigned char crc, unsigned char nxt_byte) {
  unsigned char i, highest_bit;
  for (i = 0; i < 8; ++i) {
    highest_bit = crc >> 7;
    crc = (crc << 1) + ((nxt_byte >> (7 - i)) & 0x01);
    if (highest_bit)
      crc ^= sd_crc7_ply;
  }
  return crc;
}

unsigned char SD_crc7Update(void* msg, short length, unsigned char crc) {
  unsigned short i = 0;
  char* message = (char*) msg;
  for (i = 0; i < length; ++i)
    crc = SD_crc7Rotate(crc, message[i]);
  return crc;
}

unsigned char SD_crc7Publish(unsigned char crc) {
  crc = SD_crc7Rotate(crc, 0);
  crc |= 0x01;
  return crc;
}

char SD_getUntil(unsigned char response, unsigned char flag) {
  unsigned char value;
  unsigned char retry = 0;
  for (retry = 0; retry < 5; ++retry){
    value = SD_spiReadByte();
    if ((value & flag) == response)
      return 1;
    waitUs(10);
  }
  return 0;
}

// return true if successful -> can trust value
// return false if limit reached -> do not trust content value
char SD_getUntilNot(unsigned char response, unsigned char flag, unsigned short delay_10us, unsigned char retry_limit, unsigned char* value_p) {
  unsigned char retry;
  unsigned short i;
  for (retry = 0; retry < retry_limit; ++retry) {
    *value_p = SD_spiReadByte();
    if ((*value_p & flag) != response)
      return 1;
    for (i = 0; i < delay_10us; ++i)
    	waitUs(10);
  }
  return 0;
}

//expected minimum time: 0.03 + 0.01 + 0.005 = 0.045ms
//expected maximum time: 0.3 + 0.3 + 0.05 = 0.65ms
char SD_sendCommand(char command, unsigned long arg, unsigned char* value_p, unsigned short delay_ms) {
  char success;
  char array[6];

  array[0] = command | 0x40;
  memcpy(&array[1], &arg, 4);
  array[5] = SD_crc7Publish(SD_crc7Update(array, 5, 0)); // there should be a checksum here

  NSSMD0 = SD_ASSERT;
  SPI0_putChars(array, 6); //Min-max time: 0.03 - 0.3ms
  success = SD_getUntilNot(0xff, 0xff, delay_ms, 5, value_p); // we do not expect to get 0xff return here, but other return is passed to the caller
  SD_spiReadByte(); //Min-max time: 0.005 - 0.05ms. Extra 8 clock for buffering, due to hardware synchronization problem.Some SD cards will return two extra bytes, others are one byte
  NSSMD0 = SD_DEASSERT;

  return success;
}

char SD_commandUntil(char cmd, unsigned long arg, char response, unsigned short delay_10us) {
  char success;
  unsigned char value;
  unsigned char retry;
  for (retry = 0; retry < 3; ++retry) { //Works on the 1st trial most of the time. Set the number of retry to be 5 here.
    success = SD_sendCommand(cmd, arg, &value, delay_10us);
    if (success && value == response)
      return 1;
    waitUs(10); //ian (2013-04-02): 10us works really well, gives about 10us delay here
  }
  return 0;
}

void SD_writeBlockData(void* buffer, short length) {
  short counter;
  SD_spiWriteByte(0xfe); // Send start block token 0xfe (0x11111110) //Min-max time: 0.005 - 0.05ms
  SPI0_putChars(buffer, length);
  for (counter = length; counter < SD_CARD_BUFFER_SIZE; ++counter)
    SD_spiWriteByte(0x00); // Min-max time: 2.56 - 25.6ms
  SD_spiWriteByte(0xff); // transmit dummy CRC (16-bit), CRC is ignored here
  SD_spiWriteByte(0xff); // Min-max time: 0.01 - 0.1ms
}

void SD_readBlockData(void* buffer, short length) {
  short counter;
  memset(buffer, 0xff, length);
  SPI0_putGetChars(buffer, length); //read 512 bytes
  for (counter = length; counter < SD_CARD_BUFFER_SIZE; counter++)
    SD_spiReadByte(); //Min-max time: 2.56 - 25.6ms
  SD_spiReadByte(); // receive incoming CRC (16-bit), CRC is ignored here
  SD_spiReadByte();
  SD_spiReadByte(); // Extra 8 clock for buffering. Min-max time: 0.015 - 0.15ms
}

void SD_eraseBlockData() {
  short counter;
  SD_spiWriteByte(0xfe); // Send start block token 0xfe (0x11111110) //Min-max time: 0.005 - 0.05ms
  for (counter = 0; counter < SD_CARD_BUFFER_SIZE; ++counter)
    SD_spiWriteByte(0xff); // Min-max time: 2.56 - 25.6ms
  SD_spiWriteByte(0xff); // transmit dummy CRC (16-bit), CRC is ignored here
  SD_spiWriteByte(0xff); // Min-max time: 0.01 - 0.1ms
}

//------------------------------------------------------------------------------------
// SD_Card_Initialization
//------------------------------------------------------------------------------------
unsigned short initSD (void)
{
  unsigned char success = 1;
  unsigned char dummy_value;
  char array[10];
  memset(array, 0xff, sizeof array);
  if (!SD_sendCommand(SD_SEND_STATUS, 0, &dummy_value, 1)) { //Min-max time: 0.045-0.65ms ian (2013-05-29): there was a silly mistake here =.=' -> SD_sendCommand(SD_SEND_STATUS, 0) is changed to !SD_sendCommand(SD_SEND_STATUS, 0)
    SPI0_putChars(array, 10); // send 10 bytes of data to make sure that SD card is receiving. Min-max time: 0.05-0.5ms
    success = SD_commandUntil(SD_GO_IDLE_STATE, 0, 0x01, INIT_DELAY_10us_UNIT); // ian (2013-07-22): first initialization time has to be set higher even 42ms is not enough for SANDisk SD card! Set as 70ms x 9 = 630ms for safety reason. Ian (2012-05-15): 0x01 is the expected return from SD card when it receives idle state command.
    if (!success)
      return EC_SD_CARD_INITIALIZATION_ERROR;
    SPI0_putChars(array, 2); // Flushing, to make sure SD card is clear to receive command. Min-max time: 0.01 - 0.1ms
    success = SD_commandUntil(SD_SEND_OP_COND, 0, 0x00, 1);
  }  
  return success ? EC_SUCCESSFUL : EC_SD_CARD_INITIALIZATION_ERROR;
}

//------------------------------------------------------------------------------------
//Function: to write to a single block of SD card
//Arguments: none
//return: unsigned char; will be 0 if no error,
// otherwise the response byte will be sent
//------------------------------------------------------------------------------------
#define SD_CARD_COMMAND_SHIFT_BIT  9
unsigned short sdWriteSingleBlock(unsigned long block, void* buffer, unsigned short length)
{
  unsigned char value;
  char success;
  unsigned short ec_init;
  ec_init = initSD();
  if (ec_init != EC_SUCCESSFUL) //ian (2013-04-09): add this. //Min-max time: 0.16 - 10.55ms
    return ec_init;
  success = SD_commandUntil(SD_WRITE_SINGLE_BLOCK, block << SD_CARD_COMMAND_SHIFT_BIT, 0, RNW_DELAY_10us_UNIT); //ian 2013-07-22: 70us microsecond x 5 retry is minimum delay for SANDisk. Put 200us x 9 = 1.8ms for safety reason
  if (!success) // sdSendCommand shall return non-zero value when there is an error by hardware design.. check for SD status: 0x00 - OK (No flags set). // write a Block command. Min-max time: 0.045-0.65ms
    return EC_SD_CARD_WRITE_BLOCK_ERROR;
  NSSMD0 = SD_ASSERT;
  SD_writeBlockData(buffer, length);
  if (!SD_getUntil(0x05, 0x1f)) //response= 0xXXX0AAA1; AAA='010' - data accepted, //AAA='101'-data rejected due to CRC error. //AAA='110'-data rejected due to write error
    return EC_SD_CARD_WRITE_BLOCK_ERROR;
  if (!SD_getUntilNot(0x00, 0xff, STORE_DELAY_10us_UNIT, 100, &value)) // we are waiting for any non zero reply from SD card
    return EC_SD_CARD_WRITE_BLOCK_ERROR;
  SD_spiWriteByte(0xff); // Extra 8 clock for buffering. Min-max time: 0.005 - 0.05ms
  NSSMD0 = SD_DEASSERT;
  return EC_SUCCESSFUL;
}

//******************************************************************
//Function: to read a single block from SD card
//Arguments: none
//return: unsigned char; will be 0 if no error,
// otherwise the response byte will be sent
//******************************************************************
unsigned short sdReadSingleBlock(unsigned long block)
{
  char success;
  unsigned short ec_init;
  ec_init = initSD();
  if (ec_init != EC_SUCCESSFUL) //ian (2013-04-09): add this. //Min-max time: 0.16 - 10.55ms
    return ec_init;
  success = SD_commandUntil(SD_READ_SINGLE_BLOCK, block << SD_CARD_COMMAND_SHIFT_BIT, 0, RNW_DELAY_10us_UNIT); //ian 2013-07-22: 70us microsecond x 5 retry is minimum delay for SANDisk. Put 200us x 9 = 1.8ms for safety reason
  if (!success) // read a Block command. Min-max time: 0.045-0.65ms. check for SD status: 0x00 - OK (No flags set)
    return EC_SD_CARD_READ_BLOCK_ERROR;
  NSSMD0 = SD_ASSERT;
  if (!SD_getUntil(0xfe, 0xff)) // 0xfe is the start block token expected to be received from SD card when it is ready to send one block of data.
    return EC_SD_CARD_READ_BLOCK_ERROR;
  SD_readBlockData(g_uc_sd_data_buffer, SD_CARD_BUFFER_SIZE);
  NSSMD0 = SD_DEASSERT;
  return EC_SUCCESSFUL;
}

#if (OBDH_CMD_ENABLE == 1)
//------------------------------------------------------------------------------------
//Function: to erase single block in SD card
//Arguments: none
//return: unsigned char; will be 0 if no error,
// otherwise the response byte will be sent
//------------------------------------------------------------------------------------
unsigned short sdEraseSingleBlock(unsigned long block)
{
  unsigned char value;
  char success;
  unsigned short ec_init;
  ec_init = initSD();
  if (ec_init != EC_SUCCESSFUL) //ian (2013-04-09): add this. //Min-max time: 0.16 - 10.55ms
    return ec_init;
  success = SD_commandUntil(SD_WRITE_SINGLE_BLOCK, block << SD_CARD_COMMAND_SHIFT_BIT, 0, RNW_DELAY_10us_UNIT); //ian 2013-07-22: 70us microsecond x 5 retry is minimum delay for SANDisk. Put 200us x 9 = 1.8ms for safety reason
  if (!success) // sdSendCommand shall return non-zero value when there is an error by hardware design.. check for SD status: 0x00 - OK (No flags set). // write a Block command. Min-max time: 0.045-0.65ms
    return EC_SD_CARD_ERASE_BLOCK_ERROR;
  NSSMD0 = SD_ASSERT;
  SD_eraseBlockData();
  if (!SD_getUntil(0x05, 0x1f)) //response= 0xXXX0AAA1; AAA='010' - data accepted, //AAA='101'-data rejected due to CRC error. //AAA='110'-data rejected due to write error
    return EC_SD_CARD_ERASE_BLOCK_ERROR;
  if (!SD_getUntilNot(0x00, 0xff, STORE_DELAY_10us_UNIT, 100, &value)) // we are waiting for any non zero reply from SD card
    return EC_SD_CARD_ERASE_BLOCK_ERROR;
  SD_spiWriteByte(0xff); // Extra 8 clock for buffering. Min-max time: 0.005 - 0.05ms
  NSSMD0 = SD_DEASSERT;
  return EC_SUCCESSFUL;
}
#endif
#endif

