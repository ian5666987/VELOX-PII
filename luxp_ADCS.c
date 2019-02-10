//------------------------------------------------------------------------------------
// Luxp_ADCS.c
//------------------------------------------------------------------------------------
// Copyright (C) 2013 Nanyang Technological University.
//
// AUTH: IAN
// DATE: 03 APR 2013
//
// The program Luxp_ADCS.c will be downloaded to the Pumpkin Motherboard with 
// C8051F120 uC as part of the VELOX-P Pico-Satellite Flight Software. 
//
// This file contains all functions related to communication with ADCS subsystem
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
//
//------------------------------------------------------------------------------------

#include <luxp_ADCS.h>

//------------------------------------------------------------------------------------
// ADCS Messaging Functions
//------------------------------------------------------------------------------------

#if (ADCS_CMD_ENABLE == 1)
//------------------------------------------------------------------------------------
// ADCS I2C function (without input parameter)
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
unsigned short adcsI2C(unsigned char l_uc_msg_out, unsigned char l_uc_func_par)
{
	unsigned short l_us_error_code = EC_INIT;

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_func_par & 0x01) startTiming();
	#endif

	//To initialize and assign transmission parameters	
	g_uc_i2c_msg_out 				= l_uc_msg_out;
	g_uc_i2c_target 				= ADCS_ADDR;
	g_uc_i2c_transferred_byte_out 	= 1;
	g_uc_i2c_data_out[0] 			= g_uc_i2c_msg_out;

	switch (g_uc_i2c_msg_out){
		case ADCS_I2C_GET_HK:
			g_uc_i2c_transferred_byte_in = ADCS_I2C_GET_HK_RD;
			l_uc_func_par |= 0x0E;	//write, read, and store
			break;

		case ADCS_I2C_GET_SSCP:
			g_uc_i2c_transferred_byte_in = ADCS_I2C_GET_SSCP_RD;
			l_uc_func_par |= 0x0E;	//write, read, and store
			break;

		case ADCS_I2C_SET_STANDBY:
			g_uc_i2c_transferred_byte_in = ADCS_I2C_SET_STANDBY_RD;
			l_uc_func_par |= 0x06;	//write and read
			break;

		case ADCS_I2C_SET_SELFTEST:
			g_uc_i2c_transferred_byte_in = ADCS_I2C_SET_SELFTEST_RD;			
			l_uc_func_par |= 0x06;	//write and read
			break;

		case ADCS_I2C_SET_GPS_ON:
		case ADCS_I2C_SET_GPS_OFF:
		case ADCS_I2C_RESET_SW:
		case ADCS_I2C_RESET_HW:
			l_uc_func_par |= 0x04;	//write
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
			l_us_error_code = i2cWaitRead(TIME_DELAY_ADCS_I2C_CMD_WAITING_TENTH_MS,TIME_DELAY_ADCS_I2C_CMD_DELAY_TENTH_MS);
	}

	//If there is store bit, store the I2C data.
	if (!l_us_error_code && (l_uc_func_par & 0x08)){
		g_us_data_counter = 0;
		storeADCS(g_uc_i2c_data_in);
	}

	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_func_par & 0x01) endTiming();
	#endif

	return l_us_error_code;
}

//------------------------------------------------------------------------------------
// Get single ADCS data
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short adcsGetData (unsigned char l_uc_data_id, unsigned char l_uc_record_time_par)
#else
unsigned short adcsGetData (unsigned char l_uc_data_id)
#endif
{
	unsigned short l_us_error_code = EC_INIT;

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = ADCS_ADDR;
	g_uc_i2c_msg_out = ADCS_I2C_GET_DATA;
	g_uc_i2c_transferred_byte_out = ADCS_I2C_GET_DATA_WR;
	g_uc_i2c_transferred_byte_in = ADCS_I2C_GET_DATA_RD;

	g_uc_i2c_data_out[0] = g_uc_i2c_msg_out;
	g_uc_i2c_data_out[1] = l_uc_data_id;
	
	//To transmit I2C command
	l_us_error_code = i2cWrite();
	
	//To receive I2C data
	if (l_us_error_code == EC_SUCCESSFUL)
		l_us_error_code = i2cWaitRead(TIME_DELAY_ADCS_I2C_CMD_WAITING_TENTH_MS,TIME_DELAY_ADCS_I2C_CMD_DELAY_TENTH_MS);

	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;
}

//------------------------------------------------------------------------------------
// Set ADCS Mode
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short adcsSetMode (unsigned char l_uc_acs_mode, unsigned short l_us_lapse_time_10s, unsigned char l_uc_record_time_par)
#else
unsigned short adcsSetMode (unsigned char l_uc_acs_mode, unsigned short l_us_lapse_time_10s)
#endif
{
	unsigned short l_us_error_code = EC_INIT;

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = ADCS_ADDR;
	g_uc_i2c_msg_out = ADCS_I2C_SET_MODE;
	g_uc_i2c_transferred_byte_out = ADCS_I2C_SET_MODE_WR;

	g_uc_i2c_data_out[0] = g_uc_i2c_msg_out;
	g_uc_i2c_data_out[1] = l_uc_acs_mode;
	g_uc_i2c_data_out[2] = l_us_lapse_time_10s >> 8;
	g_uc_i2c_data_out[3] = l_us_lapse_time_10s;
	
	//To transmit I2C command
	l_us_error_code = i2cWrite();
	
	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;
}

//------------------------------------------------------------------------------------
// Set ADCS sun sensor threshold
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short adcsSetSSThreshold (short l_s_adcs_ss_threshold, unsigned char l_uc_record_time_par)
#else
unsigned short adcsSetSSThreshold (short l_s_adcs_ss_threshold)
#endif
{
	unsigned short l_us_error_code = EC_INIT;

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = ADCS_ADDR;
	g_uc_i2c_msg_out = ADCS_I2C_SET_SS_THRESHOLD;
	g_uc_i2c_transferred_byte_out = ADCS_I2C_SET_SS_THRESHOLD_WR;

	g_uc_i2c_data_out[0] = g_uc_i2c_msg_out;
	g_uc_i2c_data_out[1] = l_s_adcs_ss_threshold >> 8;
	g_uc_i2c_data_out[2] = l_s_adcs_ss_threshold;
	
	//To transmit I2C command
	l_us_error_code = i2cWrite();
	
	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;
}

//------------------------------------------------------------------------------------
// Set ADCS CGain
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short adcsSetCGain (unsigned char l_uc_record_time_par)
#else
unsigned short adcsSetCGain (void)
#endif
{
	unsigned short l_us_error_code = EC_INIT;
	unsigned short l_us_i2c_size = 0;
	unsigned char l_uc_i;

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = ADCS_ADDR;
	g_uc_i2c_msg_out = ADCS_I2C_SET_CGAIN;
	g_uc_i2c_transferred_byte_out = ADCS_I2C_SET_CGAIN_WR;

	g_uc_i2c_data_out[l_us_i2c_size++] = g_uc_i2c_msg_out;
	for (l_uc_i = 0; l_uc_i < NO_OF_ADCS_CGAIN; l_uc_i++)
		l_us_i2c_size = insertFloatToArray(str_adcs_par.f_cgain[l_uc_i], &g_uc_i2c_data_out, l_us_i2c_size);
			
	//To transmit I2C command
	l_us_error_code = i2cWrite();
	
	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;
}

//------------------------------------------------------------------------------------
// Set ADCS TLE
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short adcsSetTLE (unsigned char l_uc_record_time_par)
#else
unsigned short adcsSetTLE (void)
#endif
{
	unsigned short l_us_error_code = EC_INIT;
	unsigned short l_us_i2c_size = 0;

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = ADCS_ADDR;
	g_uc_i2c_msg_out = ADCS_I2C_SET_TLE;
	g_uc_i2c_transferred_byte_out = ADCS_I2C_SET_TLE_WR;

	g_uc_i2c_data_out[l_us_i2c_size++] = g_uc_i2c_msg_out;
	l_us_i2c_size = insertFloatToArray(str_adcs_par.f_tle_epoch, &g_uc_i2c_data_out, l_us_i2c_size);
	l_us_i2c_size = insertFloatToArray(str_adcs_par.f_tle_inc, &g_uc_i2c_data_out, l_us_i2c_size);
	l_us_i2c_size = insertFloatToArray(str_adcs_par.f_tle_raan, &g_uc_i2c_data_out, l_us_i2c_size);
	l_us_i2c_size = insertFloatToArray(str_adcs_par.f_tle_arg_per, &g_uc_i2c_data_out, l_us_i2c_size);
	l_us_i2c_size = insertFloatToArray(str_adcs_par.f_tle_ecc, &g_uc_i2c_data_out, l_us_i2c_size);
	l_us_i2c_size = insertFloatToArray(str_adcs_par.f_tle_ma, &g_uc_i2c_data_out, l_us_i2c_size);
	l_us_i2c_size = insertShortToArray(str_adcs_par.us_tle_mm, &g_uc_i2c_data_out, l_us_i2c_size);
			
	//To transmit I2C command
	l_us_error_code = i2cWrite();
	
	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;
}

//------------------------------------------------------------------------------------
// Set ADCS Time
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short adcsSetTime (unsigned long l_ul_time_from_tai, unsigned char l_uc_record_time_par)
#else
unsigned short adcsSetTime (unsigned long l_ul_time_from_tai)
#endif
{
	unsigned short l_us_error_code = EC_INIT;

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = ADCS_ADDR;
	g_uc_i2c_msg_out = ADCS_I2C_SET_TIME;
	g_uc_i2c_transferred_byte_out = ADCS_I2C_SET_TIME_WR;

	g_uc_i2c_data_out[0] = g_uc_i2c_msg_out;
	g_uc_i2c_data_out[1] = l_ul_time_from_tai >> 24;
	g_uc_i2c_data_out[2] = l_ul_time_from_tai >> 16;
	g_uc_i2c_data_out[3] = l_ul_time_from_tai >> 8;
	g_uc_i2c_data_out[4] = l_ul_time_from_tai;
			
	//To transmit I2C command
	l_us_error_code = i2cWrite();
	
	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;
}

//------------------------------------------------------------------------------------
// Set ADCS vector align to sun
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short adcsSetVectorAlignToSun (unsigned char l_uc_record_time_par)
#else
unsigned short adcsSetVectorAlignToSun (void)
#endif
{
	unsigned short l_us_error_code = EC_INIT;

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = ADCS_ADDR;
	g_uc_i2c_msg_out = ADCS_I2C_SET_VECTOR_ALIGN2SUN;
	g_uc_i2c_transferred_byte_out = ADCS_I2C_SET_VECTOR_ALIGN2SUN_WR;

	g_uc_i2c_data_out[0] = g_uc_i2c_msg_out;
	g_uc_i2c_data_out[1] = str_adcs_par.s_vector_2sun[0] >> 8;
	g_uc_i2c_data_out[2] = str_adcs_par.s_vector_2sun[0];
	g_uc_i2c_data_out[3] = str_adcs_par.s_vector_2sun[1] >> 8;
	g_uc_i2c_data_out[4] = str_adcs_par.s_vector_2sun[1];
	g_uc_i2c_data_out[5] = str_adcs_par.s_vector_2sun[2] >> 8;
	g_uc_i2c_data_out[6] = str_adcs_par.s_vector_2sun[2];
			
	//To transmit I2C command
	l_us_error_code = i2cWrite();
	
	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;
}

//------------------------------------------------------------------------------------
// Set ADCS to toggle IMU
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short adcsToggleIMU (char l_uc_imu, unsigned char l_uc_record_time_par)
#else
unsigned short adcsToggleIMU (char l_uc_imu)
#endif
{
	unsigned short l_us_error_code = EC_INIT;

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = ADCS_ADDR;
	g_uc_i2c_msg_out = ADCS_I2C_SET_TOGGLE_IMU;
	g_uc_i2c_transferred_byte_out = ADCS_I2C_SET_TOGGLE_IMU_WR;

	g_uc_i2c_data_out[0] = g_uc_i2c_msg_out;
	g_uc_i2c_data_out[1] = l_uc_imu;
			
	//To transmit I2C command
	l_us_error_code = i2cWrite();
	
	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;
}

//------------------------------------------------------------------------------------
// Set ADCS body control rate
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short adcsSetBodyRate (char l_uc_adcs_ctrl_br_x, char l_uc_adcs_ctrl_br_y, char l_uc_adcs_ctrl_br_z, unsigned char l_uc_record_time_par)
#else
unsigned short adcsSetBodyRate (char l_uc_adcs_ctrl_br_x, char l_uc_adcs_ctrl_br_y, char l_uc_adcs_ctrl_br_z)
#endif
{
	unsigned short l_us_error_code = EC_INIT;

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = ADCS_ADDR;
	g_uc_i2c_msg_out = ADCS_I2C_SET_BODY_RATE;
	g_uc_i2c_transferred_byte_out = ADCS_I2C_SET_BODY_RATE_WR;

	g_uc_i2c_data_out[0] = g_uc_i2c_msg_out;
	g_uc_i2c_data_out[1] = l_uc_adcs_ctrl_br_x;
	g_uc_i2c_data_out[2] = l_uc_adcs_ctrl_br_y;
	g_uc_i2c_data_out[3] = l_uc_adcs_ctrl_br_z;
			
	//To transmit I2C command
	l_us_error_code = i2cWrite();
	
	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;
}

//------------------------------------------------------------------------------------
// Set ADCS body control rate
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short adcsSetDGain (short l_s_adcs_dgain, unsigned char l_uc_record_time_par)
#else
unsigned short adcsSetDGain (short l_s_adcs_dgain)
#endif
{
	unsigned short l_us_error_code = EC_INIT;

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize transmission parameters	
	g_uc_i2c_target = ADCS_ADDR;
	g_uc_i2c_msg_out = ADCS_I2C_SET_DGAIN;
	g_uc_i2c_transferred_byte_out = ADCS_I2C_SET_DGAIN_WR;

	g_uc_i2c_data_out[0] = g_uc_i2c_msg_out;
	g_uc_i2c_data_out[1] = (l_s_adcs_dgain >> 8);
	g_uc_i2c_data_out[2] = l_s_adcs_dgain;
			
	//To transmit I2C command
	l_us_error_code = i2cWrite();
	
	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;
}
#endif
