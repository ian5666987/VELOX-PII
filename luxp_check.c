//------------------------------------------------------------------------------------
// Luxp_check.c
//------------------------------------------------------------------------------------
// Copyright (C) 2013 Nanyang Technological University.
//
// AUTH: IAN
//		 CHEW YAN CHONG
// DATE: 03 APR 2013
//
// The program Luxp_check.c will be downloaded to the Pumpkin Motherboard with 
// C8051F120 uC as part of the VELOX-P Pico-Satellite Flight Software. 
//
// This file contains checking functions of the Flight Software
//
// Starts from version 0.7x, only up to checksum 16-bit is supported. Checksum 32-bit
// is removed
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

#include <luxp_check.h>

#if (CRC_ENABLE == 1)
// private function (only visible to checksum.c)
unsigned short Checksum_rotate(unsigned short crc, unsigned char value) {
  return ((crc << 8) + value) ^ g_us_crc16_table[(crc >> 8) & 0xFF];
}

// Ian: first call to checksum_update() should be with crc
unsigned short Checksum_update(void* msg, short length, unsigned short crc) {
  short count = 0;
  char* message = (char*) msg;
  for (count = 0; count < length; ++count)
    crc = Checksum_rotate(crc, message[count]);
  return crc;
}

unsigned short Checksum_publish(unsigned short crc) {
  crc = Checksum_rotate(crc, 0);
  crc = Checksum_rotate(crc, 0);
  return crc;
}

//------------------------------------------------------------------------------------
// To create CRC/Checksum bytes	(16/32 bits)
//------------------------------------------------------------------------------------
unsigned short createChecksum (unsigned char *l_uc_msg_pointer, unsigned short l_us_msg_length, unsigned char l_uc_checksum_type)
{
	unsigned short 	l_us_checksum_register = 0x0000;
	//Creating CRC bytes
	if (l_uc_checksum_type == CHECKSUM_CRC16_TABLE)
		l_us_checksum_register = Checksum_publish(Checksum_update(l_uc_msg_pointer, l_us_msg_length, l_us_checksum_register));
	else if (l_uc_checksum_type == CHECKSUM_16BIT)	//Creating checksum 16-bit
		while (l_us_msg_length--) l_us_checksum_register += *l_uc_msg_pointer++;

	return l_us_checksum_register;
}

//------------------------------------------------------------------------------------
// To add CRC/Checksum bytes created (16 Bits)
//------------------------------------------------------------------------------------
void addChecksumBytes16 (unsigned char *l_uc_msg_pointer, unsigned short l_us_msg_length, unsigned short l_us_checksum_bytes)
{
	*(l_uc_msg_pointer + l_us_msg_length) 		= l_us_checksum_bytes >> 8;
	*(l_uc_msg_pointer + l_us_msg_length + 1) 	= l_us_checksum_bytes;
}

//------------------------------------------------------------------------------------
// To check wether CRC16 bytes of a message is matching
//------------------------------------------------------------------------------------
unsigned short checkCRC16Match (unsigned char *l_uc_msg_p, unsigned short l_us_msg_length)
{
	unsigned short	l_us_matching_result = EC_SUCCESSFUL;

	unsigned short 	l_us_read_checksum_bytes;
	unsigned short 	l_us_expected_checksum_bytes;

	l_us_read_checksum_bytes 			= (unsigned short)(*(l_uc_msg_p + l_us_msg_length) << 8) + *(l_uc_msg_p + l_us_msg_length + 1);
	l_us_expected_checksum_bytes 		= createChecksum(l_uc_msg_p,l_us_msg_length,CHECKSUM_CRC16_TABLE);

	if (l_us_read_checksum_bytes == l_us_expected_checksum_bytes)
		l_us_matching_result = EC_SUCCESSFUL;
	else l_us_matching_result = EC_CRC_BYTES_DO_NOT_MATCH;

	return l_us_matching_result;
}
#endif

#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)
#if (ISIS_CMD_ENABLE == 1)
//------------------------------------------------------------------------------------
// To check a command received by GSCH, whether it passes time window check
// The flag GSCH_FLAG_TIME_WINDOW_VALID will be raised if the GSCH command is valid.
// The result will be different (GSCH_FLAG_TIME_WINDOW_VALID is not raised) if the
// command is not valid.
//------------------------------------------------------------------------------------
void timeWindowCheck (void)
{
	unsigned char l_uc_i;
	unsigned long l_ul_gdsn_time;
	unsigned long l_ul_lower_time_limit;
	unsigned long l_ul_upper_time_limit;

	//Assume it does not pass time window validation first
	g_uc_gsch_flag &= ~GSCH_FLAG_TIME_WINDOW_VALID;

	//Get GDSN time	
	l_ul_gdsn_time = uchar2ulong(g_uc_gsch_gs_arg[0],g_uc_gsch_gs_arg[1],g_uc_gsch_gs_arg[2],g_uc_gsch_gs_arg[3]);
	l_ul_gdsn_time ^= 0xFF00FF00;

	//Lock OBC time before being used
	g_ul = str_obdh_data.ul_obc_time_s + SECONDS_FROM_TAI_TO_KILL_SWITCH_DEACTIVATION;

	//Set lower time window limit
	l_ul_lower_time_limit = g_ul - (unsigned short)(str_obdh_par.uc_gsch_time_window_10s * 10);

	//Set upper time windoe limit
	l_ul_upper_time_limit = g_ul + (unsigned short)(str_obdh_par.uc_gsch_time_window_10s * 10);

	//Check if the GDSN time passes time window
	if ((l_ul_gdsn_time >= l_ul_lower_time_limit) && (l_ul_gdsn_time <= l_ul_upper_time_limit)){
		//Set time window validation flag
		g_uc_gsch_flag |= GSCH_FLAG_TIME_WINDOW_VALID;

		//Shift the command argument
		for (l_uc_i = 0; l_uc_i < MAX_GSCH_CMD_ARG_LEN-4; l_uc_i++)
			g_uc_gsch_gs_arg[l_uc_i] = g_uc_gsch_gs_arg[l_uc_i+4];

		//Change the command length
		g_uc_gsch_gs_arg_len = (g_uc_gsch_gs_arg_len < 4) ? 0 : (g_uc_gsch_gs_arg_len-4);

		//Delete the excessive bytes
		//Ian (2012-05-15): the excessive bytes to be deleted here are the last four, following the shift
		//					of the command argument as shown above.
		for (l_uc_i = MAX_GSCH_CMD_ARG_LEN-4; l_uc_i < MAX_GSCH_CMD_ARG_LEN; l_uc_i++)
			g_uc_gsch_gs_arg[l_uc_i] = 0;
	}	
}

#if (SATELLITE_SCHEDULE_HANDLER_ENABLE == 1)
//------------------------------------------------------------------------------------
// To check if there is ADCS data in Log Data Type
//------------------------------------------------------------------------------------
unsigned char checkADCSDataInLogDataType (void){
	unsigned char l_uc_adcs_data_present = 0;
	unsigned char l_uc_i = 0;

	for (l_uc_i = 0; l_uc_i < MAX_NO_OF_DATA_TYPE_LOGGED; l_uc_i++)
		if ((str_obdh_par.us_log_data_id[l_uc_i] >> 8) == ADCS_ADDR) {
			l_uc_adcs_data_present = 0xff;
			break;
		}

	return l_uc_adcs_data_present;
}
#endif
#endif
#endif

//------------------------------------------------------------------------------------
// To check event and eventually log into SD
//dec22_yc
//------------------------------------------------------------------------------------
/*
unsigned char g_uc_eventlog_data_buffer[EVENTLOG_DATA_BUFFER_SIZE] = 0;
unsigned short g_us_no_of_eventlog_this_orbit = 0;
unsigned char g_uc_eventlog_status_flag = 0;//missing logs... full logs, time to save log to SD
*/
//dec22_yc tested
#if (EVENT_HANDLER_ENABLE == 1)
#if (STORING_ENABLE == 1)
void eventHandler (unsigned short l_us_error_code, unsigned char l_uc_event_arg_0,unsigned char l_uc_event_arg_1,
						unsigned char l_uc_event_arg_2,unsigned char l_uc_event_arg_3,unsigned char l_uc_event_arg_4,unsigned char l_uc_event_arg_5){
	
	unsigned short l_us_data_counter=0;
	unsigned char l_uc_temp;
	
	//As str_obdh_data.ul_obc_time_s might have been updated in the midst of function execution (due to interrupt)
	//the value is first captured in the beginning of the function
	g_ul = str_obdh_data.ul_obc_time_s + SECONDS_FROM_TAI_TO_KILL_SWITCH_DEACTIVATION;	
		
	// flag set 1: is the event buffer full or g_us_no_of_eventlog_this_orbit full?
	//Style justified.
	if((g_uc_eventlog_status_flag & EVENTLOG_STATUS_REQUIRE_SAVING) || (g_uc_eventlog_status_flag & EVENTLOG_STATUS_FULL)){
		g_uc_eventlog_status_flag |= EVENTLOG_STATUS_MISSING_ENTRY;	//event not logged
	}
	else{ 
		l_us_data_counter=(g_us_no_of_eventlog_this_orbit % NO_OF_EVENTLOG_DATA_SET_PER_BLOCK) * EVENTLOG_DATA_SET_SIZE;
		
		// execution step 1 : missing events due to last event unable to log.
		if(g_uc_eventlog_status_flag & EVENTLOG_STATUS_MISSING_ENTRY){	
			l_us_data_counter = insertLongToArray(g_ul,g_uc_eventlog_data_buffer,l_us_data_counter);//yc added 20120224
			g_uc_eventlog_data_buffer[l_us_data_counter++]=EV_EVENT_HANDLER>>8;
			g_uc_eventlog_data_buffer[l_us_data_counter++]=g_uc_eventlog_status_flag;
			g_us_no_of_eventlog_this_orbit++;
			
			g_uc_eventlog_status_flag &= ~EVENTLOG_STATUS_MISSING_ENTRY;//flag clear 1: update flag when done
		}
		//execution step 2: event save to g_uc_eventlog_data_buffer
		//dec22_yc prevent buffer overflow check
		l_us_data_counter=(g_us_no_of_eventlog_this_orbit % NO_OF_EVENTLOG_DATA_SET_PER_BLOCK) * EVENTLOG_DATA_SET_SIZE;

		if(l_us_data_counter<EVENTLOG_DATA_BUFFER_SIZE){

			l_us_data_counter = insertLongToArray(g_ul,g_uc_eventlog_data_buffer,l_us_data_counter); //yc added 20120224	
			
			g_uc_eventlog_data_buffer[l_us_data_counter++]=l_us_error_code>>8;
			g_uc_eventlog_data_buffer[l_us_data_counter++]=l_us_error_code;
			
			g_uc_eventlog_data_buffer[l_us_data_counter++]=l_uc_event_arg_0;
			g_uc_eventlog_data_buffer[l_us_data_counter++]=l_uc_event_arg_1;
			g_uc_eventlog_data_buffer[l_us_data_counter++]=l_uc_event_arg_2;
			g_uc_eventlog_data_buffer[l_us_data_counter++]=l_uc_event_arg_3;
			g_uc_eventlog_data_buffer[l_us_data_counter++]=l_uc_event_arg_4;
			g_uc_eventlog_data_buffer[l_us_data_counter++]=l_uc_event_arg_5;
			g_us_no_of_eventlog_this_orbit++;
		}
		
		//flag set 2: check after adding this event is the g_us_no_of_eventlog_this_orbit almost full?
		if(g_us_no_of_eventlog_this_orbit>(NO_OF_EVENTLOG_DATA_SET_PER_BLOCK*100-3)){//estimate 100 min per loop, of same size as WOD maximum  dec22_yc testing purposes set as 50 full.

			//dec22_yc moved up to update flag prior to saving tested.
			g_uc_eventlog_status_flag |= EVENTLOG_STATUS_FULL; //update full flag when done this is to be cleared when orbit number changed, 
			g_uc_eventlog_status_flag |= EVENTLOG_STATUS_REQUIRE_SAVING; //force saving to SD once.

			g_ul = str_obdh_data.ul_obc_time_s + SECONDS_FROM_TAI_TO_KILL_SWITCH_DEACTIVATION;
			l_us_data_counter = insertLongToArray(g_ul,g_uc_eventlog_data_buffer,l_us_data_counter);//yc added 20120224	
			
			g_uc_eventlog_data_buffer[l_us_data_counter++]=EV_EVENT_HANDLER>>8;
			g_uc_eventlog_data_buffer[l_us_data_counter++]=g_uc_eventlog_status_flag;
			g_us_no_of_eventlog_this_orbit++;  					
		}		
		//flag set 3: check after adding this event is there a need to save to SD card?
		if((g_us_no_of_eventlog_this_orbit % NO_OF_EVENTLOG_DATA_SET_PER_BLOCK)==0){	
			g_uc_eventlog_status_flag |= EVENTLOG_STATUS_REQUIRE_SAVING;	//saving required next		
		}
			
	}

	//flag clear 2 : reset g_us_no_of_eventlog_this_orbit if changed orbit no.
	//dec22_yc moved down to include check even if orbit is full
	//last entry in an orbit round is EV_ORBIT_UPDATE_HANDLER.. save to SD to prepare fresh page for new orbit
	if(l_us_error_code==EV_ORBIT_UPDATE_HANDLER){
		g_uc_eventlog_status_flag |= EVENTLOG_STATUS_REQUIRE_SAVING; //force saving to SD once
		g_uc_eventlog_status_flag &= ~EVENTLOG_STATUS_FULL;//update flag when done
		g_us_no_of_eventlog_this_orbit = 0;	//reset buffer
	}

	//execution step 3: g_uc_eventlog_data_buffer required to save to SD dec22_yc
	if( g_uc_eventlog_status_flag & EVENTLOG_STATUS_REQUIRE_SAVING){
		l_uc_temp = g_uc_strsnd_flag;
		g_uc_strsnd_flag &= ~STRNG_FLAG_SAVE_DATA_HOLD;
		if(!saveDataProtected(ALL_SUBSYSTEMS,SD_EVENTLOG_DATA,g_uc_temporary_data_buffer)){
			for(l_us_data_counter=0;l_us_data_counter<EVENTLOG_DATA_BUFFER_SIZE;l_us_data_counter++)	
				g_uc_eventlog_data_buffer[l_us_data_counter] = 0;	//reset buffer
			g_uc_eventlog_status_flag &= ~EVENTLOG_STATUS_REQUIRE_SAVING;//flag clear 3: update flag when done
		}
		g_uc_strsnd_flag = l_uc_temp;
	}	 
}

//yc added 20120224
void eventHandlerUpdateStateMode (unsigned char l_uc_sat_mode_next)
{		
	eventHandler(EV_SATELLITE_MODE_HANDLER,str_obdh_hk.uc_sat_mode,l_uc_sat_mode_next,
		str_obdh_hk.us_brh_flag>>8,str_obdh_hk.us_brh_flag,
	 	str_obdh_hk.us_imh_flag>>8,str_obdh_hk.us_imh_flag);
}

#if (SATELLITE_MODE_HANDLER_ENABLE == 1)
#if (SAFE_MODE_HANDLER_ENABLE == 1)
#if (ADCS_CMD_ENABLE == 1)
#if (PWRS_CMD_ENABLE == 1)
//yc added 20120224
void eventHandlerSafeholdMode (void)
{
	//yc added 20120224
	eventHandler(EV_SMH_HANDLER,str_obdh_hk.uc_smh_flag,str_obdh_data.uc_batt_soc,0,0,0,0);
}
#endif
#endif
#endif			
#endif			

#endif
#endif			
