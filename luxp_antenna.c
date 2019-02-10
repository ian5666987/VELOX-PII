//------------------------------------------------------------------------------------
// Luxp_antenna.c
//------------------------------------------------------------------------------------
// Copyright (C) 2013 Nanyang Technological University.
//
// AUTH: IAN
// DATE: 03 APR 2013
//
// The program Luxp_antenna.c will be downloaded to the Pumpkin Motherboard with 
// C8051F120 uC as part of the VELOX-P Pico-Satellite Flight Software. 
//
// This file contains all functions related to communication with
// antenna deployment board.
//
//------------------------------------------------------------------------------------
// Overall functional test status
//------------------------------------------------------------------------------------
// Completely tested, some fully functional
//
//------------------------------------------------------------------------------------
// Overall documentation status
//------------------------------------------------------------------------------------
// Minimally documented
//------------------------------------------------------------------------------------

#include <luxp_antenna.h>

#if (ANTENNA_CMD_ENABLE == 1)
//------------------------------------------------------------------------------------
// All antenna I2C functions without parameters
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
unsigned short antennaI2C (unsigned char l_uc_antenna_board_addr, unsigned char l_uc_msg_out, unsigned char l_uc_func_par)
{
	unsigned short l_us_error_code = EC_INIT;	

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_func_par & 0x01) startTiming();
	#endif

	//To initialize and assign transmission parameters	
	g_uc_i2c_msg_out = l_uc_msg_out;
	g_uc_i2c_target = l_uc_antenna_board_addr;	
	g_uc_i2c_transferred_byte_out = ANTENNA_I2C_WR;
	g_uc_i2c_data_out[0] = l_uc_msg_out;	

	//Message out switch case
	switch (l_uc_msg_out){
		case ANTENNA_I2C_RESET:
		case ANTENNA_I2C_ARM:
		case ANTENNA_I2C_DISARM:
		case ANTENNA_I2C_CANCEL_DPLY:
			l_uc_func_par |= 0x04;	//write
			break;

		case ANTENNA_I2C_GET_DPLY_1_COUNT:
		case ANTENNA_I2C_GET_DPLY_2_COUNT:
		case ANTENNA_I2C_GET_DPLY_3_COUNT:
		case ANTENNA_I2C_GET_DPLY_4_COUNT:
		case ANTENNA_I2C_GET_DPLY_1_TIME:
		case ANTENNA_I2C_GET_DPLY_2_TIME:
		case ANTENNA_I2C_GET_DPLY_3_TIME:
		case ANTENNA_I2C_GET_DPLY_4_TIME:
			g_uc_i2c_transferred_byte_in = ANTENNA_I2C_ONE_BYTE_RD;			
			l_uc_func_par |= 0x0E;	//write, read, and store	
			break;

		case ANTENNA_I2C_GET_TEMP:
		case ANTENNA_I2C_GET_DPLY_STAT:
			g_uc_i2c_transferred_byte_in = ANTENNA_I2C_TWO_BYTES_RD;			
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
			l_us_error_code = i2cWaitRead(TIME_DELAY_ANTENNA_I2C_CMD_WAITING_TENTH_MS,TIME_DELAY_ANTENNA_I2C_CMD_DELAY_TENTH_MS);
	}

	//If there is store bit, and the reading is successful, store the I2C data.
	if (!l_us_error_code && (l_uc_func_par & 0x08))
		storeAntenna();

	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_func_par & 0x01) endTiming();
	#endif

	return l_us_error_code;
}

//------------------------------------------------------------------------------------
// All antenna I2C functions with parameters (except messaging)
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short antennaI2CPar (unsigned char l_uc_antenna_board_addr, unsigned char l_uc_msg_out, unsigned char l_uc_msg_par, unsigned char l_uc_record_time_par)
#else
unsigned short antennaI2CPar (unsigned char l_uc_antenna_board_addr, unsigned char l_uc_msg_out, unsigned char l_uc_msg_par)
#endif
{
	unsigned short l_us_error_code = EC_INIT;

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if (l_uc_record_time_par & 0x01) startTiming();
	#endif

	//To initialize and assign transmission parameters	
	g_uc_i2c_msg_out = l_uc_msg_out;
	g_uc_i2c_target = l_uc_antenna_board_addr;	
	g_uc_i2c_transferred_byte_out = ANTENNA_I2C_PAR_WR;
	g_uc_i2c_data_out[0] = l_uc_msg_out;	
	g_uc_i2c_data_out[1] = l_uc_msg_par;	

	//Transmit command	
	l_us_error_code = i2cWrite();

	#if (DEBUGGING_FEATURE == 1)
	//To end time recording
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	return l_us_error_code;
}
#endif
