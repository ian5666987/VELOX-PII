//------------------------------------------------------------------------------------
// Luxp_add.c
//------------------------------------------------------------------------------------
// Copyright (C) 2013 Nanyang Technological University.
//
// AUTH: IAN
// DATE: 03 APR 2013
//
// The program Luxp_add.c will be downloaded to the Pumpkin Motherboard with 
// C8051F120 uC as part of the VELOX-P Pico-Satellite Flight Software. 
//
// There are six main type of functions contained in this file, namely:
// 1) Bytes manipulation
// 2) Clear functions
// 3) Reset functions
// 4) Insertion functions
// 5) Fill data functions
//
//------------------------------------------------------------------------------------
// Overall functional test status
//------------------------------------------------------------------------------------
// Partially tested, all fully functional
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

#include <luxp_add.h>

//------------------------------------------------------------------------------------
// Convert two shorts into long integer
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
unsigned long ushort2ulong (unsigned short l_us_high_short, unsigned short l_us_low_short)
{
	unsigned long l_ul_long_int = 0x00000000;

	l_ul_long_int = l_us_high_short;
	l_ul_long_int = l_ul_long_int << 16;
	l_ul_long_int += l_us_low_short;

	return l_ul_long_int;
}

//------------------------------------------------------------------------------------
// Convert four chars into a long integer
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
unsigned long uchar2ulong (unsigned char l_uc_hh_byte, unsigned char l_uc_hl_byte, unsigned char l_uc_lh_byte, unsigned char l_uc_ll_byte)
{
	unsigned long l_ul_long_int = 0x00000000;
	unsigned short l_us_low_short = 0x0000;
	unsigned short l_us_high_short = 0x0000;

	l_us_low_short = ((unsigned short)(l_uc_lh_byte << 8) + l_uc_ll_byte);
	l_us_high_short = ((unsigned short)(l_uc_hh_byte << 8) + l_uc_hl_byte);
	l_ul_long_int = ushort2ulong(l_us_high_short,l_us_low_short);

	return l_ul_long_int;
}

//------------------------------------------------------------------------------------
// Convert four characters to float
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
float uchar2float (unsigned char l_uc_hh_byte, unsigned char l_uc_hl_byte, unsigned char l_uc_lh_byte, unsigned char l_uc_ll_byte)
{
	float l_f_float;
	(long)l_f_float = (long)uchar2ulong(l_uc_hh_byte,l_uc_hl_byte,l_uc_lh_byte,l_uc_ll_byte);
 	return l_f_float;
}

//------------------------------------------------------------------------------------
// Convert a float to low low char
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
unsigned char float2ullchar (float l_f_float)
{
	unsigned char *l_uc_float_p;
	l_uc_float_p = &l_f_float;
 	return *(l_uc_float_p + 3);
}

//------------------------------------------------------------------------------------
// Convert a float to low high char
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
unsigned char float2ulhchar (float l_f_float)
{
	unsigned char *l_uc_float_p;
	l_uc_float_p = &l_f_float;
 	return *(l_uc_float_p + 2);
}

//------------------------------------------------------------------------------------
// Convert a float to high low char
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
unsigned char float2uhlchar (float l_f_float)
{
	unsigned char *l_uc_float_p;
	l_uc_float_p = &l_f_float;
 	return *(l_uc_float_p + 1);
}

//------------------------------------------------------------------------------------
// Convert a float to high high char
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
unsigned char float2uhhchar (float l_f_float)
{
	unsigned char *l_uc_float_p;
	l_uc_float_p = &l_f_float;
 	return *l_uc_float_p;
}

#if (TIME_ENABLE == 1)
//------------------------------------------------------------------------------------
// To change two 0-9 decimal numbers in one byte character to its proper one byte 
// hexadecimal character
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
// Ian (2012-05-15): This function is used to standardize real-time clock (RTC) chip, 
//                   an embedded chip to the on board computer, data format with the 
//                   rests of the data in the on board computer. Because the data
//					 format sent from the RTC is in hexadecimal, though it represents
//					 decimal value. 
//
//					 Ex: 
//					 (1) Day data byte sent = 0x23 (true dec value = 35, but RTC 
//  					 actually says dec value 23 - or day 23 of the month).
//
//					 The function oneHex2twoDec does the reverse of this function.
unsigned char twoDec2oneHex (unsigned char l_uc_two_dec_no)
{
	unsigned char l_uc_one_hex_no;
	unsigned char l_uc_h_dec_no;
	unsigned char l_uc_l_dec_no;

	l_uc_h_dec_no = l_uc_two_dec_no & 0xF0;
	l_uc_h_dec_no = l_uc_h_dec_no >> 4;
	l_uc_h_dec_no = l_uc_h_dec_no & 0x0F;
	l_uc_h_dec_no = l_uc_h_dec_no * 10;

	l_uc_l_dec_no = l_uc_two_dec_no & 0x0F;

	l_uc_one_hex_no = l_uc_h_dec_no + l_uc_l_dec_no;
	
	return l_uc_one_hex_no;	
}
#endif

#if (RTC_CMD_ENABLE == 1)
//------------------------------------------------------------------------------------
// To change one hexadecimal character in one byte to its two 0-9 decimal numbers in 
// one byte character
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
unsigned char oneHex2twoDec (unsigned char l_uc_one_hex_no)
{
	unsigned char l_uc_two_dec_no;
	unsigned char l_uc_h_dec_no;
	unsigned char l_uc_l_dec_no;
	unsigned char l_uc_temp_dec_no;

	l_uc_h_dec_no = l_uc_one_hex_no/10;
	l_uc_h_dec_no = l_uc_h_dec_no & 0x0F;
	l_uc_h_dec_no = l_uc_h_dec_no << 4;
	l_uc_h_dec_no = l_uc_h_dec_no & 0xF0;

	l_uc_temp_dec_no = l_uc_h_dec_no >> 4;
	l_uc_temp_dec_no = l_uc_temp_dec_no & 0x0F;
	l_uc_temp_dec_no = l_uc_temp_dec_no * 10;
	l_uc_l_dec_no = l_uc_one_hex_no - l_uc_temp_dec_no;

	l_uc_two_dec_no = l_uc_h_dec_no + l_uc_l_dec_no;

	return l_uc_two_dec_no;
}
#endif

//------------------------------------------------------------------------------------
// To clear UARTBuffer
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
void clearUARTBuffer (void)
{
	int i;
	for (i = 0; i < UART_BUFFER_SIZE; i++) g_uc_uart_buffer[i] = '\0';
}

//------------------------------------------------------------------------------------
// To clear call task buffer
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
void clearCallTaskBuffer (void)
{
	int i;
	for (i = 0; i < CALL_TASK_BUFFER_SIZE; i++) g_uc_call_task_buffer[i] = '\0';
}

//------------------------------------------------------------------------------------
// To reset I2C Communication
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
void resetI2CCommunication (void)
{
	char SFRPAGE_SAVE = SFRPAGE;			
	SFRPAGE = SMB0_PAGE;
	
	SMB0CN &= ~0x40;                 // Reset communication
	SMB0CN |= 0x40;
	STA = 0;
	STO = 0;
	AA = 0;
	g_us_i2c_flag &= ~I2C_FLAG_BUSY;				//Free SMBus line		

	SFRPAGE = SFRPAGE_SAVE;             
}

#if (TASK_ENABLE == 1)
//------------------------------------------------------------------------------------
// To reset a satellite's task
//------------------------------------------------------------------------------------
void resetTask (unsigned char l_uc_task)
{
	//Scriptspace is not reset due to possible user setting to set a task to different
	//scriptspace. Yet, user setting is not implemented for VELOX-P
	str_task[l_uc_task].uc_status 								= 0x00;	
	str_task[l_uc_task].uc_script_running_flags 				= 0x01;
	str_task[l_uc_task].uc_script_error_flags 					= 0x00;
	str_task[l_uc_task].uc_script_completed_flags 				= 0x00;
	str_task[l_uc_task].uc_script_timer_owning_flags 			= 0x00;
	str_task[l_uc_task].uc_script_timer_running_flags 			= 0x00;
	str_task[l_uc_task].uc_script_retry_counter_owning_flags 	= 0x00;

	//Ian (2012-05-15): It is noted that debugging feature is not removed here.
	#if (DEBUGGING_FEATURE == 1)
	str_task[l_uc_task].uc_status_held							= 0x00;	//uc_status_held cleared
	#endif
}
#endif

#if (SATELLITE_MODE_HANDLER_ENABLE == 1)
#if (BODY_RATE_HANDLER_ENABLE == 1)
#if (ADCS_CMD_ENABLE == 1)
#if (PWRS_CMD_ENABLE == 1)
//------------------------------------------------------------------------------------
// To reset body rate values to high
//------------------------------------------------------------------------------------
// Function's test result: tested
//------------------------------------------------------------------------------------
// Ian (2012-05-15): The initial rate vector is expected to be high.
//					 Since BRH_FLAG_BR_HI can only be set high if the initial rate
//					 vector is high, so the initial vector is also to be set as high.
void resetBodyRateValue (void)
{
	unsigned char l_uc_i;
	unsigned char l_uc_j;

	for (l_uc_i = 0; l_uc_i < 3; l_uc_i++){
		str_obdh_data.us_br_eci_avg[l_uc_i] = INITIAL_BR_VALUE;
		for (l_uc_j = 0; l_uc_j < BR_ECI_SAMPLE_NO; l_uc_j++)
			g_s_br_eci[l_uc_i][l_uc_j] = INITIAL_BR_VALUE;
	}

	str_obdh_hk.us_brh_flag |= BRH_FLAG_BR_HI;
}
#endif
#endif
#endif
#endif

//------------------------------------------------------------------------------------
// Insert float to an array
//------------------------------------------------------------------------------------
// Function's test result: tested
//------------------------------------------------------------------------------------
unsigned short insertFloatToArray (float l_f_to_insert, unsigned char *l_uc_array_p, unsigned short l_us_starting_counter)
{
	*(l_uc_array_p + l_us_starting_counter) 	= float2uhhchar(l_f_to_insert);
	*(l_uc_array_p + l_us_starting_counter + 1) = float2uhlchar(l_f_to_insert);
	*(l_uc_array_p + l_us_starting_counter + 2) = float2ulhchar(l_f_to_insert);
	*(l_uc_array_p + l_us_starting_counter + 3) = float2ullchar(l_f_to_insert);

	return l_us_starting_counter+4;
}

//------------------------------------------------------------------------------------
// Insert long to an array
//------------------------------------------------------------------------------------
// Function's test result: tested
//------------------------------------------------------------------------------------
unsigned short insertLongToArray (unsigned long l_ul_to_insert, unsigned char *l_uc_array_p, unsigned short l_us_starting_counter)
{
	*(l_uc_array_p + l_us_starting_counter) 	= l_ul_to_insert >> 24;
	*(l_uc_array_p + l_us_starting_counter + 1) = l_ul_to_insert >> 16;
	*(l_uc_array_p + l_us_starting_counter + 2) = l_ul_to_insert >> 8;
	*(l_uc_array_p + l_us_starting_counter + 3) = l_ul_to_insert;

	return l_us_starting_counter+4;
}

//------------------------------------------------------------------------------------
// Insert short to an array
//------------------------------------------------------------------------------------
// Function's test result: tested
//------------------------------------------------------------------------------------
unsigned short insertShortToArray (unsigned short l_us_to_insert, unsigned char *l_uc_array_p, unsigned short l_us_starting_counter)
{
	*(l_uc_array_p + l_us_starting_counter) 	= l_us_to_insert >> 8;
	*(l_uc_array_p + l_us_starting_counter + 1) = l_us_to_insert;

	return l_us_starting_counter+2;
}

//------------------------------------------------------------------------------------
// Insert an array to a float using global counter
//------------------------------------------------------------------------------------
// Function's test result: tested
//------------------------------------------------------------------------------------
float insertArrayToFloatGC (unsigned char *l_uc_array_p)
{
	float l_f_result;

	l_f_result = uchar2float(*(l_uc_array_p+g_us_data_counter),*(l_uc_array_p+g_us_data_counter+1),*(l_uc_array_p+g_us_data_counter+2),*(l_uc_array_p+g_us_data_counter+3));
	g_us_data_counter += 4;

	return l_f_result;
}

//------------------------------------------------------------------------------------
// Insert an array to a long using global counter
//------------------------------------------------------------------------------------
// Function's test result: tested
//------------------------------------------------------------------------------------
unsigned long insertArrayToLongGC (unsigned char *l_uc_array_p)
{
	unsigned long l_ul_result;

	l_ul_result = uchar2ulong (*(l_uc_array_p+g_us_data_counter),*(l_uc_array_p+g_us_data_counter+1),*(l_uc_array_p+g_us_data_counter+2),*(l_uc_array_p+g_us_data_counter+3));
	g_us_data_counter += 4;

	return l_ul_result;
}

//------------------------------------------------------------------------------------
// Insert an array to a short using global counter
//------------------------------------------------------------------------------------
// Function's test result: tested
//------------------------------------------------------------------------------------
unsigned short insertArrayToShortGC (unsigned char *l_uc_array_p)
{
	unsigned short l_us_result;

	l_us_result = ((unsigned short)(*(l_uc_array_p+g_us_data_counter) << 8) + *(l_uc_array_p+g_us_data_counter+1));
	g_us_data_counter += 2;

	return l_us_result;
}

#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)
#if (ISIS_CMD_ENABLE == 1)
//------------------------------------------------------------------------------------
// Insert GDSN command to GSCH variables and return its validity
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
void insertGDSNCommandToGSCHVar (void)
{
	unsigned short l_us_i;
	unsigned short l_us_comm_error_code;
	#if (CRC_ENABLE == 1)
	unsigned short l_us_crc16_matching_result;
	#endif
	
	//Get AX.25 frame from COMM board
	l_us_comm_error_code = isisI2C(IMC_ADDR,IMC_I2C_GET_FR,0);			

	//Only check command validity if there is no error code so far
	if(!l_us_comm_error_code){
		//Assume command to be invalid first
		g_us_gsch_command_validation_error_code = EC_INVALID_GDSN_COMMAND; 
		
		//Read the command (in the whole frame)
		g_uc_gsch_gs_header 	= g_uc_i2c_data_in[1];
		g_uc_gsch_gs_cmd_header = g_uc_i2c_data_in[2];
		g_uc_gsch_gs_cmd_tail 	= g_uc_i2c_data_in[3];
		g_uc_gsch_gs_arg_len 	= g_uc_i2c_data_in[4];
		for (l_us_i = 5; l_us_i < 5+MAX_GSCH_CMD_ARG_LEN ; l_us_i++)
			g_uc_gsch_gs_arg[l_us_i-5] = g_uc_i2c_data_in[l_us_i];
	
		//CRC16 record, the CRC bytes are always in the last two bytes of the commands
		g_us_gsch_gs_cmd_crc = (unsigned short)(g_uc_i2c_data_in[COMMAND_LENGTH-1] << 8) + g_uc_i2c_data_in[COMMAND_LENGTH];
	
		//Remove frame as it has been obtained
		l_us_comm_error_code = isisI2C(IMC_ADDR,IMC_I2C_RMV_FR,0);

		//Only check command validity if there is no error code so far
		if(!l_us_comm_error_code){
			//Check validity of the command for different GSCH states
			if (str_obdh_hk.uc_gsch_state == GSCH_STATE_IDLE){
				//Check the command's validity first
				//The only valid command in this mode is GDSN_REQUEST_GDPASS
				//The only valid header in this mode is GDSN ADDR
				if (g_uc_gsch_gs_header == GDSN_ADDR && g_uc_gsch_gs_cmd_header == GDSN_ADDR && g_uc_gsch_gs_cmd_tail == GDSN_REQUEST_GDPASS)
					g_us_gsch_command_validation_error_code = EC_SUCCESSFUL;
				else g_us_gsch_command_validation_error_code = EC_INVALID_GDSN_COMMAND_IDLE_STATE;
			}
			else if (str_obdh_hk.uc_gsch_state == GSCH_STATE_WAITING_FOR_KEY_REPLY){
				//Check the command's validity first
				//The only valid header in this mode is GDSN ADDR
				if (g_uc_gsch_gs_header == GDSN_ADDR && g_uc_gsch_gs_cmd_header == GDSN_ADDR)
					g_us_gsch_command_validation_error_code = EC_SUCCESSFUL;
				else g_us_gsch_command_validation_error_code = EC_INVALID_GDSN_COMMAND_WAITING_FOR_KEY_REPLY_STATE;
			}
			else if (str_obdh_hk.uc_gsch_state == GSCH_STATE_GROUND_PASS){
				//Check the command's validity first
				if (g_uc_gsch_gs_cmd_tail == GDSN_REQUEST_GDPASS || g_uc_gsch_gs_cmd_tail == GDSN_SEND_GDPASS_CODE)
					g_us_gsch_command_validation_error_code = EC_INVALID_GDSN_COMMAND_GROUND_PASS_STATE;
				else if (str_obdh_hk.uc_sat_mode < SATELLITE_MODE_M5 && (g_uc_gsch_gs_cmd_tail == GDSN_LOG_DATA || g_uc_gsch_gs_cmd_tail == GDSN_GET_DATA_SPEC || g_uc_gsch_gs_cmd_tail == GDSN_SCHEDULE_SUN_TRACKING || g_uc_gsch_gs_cmd_tail == GDSN_UPLOAD_TCMD_A || g_uc_gsch_gs_cmd_tail == GDSN_UPLOAD_TCMD_B || g_uc_gsch_gs_cmd_tail == GDSN_SCHEDULE_MISSION_MODE))
					g_us_gsch_command_validation_error_code = EC_INVALID_GDSN_COMMAND_ON_LEOP;
				else g_us_gsch_command_validation_error_code = EC_SUCCESSFUL;
			}

			//If the command is valid so far, check its length
			if (!g_us_gsch_command_validation_error_code)
				if (g_uc_gsch_gs_arg_len > MAX_GSCH_CMD_ARG_LEN)
					g_us_gsch_command_validation_error_code = EC_INVALID_GDSN_COMMAND_ARG_LENGTH;
		
			#if (CRC_ENABLE == 1)
			//If the command is valid so far
			if (!g_us_gsch_command_validation_error_code){
				//Double check the command with CRC16 matching, the CRC bytes are always in the last two bytes
				l_us_crc16_matching_result = checkCRC16Match(&g_uc_i2c_data_in[1],COMMAND_LENGTH-2);
		
				//If the CRC16 bytes show that the command is indeed valid, then we can process the command, 
				//else consider the command invalid
				if (l_us_crc16_matching_result) //If CRC matching result is not zero, there is something fishy about the command...
					g_us_gsch_command_validation_error_code = EC_INVALID_GDSN_COMMAND_CRC_BYTES_DO_NOT_MATCH;
			}
			#endif
		}
	}
}
#endif
#endif

#if (CQUEUE_ENABLE == 1)
//------------------------------------------------------------------------------------
// Insert GSCH command to command queue
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
void insertGSCHCommandToQueue (void)
{
	#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)

	unsigned short l_us_i;

	//Pass the command to the command executed register (overwrite whatever currently in the register)
	//By right, there cannot be any unexecuted command (in the command register) at this point
	//The only possibility whereby there is any command in the execution register here is if 
	//the str_obdh_par.uc_cq_execution_hold_activation is activated
	//(But this is only possible if we want to debug the program, somemore,
	//str_obdh_par.uc_cq_execution_hold_activation is now removed)
	//Thus, command executed register must be empty at this point
	g_uc_command_queue[0][0] = 0xBB;
	g_uc_command_queue[0][1] = g_uc_gsch_gs_cmd_header;
	g_uc_command_queue[0][2] = g_uc_gsch_gs_cmd_tail;
	g_uc_command_queue[0][3] = g_uc_gsch_gs_arg_len;
	for (l_us_i = COMMAND_HEADER_SIZE; l_us_i < g_uc_gsch_gs_arg_len+COMMAND_HEADER_SIZE; l_us_i++)
		g_uc_command_queue[0][l_us_i] = g_uc_gsch_gs_arg[l_us_i-COMMAND_HEADER_SIZE];

	#if (CRC_ENABLE == 1)
	//Create CRC16 bytes for this command (may not be necessary, but well...)
	insertCRC16(&g_uc_command_queue[0], g_uc_gsch_gs_arg_len+COMMAND_HEADER_SIZE);
	#endif

	#if (EVENT_HANDLER_ENABLE == 1)
	//dec22_yc here add even logging for command added to queue tested			
	eventHandler(EV_MCC_TO_COMMAND,g_uc_gsch_gs_cmd_header,g_uc_gsch_gs_cmd_tail,
			g_uc_command_queue[0][COMMAND_HEADER_SIZE],g_uc_command_queue[0][COMMAND_HEADER_SIZE+1],g_uc_command_queue[0][COMMAND_HEADER_SIZE+2],g_uc_command_queue[0][COMMAND_HEADER_SIZE+3]);
	#endif

	//Add command queue size
	g_uc_command_queue_size++;

	#endif
}
#endif

#if (CRC_ENABLE == 1)
//------------------------------------------------------------------------------------
// To insert CRC16 bytes to an array
//------------------------------------------------------------------------------------
void insertCRC16 (unsigned char *l_uc_msg_pointer, unsigned short l_us_msg_length)
{
	unsigned short l_us_temp_crc16 = 0xffff;

	l_us_temp_crc16 = createChecksum(l_uc_msg_pointer, l_us_msg_length, CHECKSUM_CRC16_TABLE);
	addChecksumBytes16 (l_uc_msg_pointer, l_us_msg_length, l_us_temp_crc16);

}
#endif

//------------------------------------------------------------------------------------
// To fill an array with various message
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
unsigned short fillData (unsigned char *l_uc_array_p, unsigned short l_us_data_counter, unsigned short l_us_data_type, unsigned short l_us_fill_argument)
{
	unsigned short 	l_us_counter_i;
	#if (FULL_ADD_ENABLE == 1)
	unsigned short 	l_us_counter_j;
	unsigned char	l_uc_counter_k;
	unsigned char	l_uc_max_counter_k;
	unsigned short	l_us_dummy_var;
	#endif
	unsigned short 	l_us_timer_multiplier = SYSCLK/12/200;
	float 			l_f_timer_mili_second[2];

	//Determines which data to send
	switch (l_us_data_type){
		//Fill global error code (2)
		case DATA_OBDH_G_US_ERROR_CODE:
			l_us_data_counter = insertShortToArray(g_us_error_code,l_uc_array_p,l_us_data_counter);
			break;

		//Fill run time (4)
		case DATA_OBDH_G_F_RUN_TIME:
			for (l_us_counter_i = 0; l_us_counter_i < 2; l_us_counter_i++){
				g_us_timer_value[l_us_counter_i] 		= ((unsigned short)((g_uc_timer_value_high[l_us_counter_i] << 8) & 0xFF00) + g_uc_timer_value_low[l_us_counter_i]);
				l_f_timer_mili_second[l_us_counter_i] 	= g_us_timer_value[l_us_counter_i] / 2041.6 + l_us_timer_multiplier / 2041.6 * g_us_timer_flag_counter_value[l_us_counter_i]; //right constant here is 2041.6
			}
			
			l_f_timer_mili_second[1] -= l_f_timer_mili_second[0];	
			l_us_data_counter = insertFloatToArray(l_f_timer_mili_second[1],l_uc_array_p,l_us_data_counter);
			break;

		//Fill OBDH global multipurpose variables: g_uc
		//Ian (2012-05-15): It is noted that the global multipurpose variables are not initialized
		//					since they have no function on their own except as intermediate variables.
		case DATA_CAT_OBDH_MP_VAR_G_UC:
			*(l_uc_array_p + l_us_data_counter++) = g_uc;
			break;

		//Fill OBDH global multipurpose variables: g_us
		case DATA_CAT_OBDH_MP_VAR_G_US:
			l_us_data_counter = insertShortToArray(g_us,l_uc_array_p,l_us_data_counter);
			break;

		//Fill OBDH global multipurpose variables: g_s
		case DATA_CAT_OBDH_MP_VAR_G_S:
			l_us_data_counter = insertShortToArray(g_s,l_uc_array_p,l_us_data_counter);
			break;

		//Fill OBDH global multipurpose variables: g_ul
		case DATA_CAT_OBDH_MP_VAR_G_UL:
			l_us_data_counter = insertLongToArray(g_ul,l_uc_array_p,l_us_data_counter);
			break;

		//Fill OBDH global multipurpose variables: g_f
		case DATA_CAT_OBDH_MP_VAR_G_F:
			l_us_data_counter = insertFloatToArray(g_f,l_uc_array_p,l_us_data_counter);
			break;

		#if (FULL_ADD_ENABLE == 1)
		
		//Fill for latest HK sampling time and orbit (TAI format)
		case DATA_CAT_LATEST_HK_SAMPLING_TIME_AND_ORBIT_TAI_FORMAT:
			g_ul = str_obdh_data.ul_latest_hk_sampling_time_s + SECONDS_FROM_TAI_TO_KILL_SWITCH_DEACTIVATION;
			l_us_data_counter = insertLongToArray(g_ul,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(str_obdh_data.us_latest_hk_sampling_orbit,l_uc_array_p,l_us_data_counter);
			break;

		//Fill for latest HK sampling time (from TAI)
		case DATA_CAT_LATEST_HK_SAMPLING_TIME_TAI_FORMAT:
			g_ul = str_obdh_data.ul_latest_hk_sampling_time_s + SECONDS_FROM_TAI_TO_KILL_SWITCH_DEACTIVATION;
			l_us_data_counter = insertLongToArray(g_ul,l_uc_array_p,l_us_data_counter);
			break;

		//Fill satellite time (in second)
		case DATA_CAT_OBDH_UL_OBC_TIME_S:
			l_us_data_counter = insertLongToArray(str_obdh_data.ul_obc_time_s,l_uc_array_p,l_us_data_counter);
			break;

		//Fill satellite time (in second, TAI format)
		case DATA_CAT_OBDH_PAR_TAI_UL_OBC_TIME_S:
			//Lock OBC time before being used
			g_ul = str_obdh_data.ul_obc_time_s + SECONDS_FROM_TAI_TO_KILL_SWITCH_DEACTIVATION;
			l_us_data_counter = insertLongToArray(g_ul,l_uc_array_p,l_us_data_counter);
			break;

		//Fill for latest current orbit and latest current time in TAI format
		case DATA_CAT_LATEST_TIME_AND_ORBIT_TAI_FORMAT:
			//Lock OBC time before being used
			g_ul = str_obdh_data.ul_obc_time_s + SECONDS_FROM_TAI_TO_KILL_SWITCH_DEACTIVATION;
			l_us_data_counter = insertLongToArray(g_ul,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(str_obdh_data.us_current_rev_no,l_uc_array_p,l_us_data_counter);
			break;

		#endif

		//Fill I2C data in (vary)
		case DATA_CAT_BUF_I2C_DATA_IN_VARY:
			for (l_us_counter_i = 0; l_us_counter_i < l_us_fill_argument; l_us_counter_i++)
				*(l_uc_array_p + l_us_data_counter++) = g_uc_i2c_data_in[l_us_counter_i];		
			break;

		//Fill I2C data out (vary)
		case DATA_CAT_BUF_I2C_DATA_OUT_VARY:
			for (l_us_counter_i = 0; l_us_counter_i < l_us_fill_argument; l_us_counter_i++)
				*(l_uc_array_p + l_us_data_counter++) = g_uc_i2c_data_out[l_us_counter_i];		
			break;

		#if (FULL_ADD_ENABLE == 1)
		
		//Fill SD card data in
		case DATA_CAT_BUF_SD_DATA_IN:
			for (l_us_counter_i = 0; l_us_counter_i < SD_CARD_BUFFER_SIZE; l_us_counter_i++)
				*(l_uc_array_p + l_us_data_counter++) = g_uc_sd_data_buffer[l_us_counter_i];
			break;

		//Fill SD card data out
		case DATA_CAT_BUF_SD_DATA_OUT:
			for (l_us_counter_i = 0; l_us_counter_i < SD_CARD_BUFFER_SIZE; l_us_counter_i++)
				*(l_uc_array_p + l_us_data_counter++) = g_uc_sd_data_buffer[l_us_counter_i];
			break;

		//Fill global checksum register
		case DATA_OBDH_G_US_CHECKSUM_REGISTER:
			l_us_data_counter = insertShortToArray(g_us_checksum_register,l_uc_array_p,l_us_data_counter);
			break;

		//Fill satellite time
		case DATA_CAT_SAT_TIME:
			//Calculate current satellite time from str_obdh_data.ul_obc_time_s
			//Lock OBC time before being used
			g_ul = str_obdh_data.ul_obc_time_s + SECONDS_FROM_TAI_TO_KILL_SWITCH_DEACTIVATION;

			//Get satellite day
			str_sat_time.us_day = g_ul/86400;
			g_ul %= 86400;

			//Get satellite hour
			str_sat_time.uc_hour = g_ul/3600;
			g_ul %= 3600;

			//Set satellite minute
			str_sat_time.uc_min = g_ul/60;
			g_ul %= 60;

			//Set satellite seconds
			str_sat_time.uc_s = g_ul;

			//Set satellite miliseconds as zero for now
			str_sat_time.us_ms = 0;

			l_us_data_counter = insertShortToArray(str_sat_time.us_day,l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = str_sat_time.uc_hour; 
			*(l_uc_array_p + l_us_data_counter++) = str_sat_time.uc_min; 
			*(l_uc_array_p + l_us_data_counter++) = str_sat_time.uc_s; 
			l_us_data_counter = insertShortToArray(str_sat_time.us_ms,l_uc_array_p,l_us_data_counter);
			break;

		//Fill satellite current date
		case DATA_CAT_SAT_DATE_CURRENT:
			//The order in the struct is consistent with hardware specs
			//The order here is as the user/programmer would be more interested to see
			*(l_uc_array_p + l_us_data_counter++) = str_sat_date.uc_current_day_of_month; 
			*(l_uc_array_p + l_us_data_counter++) = str_sat_date.uc_current_month; 
			*(l_uc_array_p + l_us_data_counter++) = str_sat_date.uc_current_year; 
			*(l_uc_array_p + l_us_data_counter++) = str_sat_date.uc_current_hour; 
			*(l_uc_array_p + l_us_data_counter++) = str_sat_date.uc_current_min; 
			*(l_uc_array_p + l_us_data_counter++) = str_sat_date.uc_current_s; 
			l_us_data_counter = insertShortToArray(str_sat_date.us_current_ms,l_uc_array_p,l_us_data_counter);
			break;

		//Fill satellite reference date
		case DATA_CAT_SAT_DATE_INIT:
			*(l_uc_array_p + l_us_data_counter++) = str_sat_date.uc_init_day_of_month; 
			*(l_uc_array_p + l_us_data_counter++) = str_sat_date.uc_init_month; 
			*(l_uc_array_p + l_us_data_counter++) = str_sat_date.uc_init_year; 
			*(l_uc_array_p + l_us_data_counter++) = str_sat_date.uc_init_hour; 
			*(l_uc_array_p + l_us_data_counter++) = str_sat_date.uc_init_min; 
			*(l_uc_array_p + l_us_data_counter++) = str_sat_date.uc_init_s; 
			l_us_data_counter = insertShortToArray(str_sat_date.us_init_ms,l_uc_array_p,l_us_data_counter);
			break;

		//Fill satellite update date
		case DATA_CAT_SAT_DATE_UPDATE:
			*(l_uc_array_p + l_us_data_counter++) = str_date_update.uc_day; 
			*(l_uc_array_p + l_us_data_counter++) = str_date_update.uc_month; 
			*(l_uc_array_p + l_us_data_counter++) = str_date_update.uc_year; 
			*(l_uc_array_p + l_us_data_counter++) = str_date_update.uc_hour; 
			*(l_uc_array_p + l_us_data_counter++) = str_date_update.uc_min; 
			*(l_uc_array_p + l_us_data_counter++) = str_date_update.uc_s; 
			l_us_data_counter = insertShortToArray(str_date_update.us_ms,l_uc_array_p,l_us_data_counter);
			break;

		//Fill OBDH firmware version
		case DATA_CAT_OBDH_FIRMWARE_VERSION:
			for (l_us_counter_i = 0; l_us_counter_i < OBDH_FIRMWARE_VERSION_LENGTH; l_us_counter_i++)
				*(l_uc_array_p + l_us_data_counter++) = g_uc_obdh_firmware_version[l_us_counter_i];
			break;

		//Fill with latest from to callsign
		case DATA_CAT_FROM_TO_CALLSIGN:
			for (l_us_counter_i = 0; l_us_counter_i < AX25_CALLSIGN_SIZE; l_us_counter_i++)
				*(l_uc_array_p + l_us_data_counter++) = g_uc_from_to_callsign[l_us_counter_i];
			break;

		//Fill with OBDH selected HK data
		case DATA_CAT_OBDH_SELECTED_HK:
			*(l_uc_array_p + l_us_data_counter++) = str_obdh_hk.uc_sat_mode;
			l_us_data_counter = insertLongToArray(str_obdh_hk.ul_wod_sample_count,l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = str_obdh_hk.uc_scheduler_block_size;
			l_us_data_counter = insertShortToArray(str_obdh_hk.us_beacon_flag,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(str_obdh_hk.us_cw_char_rate_par,l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = str_obdh_hk.uc_ax25_bit_rate_par;
			*(l_uc_array_p + l_us_data_counter++) = str_obdh_hk.uc_mcu_rst_count;
			l_us_data_counter = insertLongToArray(str_obdh_hk.ul_data_byte_sent,l_uc_array_p,l_us_data_counter);
			break;

		//Fill with OBDH selected data
		case DATA_CAT_OBDH_SELECTED_DATA:
			//Lock rev offset time before being used
			g_us = str_obdh_data.us_rev_offset_time;

			//Lock auto reference no before being used
			l_us_dummy_var = str_obdh_data.us_auto_reference_rev_no;

			*(l_uc_array_p + l_us_data_counter++) = str_obdh_data.uc_sat_rstsrc;
			l_us_data_counter = insertLongToArray(str_obdh_data.ul_obc_time_s,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(str_obdh_data.us_current_rev_no,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(g_us,l_uc_array_p,l_us_data_counter);
			for (l_us_counter_i = 0; l_us_counter_i < OBDH_DATA_INTERVAL_PERIOD_SIZE; l_us_counter_i++)
				l_us_data_counter = insertShortToArray(str_obdh_data.us_interval_period[l_us_counter_i],l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = str_obdh_data.uc_previous_mode;
			l_us_data_counter = insertFloatToArray(str_obdh_data.f_gs_reference_rev_no,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertLongToArray(str_obdh_data.ul_gs_ref_time_s,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(l_us_dummy_var,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(str_obdh_data.us_gs_reference_rev_no,l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = str_obdh_data.uc_orbit_no_changed_counter;			
			break;

		//Fill OBDH latest state
		case DATA_CAT_SAT_STATE:
			*(l_uc_array_p + l_us_data_counter++) = g_uc_evha_flag;			
			break;

		//Fill with OBDH housekeeping data
		case DATA_CAT_OBDH_HK:
			*(l_uc_array_p + l_us_data_counter++) = str_obdh_hk.uc_sat_mode;
			*(l_uc_array_p + l_us_data_counter++) = str_obdh_hk.uc_gsch_state;
			l_us_data_counter = insertLongToArray(str_obdh_hk.ul_wod_sample_count,l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = str_obdh_hk.uc_scheduler_block_size;
			l_us_data_counter = insertShortToArray(str_obdh_hk.us_beacon_flag,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(str_obdh_hk.us_brh_flag,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(str_obdh_hk.us_imh_flag,l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = str_obdh_hk.uc_smh_flag;
			l_us_data_counter = insertShortToArray(str_obdh_hk.us_cw_char_rate_par,l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = str_obdh_hk.uc_ax25_bit_rate_par;
			*(l_uc_array_p + l_us_data_counter++) = str_obdh_hk.uc_mcu_rstsrc;
			*(l_uc_array_p + l_us_data_counter++) = str_obdh_hk.uc_mcu_rst_count;
			l_us_data_counter = insertShortToArray(str_obdh_hk.s_mcu_temp,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertLongToArray(str_obdh_hk.ul_data_byte_sent,l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = str_obdh_hk.uc_sat_rstsrc;
			break;

		//Fill OBDH SSCP
		case DATA_CAT_OBDH_SSCP:
			*(l_uc_array_p + l_us_data_counter++) = str_obdh_par.uc_manual_control_enabled;
			*(l_uc_array_p + l_us_data_counter++) = str_obdh_par.uc_no_of_interrupt_per_s;
			*(l_uc_array_p + l_us_data_counter++) = str_obdh_par.uc_gsch_time_window_10s;
			for (l_us_counter_i = 0; l_us_counter_i < NO_OF_SOC_THRESHOLD_PAR; l_us_counter_i++)			
				*(l_uc_array_p + l_us_data_counter++) = str_obdh_par.uc_soc_threshold[l_us_counter_i];
			l_us_data_counter = insertShortToArray(str_obdh_par.us_rev_time_s,l_uc_array_p,l_us_data_counter);
			for (l_us_counter_i = 0; l_us_counter_i < MAX_NO_OF_DATA_TYPE_LOGGED; l_us_counter_i++)
				l_us_data_counter = insertShortToArray(str_obdh_par.us_log_data_id[l_us_counter_i],l_uc_array_p,l_us_data_counter);
			for (l_us_counter_i = 0; l_us_counter_i < 3; l_us_counter_i++)
				l_us_data_counter = insertShortToArray(str_obdh_par.us_br_threshold_low[l_us_counter_i],l_uc_array_p,l_us_data_counter);
			for (l_us_counter_i = 0; l_us_counter_i < 3; l_us_counter_i++)
				l_us_data_counter = insertShortToArray(str_obdh_par.us_br_threshold_high[l_us_counter_i],l_uc_array_p,l_us_data_counter);
			for (l_us_counter_i = 0; l_us_counter_i < NO_OF_BEACON_INTERVAL; l_us_counter_i++)
				*(l_uc_array_p + l_us_data_counter++) = str_obdh_par.uc_beacon_interval[l_us_counter_i];
			l_us_data_counter = insertLongToArray(str_obdh_par.ul_det_uptime_limit_s,l_uc_array_p,l_us_data_counter);
			break;

		//Fill OBDH block pointers
		case DATA_CAT_OBDH_BP:
			l_us_data_counter = insertLongToArray(str_bp.ul_hk_write_p,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertLongToArray(str_bp.ul_datalog_write_p,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertLongToArray(str_bp.ul_package_recovery_write_p,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertLongToArray(str_bp.ul_eventlog_write_p,l_uc_array_p,l_us_data_counter);//dec22_yc
			*(l_uc_array_p + l_us_data_counter++) = str_bp.uc_hk_overwritten;
			*(l_uc_array_p + l_us_data_counter++) = str_bp.uc_orbit_indexing_overwritten;
			break;

		//Fill COMM complete HK
		case DATA_CAT_COMM_HK:
			l_us_data_counter = insertShortToArray(str_imc_hk.us_doppler_offset,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(str_imc_hk.us_receiver_signal_strength,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(str_imc_hk.us_transmitter_reflected_power,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(str_imc_hk.us_transmitter_forward_power,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(str_imc_hk.us_transmitter_current_consumption,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(str_imc_hk.us_receiver_current_consumption,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(str_imc_hk.us_power_amplifier_temperature,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(str_imc_hk.us_power_bus_voltage,l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = str_itc_hk.uc_transmitter_state;
			break;

		//Fill with the remaining COMM data (Non-HK)
		case DATA_CAT_COMM_DATA:
			*(l_uc_array_p + l_us_data_counter++) = str_imc_hk.uc_receiver_no_of_frame;
			*(l_uc_array_p + l_us_data_counter++) = str_imc_hk.uc_receiver_uptime_minutes;
			*(l_uc_array_p + l_us_data_counter++) = str_imc_hk.uc_receiver_uptime_hours;
			*(l_uc_array_p + l_us_data_counter++) = str_imc_hk.uc_receiver_uptime_days;
			*(l_uc_array_p + l_us_data_counter++) = str_itc_hk.uc_transmitter_uptime_minutes;
			*(l_uc_array_p + l_us_data_counter++) = str_itc_hk.uc_transmitter_uptime_hours;
			*(l_uc_array_p + l_us_data_counter++) = str_itc_hk.uc_transmitter_uptime_days;
			*(l_uc_array_p + l_us_data_counter++) = str_obdh_hk.uc_ax25_bit_rate_par;
			l_us_data_counter = insertShortToArray(str_obdh_hk.us_cw_char_rate_par,l_uc_array_p,l_us_data_counter);
			break;

		#endif

		//Fill PWRS complete HK
		case DATA_CAT_PWRS_HK:
			for (l_us_counter_i = 0; l_us_counter_i < 5; l_us_counter_i++)
				*(l_uc_array_p + l_us_data_counter++) = str_pwrs_hk.uc_sp_temp[l_us_counter_i];
			for (l_us_counter_i = 0; l_us_counter_i < 3; l_us_counter_i++)
				l_us_data_counter = insertShortToArray(str_pwrs_hk.us_pv_v[l_us_counter_i],l_uc_array_p,l_us_data_counter);
			for (l_us_counter_i = 0; l_us_counter_i < 3; l_us_counter_i++)
				l_us_data_counter = insertShortToArray(str_pwrs_hk.us_pv_i[l_us_counter_i],l_uc_array_p,l_us_data_counter);
			for (l_us_counter_i = 0; l_us_counter_i < 2; l_us_counter_i++)
				l_us_data_counter = insertShortToArray(str_pwrs_hk.us_batt_v[l_us_counter_i],l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(str_pwrs_hk.us_batt_i,l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = str_pwrs_hk.uc_batt_soc;
			*(l_uc_array_p + l_us_data_counter++) = str_pwrs_hk.uc_batt_temp[0];
			*(l_uc_array_p + l_us_data_counter++) = str_pwrs_hk.uc_batt_temp[1];
			l_us_data_counter = insertShortToArray(str_pwrs_hk.us_sys_i,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(str_pwrs_hk.us_pcc_v,l_uc_array_p,l_us_data_counter);
			for (l_us_counter_i = 0; l_us_counter_i < 4; l_us_counter_i++)
				l_us_data_counter = insertShortToArray(str_pwrs_hk.us_load_v[l_us_counter_i],l_uc_array_p,l_us_data_counter);
			for (l_us_counter_i = 0; l_us_counter_i < 4; l_us_counter_i++)
				l_us_data_counter = insertShortToArray(str_pwrs_hk.us_load_i[l_us_counter_i],l_uc_array_p,l_us_data_counter);
			for (l_us_counter_i = 0; l_us_counter_i < 4; l_us_counter_i++)
				*(l_uc_array_p + l_us_data_counter++) = str_pwrs_hk.uc_latch_count[l_us_counter_i];
			*(l_uc_array_p + l_us_data_counter++) = str_pwrs_hk.uc_rst_count;
			*(l_uc_array_p + l_us_data_counter++) = str_pwrs_hk.uc_rst_cause;
			*(l_uc_array_p + l_us_data_counter++) = str_pwrs_hk.uc_mode;
			l_us_data_counter = insertShortToArray(str_pwrs_hk.s_mcu_temp,l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = str_pwrs_hk.c_mcu_rstsrc;
			break;

		//Fill PWRS complete SSCP
		case DATA_CAT_PWRS_SSCP:
			*(l_uc_array_p + l_us_data_counter++) = str_pwrs_par.uc_mppt_mode;
			*(l_uc_array_p + l_us_data_counter++) = str_pwrs_par.uc_batt_heater;
			*(l_uc_array_p + l_us_data_counter++) = str_pwrs_par.uc_batt_charge_upp_limit;
			*(l_uc_array_p + l_us_data_counter++) = str_pwrs_par.uc_batt_charge_low_limit;
			break;

		//Fill PWRS complete data
		case DATA_CAT_PWRS_DATA:
			*(l_uc_array_p + l_us_data_counter++) = str_pwrs_data.uc_channel_status;		
			for (l_us_counter_i = 0; l_us_counter_i < 3; l_us_counter_i++)
				l_us_data_counter = insertShortToArray(str_pwrs_data.us_pv_fv[l_us_counter_i],l_uc_array_p,l_us_data_counter);
			break;

		//Fill ADCS complete HK
		case DATA_CAT_ADCS_HK:
			for (l_us_counter_i = 0; l_us_counter_i < 3; l_us_counter_i++)
				l_us_data_counter = insertFloatToArray(str_adcs_hk.f_sunv[l_us_counter_i],l_uc_array_p,l_us_data_counter);
			for (l_us_counter_i = 0; l_us_counter_i < 4; l_us_counter_i++)
				l_us_data_counter = insertFloatToArray(str_adcs_hk.f_qeb[l_us_counter_i],l_uc_array_p,l_us_data_counter);
			for (l_us_counter_i = 0; l_us_counter_i < 3; l_us_counter_i++)
				l_us_data_counter = insertShortToArray(str_adcs_hk.s_br_eci[l_us_counter_i],l_uc_array_p,l_us_data_counter);
			for (l_us_counter_i = 0; l_us_counter_i < 3; l_us_counter_i++)
				l_us_data_counter = insertFloatToArray(str_adcs_hk.f_posv[l_us_counter_i],l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = str_adcs_hk.c_sensor_enable;
			*(l_uc_array_p + l_us_data_counter++) = str_adcs_hk.c_sensor_credit;
			for (l_us_counter_i = 0; l_us_counter_i < 3; l_us_counter_i++)
				*(l_uc_array_p + l_us_data_counter++) = str_adcs_hk.c_ac_m[l_us_counter_i];
			for (l_us_counter_i = 0; l_us_counter_i < 3; l_us_counter_i++)
				*(l_uc_array_p + l_us_data_counter++) = str_adcs_hk.c_mt_i[l_us_counter_i];
			*(l_uc_array_p + l_us_data_counter++) = str_adcs_hk.c_actuator_enable;
			*(l_uc_array_p + l_us_data_counter++) = str_adcs_hk.c_mode;
			l_us_data_counter = insertShortToArray(str_adcs_hk.s_mcu_temp,l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = str_adcs_hk.c_mcu_rstsrc;
			break;

		//Fill ADCS complete SSCP
		case DATA_CAT_ADCS_SSCP:
			l_us_data_counter = insertShortToArray(str_adcs_par.s_ss_threshold,l_uc_array_p,l_us_data_counter);
			for (l_us_counter_i = 0; l_us_counter_i < NO_OF_ADCS_CGAIN; l_us_counter_i++)
				l_us_data_counter = insertFloatToArray(str_adcs_par.f_cgain[l_us_counter_i],l_uc_array_p,l_us_data_counter);	
			l_us_data_counter = insertFloatToArray(str_adcs_par.f_tle_epoch,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertFloatToArray(str_adcs_par.f_tle_inc,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertFloatToArray(str_adcs_par.f_tle_raan,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertFloatToArray(str_adcs_par.f_tle_arg_per,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertFloatToArray(str_adcs_par.f_tle_ecc,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertFloatToArray(str_adcs_par.f_tle_ma,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(str_adcs_par.us_tle_mm,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertLongToArray(str_adcs_par.ul_time,l_uc_array_p,l_us_data_counter);
			for (l_us_counter_i = 0; l_us_counter_i < 3; l_us_counter_i++)
				l_us_data_counter = insertShortToArray(str_adcs_par.s_vector_2sun[l_us_counter_i],l_uc_array_p,l_us_data_counter);
			for (l_us_counter_i = 0; l_us_counter_i < 3; l_us_counter_i++)
				*(l_uc_array_p + l_us_data_counter++) = str_adcs_par.c_ctrl_br[l_us_counter_i];
			l_us_data_counter = insertShortToArray(str_adcs_par.s_dgain,l_uc_array_p,l_us_data_counter);
			break;

		#if (FULL_ADD_ENABLE == 1)
		
		//Fill with last housekeeping error codes
		case DATA_CAT_LAST_HK_EC:
			l_us_data_counter = insertShortToArray(g_us_latest_pwrs_hk_ec,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(g_us_latest_adcs_hk_ec,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(g_us_latest_comm_hk_ec,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(g_us_latest_obdh_hk_ec,l_uc_array_p,l_us_data_counter);
			break;

		//Fill with last SSCP error codes
		case DATA_CAT_LAST_SSCP_EC:
			l_us_data_counter = insertShortToArray(g_us_latest_pwrs_sscp_ec,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(g_us_latest_adcs_sscp_ec,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(g_us_latest_obdh_sscp_ec,l_uc_array_p,l_us_data_counter);
			break;

		//Fill task status (with argument)
		case DATA_CAT_SINGLE_TASK_UC_STATUS:
			*(l_uc_array_p + l_us_data_counter++) = str_task[l_us_fill_argument].uc_status;
			break;

		//Fill task structure (single)
		case DATA_CAT_SINGLE_TASK_STRUCTURE:
			*(l_uc_array_p + l_us_data_counter++) = str_task[l_us_fill_argument].uc_status;
			*(l_uc_array_p + l_us_data_counter++) = str_task[l_us_fill_argument].uc_scriptspace;
			*(l_uc_array_p + l_us_data_counter++) = str_task[l_us_fill_argument].uc_script_running_flags;
			*(l_uc_array_p + l_us_data_counter++) = str_task[l_us_fill_argument].uc_script_error_flags;
			*(l_uc_array_p + l_us_data_counter++) = str_task[l_us_fill_argument].uc_script_completed_flags;
			*(l_uc_array_p + l_us_data_counter++) = str_task[l_us_fill_argument].uc_script_timer_owning_flags;
			*(l_uc_array_p + l_us_data_counter++) = str_task[l_us_fill_argument].uc_script_timer_running_flags;
			*(l_uc_array_p + l_us_data_counter++) = str_task[l_us_fill_argument].uc_script_retry_counter_owning_flags;
			break;

		//Fill scriptspace (with argument)
		//This case could be invoked from PC by a debugging software as some other cases
		case DATA_CAT_SINGLE_SCRIPTSPACE:
			for (l_us_counter_i = 0; l_us_counter_i < MAX_SCRIPTLINE_SIZE; l_us_counter_i++)
				for (l_us_counter_j = 0; l_us_counter_j < MAX_SCRIPT_LENGTH; l_us_counter_j++)
				*(l_uc_array_p + l_us_data_counter++) = str_scriptspace[l_us_fill_argument].uc_script[l_us_counter_i][l_us_counter_j];
			break;

		//Fill scriptline timer
		case DATA_CAT_SCRIPTLINE_TIMER:
			*(l_uc_array_p + l_us_data_counter++) = str_scriptline_timer[l_us_fill_argument].uc_scriptspace_owner;
			*(l_uc_array_p + l_us_data_counter++) = str_scriptline_timer[l_us_fill_argument].uc_scriptline_owner;
			*(l_uc_array_p + l_us_data_counter++) = str_scriptline_timer[l_us_fill_argument].uc_started;
			*(l_uc_array_p + l_us_data_counter++) = str_scriptline_timer[l_us_fill_argument].uc_counter;
			l_us_data_counter = insertLongToArray(str_scriptline_timer[l_us_fill_argument].ul_timer,l_uc_array_p,l_us_data_counter);
			break;

		//Fill task retry counter
		case DATA_CAT_SCRIPTLINE_RETRY_COUNTER:
			*(l_uc_array_p + l_us_data_counter++) = str_scriptline_retry_counter[l_us_fill_argument].uc_scriptspace_owner;
			*(l_uc_array_p + l_us_data_counter++) = str_scriptline_retry_counter[l_us_fill_argument].uc_scriptline_owner;
			*(l_uc_array_p + l_us_data_counter++) = str_scriptline_retry_counter[l_us_fill_argument].uc_counter;
			break;

		//Fill schedule (with index)
		case DATA_CAT_SCHEDULE:
			*(l_uc_array_p + l_us_data_counter++) = str_obdh_hk.uc_scheduler_block_size;
			for (l_us_counter_j = 0; l_us_counter_j < MAX_SCHEDULER_ARG; l_us_counter_j++)
				*(l_uc_array_p + l_us_data_counter++) = str_scheduler.uc_arg_register[l_us_counter_j];
	
			//Magic constant 10 is changed to NO_OF_SCHEDULE_PER_DATA_CATEGORY (20120328)
			for (l_us_counter_i = (l_us_fill_argument-1)*NO_OF_SCHEDULE_PER_DATA_CATEGORY; l_us_counter_i < l_us_fill_argument*NO_OF_SCHEDULE_PER_DATA_CATEGORY; l_us_counter_i++){
				*(l_uc_array_p + l_us_data_counter++) = str_scheduler.uc_reindexing[l_us_counter_i];
				l_us_data_counter = insertLongToArray(str_scheduler.ul_scheduled_time[l_us_counter_i],l_uc_array_p,l_us_data_counter);
				l_us_data_counter = insertShortToArray(str_scheduler.us_cmd[l_us_counter_i],l_uc_array_p,l_us_data_counter);
				*(l_uc_array_p + l_us_data_counter++) = str_scheduler.uc_arg_length[l_us_counter_i];
				for (l_us_counter_j = 0; l_us_counter_j < MAX_SCHEDULER_ARG; l_us_counter_j++)
					*(l_uc_array_p + l_us_data_counter++) = str_scheduler.uc_arg[l_us_counter_i][l_us_counter_j];
			}
			break;

		//Fill schedule (fix)
		//Schedule parameter
		case DATA_CAT_SCHEDULE_PAR:
			*(l_uc_array_p + l_us_data_counter++) = str_obdh_hk.uc_scheduler_block_size;
			break;

		//Schedule 1 to 70
		case DATA_CAT_SCHEDULE_1_TO_10:
		case DATA_CAT_SCHEDULE_11_TO_20:
		case DATA_CAT_SCHEDULE_21_TO_30:
		case DATA_CAT_SCHEDULE_31_TO_40:
		case DATA_CAT_SCHEDULE_41_TO_50:
		case DATA_CAT_SCHEDULE_51_TO_60:
		case DATA_CAT_SCHEDULE_61_TO_70:
			for (l_us_counter_i = (l_us_data_type-DATA_CAT_SCHEDULE_1_TO_10)*10; l_us_counter_i < (l_us_data_type-DATA_CAT_SCHEDULE_1_TO_10+1)*10; l_us_counter_i++){
				*(l_uc_array_p + l_us_data_counter++) = str_scheduler.uc_reindexing[l_us_counter_i];
				l_us_data_counter = insertLongToArray(str_scheduler.ul_scheduled_time[l_us_counter_i],l_uc_array_p,l_us_data_counter);
				l_us_data_counter = insertShortToArray(str_scheduler.us_cmd[l_us_counter_i],l_uc_array_p,l_us_data_counter);
				*(l_uc_array_p + l_us_data_counter++) = str_scheduler.uc_arg_length[l_us_counter_i];
				for (l_us_counter_j = 0; l_us_counter_j < MAX_SCHEDULER_ARG; l_us_counter_j++)
					*(l_uc_array_p + l_us_data_counter++) = str_scheduler.uc_arg[l_us_counter_i][l_us_counter_j];
			}
			break;

		//Fill scriptline (fix)
		//Scripspace parameter
		case DATA_CAT_SCRIPTSPACE_PAR:
			for (l_us_counter_i = 0; l_us_counter_i < TASK_SIZE; l_us_counter_i++)
				*(l_uc_array_p + l_us_data_counter++) = str_task[l_us_counter_i].uc_scriptspace;
			break;

		//Scriptspace 1 to 10
		case DATA_CAT_SCRIPTSPACE_1:
		case DATA_CAT_SCRIPTSPACE_2:
		case DATA_CAT_SCRIPTSPACE_3:
		case DATA_CAT_SCRIPTSPACE_4:
		case DATA_CAT_SCRIPTSPACE_5:
		case DATA_CAT_SCRIPTSPACE_6:
		case DATA_CAT_SCRIPTSPACE_7:
		case DATA_CAT_SCRIPTSPACE_8:
		case DATA_CAT_SCRIPTSPACE_9:
		case DATA_CAT_SCRIPTSPACE_10:
			for (l_us_counter_i = 0; l_us_counter_i < MAX_SCRIPTLINE_SIZE; l_us_counter_i++)
				for (l_us_counter_j = 0; l_us_counter_j < MAX_SCRIPT_LENGTH; l_us_counter_j++)
					*(l_uc_array_p + l_us_data_counter++) = str_scriptspace[l_us_data_type-DATA_CAT_SCRIPTSPACE_1].uc_script[l_us_counter_i][l_us_counter_j];
			break;

		//Fill task structure (complete)
		case DATA_CAT_COMPLETE_TASK_STRUCTURE:
			for (l_us_counter_i = 0; l_us_counter_i < TASK_SIZE; l_us_counter_i++){
				*(l_uc_array_p + l_us_data_counter++) = str_task[l_us_counter_i].uc_status;
				*(l_uc_array_p + l_us_data_counter++) = str_task[l_us_counter_i].uc_scriptspace;
				*(l_uc_array_p + l_us_data_counter++) = str_task[l_us_counter_i].uc_script_running_flags;
				*(l_uc_array_p + l_us_data_counter++) = str_task[l_us_counter_i].uc_script_error_flags;
				*(l_uc_array_p + l_us_data_counter++) = str_task[l_us_counter_i].uc_script_completed_flags;
				*(l_uc_array_p + l_us_data_counter++) = str_task[l_us_counter_i].uc_script_timer_owning_flags;
				*(l_uc_array_p + l_us_data_counter++) = str_task[l_us_counter_i].uc_script_timer_running_flags;
				*(l_uc_array_p + l_us_data_counter++) = str_task[l_us_counter_i].uc_script_retry_counter_owning_flags;
			}
			break;

		//Fill SD header
		case DATA_CAT_SD_HEADER:
			//Lock OBC time before being written
			g_ul = (str_obdh_data.ul_obc_time_s + SECONDS_FROM_TAI_TO_KILL_SWITCH_DEACTIVATION);
			*(l_uc_array_p + l_us_data_counter++) = g_uc_sd_header_subsystem;
			*(l_uc_array_p + l_us_data_counter++) = g_uc_sd_header_data_type;
			l_us_data_counter = insertShortToArray(g_us_sd_header_block_property,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(g_us_sd_header_no_of_this_block,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray((g_us_sd_header_this_block_data_size-1),l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertLongToArray(g_ul_initial_sd_card_bp,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertLongToArray(g_ul,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(str_obdh_data.us_current_rev_no,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertLongToArray(g_ul_sd_header_sd_write_block,l_uc_array_p,l_us_data_counter);
			break;

		//Fill AX.25 header
		case DATA_CAT_AX25_HEADER:
			*(l_uc_array_p + l_us_data_counter++) = g_uc_ax25_header_subsystem;
			*(l_uc_array_p + l_us_data_counter++) = g_uc_ax25_header_data_type;
			*(l_uc_array_p + l_us_data_counter++) = g_uc_ax25_header_package_property;
			*(l_uc_array_p + l_us_data_counter++) = g_uc_ax25_header_no_of_this_package;
			*(l_uc_array_p + l_us_data_counter++) = g_uc_ax25_header_this_package_size;
			break;

		//Fill data storing variables
		case DATA_CAT_STORING_VAR:
			*(l_uc_array_p + l_us_data_counter++) = g_uc_no_of_data_saved;
			for (l_us_counter_i = 0; l_us_counter_i < MAX_NO_OF_DATA_TYPE_TO_BE_SAVED; l_us_counter_i++)
				*(l_uc_array_p + l_us_data_counter++) = g_uc_data_type_saved_buffer[l_us_counter_i];
			l_us_data_counter = insertLongToArray(g_ul_initial_sd_card_bp,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertLongToArray((*g_ul_initial_sd_card_bp_address),l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = g_uc_no_of_copies;
			*(l_uc_array_p + l_us_data_counter++) = (*g_uc_no_of_overwritten_p);
			*(l_uc_array_p + l_us_data_counter++) = g_uc_data_sets_property;
			l_us_data_counter = insertLongToArray(g_ul_memory_address_lower_limit,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertLongToArray(g_ul_memory_address_upper_limit,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertLongToArray(g_ul_memory_address_end,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertLongToArray(g_ul_end_sd_card_load_bp,l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = g_uc_sd_load_data_timeout_counter;
			l_us_data_counter = insertShortToArray(g_us_sd_load_error_code,l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = g_uc_save_data_hold_timeout_counter;
			break;

		//Fill data sending variables
		case DATA_CAT_SENDING_VAR:
			*(l_uc_array_p + l_us_data_counter++) = g_uc_no_of_data_sent;
			for (l_us_counter_i = 0; l_us_counter_i < MAX_NO_OF_DATA_TYPE_TO_BE_SENT; l_us_counter_i++)
				*(l_uc_array_p + l_us_data_counter++) = g_uc_data_type_sent_buffer[l_us_counter_i];
			l_us_data_counter = insertShortToArray(g_us_sd_card_meaningful_data_size,l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = g_uc_current_data_sent_counter;
			l_us_data_counter = insertShortToArray(g_us_beacon_send_error_code,l_uc_array_p,l_us_data_counter);
			break;

		//Fill OBS variables
		case DATA_CAT_OBS_VAR:
			l_us_data_counter = insertLongToArray(g_ul_obs_initial_bp,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertLongToArray(g_ul_obs_end_bp,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(g_us_obs_first_orbit_request,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(g_us_obs_last_orbit_request,l_uc_array_p,l_us_data_counter);
			break;

		//Fill with complete beacon handler variables
		case DATA_CAT_COMPLETE_BEACON_DATA_VAR:
			l_us_data_counter = insertShortToArray(str_obdh_hk.us_beacon_flag,l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = g_uc_beacon_time_since_last_timeout_sending;
			*(l_uc_array_p + l_us_data_counter++) = g_uc_beacon_timeout_value;
			*(l_uc_array_p + l_us_data_counter++) = g_uc_beacon_retry_counter;
			*(l_uc_array_p + l_us_data_counter++) = g_uc_beacon_timeout_retry_counter;
			l_us_data_counter = insertShortToArray(g_us_beacon_init_ax25_error_code,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(g_us_beacon_monitoring_time,l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = g_uc_beacon_error_counter;
			for (l_us_counter_i = 0; l_us_counter_i < NO_OF_BEACON_INTERVAL; l_us_counter_i++)
				*(l_uc_array_p + l_us_data_counter++) = str_obdh_par.uc_beacon_interval[l_us_counter_i];
			break;

		#if (BODY_RATE_HANDLER_ENABLE == 1)
		#if (ADCS_CMD_ENABLE == 1)
		#if (PWRS_CMD_ENABLE == 1)
		//Fill with complete bodyRateHandler variables
		case DATA_CAT_COMPLETE_BRH_DATA_VAR:
			l_us_data_counter = insertShortToArray(str_obdh_hk.us_brh_flag,l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = g_uc_brh_retry_counter;
			*(l_uc_array_p + l_us_data_counter++) = g_uc_brh_timeout_retry_counter;
			*(l_uc_array_p + l_us_data_counter++) = g_uc_brh_wait_det_time_s;
			l_us_data_counter = insertLongToArray(g_ul_det_uptime_s,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertLongToArray(str_obdh_par.ul_det_uptime_limit_s,l_uc_array_p,l_us_data_counter);
			for (l_us_counter_i = 0; l_us_counter_i < 3; l_us_counter_i++)
				l_us_data_counter = insertShortToArray(str_obdh_data.us_br_eci_avg[l_us_counter_i],l_uc_array_p,l_us_data_counter);
			for (l_us_counter_i = 0; l_us_counter_i < 3; l_us_counter_i++)
				for (l_us_counter_j = 0; l_us_counter_j < BR_ECI_SAMPLE_NO; l_us_counter_j++)
					l_us_data_counter = insertShortToArray(g_s_br_eci[l_us_counter_i][l_us_counter_j],l_uc_array_p,l_us_data_counter);
			break;
		#endif
		#endif
		#endif

		#if (IDLE_MODE_HANDLER_ENABLE == 1)
		#if (PWRS_CMD_ENABLE == 1)
		//Fill with complete idleModeHandler variables
		case DATA_CAT_COMPLETE_IMH_DATA_VAR:
			l_us_data_counter = insertShortToArray(str_obdh_hk.us_imh_flag,l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = g_uc_imh_retry_counter;
			*(l_uc_array_p + l_us_data_counter++) = g_uc_imh_acs_timeout_retry_counter;
			*(l_uc_array_p + l_us_data_counter++) = g_uc_imh_ads_timeout_retry_counter;
			*(l_uc_array_p + l_us_data_counter++) = g_uc_imh_wait_acs_time_s;
			*(l_uc_array_p + l_us_data_counter++) = g_uc_imh_wait_ads_time_s;
			l_us_data_counter = insertLongToArray(g_ul_idle_time_register,l_uc_array_p,l_us_data_counter);
			break;
		#endif
		#endif

		#if (SAFE_MODE_HANDLER_ENABLE == 1)
		#if (ADCS_CMD_ENABLE == 1)
		#if (PWRS_CMD_ENABLE == 1)
		//Fill with complete safeModeHandler variables
		case DATA_CAT_COMPLETE_SMH_DATA_VAR:
			*(l_uc_array_p + l_us_data_counter++) = str_obdh_hk.uc_smh_flag;
			l_us_data_counter = insertLongToArray(g_ul_smh_time_register,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertLongToArray(g_ul_smh_saved_time,l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = g_uc_soc_reading_when_entering_safe_mode;
			*(l_uc_array_p + l_us_data_counter++) = g_uc_soc_after_one_orbit_in_safe_mode;
			break;
		#endif
		#endif
		#endif

		//Fill hold activation variables
		case DATA_CAT_HOLD_ACTIVATION:
			*(l_uc_array_p + l_us_data_counter++) = str_obdh_data.uc_software_mode;
			break;

		//Fill complete AX25 frame buffer
		case DATA_CAT_COMPLETE_AX25_FRAME_BUFFER:
			for (l_us_counter_i = 0; l_us_counter_i < AX25_BUFFER_SIZE; l_us_counter_i++)
				*(l_uc_array_p + l_us_data_counter++) = g_uc_ax25_message[l_us_counter_i];
			break;

		//Fill with single data from a subsystem
		case DATA_CAT_SINGLE_SUBSYSTEM_DATA:
			for (l_us_counter_i = 0; l_us_counter_i < 4; l_us_counter_i++)
				*(l_uc_array_p + l_us_data_counter++) = g_uc_i2c_data_in[l_us_counter_i];
			break;

		//Fill with acknowledgement message
		case DATA_CAT_ACK_MSG:
			*(l_uc_array_p + l_us_data_counter++) = GSCH_ACK_CHAR;
			l_us_data_counter = insertShortToArray(g_us_gsch_gs_cmd_crc,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(g_us_gsch_error_code,l_uc_array_p,l_us_data_counter);
			break;

		//Fill with error codes of scriptline items
		case DATA_CAT_SCRIPTLINE_ITEM_ERROR_CODES:
			l_us_data_counter = insertShortToArray(g_us_scriptline_timer_error_code,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(g_us_scriptline_retry_counter_error_code,l_uc_array_p,l_us_data_counter);
			break;

		//Fill with GSCH variables
		case DATA_CAT_GSCH_VAR:
			*(l_uc_array_p + l_us_data_counter++) = str_obdh_hk.uc_gsch_state;
			l_us_data_counter = insertShortToArray(g_us_gsch_error_code,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(g_us_gsch_command_validation_error_code,l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = g_uc_gsch_comm_retry_counter;
			*(l_uc_array_p + l_us_data_counter++) = g_uc_gsch_loading_retry_counter;
			l_us_data_counter = insertShortToArray(g_us_gsch_monitoring_transitional_period,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(g_us_gsch_monitoring_period,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(g_us_gsch_key,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(g_us_gsch_answer_1,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(g_us_gsch_answer_2,l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = g_uc_gsch_gs_header;
			*(l_uc_array_p + l_us_data_counter++) = g_uc_gsch_gs_cmd_header;
			*(l_uc_array_p + l_us_data_counter++) = g_uc_gsch_gs_cmd_tail;
			*(l_uc_array_p + l_us_data_counter++) = g_uc_gsch_gs_arg_len;
			for (l_us_counter_i = 0; l_us_counter_i < MAX_GSCH_CMD_ARG_LEN; l_us_counter_i++)
				*(l_uc_array_p + l_us_data_counter++) = g_uc_gsch_gs_arg[l_us_counter_i];
			l_us_data_counter = insertShortToArray(g_us_gsch_gs_cmd_crc,l_uc_array_p,l_us_data_counter);
			*(l_uc_array_p + l_us_data_counter++) = g_uc_gsch_subsystem_to_load;
			*(l_uc_array_p + l_us_data_counter++) = g_uc_gsch_data_type_to_load;
			*(l_uc_array_p + l_us_data_counter++) = g_uc_gsch_sd_block_property;
			*(l_uc_array_p + l_us_data_counter++) = g_uc_gsch_no_of_loading;
			*(l_uc_array_p + l_us_data_counter++) = g_uc_gsch_sampling_step;
			for (l_us_counter_i = 0; l_us_counter_i < (MAX_SCHEDULER_ARG+SCHEDULER_NON_ARG_SIZE+2); l_us_counter_i++)
				*(l_uc_array_p + l_us_data_counter++) = g_uc_gsch_sch_register[l_us_counter_i];
			l_us_data_counter = insertShortToArray(g_us_gsch_total_sending_time,l_uc_array_p,l_us_data_counter);
			break;

		//Fill with GSCH generated key
		case DATA_CAT_GP_REQUEST_KEY:
			l_us_data_counter = insertShortToArray(g_us_gsch_key,l_uc_array_p,l_us_data_counter);
			break;

		//Fill with datalog initial set
		case DATA_CAT_LOG_DATA_INIT_SET:
			g_ul = str_obdh_data.ul_log_data_start_time + SECONDS_FROM_TAI_TO_KILL_SWITCH_DEACTIVATION;
			for (l_us_counter_i = 0; l_us_counter_i < MAX_NO_OF_DATA_TYPE_LOGGED; l_us_counter_i++)
				l_us_data_counter = insertShortToArray(str_obdh_par.us_log_data_id[l_us_counter_i],l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertLongToArray(g_ul,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(str_obdh_data.us_log_data_duration_s,l_uc_array_p,l_us_data_counter);
			break;

		//Fill with datalog set
		case DATA_CAT_LOG_DATA_SET:
			for (l_us_counter_i = 0; l_us_counter_i < LOG_DATA_SET_SIZE; l_us_counter_i++)
				*(l_uc_array_p + l_us_data_counter++) = g_uc_log_data_set[l_us_counter_i];		
			break;
		
		//Fill with log data variables
		case DATA_CAT_LOG_DATA_VAR:
			g_ul = str_obdh_data.ul_log_data_start_time + SECONDS_FROM_TAI_TO_KILL_SWITCH_DEACTIVATION;
			for (l_us_counter_i = 0; l_us_counter_i < MAX_NO_OF_DATA_TYPE_LOGGED; l_us_counter_i++)
				l_us_data_counter = insertShortToArray(str_obdh_par.us_log_data_id[l_us_counter_i],l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertLongToArray(g_ul,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(str_obdh_data.us_log_data_duration_s,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(g_us_log_data_elapsed_time_s,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(g_us_log_data_buffer_index,l_uc_array_p,l_us_data_counter);
			l_us_data_counter = insertShortToArray(g_us_log_data_block_size,l_uc_array_p,l_us_data_counter);
			break;

		//Fill with datalog buffer
		case DATA_CAT_LOG_DATA_BUFFER:
			for (l_us_counter_i = 0; l_us_counter_i < g_us_log_data_buffer_index; l_us_counter_i++)
				*(l_uc_array_p + l_us_data_counter++) = g_uc_log_data_buffer[l_us_counter_i];		
			break;

		//Fill with eventlog buffer
		//dec22_yc
		case DATA_CAT_EVENTLOG_DATA_BUFFER:
			for (l_us_counter_i = 0; l_us_counter_i < EVENTLOG_DATA_BUFFER_SIZE; l_us_counter_i++)
				*(l_uc_array_p + l_us_data_counter++) = g_uc_eventlog_data_buffer[l_us_counter_i];		
			break;

		//Fill with filled schedule
		case DATA_CAT_FILLED_SCHEDULE:
			l_uc_counter_k = 0;
			//Because the 1st byte of the first frame, had this been used for sending would be filled by scheduler size (one byte),
			//there is -1 value in the numerator. (20120328)
			l_uc_max_counter_k = (MAX_AX25_CONTENT_SIZE-ACK_MSG_SIZE-1)/(SCHEDULER_NON_ARG_SIZE+MAX_SCHEDULER_ARG);
			while (l_uc_counter_k < l_uc_max_counter_k && (g_uc_schedule_sent_counter+l_uc_counter_k) < str_obdh_hk.uc_scheduler_block_size){
				g_ul = str_scheduler.ul_scheduled_time[str_scheduler.uc_reindexing[g_uc_schedule_sent_counter+l_uc_counter_k]] + SECONDS_FROM_TAI_TO_KILL_SWITCH_DEACTIVATION;
				l_us_data_counter = insertLongToArray(g_ul,l_uc_array_p,l_us_data_counter);
				l_us_data_counter = insertShortToArray(str_scheduler.us_cmd[str_scheduler.uc_reindexing[g_uc_schedule_sent_counter+l_uc_counter_k]],l_uc_array_p,l_us_data_counter);
				*(l_uc_array_p + l_us_data_counter++) = str_scheduler.uc_arg_length[str_scheduler.uc_reindexing[g_uc_schedule_sent_counter+l_uc_counter_k]];
				for (l_us_counter_j = 0; l_us_counter_j < MAX_SCHEDULER_ARG; l_us_counter_j++)
					*(l_uc_array_p + l_us_data_counter++) = str_scheduler.uc_arg[str_scheduler.uc_reindexing[g_uc_schedule_sent_counter+l_uc_counter_k]][l_us_counter_j];				
				l_uc_counter_k++;
			}
			if (l_us_fill_argument)
				g_uc_schedule_sent_counter += l_uc_counter_k;
			break;
		
		//Fill with SD header meaningful (reduced) data from the SD card buffer
		case DATA_CAT_SD_HEADER_INFO_SENT:
			//Get the original block address
			g_ul = uchar2ulong(0x00,g_uc_sd_data_buffer[19],g_uc_sd_data_buffer[20],g_uc_sd_data_buffer[21]);

			//Limit the block address to SD card block limit
			if (g_ul > SD_BP_MAX)
				g_ul = SD_BP_MAX;

			//Get what msg ends of the SD card data
			l_us_dummy_var = (unsigned short)(g_uc_sd_data_buffer[6] << 8) + g_uc_sd_data_buffer[7];

			//Adjust the size
			//Completed data size is (l_us_dummy_var+3) , including CRC16 bytes
			//Original data size should be (l_us_dummy_var+1), excluding CRC16 bytes of the SD card.
			//The data being sent down omit CRC16 bytes and unecessary info
			//Thus, it is (l_us_dummy_var+1) - SD_CARD_HEADER_SIZE + 5, since only 5 bytes of the info are sent
			l_us_dummy_var += 1;
			l_us_dummy_var -= (SD_CARD_HEADER_SIZE-5);

			//Now the maximum value of the data size should be MAX_SD_CARD_BLOCK_CONTENT_SIZE+5
			//Limit the maximum size
			if (l_us_dummy_var > (MAX_SD_CARD_BLOCK_CONTENT_SIZE+5))
				l_us_dummy_var = MAX_SD_CARD_BLOCK_CONTENT_SIZE+5;

			//Insert the last three
			*(l_uc_array_p + l_us_data_counter++) = g_ul >> 16;
			*(l_uc_array_p + l_us_data_counter++) = g_ul >> 8;
			*(l_uc_array_p + l_us_data_counter++) = g_ul;
			*(l_uc_array_p + l_us_data_counter++) = l_us_dummy_var >> 8;
			*(l_uc_array_p + l_us_data_counter++) = l_us_dummy_var;
			break;

		//Fill with satellite revolution no in float
		case DATA_CAT_SAT_REV_NO_IN_FLOAT:
			g_f = str_obdh_data.us_current_rev_no;
			g_f += (float)str_obdh_data.us_rev_offset_time/(float)str_obdh_par.us_rev_time_s;
			l_us_data_counter = insertFloatToArray(g_f,l_uc_array_p,l_us_data_counter);
			break;

		//Fill with filled AX25 frame package
		case DATA_CAT_FILLED_AX25_FRAME:
			//Get the length of the frame (excluding CRC16 bytes)
			l_us_dummy_var = g_uc_ax25_message[4];

			//Length limit check
			if (l_us_dummy_var > (AX25_BUFFER_SIZE-2)) l_us_dummy_var = (AX25_BUFFER_SIZE-2);

			//Fill the data
			for (l_us_counter_i = 0; l_us_counter_i < l_us_dummy_var; l_us_counter_i++)
				*(l_uc_array_p + l_us_data_counter++) = g_uc_ax25_message[l_us_counter_i];
			break;

		//Fill with function module error code
		case DATA_CAT_FUNCTION_MODULE_ERROR_CODE:
			l_us_data_counter = insertShortToArray(g_us_function_module_error_code,l_uc_array_p,l_us_data_counter);
			break;

		#endif

		//Error in calling this command
		default:
			l_us_data_counter = insertShortToArray(EC_INVALID_READING_DATA_REQUEST,l_uc_array_p,l_us_data_counter);
			break;
	}
	
	return l_us_data_counter;
}

