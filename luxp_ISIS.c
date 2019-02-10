//------------------------------------------------------------------------------------
// Luxp_ISIS.c
//------------------------------------------------------------------------------------
// Copyright (C) 2013 Nanyang Technological University.
//
// AUTH: IAN
// DATE: 03 APR 2013
//
// The program Luxp_ISIS.c will be downloaded to the Pumpkin Motherboard with 
// C8051F120 uC as part of the VELOX-P Pico-Satellite Flight Software. 
//
// This file contains all functions related to communication with
// ISIS IMC and ITS boards.
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

#include <luxp_ISIS.h>

#if (ISIS_CMD_ENABLE == 1)
//------------------------------------------------------------------------------------
// ISIS to get all housekeeping info
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
#if (DEBUGGING_FEATURE == 1)
unsigned short isisGetHousekeeping (unsigned char l_uc_record_time_par) 
#else
unsigned short isisGetHousekeeping (void) 
#endif
{
	unsigned short l_us_error_code[11] = EC_INIT;
	unsigned short l_us_error_indicator = EC_SUCCESSFUL;
	unsigned char l_uc_i;
	unsigned char l_uc_ec_counter = 0;

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_record_time_par & 0x01) startTiming();
	#endif

	//Get all housekeeping data
	//It is noted that individual error code could be collectively made into one-variable flags' collection
	// if an alternative style of writing code is implemented (20120328)
	l_us_error_code[l_uc_ec_counter++] = isisI2C(IMC_ADDR,IMC_I2C_REC_DOFF,0x00);
	l_us_error_code[l_uc_ec_counter++] = isisI2C(IMC_ADDR,IMC_I2C_REC_SSTR,0x00);
	l_us_error_code[l_uc_ec_counter++] = isisI2C(IMC_ADDR,IMC_I2C_TRN_RPOW,0x00);
	l_us_error_code[l_uc_ec_counter++] = isisI2C(IMC_ADDR,IMC_I2C_TRN_FPOW,0x00);
	l_us_error_code[l_uc_ec_counter++] = isisI2C(IMC_ADDR,IMC_I2C_TRN_ICON,0x00);
	l_us_error_code[l_uc_ec_counter++] = isisI2C(IMC_ADDR,IMC_I2C_REC_ICON,0x00);
	l_us_error_code[l_uc_ec_counter++] = isisI2C(IMC_ADDR,IMC_I2C_POW_AMPT,0x00);
	l_us_error_code[l_uc_ec_counter++] = isisI2C(IMC_ADDR,IMC_I2C_POW_BUSV,0x00);
	l_us_error_code[l_uc_ec_counter++] = isisI2C(IMC_ADDR,IMC_I2C_REC_UPTM,0x00);
	l_us_error_code[l_uc_ec_counter++] = isisI2C(ITC_ADDR,ITC_I2C_TRN_UPTM,0x00);
	l_us_error_code[l_uc_ec_counter++] = isisI2C(ITC_ADDR,ITC_I2C_TRN_STATE,0x00);

	//It is noted that l_us_error_code[11] has different meaning from the rests of l_us_error_code[]
	//For clarification, the variable name is changed to l_us_error_indicator
	//Ian (2012-05-15): g_us_latest_comm_hk_ec = EC_SUCCESSFUL; statement is moved out from the for loop
	g_us_latest_comm_hk_ec = EC_SUCCESSFUL;
	for (l_uc_i = 0; l_uc_i < l_uc_ec_counter; l_uc_i++){
		l_us_error_indicator += l_us_error_code[l_uc_i];
		if (l_us_error_indicator) {
			g_us_latest_comm_hk_ec = EC_GET_COMM_HK_FAIL;
			break;
		}
	}

	#if (DEBUGGING_FEATURE == 1)
	//To end time recording (if assigned)
	if (l_uc_record_time_par & 0x01) endTiming();
	#endif

	//Return the cause of error if there is any error code
	//Ian (2012-05-15): error indicator is returned 
	return l_us_error_indicator;
}

//------------------------------------------------------------------------------------
// ISIS All I2C functions without parameters (except messaging)
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
unsigned short isisI2C (unsigned char l_uc_isis_board_addr, unsigned char l_uc_msg_out, unsigned char l_uc_func_par) 
{
	unsigned short l_us_error_code = EC_INIT;

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_func_par & 0x01) startTiming();
	#endif

	//To initialize and assign transmission parameters	
	g_uc_i2c_msg_out = l_uc_msg_out;
	
	if (l_uc_isis_board_addr == IMC_ADDR){ //Handle message for IMC

		g_uc_i2c_target = IMC_ADDR;
		g_uc_i2c_transferred_byte_out = IMC_I2C_WR;
		g_uc_i2c_data_out[0] = l_uc_msg_out;

		switch (l_uc_msg_out){
			case IMC_I2C_RESET:
				l_uc_func_par |= 0x04;	//write
				break;
	
			case IMC_I2C_GET_NOFR:
				g_uc_i2c_transferred_byte_in = IMC_I2C_GET_NOFR_RD;
				l_uc_func_par |= 0x0E;	//write, read, and store
				break;
	
			case IMC_I2C_GET_FR:
				g_uc_i2c_transferred_byte_in = IMC_I2C_GET_FR_RD;			
				l_uc_func_par |= 0x06;	//write and read
				break;
	
			case IMC_I2C_RMV_FR:
				l_uc_func_par |= 0x04;	//write
				break;
	
			case IMC_I2C_REC_DOFF:
				g_uc_i2c_transferred_byte_in = IMC_I2C_REC_DOFF_RD;
				l_uc_func_par |= 0x0E;	//write, read, and store	
				break;
	
			case IMC_I2C_REC_SSTR:
				g_uc_i2c_transferred_byte_in = IMC_I2C_REC_SSTR_RD;
				l_uc_func_par |= 0x0E;	//write, read, and store		
				break;
	
			case IMC_I2C_TRN_RPOW:
				g_uc_i2c_transferred_byte_in = IMC_I2C_TRN_RPOW_RD;			
				l_uc_func_par |= 0x0E;	//write, read, and store		
				break;
	
			case IMC_I2C_TRN_FPOW:
				g_uc_i2c_transferred_byte_in = IMC_I2C_TRN_FPOW_RD;
				l_uc_func_par |= 0x0E;	//write, read, and store
				break;
	
			case IMC_I2C_TRN_ICON:
				g_uc_i2c_transferred_byte_in = IMC_I2C_TRN_ICON_RD;
				l_uc_func_par |= 0x0E;	//write, read, and store	
				break;
	
			case IMC_I2C_REC_ICON:
				g_uc_i2c_transferred_byte_in = IMC_I2C_REC_ICON_RD;
				l_uc_func_par |= 0x0E;	//write, read, and store		
				break;
	
			case IMC_I2C_POW_AMPT:
				g_uc_i2c_transferred_byte_in = IMC_I2C_POW_AMPT_RD;	
				l_uc_func_par |= 0x0E;	//write, read, and store			
				break;
	
			case IMC_I2C_POW_BUSV:
				g_uc_i2c_transferred_byte_in = IMC_I2C_POW_BUSV_RD;
				l_uc_func_par |= 0x0E;	//write, read, and store			
				break;
	
			case IMC_I2C_POW_BUSV_ICON:
				g_uc_i2c_transferred_byte_in = IMC_I2C_POW_BUSV_ICON_RD;
				l_uc_func_par |= 0x0E;	//write, read, and store			
				break;
	
			case IMC_I2C_REC_UPTM:
				g_uc_i2c_transferred_byte_in = IMC_I2C_REC_UPTM_RD;
				l_uc_func_par |= 0x0E;	//write, read, and store			
				break;
	
			default:
				break;
		}
	}
	
	else{ //Handle message for ITC
		g_uc_i2c_target = ITC_ADDR;
		g_uc_i2c_data_out[0] = g_uc_i2c_msg_out;

		switch (l_uc_msg_out){
			case ITC_I2C_RESET:
				g_uc_i2c_transferred_byte_out = ITC_I2C_WR;
				l_uc_func_par |= 0x84;	//write, special update
				break;

			case ITC_I2C_CLR_BCN:
				g_uc_i2c_transferred_byte_out = ITC_I2C_WR;
				l_uc_func_par |= 0x84;	//write, special update
				break;

			case ITC_I2C_TRN_UPTM:
				g_uc_i2c_transferred_byte_out = ITC_I2C_WR;
				g_uc_i2c_transferred_byte_in = ITC_I2C_TRN_UPTM_RD;
				l_uc_func_par |= 0x0E;	//write, read, store
				break;

			case ITC_I2C_TRN_STATE:
				g_uc_i2c_transferred_byte_out = ITC_I2C_WR;
				g_uc_i2c_transferred_byte_in = ITC_I2C_TRN_STATE_RD;
				l_uc_func_par |= 0x0E;	//write, read, store
				break;

			default:
				break;
		}		
	}

	//If there is data out bit, transmit I2C command
	if (l_uc_func_par & 0x04) 
		l_us_error_code = i2cWrite();

	//If there is data in bit, retrieve I2C data
	if (l_uc_func_par & 0x02){
		if (l_us_error_code == EC_SUCCESSFUL)
			l_us_error_code = i2cWaitRead(TIME_DELAY_ISIS_I2C_CMD_WAITING_TENTH_MS,TIME_DELAY_ISIS_I2C_CMD_DELAY_TENTH_MS);
	}

	//If there is a special update, and no error code, update it
	if ((l_uc_func_par & 0x80) && (!l_us_error_code)){
		str_obdh_hk.uc_ax25_bit_rate_par 	= BITRATE_1200;
		str_obdh_hk.us_cw_char_rate_par	= 0x0006;			
	}

	//If there is store bit, and the reading is successful, store the I2C data.
	if (!l_us_error_code && (l_uc_func_par & 0x08))
		storeISIS();

	#if (DEBUGGING_FEATURE == 1)
	//To end time recording (if assigned)
	if (l_uc_func_par & 0x01) endTiming();
	#endif

	return l_us_error_code;
}

//------------------------------------------------------------------------------------
// ISIS All I2C functions with parameters (except messaging)
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
unsigned short isisI2CPar (unsigned char l_uc_msg_out, unsigned short l_us_msg_par, unsigned char l_uc_func_par) 
{
	unsigned short l_us_error_code = EC_INIT;
	unsigned short l_us_value_assigned;

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_func_par & 0x01) startTiming();
	#endif

	//To initialize and assign transmission parameters	
	g_uc_i2c_msg_out = l_uc_msg_out;		
	g_uc_i2c_target = ITC_ADDR;
	g_uc_i2c_data_out[0] = g_uc_i2c_msg_out;

	switch (l_uc_msg_out){
		case ITC_I2C_SET_TRN_IDLE:
			g_uc_i2c_transferred_byte_out = ITC_I2C_SET_TRN_IDLE_WR;
			l_uc_func_par |= 0x14;	//write, input par (1)
			break;

		case ITC_I2C_SET_TRN_OUTM:
			g_uc_i2c_transferred_byte_out = ITC_I2C_SET_TRN_OUTM_WR;
			l_uc_func_par |= 0x14;	//write, input par (1)
			break;

		case ITC_I2C_SET_AX_TRN_BRT:
			g_uc_i2c_transferred_byte_out = ITC_I2C_SET_AX_TRN_BRT_WR;
			l_uc_func_par |= 0x94;	//write, input par (1), special update
			break;

		case ITC_I2C_SET_CW_CHAR_RT:
			g_uc_i2c_transferred_byte_out = ITC_I2C_SET_CW_CHAR_RT_WR;
			l_uc_func_par |= 0xA4;	//write, input par (2), special update
			break;

		default:
			break;
	}		


	//If there is input par bit, fill in the input parameters
	if (l_uc_func_par & 0x30){
		if (l_uc_func_par & 0x10) 	//1 bit input parameter case
			g_uc_i2c_data_out[1] = l_us_msg_par & 0x00FF;
		else{ 					//2 bits input parameter case
			if (l_uc_msg_out == ITC_I2C_SET_CW_CHAR_RT)
				l_us_value_assigned = 600/l_us_msg_par;
			else
				l_us_value_assigned = l_us_msg_par;
			g_uc_i2c_data_out[1] = l_us_value_assigned;
			g_uc_i2c_data_out[2] = l_us_value_assigned >> 8;
		}
	}

	//If there is data out bit, transmit I2C command
	if (l_uc_func_par & 0x04) 
		l_us_error_code = i2cWrite();

	//If there is a special update, and no error code, update it
	if ((l_uc_func_par & 0x80) && (!l_us_error_code)){
		if (l_uc_msg_out == ITC_I2C_SET_AX_TRN_BRT)
			str_obdh_hk.uc_ax25_bit_rate_par 	= l_us_value_assigned;
		else if (l_uc_msg_out == ITC_I2C_SET_CW_CHAR_RT_WR)
			str_obdh_hk.us_cw_char_rate_par	= (unsigned short)(g_uc_i2c_data_out[2] << 8) + g_uc_i2c_data_out[1];			
	}

	#if (DEBUGGING_FEATURE == 1)
	//To end time recording (if assigned)
	if (l_uc_func_par & 0x01) endTiming();
	#endif

	return l_us_error_code;
}

//------------------------------------------------------------------------------------
// ISIS All I2C functions with messaging
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
//The role of l_uc_func_par is made to combined for squeezing the byte used to determine
//"property" of the function called. Might/might not be found so favourable.
unsigned short isisI2CMsg (unsigned char l_uc_msg_out, unsigned short l_us_msg_par, unsigned short l_us_msg_length, unsigned char l_uc_func_par) 
{
	unsigned short l_us_error_code = EC_INIT;
	unsigned short l_us_value_assigned;
	int l_i_counter;

	#if (DEBUGGING_FEATURE == 1)
	//To initialize time recording (if assigned)
	if(l_uc_func_par & 0x01) startTiming();
	#endif

	//To initialize and assign transmission parameters	
	g_uc_i2c_msg_out = l_uc_msg_out;
	
	//Handle message for ITC
	g_uc_i2c_target = ITC_ADDR;
	g_uc_i2c_data_out[0] = g_uc_i2c_msg_out;

	switch (l_uc_msg_out){
		case ITC_I2C_SND_AX_FR_DFLT:
			g_uc_itc_snd_ax_fr_dflt_wr = l_us_msg_length + 1;
			g_uc_i2c_transferred_byte_out = g_uc_itc_snd_ax_fr_dflt_wr;	
			l_uc_func_par |= 0x44;	//write, message
			break;

		case ITC_I2C_SND_AX_FR_OVRD:
			g_uc_itc_snd_ax_fr_ovrd_wr = l_us_msg_length + AX25_CALLSIGN_SIZE + 1;
			g_uc_i2c_transferred_byte_out = g_uc_itc_snd_ax_fr_ovrd_wr;
			l_uc_func_par |= 0x44;	//write, message
			break;

		case ITC_I2C_SND_CW_MSG:
			g_uc_itc_snd_cw_msg_wr = l_us_msg_length + 1;
			g_uc_i2c_transferred_byte_out = g_uc_itc_snd_cw_msg_wr;
			l_uc_func_par |= 0x44;	//write, message	
			break;

		case ITC_I2C_SET_AX_BCN_DFLT:
			g_uc_itc_set_ax_bcn_dflt_wr = l_us_msg_length + 3;
			g_uc_i2c_transferred_byte_out = g_uc_itc_set_ax_bcn_dflt_wr;
			l_uc_func_par |= 0x64;	//write, input par (2), message	
			break;

		case ITC_I2C_SET_AX_BCN_OVRD:
			g_uc_itc_set_ax_bcn_ovrd_wr = l_us_msg_length + AX25_CALLSIGN_SIZE + 3;
			g_uc_i2c_transferred_byte_out = g_uc_itc_set_ax_bcn_ovrd_wr;
			l_uc_func_par |= 0x64;	//write, input par (2), message		
			break;

		case ITC_I2C_SET_CW_BCN:
			g_uc_itc_set_cw_bcn_wr = l_us_msg_length + 3;
			g_uc_i2c_transferred_byte_out = g_uc_itc_set_cw_bcn_wr;	
			l_uc_func_par |= 0x64;	//write, input par (2), message		
			break;

		case ITC_I2C_SET_DFLT_AX_TO:
			g_uc_i2c_transferred_byte_out = ITC_I2C_SET_DFLT_AX_TO_WR;
			l_uc_func_par |= 0x44;	//write, message	
			break;

		case ITC_I2C_SET_DFLT_AX_FROM:
			g_uc_i2c_transferred_byte_out = ITC_I2C_SET_DFLT_AX_FROM_WR;
			l_uc_func_par |= 0x44;	//write, message	
			break;

		default:
			break;
	}		


	//If there is input par bit, fill in the input parameters
	if (l_uc_func_par & 0x30){
		if (l_uc_func_par & 0x10) 	//1 byte input parameter case
									//This case is possible by direct user input or scheduled command.
			g_uc_i2c_data_out[1] = l_us_msg_par;
		else{ 					//2 bytes input parameter case
			if (l_uc_msg_out == ITC_I2C_SET_CW_CHAR_RT)
				l_us_value_assigned = 600/l_us_msg_par;
			else
				l_us_value_assigned = l_us_msg_par;
			//The byte ordering is reversed (lo byte before hi byte)
			//Due to ISIS hardware setting, which require the byte sent to be reversed for this case
			g_uc_i2c_data_out[1] = l_us_value_assigned;
			g_uc_i2c_data_out[2] = l_us_value_assigned >> 8;
		}
	}

	//If there is msg par bit, fill in the message
	//Here an estimated transmission time for a single transmission is calculated as follow:
	//	1) For CW transmission
	//		g_us_expected_no_of_itn_checking_per_transmission = (14*l_us_msg_length*NO_OF_BEACON_CHECK_PER_S) / (str_obdh_hk.us_cw_char_rate_par*2)
	//	2) For AX.25 transmission
	//		g_us_expected_no_of_itn_checking_per_transmission = (11*NO_OF_BEACON_CHECK_PER_S)/4 + (l_us_msg_length*NO_OF_BEACON_CHECK_PER_S) / (str_obdh_hk.uc_ax25_bit_rate_par*150)
	//Set AX.25 beacon is used for recovery purpose, but send individual CW/AX.25 data is not used for recovery purpose
	//So, only send individual CW/AX.25 data is calculating	g_us_expected_no_of_itn_checking_per_transmission

	//Except for segregation between initialization, value assignment, and actual sending, this software structure is noticed to
	//be unfavorable, for it repeats the same switch case.
	//If the switch case is combined, however, the size of the code (after compiled and optimized) is not so different from
	//this segregation. 
	//It is not confirmed whether to change this style of writing.

	//Constant AX25_CALLSIGN_SIZE is used (20120328) 
	if (l_uc_func_par & 0x40){
		switch (l_uc_msg_out){
			case ITC_I2C_SND_AX_FR_DFLT:
				g_us_expected_no_of_itn_checking_per_transmission = (11*NO_OF_BEACON_CHECK_PER_S)/4 + (l_us_msg_length*NO_OF_BEACON_CHECK_PER_S) / ((unsigned short)(str_obdh_hk.uc_ax25_bit_rate_par)*150);
				for (l_i_counter = 1; l_i_counter <= l_us_msg_length; l_i_counter++) 				
					g_uc_i2c_data_out[l_i_counter] = g_uc_ax25_message[l_i_counter-1]; 
				break;

			case ITC_I2C_SND_AX_FR_OVRD: 
				g_us_expected_no_of_itn_checking_per_transmission = (11*NO_OF_BEACON_CHECK_PER_S)/4 + (l_us_msg_length*NO_OF_BEACON_CHECK_PER_S) / ((unsigned short)(str_obdh_hk.uc_ax25_bit_rate_par)*150);
				for (l_i_counter = 1; l_i_counter <= AX25_CALLSIGN_SIZE; l_i_counter++) 							
					g_uc_i2c_data_out[l_i_counter] = g_uc_from_to_callsign[l_i_counter-1];
				for (l_i_counter = AX25_CALLSIGN_SIZE+1; l_i_counter <= (l_us_msg_length + AX25_CALLSIGN_SIZE); l_i_counter++) 		
					g_uc_i2c_data_out[l_i_counter] = g_uc_ax25_message[l_i_counter-AX25_CALLSIGN_SIZE-1];
				break;

			case ITC_I2C_SND_CW_MSG: 
				g_us_expected_no_of_itn_checking_per_transmission = (14*l_us_msg_length*NO_OF_BEACON_CHECK_PER_S) / (str_obdh_hk.us_cw_char_rate_par*2);
				for (l_i_counter = 1; l_i_counter <= l_us_msg_length; l_i_counter++) 				
					g_uc_i2c_data_out[l_i_counter] = g_uc_cw_message[l_i_counter-1];
				break;

			case ITC_I2C_SET_AX_BCN_DFLT: 
				for (l_i_counter = 3; l_i_counter <= (l_us_msg_length + 2); l_i_counter++) 			
					g_uc_i2c_data_out[l_i_counter] = g_uc_ax25_message[l_i_counter-3];
				break;

			case ITC_I2C_SET_AX_BCN_OVRD:
				for (l_i_counter = 3; l_i_counter <= AX25_CALLSIGN_SIZE + 2; l_i_counter++) 							
					g_uc_i2c_data_out[l_i_counter] = g_uc_from_to_callsign[l_i_counter-3];
				for (l_i_counter = AX25_CALLSIGN_SIZE + 3; l_i_counter <= (l_us_msg_length + AX25_CALLSIGN_SIZE + 2); l_i_counter++) 		
					g_uc_i2c_data_out[l_i_counter] = g_uc_ax25_message[l_i_counter-AX25_CALLSIGN_SIZE-3];
				break;

			case ITC_I2C_SET_CW_BCN:
				for (l_i_counter = 3; l_i_counter <= (l_us_msg_length + 2); l_i_counter++) 			
					g_uc_i2c_data_out[l_i_counter] = g_uc_cw_message[l_i_counter-3];
				break;

			case ITC_I2C_SET_DFLT_AX_TO: 
				for (l_i_counter = 1; l_i_counter <= 7; l_i_counter++) 								
					g_uc_i2c_data_out[l_i_counter] = g_uc_from_to_callsign[l_i_counter+6];
				break;

			case ITC_I2C_SET_DFLT_AX_FROM: 
				for (l_i_counter = 1; l_i_counter <= 7; l_i_counter++) 								
					g_uc_i2c_data_out[l_i_counter] = g_uc_from_to_callsign[l_i_counter-1];
				break;

			default:
				break;
		}
	}

	//If there is data out bit, transmit I2C command
	if (l_uc_func_par & 0x04) 
		l_us_error_code = i2cWrite();

	#if (DEBUGGING_FEATURE == 1)
	//To end time recording (if assigned)
	if (l_uc_func_par & 0x01) endTiming();
	#endif

	return l_us_error_code;
}
#endif
