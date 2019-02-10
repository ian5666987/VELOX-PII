//------------------------------------------------------------------------------------
// Luxp_RW.c
//------------------------------------------------------------------------------------
// Copyright (C) 2013 Nanyang Technological University.
//
// AUTH: IAN
// DATE: 03 APR 2013
//
// The program Luxp_RW.c will be downloaded to the Pumpkin Motherboard with 
// C8051F120 uC as part of the VELOX-P Pico-Satellite Flight Software. 
//
// This file contains all basic read and write functions of the Flight Software.
//
//------------------------------------------------------------------------------------
// Overall functional test status
//------------------------------------------------------------------------------------
// Completely tested, all fully functional
//
//------------------------------------------------------------------------------------
// Overall documentation status
//------------------------------------------------------------------------------------
// Completely documented, with partial long descriptions
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// List of functions				| Test status					| Documentation
//------------------------------------------------------------------------------------
// i2cWrite							| Tested, fully functional		| Complete, long
// i2cRead							| Tested, fully functional		| Complete, long
// i2cWait							| Tested, fully functional		| Complete, short
//------------------------------------------------------------------------------------

#include <luxp_RW.h>

//------------------------------------------------------------------------------------
// I2C Read and Write Functions
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// i2cWrite
//------------------------------------------------------------------------------------
//-------------------
// Short Description
//-------------------
// This function will be writing data to I2C bus line as a master transmitter. 
//
//-------------------
// Return Value 
//-------------------
// 1) l_us_error_code (unsigned char)
//		a) EC_SUCCESSFUL - successful I2C Write.
//		b) EC_I2C_WRITE_INCOMPLETE_TRANSMISSION - timeout after data transmission 
//		   starts, before it is finished.
//		c) EC_I2C_BUS_BUSY - timeout before data transmission, while waiting bus 
//		   to be freed.
//		d) EC_I2C_ISR_SLAVE_ADDR_NACK - slave device does not acknowledge.
//
//-------------------
// Parameters
//-------------------
// none
//
//------------------
// Long Description
//------------------
// The function will first check whether the I2C bus line is busy or not before it 
// starts to claim the I2C bus line for data wrting. 
//
// If the I2C bus line is found to be free, it will claim the line and start 
// the writing data transfer. 
//
// If the bus is busy, it will wait until the bus is not busy or (to avoid indefinite 
// waiting) until the timeout period of timeout ms occurs, determined by the 
// value of I2C_BUSY_TIME_LIMIT * timer4 overflow period. 
// 
// If the I2C Bus is still busy after the timeout period, it will return error
// code EC_I2C_BUS_BUSY, indicating that i2cWrite is failed due to busy bus. 
// 
// If the writing data transfer can be initialized, it will claim the bus to start
// data transmission, mark the transfer as WRITE transfer, and start the I2C 
// transmission timer.
//
// If I2C transmission timeout occurs, this function may return two possible values:
// EC_I2C_ISR_SLAVE_ADDR_NACK, if the the timeout flag is set because slave device
// does not send proper acknowledgement when its address is called, or
// EC_I2C_WRITE_INCOMPLETE_TRANSMISSION, if the timeout flag occurs after timeout 
// period is passed.
//
// If I2C transmission timeout does not occur, l_us_error_code will return 
// EC_SUCCESSFUL, indicating that i2CWrite is successful.
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
unsigned short i2cWrite (void)
{
	unsigned short l_us_error_code = EC_INIT;
	char SFRPAGE_SAVE = SFRPAGE;  

	SFRPAGE = SMB0_PAGE;

	//Check if I2C lines are busy
	l_us_error_code = i2cCheckLine();
	
	//If the I2C bus is freed before I2C busy timeout occurs
	if (!l_us_error_code){
		//Claim I2C bus (set to busy)
		g_us_i2c_flag |= I2C_FLAG_BUSY;

		//Mark this transfer as a WRITE
		g_us_i2c_flag &= ~I2C_FLAG_RW;

		//Start transfer
		STA = 1;
		
		//Start the transfer software's mechanism
		l_us_error_code = i2cTransfer();
	}

	//Return the SFR page before leaving the function
	SFRPAGE = SFRPAGE_SAVE;				
	return l_us_error_code;
}

////------------------------------------------------------------------------------------
//// i2cRead
////------------------------------------------------------------------------------------
//// This function will be reading data to I2C bus line as a master receiver. 
////
////-------------------
//// Return Value 
////-------------------
//// 1) l_us_error_code (unsigned char)
////		a) EC_SUCCESSFUL - successful I2C Read.
////		b) EC_I2C_READ_INCOMPLETE_TRANSMISSION - timeout after data transmission 
////		   starts, before it is finished.
//		c) EC_I2C_BUS_BUSY - timeout before data transmission, while waiting bus 
//		   to be freed.
//		d) EC_I2C_ISR_SLAVE_ADDR_NACK - slave device does not acknowledge.
//
//-------------------
// Parameters
//-------------------
// none
//
//------------------
// Long Description
//------------------
// The function will first check whether the I2C bus line is busy or not before it 
// starts to claim the I2C bus line for data reading. 
//
// If the I2C bus line is found to be free, it will claim the line and start 
// the reading data transfer. 
//
// If the bus is busy, it will wait until the bus is not busy or (to avoid indefinite 
// waiting) until the timeout occurs, determined by the value of 
// I2C_BUSY_TIME_LIMIT * timer4 overflow period. 
// 
// If the I2C bus is still busy after the timeout period, it will return error
// code EC_I2C_BUS_BUSY, indicating that i2cRead is failed due to busy bus. 
// 
// If the reading data transfer can be initialized, it will claim the bus to start
// data transmission, mark the transfer as READ transfer, and starts the reading 
// data transfer. It will then wait until the reading data transfer finished or 
// timeout to occur.
// 
// If I2C transmission timeout occurs, this function may return two possible values:
// EC_I2C_ISR_SLAVE_ADDR_NACK, if the the timeout flag is set because slave device
// does not send proper acknowledgement when its address is called, or
// EC_I2C_READ_INCOMPLETE_TRANSMISSION, if the timeout flag occurs after timeout 
// period is passed.
//
// If I2C transmission timeout does not occur, l_us_error_code will return 
// EC_SUCCESSFUL, indicating that the i2cRead is successful.
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
unsigned short i2cRead (void)
{	
	unsigned short l_us_error_code = EC_INIT;
	char SFRPAGE_SAVE = SFRPAGE;  

	SFRPAGE = SMB0_PAGE;

	//Check if I2C lines are busy
	l_us_error_code = i2cCheckLine();
	
	//If the I2C bus is freed before I2C busy timeout occurs
	if (!l_us_error_code){
		//Claim I2C bus (set to busy)		
		g_us_i2c_flag |= I2C_FLAG_BUSY;

		//Mark this transfer as a READ
		g_us_i2c_flag |= I2C_FLAG_RW;

		//Start transfer	
		STA = 1;				

		//Start the transfer software's mechanism
		l_us_error_code = i2cTransfer();
	}

	//Return the SFR page before leaving the function
	SFRPAGE = SFRPAGE_SAVE; 	
	return l_us_error_code;
}

//------------------------------------------------------------------------------------
// To check and wait i2c line status
//------------------------------------------------------------------------------------
unsigned short i2cCheckLine (void)
{
	unsigned short l_us_error_code = EC_INIT;

	//Indicate that waiting for I2C bus to free is initiated, starts I2C busy timer.
	g_us_i2c_flag |= I2C_FLAG_BUSY_START;							
	
	//Wait for bus to be free or I2C busy timeout to occur.
	while ((g_us_i2c_flag & I2C_FLAG_BUSY) && !(g_us_i2c_flag & I2C_FLAG_BUSY_TIMEOUT));	
	
	//Reset I2C_FLAG_BUSY_START value, stops I2C busy timer
	g_us_i2c_flag &= ~I2C_FLAG_BUSY_START;							
	
	//If I2C busy timeout occurs
	if (g_us_i2c_flag & I2C_FLAG_BUSY_TIMEOUT){
		//Ian (2012-05-15): here, we want to check whether the I2C is used in any other part of the code (busy)
		//					thus, unlike in i2cTransfer, the I2C busy flag is not set free if it is not
		//					found to be busy here.
		//Reset I2C_FLAG_BUSY_TIMEOUT value
		g_us_i2c_flag &= ~I2C_FLAG_BUSY_TIMEOUT;			

		//Set timeout error code before any data transmission
		l_us_error_code = EC_I2C_BUS_BUSY;		
	}
	else l_us_error_code = EC_SUCCESSFUL;

	return l_us_error_code; 	  	
}

//------------------------------------------------------------------------------------
// Common sequence between i2cWrite and i2cRead
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
unsigned short i2cTransfer (void)
{
	unsigned short l_us_error_code = EC_INIT;

	//Set I2C_FLAG_TRANSMISSION_START value, starts I2C transmission timer	
	g_us_i2c_flag |= I2C_FLAG_TRANSMISSION_START;							
	
	//Wait for bus to be free or I2C transmission timeout to occur.
	while ((g_us_i2c_flag & I2C_FLAG_BUSY) && !(g_us_i2c_flag & I2C_FLAG_TRANSMISSION_TIMEOUT));	
	
	//Reset I2C_FLAG_TRANSMISSION_START value, stops I2C transmission timer
	g_us_i2c_flag &= ~I2C_FLAG_TRANSMISSION_START;							
	
	//If I2C transmission timeout occurs
	if (g_us_i2c_flag & I2C_FLAG_TRANSMISSION_TIMEOUT){
		//Reset I2C_FLAG_TRANSMISSION_TIMEOUT value
		g_us_i2c_flag &= ~I2C_FLAG_TRANSMISSION_TIMEOUT;	
					
		//Set I2C bus free
		g_us_i2c_flag &= ~I2C_FLAG_BUSY;

		//If the time out occurs because the flag is raised after slave device
		//does not acknowledge its address
		if (g_us_i2c_isr_error_code == EC_I2C_ISR_SLAVE_ADDR_NACK){ 	
			//Set error code to indicate which slave device does not acknowledge
			if (g_uc_i2c_target == ADCS_ADDR)
				l_us_error_code = EC_I2C_ISR_ADCS_SLAVE_ADDR_NACK;
			else if (g_uc_i2c_target == PWRS_ADDR)
				l_us_error_code = EC_I2C_ISR_PWRS_SLAVE_ADDR_NACK;
			else if (g_uc_i2c_target == IMC_ADDR)
				l_us_error_code = EC_I2C_ISR_IMC_SLAVE_ADDR_NACK;
			else if (g_uc_i2c_target == ITC_ADDR)
				l_us_error_code = EC_I2C_ISR_ITC_SLAVE_ADDR_NACK;
			else if (g_uc_i2c_target == PAYL_ADDR)
				l_us_error_code = EC_I2C_ISR_PAYL_SLAVE_ADDR_NACK;
			else if (g_uc_i2c_target == RTC_ADDR)
				l_us_error_code = EC_I2C_ISR_RTC_SLAVE_ADDR_NACK;
			else if (g_uc_i2c_target == GOM_ADDR)
				l_us_error_code = EC_I2C_ISR_GOM_PWRS_SLAVE_ADDR_NACK;
			else if (g_uc_i2c_target == ANTENNA_1_ADDR)
				l_us_error_code = EC_I2C_ISR_ANTENNA_1_SLAVE_ADDR_NACK;
			else if (g_uc_i2c_target == ANTENNA_2_ADDR)
				l_us_error_code = EC_I2C_ISR_ANTENNA_2_SLAVE_ADDR_NACK;
			else l_us_error_code = g_us_i2c_isr_error_code;
			//Reset I2C ISR error code
			g_us_i2c_isr_error_code = EC_INIT;						
		}
		//Else if there is I2C communication fail for whatsoever reason, returns the error code
		else if (g_us_i2c_isr_error_code == EC_I2C_ISR_COMMUNICATION_FAIL){
			l_us_error_code = g_us_i2c_isr_error_code;

			//Reset I2C ISR error code
			g_us_i2c_isr_error_code = EC_INIT;				
		}
		else{ 													
			//If the transfer is 'write'
			if (!(g_us_i2c_flag & I2C_FLAG_RW))
				//Set timeout error code after any data transmission
				l_us_error_code = EC_I2C_WRITE_INCOMPLETE_TRANSMISSION;	
			else
				//Set timeout error code after any data transmission
				l_us_error_code = EC_I2C_READ_INCOMPLETE_TRANSMISSION;
		}
	}

	//If no timeout occurs, indicate that i2cWrite is successful
	else l_us_error_code = EC_SUCCESSFUL;			

	return l_us_error_code;
}

//------------------------------------------------------------------------------------
// To read from SMBus lines with some delays
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
unsigned short i2cWaitRead (unsigned char l_uc_delay_before_10thms, unsigned char l_uc_delay_after_10thms)
{
	unsigned short l_us_error_code = EC_INIT;

	waitTenthMs(l_uc_delay_before_10thms); //is expected to be sufficient to get the data requested prepared	
	l_us_error_code = i2cRead();
	waitTenthMs(l_uc_delay_after_10thms); 
	
	return l_us_error_code; 	  	
}