//------------------------------------------------------------------------------------
// Luxp_PWRS.c
//------------------------------------------------------------------------------------
// Copyright (C) 2013 Nanyang Technological University.
//
// AUTH: IAN
// DATE: 03 APR 2013
//
// The program Luxp_PWRS.c will be downloaded to the Pumpkin Motherboard with 
// C8051F120 uC as part of the VELOX-P Pico-Satellite Flight Software. 
//
// This file contains all functions related to communication with PWRS subsystem
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

//------------------------------------------------------------------------------------
// List of functions				| Test status					| Documentation
//------------------------------------------------------------------------------------
// NA								| NA							| NA
//------------------------------------------------------------------------------------

#include <luxp_PWRS.h>

#if (PWRS_CMD_ENABLE == 1)
//------------------------------------------------------------------------------------
// PWRS I2C function (without input parameter)
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
unsigned short pwrsI2C (unsigned char l_uc_msg_out, unsigned char l_uc_func_par)
{
	unsigned short l_us_error_code = EC_INIT;	

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_func_par & 0x01) startTiming();
	#endif

	//To initialize and assign transmission parameters	
	g_uc_i2c_msg_out 				= l_uc_msg_out;
	g_uc_i2c_target 				= PWRS_ADDR;
	g_uc_i2c_transferred_byte_out 	= 1;
	g_uc_i2c_data_out[0] 			= g_uc_i2c_msg_out;

	switch (g_uc_i2c_msg_out){
		case PWRS_I2C_GET_HK:
			g_uc_i2c_transferred_byte_in = PWRS_I2C_GET_HK_RD;
			l_uc_func_par |= 0x0E;	//write, read, and store
			break;

		case PWRS_I2C_GET_SSCP:
			g_uc_i2c_transferred_byte_in = PWRS_I2C_GET_SSCP_RD;
			l_uc_func_par |= 0x0E;	//write, read, and store
			break;

		default:
			break;
	}

	//If there is data out bit, transmit I2C command
	if (l_uc_func_par & 0x04) 
		l_us_error_code = i2cWrite();

	//If there is data in bit, retrieve I2C data
	if (l_uc_func_par & 0x02){
		if (l_us_error_code == EC_SUCCESSFUL)
			l_us_error_code = i2cWaitRead(TIME_DELAY_PWRS_I2C_CMD_WAITING_TENTH_MS,TIME_DELAY_PWRS_I2C_CMD_DELAY_TENTH_MS);
	}

	//If there is store bit, store the I2C data.
	if (!l_us_error_code && (l_uc_func_par & 0x08)){
		g_us_data_counter = 0;
		storePWRS(g_uc_i2c_data_in);
	}

	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_func_par & 0x01) endTiming();
	#endif

	return l_us_error_code;	
}

//------------------------------------------------------------------------------------
// Get single PWRS data
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short pwrsGetData (unsigned char l_uc_data_id, unsigned char l_uc_record_time_par)
#else
unsigned short pwrsGetData (unsigned char l_uc_data_id)
#endif
{
	unsigned short l_us_error_code = EC_INIT;	
	unsigned short l_us_data_id;

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = PWRS_ADDR;
	g_uc_i2c_msg_out = PWRS_I2C_GET_DATA;
	g_uc_i2c_transferred_byte_out = PWRS_I2C_GET_DATA_WR;
	g_uc_i2c_transferred_byte_in = PWRS_I2C_GET_DATA_RD;

	g_uc_i2c_data_out[0] = g_uc_i2c_msg_out;
	g_uc_i2c_data_out[1] = l_uc_data_id;

	l_us_data_id = (unsigned short)(g_uc_i2c_target << 8) + l_uc_data_id;
	
	//To transmit I2C command
	l_us_error_code = i2cWrite();
	
	//To receive I2C data
	if (l_us_error_code == EC_SUCCESSFUL)
		l_us_error_code = i2cWaitRead(TIME_DELAY_PWRS_I2C_CMD_WAITING_TENTH_MS,TIME_DELAY_PWRS_I2C_CMD_DELAY_TENTH_MS);

	//For special data, OBC would like to save
	if (!l_us_error_code) 
		switch (l_us_data_id){
			case DATA_PWRS_UC_CHANNEL_STATUS:
			case DATA_PWRS_US_PV_FV_1:
			case DATA_PWRS_US_PV_FV_2:
			case DATA_PWRS_US_PV_FV_3:
				g_us_data_counter = 0;
				storePWRS(g_uc_i2c_data_in);
				break;

			default:
				break;
		}

	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;	
}

//------------------------------------------------------------------------------------
// To update battery heater parameter
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short pwrsUpdateBatteryHeater (unsigned char l_uc_pwrs_battery_heater, unsigned char l_uc_record_time_par)
#else
unsigned short pwrsUpdateBatteryHeater (unsigned char l_uc_pwrs_battery_heater)
#endif
{
	unsigned short l_us_error_code = EC_INIT;	

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = PWRS_ADDR;
	g_uc_i2c_msg_out = PWRS_I2C_SET_HT;
	g_uc_i2c_transferred_byte_out = PWRS_I2C_SET_HT_WR;

	g_uc_i2c_data_out[0] = g_uc_i2c_msg_out;
	g_uc_i2c_data_out[1] = l_uc_pwrs_battery_heater;
	
	//To transmit I2C command
	l_us_error_code = i2cWrite();
	
	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;	
}

//------------------------------------------------------------------------------------
// To set multiple outputs to ON/OFF
// PWRS channel enable byte is the new PWRS reference
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short pwrsSetOutput (unsigned char l_uc_pwrs_channel_enable, unsigned char l_uc_record_time_par)
#else
unsigned short pwrsSetOutput (unsigned char l_uc_pwrs_channel_enable)
#endif
{
	unsigned short l_us_error_code = EC_INIT;	

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = PWRS_ADDR;
	g_uc_i2c_msg_out = PWRS_I2C_SET_OUTPUT;
	g_uc_i2c_transferred_byte_out = PWRS_I2C_SET_OUTPUT_WR;

	g_uc_i2c_data_out[0] = g_uc_i2c_msg_out;
	g_uc_i2c_data_out[1] = l_uc_pwrs_channel_enable;
	
	//To transmit I2C command
	l_us_error_code = i2cWrite();
	
	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;	
}

//------------------------------------------------------------------------------------
// To set single channel ON/OFF with a delay time
//------------------------------------------------------------------------------------
// Bit0: ADS
// Bit1: ACS
// Bit2: PAYL
// Bit3: Mechanical Deployment (thermal knife)
// Bit4: COMM
// Bit5: PAYL 3V3 channel
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short pwrsSetSingleOutput (unsigned char l_uc_pwrs_var_channel, unsigned char l_uc_value, unsigned short l_us_delay, unsigned char l_uc_record_time_par)
#else
unsigned short pwrsSetSingleOutput (unsigned char l_uc_pwrs_var_channel, unsigned char l_uc_value, unsigned short l_us_delay)
#endif
{
	unsigned short l_us_error_code = EC_INIT;	

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = PWRS_ADDR;
	g_uc_i2c_msg_out = PWRS_I2C_SET_SINGLE_OUTPUT;
	g_uc_i2c_transferred_byte_out = PWRS_I2C_SET_SINGLE_OUTPUT_WR;

	g_uc_i2c_data_out[0] = g_uc_i2c_msg_out;
	g_uc_i2c_data_out[1] = l_uc_pwrs_var_channel;
	g_uc_i2c_data_out[2] = l_uc_value;
	g_uc_i2c_data_out[3] = l_us_delay >> 8;
	g_uc_i2c_data_out[4] = l_us_delay;
	
	//To transmit I2C command
	l_us_error_code = i2cWrite();
	
	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;	
}

//------------------------------------------------------------------------------------
// To set PWRS MPPT mode
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short pwrsSetMPPTMode (unsigned char l_uc_pwrs_mppt_mode, unsigned char l_uc_record_time_par)
#else
unsigned short pwrsSetMPPTMode (unsigned char l_uc_pwrs_mppt_mode)
#endif
{
	unsigned short l_us_error_code = EC_INIT;	

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = PWRS_ADDR;
	g_uc_i2c_msg_out = PWRS_I2C_SET_MPPT_MODE;
	g_uc_i2c_transferred_byte_out = PWRS_I2C_SET_MPPT_MODE_WR;

	g_uc_i2c_data_out[0] = g_uc_i2c_msg_out;
	g_uc_i2c_data_out[1] = l_uc_pwrs_mppt_mode;
	
	//To transmit I2C command
	l_us_error_code = i2cWrite();
	
	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;	
}

//------------------------------------------------------------------------------------
// To set PWRS PV Volt
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short pwrsSetPVVolt (unsigned char l_uc_record_time_par)
#else
unsigned short pwrsSetPVVolt (void)
#endif
{
	unsigned short l_us_error_code = EC_INIT;	

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = PWRS_ADDR;
	g_uc_i2c_msg_out = PWRS_I2C_SET_PV_VOLT;
	g_uc_i2c_transferred_byte_out = PWRS_I2C_SET_PV_VOLT_WR;

	g_uc_i2c_data_out[0] = g_uc_i2c_msg_out;
	g_uc_i2c_data_out[1] = str_pwrs_data.us_pv_fv[0] >> 8;
	g_uc_i2c_data_out[2] = str_pwrs_data.us_pv_fv[0];
	g_uc_i2c_data_out[3] = str_pwrs_data.us_pv_fv[1] >> 8;
	g_uc_i2c_data_out[4] = str_pwrs_data.us_pv_fv[1];
	g_uc_i2c_data_out[5] = str_pwrs_data.us_pv_fv[2] >> 8;
	g_uc_i2c_data_out[6] = str_pwrs_data.us_pv_fv[2];
	
	//To transmit I2C command
	l_us_error_code = i2cWrite();
	
	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;	
}

//------------------------------------------------------------------------------------
// To set PWRS thermal knife output
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short pwrsSetTKOutput (unsigned char l_uc_pwrs_var_channel, unsigned char l_uc_value, unsigned short l_us_delay, unsigned char l_uc_record_time_par)
#else
unsigned short pwrsSetTKOutput (unsigned char l_uc_pwrs_var_channel, unsigned char l_uc_value, unsigned short l_us_delay)
#endif
{
	unsigned short l_us_error_code = EC_INIT;	

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = PWRS_ADDR;
	g_uc_i2c_msg_out = PWRS_I2C_SET_TK_OUTPUT;
	g_uc_i2c_transferred_byte_out = PWRS_I2C_SET_TK_OUTPUT_WR;

	g_uc_i2c_data_out[0] = g_uc_i2c_msg_out;
	g_uc_i2c_data_out[1] = l_uc_pwrs_var_channel;
	g_uc_i2c_data_out[2] = l_uc_value;
	g_uc_i2c_data_out[3] = l_us_delay >> 8;
	g_uc_i2c_data_out[4] = l_us_delay;
	
	//To transmit I2C command
	l_us_error_code = i2cWrite();
	
	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;	
}

//------------------------------------------------------------------------------------
// To reset PWRS subsystem
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short pwrsResetPWRS (unsigned short l_us_delay, unsigned char l_uc_record_time_par)
#else
unsigned short pwrsResetPWRS (unsigned short l_us_delay)
#endif
{
	unsigned short l_us_error_code = EC_INIT;	

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = PWRS_ADDR;
	g_uc_i2c_msg_out = PWRS_I2C_RESET_PWRS;
	g_uc_i2c_transferred_byte_out = PWRS_I2C_RESET_PWRS_WR;

	g_uc_i2c_data_out[0] = g_uc_i2c_msg_out;
	g_uc_i2c_data_out[1] = l_us_delay >> 8;
	g_uc_i2c_data_out[2] = l_us_delay;
	
	//To transmit I2C command
	l_us_error_code = i2cWrite();
	
	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;	
}

//------------------------------------------------------------------------------------
// To reset OBDH subsystem
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short pwrsResetOBDH (unsigned short l_us_delay, unsigned char l_uc_record_time_par)
#else
unsigned short pwrsResetOBDH (unsigned short l_us_delay)
#endif
{
	unsigned short l_us_error_code = EC_INIT;	

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = PWRS_ADDR;
	g_uc_i2c_msg_out = PWRS_I2C_RESET_OBDH;
	g_uc_i2c_transferred_byte_out = PWRS_I2C_RESET_OBDH_WR;

	g_uc_i2c_data_out[0] = g_uc_i2c_msg_out;
	g_uc_i2c_data_out[1] = l_us_delay >> 8;
	g_uc_i2c_data_out[2] = l_us_delay;
	
	//To transmit I2C command
	l_us_error_code = i2cWrite();
	
	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;	
}

//------------------------------------------------------------------------------------
// To set SOC limit
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short pwrsSetSOCLimit (unsigned char l_uc_pwrs_soc_on_limit, unsigned char l_uc_pwrs_soc_off_limit, unsigned char l_uc_record_time_par)
#else
unsigned short pwrsSetSOCLimit (unsigned char l_uc_pwrs_soc_on_limit, unsigned char l_uc_pwrs_soc_off_limit)
#endif
{
	unsigned short l_us_error_code = EC_INIT;	

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = PWRS_ADDR;
	g_uc_i2c_msg_out = PWRS_I2C_SET_SOC_LIMIT;
	g_uc_i2c_transferred_byte_out = PWRS_I2C_SET_SOC_LIMIT_WR;

	g_uc_i2c_data_out[0] = g_uc_i2c_msg_out;
	g_uc_i2c_data_out[1] = l_uc_pwrs_soc_on_limit;
	g_uc_i2c_data_out[2] = l_uc_pwrs_soc_off_limit;
	
	//To transmit I2C command
	l_us_error_code = i2cWrite();
	
	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;	
}

//------------------------------------------------------------------------------------
// To set WDT limit
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short pwrsSetWDTLimit (unsigned long l_ul_pwrs_wdt_limit, unsigned char l_uc_record_time_par)
#else
unsigned short pwrsSetWDTLimit (unsigned long l_ul_pwrs_wdt_limit)
#endif
{
	unsigned short l_us_error_code = EC_INIT;	
	unsigned short l_us_i2c_size = 0;

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = PWRS_ADDR;
	g_uc_i2c_msg_out = PWRS_I2C_SET_WDT_LIMIT;
	g_uc_i2c_transferred_byte_out = PWRS_I2C_SET_WDT_LIMIT_WR;

	g_uc_i2c_data_out[l_us_i2c_size++] = g_uc_i2c_msg_out;
	l_us_i2c_size = insertLongToArray(l_ul_pwrs_wdt_limit, &g_uc_i2c_data_out, l_us_i2c_size);	

	//To transmit I2C command
	l_us_error_code = i2cWrite();
	
	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;	
}

//------------------------------------------------------------------------------------
// To reset EEPROM to its initial state
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short pwrsInitEeprom (unsigned char l_uc_record_time_par)
#else
unsigned short pwrsInitEeprom (void)
#endif
{
	unsigned short l_us_error_code = EC_INIT;	

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = PWRS_ADDR;
	g_uc_i2c_msg_out = PWRS_I2C_EEPROM_INIT;
	g_uc_i2c_transferred_byte_out = PWRS_I2C_EEPROM_INIT_WR;

	g_uc_i2c_data_out[0] = g_uc_i2c_msg_out;

	//To transmit I2C command
	l_us_error_code = i2cWrite();
	
	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;	
}

//------------------------------------------------------------------------------------
// To set PWRS SDC
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short pwrsSetSDC (unsigned char l_uc_pwrs_sdc, unsigned char l_uc_record_time_par)
#else
unsigned short pwrsSetSDC (unsigned char l_uc_pwrs_sdc)
#endif
{
	unsigned short l_us_error_code = EC_INIT;	

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = PWRS_ADDR;
	g_uc_i2c_msg_out = PWRS_I2C_SET_PWRS_SDC;
	g_uc_i2c_transferred_byte_out = PWRS_I2C_SET_PWRS_SDC_WR;

	g_uc_i2c_data_out[0] = g_uc_i2c_msg_out;
	g_uc_i2c_data_out[1] = l_uc_pwrs_sdc;
	
	//To transmit I2C command
	l_us_error_code = i2cWrite();
	
	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;	
}
#endif
