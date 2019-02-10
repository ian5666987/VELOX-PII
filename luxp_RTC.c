//------------------------------------------------------------------------------------
// Luxp_RTC.c
//------------------------------------------------------------------------------------
// Copyright (C) 2013 Nanyang Technological University.
//
// AUTH: IAN
// DATE: 03 APR 2013
//
// The program Luxp_RTC.c will be downloaded to the Pumpkin Motherboard with 
// C8051F120 uC as part of the VELOX-P Pico-Satellite Flight Software. 
//
// This file contains all functions related to communication with
// M41T81S real-time clock.
//
//------------------------------------------------------------------------------------
// Overall functional test status
//------------------------------------------------------------------------------------
// Completely tested, all fully functional
//
//------------------------------------------------------------------------------------
// Overall documentation status
//------------------------------------------------------------------------------------
// Minimally documented
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// List of functions				| Test status					| Documentation
//------------------------------------------------------------------------------------
// rtcResetClock					| Tested, fully functional		| Minimum
// rtcGetTime						| Tested, fully functional		| Minimum
// rtcUpdateDate					| Tested, fully functional		| Average
// rtcDateInit						| Tested, fully functional		| Minimum
//------------------------------------------------------------------------------------

#include <luxp_RTC.h>

#if (RTC_CMD_ENABLE == 1)
//-----------------------------------------------------------------------------
// RTC Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// To Initialize date on RTC
//-----------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short rtcDateInit (unsigned char l_uc_record_time_par)
#else
unsigned short rtcDateInit (void)
#endif
{
	unsigned short l_us_error_code = EC_INIT;	
	unsigned char l_uc_i;
	
	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = RTC_ADDR;
	g_uc_i2c_msg_out = RTC_I2C_DATE_INIT;
	g_uc_i2c_transferred_byte_out = RTC_I2C_WR;

	g_uc_i2c_data_out[0] = 0x00;						//Word Address Pointer
	g_uc_i2c_data_out[1] = oneHex2twoDec((unsigned char)str_sat_date.us_init_ms/10);	//Init ms
	g_uc_i2c_data_out[2] = oneHex2twoDec(str_sat_date.uc_init_s);				//Init s
	g_uc_i2c_data_out[3] = oneHex2twoDec(str_sat_date.uc_init_min);				//Init min
	g_uc_i2c_data_out[4] = oneHex2twoDec(str_sat_date.uc_init_hour);			//Init hour
	g_uc_i2c_data_out[5] = oneHex2twoDec(str_sat_date.uc_init_day_of_week);		//Init day of week
	g_uc_i2c_data_out[6] = oneHex2twoDec(str_sat_date.uc_init_day_of_month);	//Init day of month
	g_uc_i2c_data_out[7] = oneHex2twoDec(str_sat_date.uc_init_month);			//Init month
	g_uc_i2c_data_out[8] = oneHex2twoDec(str_sat_date.uc_init_year);			//Init year

	for (l_uc_i = 9; l_uc_i < RTC_I2C_WR; l_uc_i++) g_uc_i2c_data_out[l_uc_i] = 0x00;

	//To transmit I2C command
	l_us_error_code = i2cWrite();	

	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;
}

//-----------------------------------------------------------------------------
// To get full set of time data from RTC
//-----------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short rtcGetTime (unsigned char l_uc_record_time_par)
#else
unsigned short rtcGetTime (void)
#endif
{	
	unsigned short l_us_error_code = EC_INIT;

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = RTC_ADDR;
	g_uc_i2c_msg_out = RTC_I2C_GET_TIME;
	g_uc_i2c_transferred_byte_in = RTC_I2C_RD;

	//To read and store I2C data, if there is no error detected
	l_us_error_code = i2cRead();
	if (!l_us_error_code)
		storeRTC();

	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;
}

//-----------------------------------------------------------------------------
// To update full set of date data from RTC
//-----------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short rtcUpdateDate (unsigned char l_uc_update_day_of_week, unsigned char l_uc_update_day_of_month, unsigned char l_uc_update_month, unsigned char l_uc_update_year, unsigned char l_uc_update_hour, unsigned char l_uc_update_min, unsigned char l_uc_update_s, unsigned char l_uc_record_time_par)
#else
unsigned short rtcUpdateDate (unsigned char l_uc_update_day_of_week, unsigned char l_uc_update_day_of_month, unsigned char l_uc_update_month, unsigned char l_uc_update_year, unsigned char l_uc_update_hour, unsigned char l_uc_update_min, unsigned char l_uc_update_s)
#endif
{	
	unsigned short l_us_error_code = EC_INIT;
	unsigned char l_uc_i;
	
	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = RTC_ADDR;
	g_uc_i2c_msg_out = RTC_I2C_UPDATE_DATE;
	g_uc_i2c_transferred_byte_out = RTC_I2C_WR;

	g_uc_i2c_data_out[0] = 0x00;									//Word Address Pointer
	g_uc_i2c_data_out[1] = 0x00;									//Update ms
	g_uc_i2c_data_out[2] = oneHex2twoDec(l_uc_update_s);			//Update s
	g_uc_i2c_data_out[3] = oneHex2twoDec(l_uc_update_min);			//Update min
	g_uc_i2c_data_out[4] = oneHex2twoDec(l_uc_update_hour);			//Update hour
	g_uc_i2c_data_out[5] = oneHex2twoDec(l_uc_update_day_of_week);	//Update day of week
	g_uc_i2c_data_out[6] = oneHex2twoDec(l_uc_update_day_of_month);	//Update day of month
	g_uc_i2c_data_out[7] = oneHex2twoDec(l_uc_update_month);		//Update month
	g_uc_i2c_data_out[8] = oneHex2twoDec(l_uc_update_year);			//Update year

	for (l_uc_i = 9; l_uc_i < RTC_I2C_WR; l_uc_i++) g_uc_i2c_data_out[l_uc_i] = 0x00;

	//To transmit I2C command
	l_us_error_code = i2cWrite();	

	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;
}
#endif
