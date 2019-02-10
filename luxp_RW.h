#ifndef luxp_RW_h
#define luxp_RW_h

#include <C8051F120.H>
#include <luxp_consvar.h>
#include <luxp_time.h>

//SMBus RW functions
unsigned short i2cWrite (void);			//To write to SMBus line
unsigned short i2cRead (void);			//To read from SMBus line
unsigned short i2cCheckLine (void);
unsigned short i2cTransfer (void);
unsigned short i2cWaitRead (unsigned char l_uc_delay_before_10thms, unsigned char l_uc_delay_after_10thms);		//To read from SMBus line with some waiting time
//unsigned short i2cWait (void);			//To wait data from SMBus line
//void i2cEnable (void);			//To enable I2C communication
//void i2cDisable (void);			//To disable I2C communication

#endif
