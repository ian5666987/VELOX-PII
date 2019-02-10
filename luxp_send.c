//------------------------------------------------------------------------------------
// Luxp_send.c
//------------------------------------------------------------------------------------
// Copyright (C) 2013 Nanyang Technological University.
//
// AUTH: IAN
// DATE: 03 APR 2013
//
// The program Luxp_send.c will be downloaded to the Pumpkin Motherboard with 
// C8051F120 uC as part of the VELOX-P Pico-Satellite Flight Software. 
//
// This file contains all functions related to sending data to the debugger and
// to the ground station
//
//------------------------------------------------------------------------------------

#include <luxp_send.h>

//------------------------------------------------------------------------------------
// Send Functions
//------------------------------------------------------------------------------------

#if (DEBUGGING_FEATURE == 1)
#if (UART_LINE == 1)
//-----------------------------------------------------------------------------
// To send a set of data in an array through UART1 interface
//-----------------------------------------------------------------------------
void sendToUART1 (unsigned char *l_uc_msg_p, unsigned short l_us_starting_index, unsigned short l_us_no_of_bytes)
{
	unsigned short l_us_counter;
	unsigned short l_us_last_index;
	char SFRPAGE_SAVE = SFRPAGE;
	
	l_us_last_index = l_us_starting_index + l_us_no_of_bytes - 1;
	 
	SFRPAGE = UART1_PAGE;

	for (l_us_counter = l_us_starting_index; l_us_counter <= l_us_last_index; l_us_counter++){
		SBUF1 = *(l_uc_msg_p + l_us_counter);
		waitUs(TIME_DELAY_UART_ONE_CHAR_TRANSMISSION_US);
		while(TI1);
		waitUs(TIME_DELAY_UART_ONE_CHAR_TRANSMISSION_US);
	}

	SFRPAGE = SFRPAGE_SAVE;
}

#elif (UART_LINE == 0)
//-----------------------------------------------------------------------------
// To send a set of data in an array through UART0 interface
//-----------------------------------------------------------------------------
void sendToUART0 (unsigned char *l_uc_msg_p, unsigned short l_us_starting_index, unsigned short l_us_no_of_bytes)
{
	unsigned short l_us_counter;
	unsigned short l_us_last_index;
	char SFRPAGE_SAVE = SFRPAGE;
	
	l_us_last_index = l_us_starting_index + l_us_no_of_bytes - 1;
	 
	SFRPAGE = UART0_PAGE;

	for (l_us_counter = l_us_starting_index; l_us_counter <= l_us_last_index; l_us_counter++){
		SBUF0 = *(l_uc_msg_p + l_us_counter);
		waitUs(TIME_DELAY_UART_ONE_CHAR_TRANSMISSION_US);
		while(TI0);
		waitUs(TIME_DELAY_UART_ONE_CHAR_TRANSMISSION_US);
	}

	SFRPAGE = SFRPAGE_SAVE;
}
#else
#endif
#endif

//-----------------------------------------------------------------------------
// To choose what are to be sent
// l_us_option_bits structure:
//		bit15-13 = send global variables (0xE000)			- 9TH, 1/2/4 BYTES
//				 001 - g_uc
//				 010 - g_us
//				 011 - g_s
//				 100 - g_ul
//				 101 - reserved
//				 110 - g_f
//				 111 - reserved
//		bit12-10 = send OBDH exclusive buffer (0x1C00) 		- 8TH, 256/512 BYTES
//				 000 - None
//				 001 - SD card data in
//				 010 - SD card data out
//				 011 - Complete SMBin
//				 100 - Complete SMBout
//		bit9 = send global checksum register (0x0200) 		- 7TH, 4 BYTES
//		bit8-6 = send OBDH exclusive info (0x01C0) 		
//				 000 - None
//				 001 - Satellite date (0x004x) 				- 6TH, 8 BYTES
//				 010 - Satellite time (0x008x) 				- 6TH, 7 BYTES
//				 011 - Satellite time (in seconds) (0x00Cx) - 6TH, 4 BYTES
//				 100 - Satellite reference date (0x010x) 	- 6TH, 8 BYTES
//				 101 - OBDH firmware version (0x014x) 		- 6TH, 17 BYTES
//				 110 - OBDH block pointers (0x018x) 		- 6TH, 56 BYTES
//				 111 - OBDH latest state (0x01Cx) 			- 6TH, 18 BYTES
//		bit5-4 = send subsystem's complete data (0x0030)
//				 00 - None
//				 01 - ISIS (0x001x) 						- 5TH, 42 BYTES
//				 10 - ADCS (0x002x) 						- 5TH, 250 BYTES
//				 11 - PWRS (0x003x) 						- 5TH, 35 BYTES
//		bit3 = send error code (0x0008) 					- 1ST, 2 BYTES 
//		bit2 = send SMBOut (0x0004) 						- 3RD, g_uc_i2c_transferred_byte_out BYTES 
//		bit1 = send SMBIn (0x0002) 							- 4TH, g_uc_i2c_transferred_byte_in BYTES
//		bit0 = send time recorded value (0x0001) 			- 2ND, 4 BYTES
//-----------------------------------------------------------------------------
unsigned short sendOptions (unsigned short l_us_option_bits, unsigned char *l_uc_array_p)
{
	unsigned short l_us_data_counter = 1;

	//Send error code
	if (l_us_option_bits & 0x0008) 
		l_us_data_counter = fillData(l_uc_array_p,l_us_data_counter,DATA_OBDH_G_US_ERROR_CODE,0);
	
	//Send time reading
	if (l_us_option_bits & 0x0001) 
		l_us_data_counter = fillData(l_uc_array_p,l_us_data_counter,DATA_OBDH_G_F_RUN_TIME,0);

	//Send SMB out
	if (l_us_option_bits & 0x0004) 
		l_us_data_counter = fillData(l_uc_array_p,l_us_data_counter,DATA_CAT_BUF_I2C_DATA_OUT_VARY,g_uc_i2c_transferred_byte_out);

	//Send SMB In
	if (l_us_option_bits & 0x0002) 
		l_us_data_counter = fillData(l_uc_array_p,l_us_data_counter,DATA_CAT_BUF_I2C_DATA_IN_VARY,g_uc_i2c_transferred_byte_in);

	//Send ISIS complete data
	if ((l_us_option_bits & 0x0030) == 0x0010) 
		l_us_data_counter = fillData(l_uc_array_p,l_us_data_counter,DATA_CAT_COMM_HK,0);

	//Send ADCS complete data
	else if ((l_us_option_bits & 0x0030) == 0x0020)
		l_us_data_counter = fillData(l_uc_array_p,l_us_data_counter,DATA_CAT_ADCS_HK,0);

	//Send PWRS complete data
	else if ((l_us_option_bits & 0x0030) == 0x0030) 
		l_us_data_counter = fillData(l_uc_array_p,l_us_data_counter,DATA_CAT_PWRS_HK,0);
	
	//Send satellite date
	if ((l_us_option_bits & 0x01C0) == 0x0040) 
		l_us_data_counter = fillData(l_uc_array_p,l_us_data_counter,DATA_CAT_SAT_DATE_CURRENT,0);

	//Send satellite time
	else if ((l_us_option_bits & 0x01C0) == 0x0080) 
		l_us_data_counter = fillData(l_uc_array_p,l_us_data_counter,DATA_CAT_SAT_TIME,0);

	//Send satellite time (in seconds)
	else if ((l_us_option_bits & 0x01C0) == 0x00C0) 
		l_us_data_counter = fillData(l_uc_array_p,l_us_data_counter,DATA_CAT_OBDH_UL_OBC_TIME_S,0);

	//Send satellite reference date
	else if ((l_us_option_bits & 0x01C0) == 0x0100) 
		l_us_data_counter = fillData(l_uc_array_p,l_us_data_counter,DATA_CAT_SAT_DATE_INIT,0);

	//Send OBDH firmware version
	else if ((l_us_option_bits & 0x01C0) == 0x0140) 
		l_us_data_counter = fillData(l_uc_array_p,l_us_data_counter,DATA_CAT_OBDH_FIRMWARE_VERSION,OBDH_FIRMWARE_VERSION_LENGTH);

	//Send OBDH block pointers
	else if ((l_us_option_bits & 0x01C0) == 0x0180) 
		l_us_data_counter = fillData(l_uc_array_p,l_us_data_counter,DATA_CAT_OBDH_BP,0);

	//Send OBDH latest state
	else if ((l_us_option_bits & 0x01C0) == 0x01C0) 
		l_us_data_counter = fillData(l_uc_array_p,l_us_data_counter,DATA_CAT_SAT_STATE,0);
	
	//Send global checksum register
	if (l_us_option_bits & 0x0200) 
		l_us_data_counter = fillData(l_uc_array_p,l_us_data_counter,DATA_OBDH_G_US_CHECKSUM_REGISTER,0);
	
	//Send SD card data in
	if ((l_us_option_bits & 0x1C00) == 0x0400)
		l_us_data_counter = fillData(l_uc_array_p,l_us_data_counter,DATA_CAT_BUF_SD_DATA_IN,SD_CARD_BUFFER_SIZE);
	
	//Send SD card data out
	else if ((l_us_option_bits & 0x1C00) == 0x0800)
		l_us_data_counter = fillData(l_uc_array_p,l_us_data_counter,DATA_CAT_BUF_SD_DATA_OUT,SD_CARD_BUFFER_SIZE);
		
	//Send complete SMB in
	else if ((l_us_option_bits & 0x1C00) == 0x0C00)
		l_us_data_counter = fillData(l_uc_array_p,l_us_data_counter,DATA_CAT_BUF_I2C_DATA_IN_VARY,I2C_BUFFER_SIZE);

	//Send complete SMB out
	else if ((l_us_option_bits & 0x1C00) == 0x1000)
		l_us_data_counter = fillData(l_uc_array_p,l_us_data_counter,DATA_CAT_BUF_I2C_DATA_OUT_VARY,I2C_BUFFER_SIZE);

	//Send global variable g_uc
	if ((l_us_option_bits & 0xE000) == 0x2000)
		l_us_data_counter = fillData(l_uc_array_p,l_us_data_counter,DATA_CAT_OBDH_MP_VAR_G_UC,0);

	//Send global variable g_us
	else if ((l_us_option_bits & 0xE000) == 0x4000)
		l_us_data_counter = fillData(l_uc_array_p,l_us_data_counter,DATA_CAT_OBDH_MP_VAR_G_US,0);

	//Send global variable g_s
	else if ((l_us_option_bits & 0xE000) == 0x6000)
		l_us_data_counter = fillData(l_uc_array_p,l_us_data_counter,DATA_CAT_OBDH_MP_VAR_G_S,0);

	//Send global variable g_ul
	else if ((l_us_option_bits & 0xE000) == 0x8000)
		l_us_data_counter = fillData(l_uc_array_p,l_us_data_counter,DATA_CAT_OBDH_MP_VAR_G_UL,0);

	//Send global variable g_f
	else if ((l_us_option_bits & 0xE000) == 0xC000)
		l_us_data_counter = fillData(l_uc_array_p,l_us_data_counter,DATA_CAT_OBDH_MP_VAR_G_F,0);
	
	return l_us_data_counter;
}

#if (SENDING_ENABLE == 1)
#if (ISIS_CMD_ENABLE == 1)
//-----------------------------------------------------------------------------
// To initialize sending data
//-----------------------------------------------------------------------------
// This function is meant to start various initializations for various data
// send request given (only valid for sending data from temporary buffer)
//-----------------------------------------------------------------------------
unsigned short initSendData (void)
{
	unsigned short l_us_error_code = EC_INIT;
	unsigned char l_uc_data_type_sent_counter = 0;
	unsigned char l_uc_max_counter_k = 0;
	unsigned char l_uc_no_of_sent_needed = 0;
	unsigned char l_uc_i = 0;

	if(!(g_uc_strsnd_flag & SNDNG_FLAG_ON_GOING)){
		if(!(g_uc_ax25_header_package_property & AX25_HEADER_PROPERTY_STORED_DATA_BIT)){
			//Initiates AX25 frame sending variables for different cases
			//------------------------------------------------------------------------------------
			if ((g_uc_ax25_header_subsystem == OBDH_SUBSYSTEM) && (g_uc_ax25_header_data_type == SD_ESSENTIAL)){
				g_uc_data_type_sent_buffer[0] 	= DATA_CAT_ACK_MSG;
				g_uc_data_type_sent_buffer[1] 	= DATA_CAT_SAT_STATE;
				g_uc_data_type_sent_buffer[2] 	= DATA_CAT_OBDH_BP;
				g_uc_no_of_data_sent 			= 3;
				l_us_error_code					= EC_SUCCESSFUL;
			}
			else if ((g_uc_ax25_header_subsystem == OBDH_SUBSYSTEM) && (g_uc_ax25_header_data_type == SD_HK)){
				g_uc_data_type_sent_buffer[0] 	= DATA_CAT_ACK_MSG;
				g_uc_data_type_sent_buffer[1] 	= DATA_CAT_LATEST_HK_SAMPLING_TIME_TAI_FORMAT;
				g_uc_data_type_sent_buffer[2] 	= DATA_CAT_LAST_HK_EC;
				g_uc_data_type_sent_buffer[3] 	= DATA_CAT_PWRS_HK;
				g_uc_data_type_sent_buffer[4] 	= DATA_CAT_ADCS_HK;
				g_uc_data_type_sent_buffer[5] 	= DATA_CAT_COMM_HK;
				g_uc_data_type_sent_buffer[6] 	= DATA_CAT_OBDH_HK;
				g_uc_no_of_data_sent 			= 7;
				l_us_error_code					= EC_SUCCESSFUL;
			}
			else if ((g_uc_ax25_header_subsystem == OBDH_SUBSYSTEM) && (g_uc_ax25_header_data_type == SD_SSCP)){
				g_uc_data_type_sent_buffer[0] 	= DATA_CAT_ACK_MSG;
				g_uc_data_type_sent_buffer[1] 	= DATA_CAT_PWRS_SSCP;
				g_uc_data_type_sent_buffer[2] 	= DATA_CAT_ADCS_SSCP;
				g_uc_data_type_sent_buffer[3] 	= DATA_CAT_OBDH_SSCP;
				g_uc_no_of_data_sent 			= 4;
				l_us_error_code					= EC_SUCCESSFUL;
			}
			else if ((g_uc_ax25_header_subsystem == OBDH_SUBSYSTEM) && (g_uc_ax25_header_data_type == SD_BP)){
				g_uc_data_type_sent_buffer[0] 	= DATA_CAT_ACK_MSG;
				g_uc_data_type_sent_buffer[1] 	= DATA_CAT_OBDH_BP;
				g_uc_no_of_data_sent 			= 2;
				l_us_error_code					= EC_SUCCESSFUL;
			}
			else if ((g_uc_ax25_header_subsystem == OBDH_SUBSYSTEM) && (g_uc_ax25_header_data_type == SD_SCHEDULE)){
				g_uc_data_type_sent_buffer[l_uc_data_type_sent_counter++] 		= DATA_CAT_ACK_MSG;
				g_uc_data_type_sent_buffer[l_uc_data_type_sent_counter++] 		= DATA_CAT_SCHEDULE_PAR;
				l_uc_max_counter_k 												= (MAX_AX25_CONTENT_SIZE-ACK_MSG_SIZE-1)/(SCHEDULER_NON_ARG_SIZE+MAX_SCHEDULER_ARG);
				l_uc_no_of_sent_needed 											= (str_obdh_hk.uc_scheduler_block_size+l_uc_max_counter_k-1)/l_uc_max_counter_k;
				if (l_uc_no_of_sent_needed > (MAX_NO_OF_DATA_TYPE_TO_BE_SENT-l_uc_data_type_sent_counter))
					l_uc_no_of_sent_needed = MAX_NO_OF_DATA_TYPE_TO_BE_SENT-l_uc_data_type_sent_counter;
				while(l_uc_no_of_sent_needed){
					g_uc_data_type_sent_buffer[l_uc_data_type_sent_counter++] 	= DATA_CAT_FILLED_SCHEDULE;
					l_uc_no_of_sent_needed--;
				}
				g_uc_no_of_data_sent 											= l_uc_data_type_sent_counter;
				l_us_error_code													= EC_SUCCESSFUL;
			}
			else if ((g_uc_ax25_header_subsystem == OBDH_SUBSYSTEM) && (g_uc_ax25_header_data_type == SD_SCRIPT)){
				g_uc_data_type_sent_buffer[0] 	= DATA_CAT_ACK_MSG;
				g_uc_data_type_sent_buffer[1] 	= DATA_CAT_SCRIPTSPACE_PAR;
				g_uc_data_type_sent_buffer[2] 	= DATA_CAT_SCRIPTSPACE_1;
				g_uc_data_type_sent_buffer[3] 	= DATA_CAT_SCRIPTSPACE_2;
				g_uc_data_type_sent_buffer[4] 	= DATA_CAT_SCRIPTSPACE_3;
				g_uc_data_type_sent_buffer[5] 	= DATA_CAT_SCRIPTSPACE_4;
				g_uc_data_type_sent_buffer[6] 	= DATA_CAT_SCRIPTSPACE_5;
				g_uc_data_type_sent_buffer[7] 	= DATA_CAT_SCRIPTSPACE_6;
				g_uc_data_type_sent_buffer[8] 	= DATA_CAT_SCRIPTSPACE_7;
				g_uc_data_type_sent_buffer[9] 	= DATA_CAT_SCRIPTSPACE_8;
				g_uc_data_type_sent_buffer[10] 	= DATA_CAT_SCRIPTSPACE_9;
				g_uc_data_type_sent_buffer[11] 	= DATA_CAT_SCRIPTSPACE_10;
				g_uc_no_of_data_sent 			= 12;
				l_us_error_code					= EC_SUCCESSFUL;
			}
			else if ((g_uc_ax25_header_subsystem == OBDH_SUBSYSTEM) && (g_uc_ax25_header_data_type == SD_COMPLETE_TASK_STRUCTURE)){
				g_uc_data_type_sent_buffer[0] 	= DATA_CAT_ACK_MSG;
				g_uc_data_type_sent_buffer[1] 	= DATA_CAT_COMPLETE_TASK_STRUCTURE;
				g_uc_no_of_data_sent 			= 2;
				l_us_error_code					= EC_SUCCESSFUL;
			}
			else if ((g_uc_ax25_header_subsystem == OBDH_SUBSYSTEM) && (g_uc_ax25_header_data_type == SD_SAT_TIME)){
				g_uc_data_type_sent_buffer[0] 	= DATA_CAT_ACK_MSG;
				g_uc_data_type_sent_buffer[1] 	= DATA_CAT_OBDH_PAR_TAI_UL_OBC_TIME_S;
				g_uc_no_of_data_sent 			= 2;
				l_us_error_code					= EC_SUCCESSFUL;
			}
			else if ((g_uc_ax25_header_subsystem == OBDH_SUBSYSTEM) && (g_uc_ax25_header_data_type == SD_SAT_REV)){
				g_uc_data_type_sent_buffer[0] 	= DATA_CAT_ACK_MSG;
				g_uc_data_type_sent_buffer[1] 	= DATA_CAT_SAT_REV_NO_IN_FLOAT;
				g_uc_no_of_data_sent 			= 2;
				l_us_error_code					= EC_SUCCESSFUL;
			}
			else if ((g_uc_ax25_header_subsystem == OBDH_SUBSYSTEM) && (g_uc_ax25_header_data_type == SD_SINGLE_SUBSYSTEM_DATA)){
				g_uc_data_type_sent_buffer[0] 	= DATA_CAT_ACK_MSG;
				g_uc_data_type_sent_buffer[1] 	= DATA_CAT_SINGLE_SUBSYSTEM_DATA;
				g_uc_no_of_data_sent 			= 2;
				l_us_error_code					= EC_SUCCESSFUL;
			}
			else if ((g_uc_ax25_header_subsystem == OBDH_SUBSYSTEM) && (g_uc_ax25_header_data_type == SD_GP_REQUEST_KEY)){
				g_uc_data_type_sent_buffer[0] 	= DATA_CAT_ACK_MSG;
				g_uc_data_type_sent_buffer[1] 	= DATA_CAT_GP_REQUEST_KEY;
				g_uc_no_of_data_sent 			= 2;
				l_us_error_code					= EC_SUCCESSFUL;
			}
			else if ((g_uc_ax25_header_subsystem == OBDH_SUBSYSTEM) && (g_uc_ax25_header_data_type == SD_REAL_TIME)){
				g_uc_data_type_sent_buffer[0] 	= DATA_CAT_LAST_HK_EC;
				g_uc_data_type_sent_buffer[1] 	= DATA_CAT_PWRS_HK;
				g_uc_data_type_sent_buffer[2] 	= DATA_CAT_ADCS_HK;
				g_uc_data_type_sent_buffer[3] 	= DATA_CAT_COMM_HK;
				g_uc_data_type_sent_buffer[4] 	= DATA_CAT_OBDH_HK;
				g_uc_no_of_data_sent 			= 5;
				l_us_error_code					= EC_SUCCESSFUL;
			}
			//Before calling to send in the free category,
			//g_uc_data_type_sent_buffer[] and g_uc_no_of_data_sent must be declared outside of this function
			else if ((g_uc_ax25_header_subsystem == OBDH_SUBSYSTEM) && (g_uc_ax25_header_data_type == SD_FREE_CATEGORY)){
				l_us_error_code					= EC_SUCCESSFUL;
			}
			else {
				g_us_gsch_error_code 			= EC_INVALID_SENDING_DATA_REQUEST;
				g_uc_data_type_sent_buffer[0] 	= DATA_CAT_ACK_MSG;
				g_uc_no_of_data_sent 			= 1;			
				l_us_error_code					= EC_INVALID_SENDING_DATA_REQUEST;
			}
			//------------------------------------------------------------------------------------
		}
		else l_us_error_code = EC_SUCCESSFUL;
	}
	else l_us_error_code = EC_SENDING_PROCESS_IS_ON_GOING;

	return l_us_error_code;
}

#if (BEACON_HANDLER_ENABLE == 1)
//-----------------------------------------------------------------------------
// To send data
//-----------------------------------------------------------------------------
// This function is to sent down whatever data asked to the ground station
//
// There are two possibilities so far as data source is concerned:
//	1) Data comes from the temporary buffer
//	2) Data comes from the permanent storage
//
// If data comes from the temporary buffer, then data can be sent by category,
// and can be processed exactly the same way as how we save data in the SD card
//
// If data comes from the permanent storage, this function is not to be mixed
// with how many data block (from the permanent storage) needs to be sent.
// Thus, this function will only capable to send data from the SD card buffer
// alone instead of from the permanent storage. The aim of this function is to
// make sure that whatever useful information in the SD card buffer is sent 
// down. But if there is a need to send data more than one SD card block. This
// requirement must be done outside of this function.
//
// Most important variable used in this function is g_uc_ax25_header_package_property
// whose bits defined as follow:
//	Bit1-0: on-going/end package
//		01 - on-going
//		10 - end
//	Bit2: temporary buffer/SD card buffer
//		0 - temporary buffer
//		1 - SD card buffer
//	Bit7-3: reserved
//-----------------------------------------------------------------------------
unsigned short sendData (void)
{
	unsigned short l_us_error_code;
    unsigned char l_uc_counter = 0;
	unsigned char l_uc_remaining_ax25_space = 0;
	unsigned char l_uc_max_counter = 0;
	unsigned char l_uc_initial_data_sent_counter;
	unsigned char l_uc_break_when_sending = 0;
    unsigned short l_us_current_data_set_length = 0;
    unsigned short l_us_total_to_be_data_length = 0;
	unsigned short l_us_sd_card_meaningful_data_size = 0;
	unsigned char *l_uc_data_type_sent_buffer_p;

	//Initialize the SD card content index buffer here (once)
	static unsigned short l_us_sd_card_content_index_p = SD_CARD_HEADER_SIZE;

	//Initialize data sets to be sent buffer pointer
	l_uc_data_type_sent_buffer_p 	= g_uc_data_type_sent_buffer;
    
    //Temporary test buffer pointer is removed

	//If this is the first time sending, 
	//Go through all the first-time initialization
	if(!(g_uc_strsnd_flag & SNDNG_FLAG_ON_GOING)){
		//Mark an on going sending
		g_uc_strsnd_flag |= SNDNG_FLAG_ON_GOING;

		//Set current data sent counter as zero
		g_uc_current_data_sent_counter = 0;
		
		//If the source is not from SD card
		if (!(g_uc_ax25_header_package_property & AX25_HEADER_PROPERTY_STORED_DATA_BIT))
			//Set current package no to 1
			g_uc_ax25_header_no_of_this_package = 1;
		//If the source is from SD card, the initialization no of the package should be declared outside of this function
	
		//Set package property to be on-going data set now, so long as this function does not go out from the main loop,
		//the package property will always be on-going
		//WARNING: the property whether this data source comes from SD card or other temporary buffer
		//must be defined outside of this function
		g_uc_ax25_header_package_property &= ~AX25_HEADER_PROPERTY_ON_GOING_END_TRANSMISSION_FILTER;
		g_uc_ax25_header_package_property |= AX25_HEADER_PROPERTY_ON_GOING_TRANSMISSION_BIT;
	
		//Initialize package size as AX25_HEADER_SIZE, which is the initial length for any AX25 data writing
		g_uc_ax25_header_this_package_size = AX25_HEADER_SIZE;

		//If the data source is SD card buffer
		if (g_uc_ax25_header_package_property & AX25_HEADER_PROPERTY_STORED_DATA_BIT){
			//Hold data saving. The counter is set before the flag
			g_uc_save_data_hold_timeout_counter = 0;
			g_uc_strsnd_flag |= STRNG_FLAG_SAVE_DATA_HOLD;
	
			//Check SD card meaningful bytes by reading its header (CRC16 included)
			g_us_sd_card_meaningful_data_size = ((g_uc_sd_data_buffer[6] << 8) + g_uc_sd_data_buffer[7]) + 3;
	
			//Limit meaningful bytes to SD_CARD_BUFFER_SIZE (maximum is 512 if we take CRC16 into account)
			if (g_us_sd_card_meaningful_data_size > SD_CARD_BUFFER_SIZE)
				g_us_sd_card_meaningful_data_size = SD_CARD_BUFFER_SIZE;
	
			//First package always contain acknowledgement message
			if (!(g_uc_ax25_header_package_property & AX25_HEADER_PROPERTY_ON_GOING_STORED_DATA_BIT)){
				g_uc_ax25_header_this_package_size = fillData(g_uc_ax25_message,g_uc_ax25_header_this_package_size,DATA_CAT_ACK_MSG,0);
				g_uc_ax25_header_package_property |= AX25_HEADER_PROPERTY_ON_GOING_STORED_DATA_BIT;
			}
	
			//Indicate that this is the beginning of the SD card buffer
			g_uc_ax25_header_package_property |= AX25_HEADER_PROPERTY_START_OF_STORED_DATA_BIT;

			//Fill the AX25 package with adjusted SD card block number and adjusted meaningful data length
			g_uc_ax25_header_this_package_size = fillData(g_uc_ax25_message,g_uc_ax25_header_this_package_size,DATA_CAT_SD_HEADER_INFO_SENT,0);
			
			//Treats as if all header bytes have been taken
			g_us_sd_card_meaningful_data_size -= SD_CARD_HEADER_SIZE;

			//Re-initialize the SD card content index pointer to point the content of the SD card
			l_us_sd_card_content_index_p = SD_CARD_HEADER_SIZE;
		}
	}

	//If the sending is on-going
	if (g_uc_strsnd_flag & SNDNG_FLAG_ON_GOING){	 
	 	//If the source is SD card buffer
		if(g_uc_ax25_header_package_property & AX25_HEADER_PROPERTY_STORED_DATA_BIT){
			//Hold data saving
			g_uc_save_data_hold_timeout_counter = 0;

			//Get the subsystem and data type information sent down from the SD card buffer header
			g_uc_ax25_header_subsystem = g_uc_sd_data_buffer[0];
			g_uc_ax25_header_data_type = g_uc_sd_data_buffer[1]; 

			//Here, if there is special recovery package bit, the case will be very different:
			//1) All initialized value in the AX25 frame will be nullified, the frame will be treated as
			//   if nothing is there. 
			//2) One thing for sure, if this is a recovery package, the content
			//   of remaining meaningful data size of the SD card will not exceed 235.
			//3) The last two bytes of the data are SD card CRC16 bytes .
			//4) Thus, by minusing the last two bytes of the SD card meaningful data size (that is, the
			//   SD card's CRC16 bytes) only the content of the SD card (which is the lost frame's content) 
			//	 is left.
			//5) This makes the maximum remaining meaningful data size of the SD card becomes 233.
			//6) Note that when we save the package sent for recovery purpose, we don't save its 
			//	 AX25 frame's CRC16 bytes, but only the content.
			if (g_uc_ax25_header_package_property & AX25_HEADER_PROPERTY_PACKAGE_RECOVERY_DATA_BIT){
				//Reinitialize the package size
				g_uc_ax25_header_this_package_size = 0;

				//Discard the last two bytes of SD card content
				g_us_sd_card_meaningful_data_size -= 2;
			}

			//While there is a remaining meaningful SD card data
			//Do not initialize the sending anymore, but declare variable for on going sending as running
			//Thus, when it comes back to this function, it will start sending things out exactly where it
			//stops. Some more, when satellite is sending, it will hold the data saving and data loading parameters
	
			//Calculate space left in the AX25 frame buffer, that is 
			//The maximum of MAX_AX25_CONTENT_SIZE + AX25_HEADER_SIZE = 233
			//Minus g_uc_ax25_header_this_package_size, in the first case should be 15
			//15 is when the very first time, since there are additional of ACK_MSG (+5) and SD_HEADER_REDUCED_INFO (+5), besides AX25_HEADER_SIZE (+5)
			//The subsequent case should be 5/10
			//10 is when the loading of new SD block, since there is an additional of SD_HEADER_REDUCED_INFO (+5), besides AX25_HEADER_SIZE (+5)
			//5 is due to AX25_HEADER_SIZE (+5) alone
			//
			//Thus 233 - 15 = 218 space left for the very first case
			//Thus 233 - 5/10 = 228/223 space left for the subsequent cases
			//
			//For special recovery case, g_uc_ax25_header_this_package_size = 0. 
			//Thus 233 space left for AX25 sending.
			//
			//The initialization for g_uc_ax25_header_this_package_size is done in the beginning of this function when
			//SNDNG_FLAG_ON_GOING flag is not raised. It is initialized as AX25_HEADER_SIZE.
			//
			//For "safety standard programming" reason, however, the initialization is also done when the variable first declared
			l_uc_remaining_ax25_space = MAX_AX25_CONTENT_SIZE + AX25_HEADER_SIZE - g_uc_ax25_header_this_package_size;
	
			//Check which one is lower, l_us_sd_card_meaningful_data_size OR l_uc_remaining_ax25_space
			//Choose that one for the maximum number for the for loop
			//If l_uc_remaining_ax25_space is less than or equal to l_us_sd_card_meaningful_data_size
			l_uc_max_counter = (l_uc_remaining_ax25_space <= g_us_sd_card_meaningful_data_size) ? l_uc_remaining_ax25_space : g_us_sd_card_meaningful_data_size;
	
			//In whichever case, cut the g_us_sd_card_meaningful_data_size by that counter
			//This is to update the size of the meaningful data in the SD card buffer to be sent down
			g_us_sd_card_meaningful_data_size -= l_uc_max_counter;
	
			//If the meaningful data size reaches zero by this time it indicates end of stored data buffer 
			if (!g_us_sd_card_meaningful_data_size){
				//Indicates end of stored data bit
				g_uc_ax25_header_package_property |= AX25_HEADER_PROPERTY_END_OF_STORED_DATA_BIT;

				//In addition, ifthe STRNG_FLAG_SD_LOAD_COMPLETED flag is raised, then 
				if (g_uc_strsnd_flag & STRNG_FLAG_SD_LOAD_COMPLETED){
					//Switch the sending package property to end
					g_uc_ax25_header_package_property &= ~AX25_HEADER_PROPERTY_ON_GOING_END_TRANSMISSION_FILTER;
					g_uc_ax25_header_package_property |= AX25_HEADER_PROPERTY_END_OF_TRANSMISSION_BIT;
				}
			}
	
			//Fill the meaningful data to the AX25 buffer
			//Update the SD card content index pointer for the next sending, it should be where the last time it stops
			//Ian (2012-05-15): the logic written should have confirmed that l_us_sd_card_content_index_p will not exceed SD_CARD_BUFFER_SIZE
			for (l_uc_counter = 0; l_uc_counter < l_uc_max_counter; l_uc_counter++)
				g_uc_ax25_message[g_uc_ax25_header_this_package_size++] = g_uc_sd_data_buffer[l_us_sd_card_content_index_p++];

			//Send the AX25 data in the buffer to the ground station
	        l_us_error_code = sendDataInAX25Frame();
	
			//Reset stored and end data bits
			g_uc_ax25_header_package_property &= ~AX25_HEADER_PROPERTY_START_END_STORED_FILTER;
			
			//If there is an error code in sending data, terminates this sending
			if(l_us_error_code){
				g_uc_strsnd_flag &= ~SNDNG_FLAG_ON_GOING;
				g_uc_strsnd_flag &= ~SNDNG_FLAG_COMPLETED;
				g_uc_strsnd_flag &= ~STRNG_FLAG_SAVE_DATA_HOLD;
				return l_us_error_code;
			}

			//If the sending is ended and no more meaningful data size, ends it
			if (!g_us_sd_card_meaningful_data_size){
				//Mark an end of sending
				g_uc_strsnd_flag &= ~SNDNG_FLAG_ON_GOING;

				//Mark that a sending is completed (to be used by a task outside of this function)
				g_uc_strsnd_flag |= SNDNG_FLAG_COMPLETED;

				//Release the hold for data saving
				g_uc_strsnd_flag &= ~STRNG_FLAG_SAVE_DATA_HOLD;

				//If loading is ended
				if (g_uc_strsnd_flag & STRNG_FLAG_SD_LOAD_COMPLETED)
					//Reset first time flag
					g_uc_ax25_header_package_property &= ~AX25_HEADER_PROPERTY_ON_GOING_STORED_DATA_BIT;				
			}
			else
				//Reset	g_uc_ax25_header_this_package_size to AX25_HEADER_SIZE (original size)
				g_uc_ax25_header_this_package_size = AX25_HEADER_SIZE;
		
			//Increase no of this package for the next sending
			g_uc_ax25_header_no_of_this_package++;
		}
	
		//If the data source is other temporary buffer
		else{
			//Set initial value for l_uc_initial_data_sent_counter as the same as g_uc_current_data_sent_counter
			l_uc_initial_data_sent_counter = g_uc_current_data_sent_counter;

		    //Main loop to send every data type given
			for (l_uc_counter = l_uc_initial_data_sent_counter; l_uc_counter < g_uc_no_of_data_sent; l_uc_counter++){

		        //Test case to calculate length of a set of data
		        l_us_current_data_set_length = fillData(g_uc_temporary_data_buffer,0,*(l_uc_data_type_sent_buffer_p+l_uc_counter),0);
		        l_us_total_to_be_data_length = g_uc_ax25_header_this_package_size + l_us_current_data_set_length;
	
				//If the total to be data length is not within allowed length: MAX_AX25_CONTENT_SIZE + AX25_HEADER_SIZE
				if (l_us_total_to_be_data_length > MAX_AX25_CONTENT_SIZE + AX25_HEADER_SIZE){
					//Send the AX25 data in the buffer to the ground station
		            l_us_error_code = sendDataInAX25Frame();
		
					//If there is an error code in sending data, terminates this sending
					if (l_us_error_code){
						g_uc_strsnd_flag &= ~SNDNG_FLAG_ON_GOING;
						g_uc_strsnd_flag &= ~SNDNG_FLAG_COMPLETED;
						return l_us_error_code;
					}
	
					//Increase no of this package for the next sending
					g_uc_ax25_header_no_of_this_package++;
		
					//Reset current package size to the original size = AX25_HEADER_SIZE
					g_uc_ax25_header_this_package_size = AX25_HEADER_SIZE;
		
					//Fill AX25 message buffer with this current data set, update its array index pointer
		            g_uc_ax25_header_this_package_size = fillData(g_uc_ax25_message,g_uc_ax25_header_this_package_size,*(l_uc_data_type_sent_buffer_p+l_uc_counter),1);

					//Increase current data sent counter
					g_uc_current_data_sent_counter++;

					//Set l_uc_break_when_sending as true
					l_uc_break_when_sending = 1;
	
					//Break from the for loop
					break;
				}
				else {//If the total to be data length is within the allowed length
					//Fill AX25 message buffer with this current data set, update its array index pointer
		            g_uc_ax25_header_this_package_size = fillData(g_uc_ax25_message,g_uc_ax25_header_this_package_size,*(l_uc_data_type_sent_buffer_p+l_uc_counter),1);

					//Increase current data sent counter
					g_uc_current_data_sent_counter++;	
				}
			}    
	
			//When the g_uc_current_data_sent_counter == g_uc_no_of_data_sent, 
			//all data must have been sent to the ground station, except the last one in the AX25 message buffer
			//If currently there is an on going transmission, don't enter this if. If not, enters it.
			if (g_uc_current_data_sent_counter >= g_uc_no_of_data_sent && !l_uc_break_when_sending){
				//Update package property to be end data set now
				g_uc_ax25_header_package_property &= ~AX25_HEADER_PROPERTY_ON_GOING_END_TRANSMISSION_FILTER;
				g_uc_ax25_header_package_property |= AX25_HEADER_PROPERTY_END_OF_TRANSMISSION_BIT;
			
				//Send the data stored in the SD card buffer to the Ground Station
			    l_us_error_code = sendDataInAX25Frame();

				//If there is an error code in sending data, terminates this sending
				if(l_us_error_code){
					g_uc_strsnd_flag &= ~SNDNG_FLAG_ON_GOING;
					g_uc_strsnd_flag &= ~SNDNG_FLAG_COMPLETED;
					return l_us_error_code;
				}

				//Mark an end of sending
				g_uc_strsnd_flag &= ~SNDNG_FLAG_ON_GOING;

				//Mark that a sending is completed (to be used by a task outside of this function)
				g_uc_strsnd_flag |= SNDNG_FLAG_COMPLETED;
			}
		}	
	}

	return l_us_error_code;
}

//-----------------------------------------------------------------------------
// To send data in the AX25 frame
//-----------------------------------------------------------------------------
// This function will send data in the AX25 frame and puts its header and CRC16
//	bytes before sending
//
// These five informations are needed to be declared before this function is
//	called (outside of this function):
//	1) g_uc_ax25_header_subsystem
//	2) g_uc_ax25_header_data_type
//	3) g_uc_ax25_header_package_property
//	4) g_uc_ax25_header_no_of_this_package
//	5) g_uc_ax25_header_this_package_size
//-----------------------------------------------------------------------------
unsigned short sendDataInAX25Frame (void)
{
	unsigned short l_us_error_code = EC_INIT;
	unsigned char *l_uc_array_p;
	unsigned char l_uc_data_counter = 0x00;

	//Initialize array pointer to point the AX.25 frame buffer
	l_uc_array_p = g_uc_ax25_message;

	//Only create proper header if this function is called using normal procedure 
	//(package recovery bit is not raised)
	if (!(g_uc_ax25_header_package_property & AX25_HEADER_PROPERTY_PACKAGE_RECOVERY_DATA_BIT)){

		//------------------------------------------------------------------------------------
		//Create header & checksum
		//Write package data type information
		*(l_uc_array_p + l_uc_data_counter++) = g_uc_ax25_header_subsystem;
		*(l_uc_array_p + l_uc_data_counter++) = g_uc_ax25_header_data_type;
		
		//Write package property, filter SD card and first-time property
		*(l_uc_array_p + l_uc_data_counter++) = g_uc_ax25_header_package_property & AX25_HEADER_PROPERTY_SENDING_FILTER;
		
		//Write no of this package
		*(l_uc_array_p + l_uc_data_counter++) = g_uc_ax25_header_no_of_this_package;
		
		//Write this package size
		*(l_uc_array_p + l_uc_data_counter++) = g_uc_ax25_header_this_package_size;
		//------------------------------------------------------------------------------------
		
		#if (STORING_ENABLE == 1)
		//------------------------------------------------------------------------------------
		//If the source of this sending is SD card, save it first (for package lost recovery purpose)
		if (g_uc_ax25_header_package_property & AX25_HEADER_PROPERTY_STORED_DATA_BIT){
			//Release the save data hold
			g_uc_strsnd_flag &= ~STRNG_FLAG_SAVE_DATA_HOLD;

			//Save the data sent
			saveDataProtected(ALL_SUBSYSTEMS,SD_RECOVERY_PACKAGE,g_uc_temporary_data_buffer);

			//Hold save data again
			g_uc_strsnd_flag |= STRNG_FLAG_SAVE_DATA_HOLD;
		}
		//------------------------------------------------------------------------------------
		#endif

	//Else, if this is special sending (lost frame recovery) case, just send the data directly. 
	}

	#if (CRC_ENABLE == 1)
	//Warning: g_uc_ax25_header_this_package_size must have been specifed correctly outside of this function
	//------------------------------------------------------------------------------------
	//Add checksum bytes
	if (CHECKSUM_DEFAULT & 0x40)	//If checksum default method is 16-bit
		insertCRC16(l_uc_array_p,g_uc_ax25_header_this_package_size);
	//------------------------------------------------------------------------------------
	#endif

	//------------------------------------------------------------------------------------
	//Send AX25 package
	l_us_error_code = isisI2CMsg (ITC_I2C_SND_AX_FR_DFLT,0,g_uc_ax25_header_this_package_size+2,0);
	//------------------------------------------------------------------------------------

	//------------------------------------------------------------------------------------
	//Record the no of data bytes sent
	if (!l_us_error_code) str_obdh_hk.ul_data_byte_sent += (g_uc_ax25_header_this_package_size+2);
	//------------------------------------------------------------------------------------
	
	return l_us_error_code;
}
#endif

#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)
//-----------------------------------------------------------------------------
// Common pattern to send ACK message only (by GSCH)
//-----------------------------------------------------------------------------
void sendAckMsg (void)
{
	unsigned short l_us_init_send_error_code = EC_INIT;

	//Indicates taking data from temporary buffer
	g_uc_ax25_header_package_property = 0x00;

	//Initialize sending data under free category
	g_uc_ax25_header_subsystem = OBDH_SUBSYSTEM;
	g_uc_ax25_header_data_type = SD_FREE_CATEGORY;

	//Initialize no of data sent
	g_uc_no_of_data_sent = 1;

	//Fill with ACK MSG
	g_uc_data_type_sent_buffer[0] = DATA_CAT_ACK_MSG;

	//Initialized sending data (once only)
	l_us_init_send_error_code = initSendData();

	//If there is unaccepted initialization error, do not proceed (re-take new command)
	//Else, proceeds to GSCH_STATE_SEND_REAL_TIME_DATA state
	if(!l_us_init_send_error_code || l_us_init_send_error_code == EC_INVALID_SENDING_DATA_REQUEST)
		//Send real-time data
		groundEnterSendRealTime();									
}

//-----------------------------------------------------------------------------
// Common pattern to send data from OBDH temporary buffer (by GSCH)
//-----------------------------------------------------------------------------
void sendCommonData (unsigned char l_uc_common_data)
{
	unsigned short l_us_init_send_error_code = EC_INIT;

	//Always successful
	g_us_gsch_error_code = EC_SUCCESSFUL;

	//Indicates taking data from temporary buffer
	g_uc_ax25_header_package_property = 0x00;

	//Taking the data initialization from the command argument
	g_uc_ax25_header_subsystem = OBDH_SUBSYSTEM;
	g_uc_ax25_header_data_type = l_uc_common_data;

	//Initialized sending data (once only)
	l_us_init_send_error_code = initSendData();

	//If there is unaccepted initialization error, do not proceed (re-take new command)
	//Else, proceeds to GSCH_STATE_SEND_REAL_TIME_DATA state
	if(!l_us_init_send_error_code || l_us_init_send_error_code == EC_INVALID_SENDING_DATA_REQUEST)
		//Send real-time data
		groundEnterSendRealTime();									
}

//-----------------------------------------------------------------------------
// Common pattern to send ket for Groundpass request (by GSCH)
//-----------------------------------------------------------------------------
void sendKeyRequest (void){
	unsigned short l_us_init_send_error_code = EC_INIT;

	//Always successful
	g_us_gsch_error_code = EC_SUCCESSFUL;

	//Indicates taking data from temporary buffer
	g_uc_ax25_header_package_property = 0x00;

	//Taking the data initialization from the command argument
	g_uc_ax25_header_subsystem = OBDH_SUBSYSTEM;
	g_uc_ax25_header_data_type = SD_GP_REQUEST_KEY;

	//Initialized sending data (once only)
	l_us_init_send_error_code = initSendData();

	//If there is unaccepted initialization error, do not proceed (re-take new command)
	//Else, proceeds to send the data
	if(!l_us_init_send_error_code || l_us_init_send_error_code == EC_INVALID_SENDING_DATA_REQUEST)
		//Start sending request
		groundStartSendRequest();
}
#endif
#endif
#endif
