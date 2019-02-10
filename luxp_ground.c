//------------------------------------------------------------------------------------
// Luxp_ground.c
//------------------------------------------------------------------------------------
// Copyright (C) 2013 Nanyang Technological University.
//
// AUTH: IAN
// DATE: 03 APR 2013
//
// The program Luxp_ground.c will be downloaded to the Pumpkin Motherboard with 
// C8051F120 uC as part of the VELOX-P Pico-Satellite Flight Software. 
//
// This file contains all task functions of the Flight Software
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

#include <luxp_ground.h>

#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)
#if (ISIS_CMD_ENABLE == 1)
//------------------------------------------------------------------------------------
// To update OBC time and RTC clock using GDSN time
//------------------------------------------------------------------------------------
void groundUpdateTime (unsigned long l_ul_tai_time)
{
	str_obdh_data.ul_gs_ref_time_s = l_ul_tai_time-SECONDS_FROM_TAI_TO_KILL_SWITCH_DEACTIVATION;
	str_obdh_data.ul_obc_time_s = str_obdh_data.ul_gs_ref_time_s;
	#if (TIME_ENABLE == 1)
	#if (RTC_CMD_ENABLE == 1)
	//Redundancy deleted
	sec2DateUpdateFromRef(str_obdh_data.ul_gs_ref_time_s);
	#if (DEBUGGING_FEATURE == 1)
	g_us_latest_gdsn_to_rtc_update_ec = rtcUpdateDate(0,str_date_update.uc_day,str_date_update.uc_month,str_date_update.uc_year,str_date_update.uc_hour,str_date_update.uc_min,str_date_update.uc_s,DEBUGGING_FEATURE);
	#else
	g_us_latest_gdsn_to_rtc_update_ec = rtcUpdateDate(0,str_date_update.uc_day,str_date_update.uc_month,str_date_update.uc_year,str_date_update.uc_hour,str_date_update.uc_min,str_date_update.uc_s);
	#endif
	#else
	g_us_latest_gdsn_to_rtc_update_ec = EC_RTC_CMD_DISABLED;
	#endif
	#endif
}

//------------------------------------------------------------------------------------
// To update OBC revolution no using GDSN input
//------------------------------------------------------------------------------------
void groundUpdateRevNo (void)
{
	//Set new orbit as directed by the GDSN
	str_obdh_data.f_gs_reference_rev_no = uchar2float(g_uc_gsch_gs_arg[0],g_uc_gsch_gs_arg[1],g_uc_gsch_gs_arg[2],g_uc_gsch_gs_arg[3]);

	//Take the integer part
	str_obdh_data.us_gs_reference_rev_no = str_obdh_data.f_gs_reference_rev_no;

	//Take the non-integer part, make use of global multipurpose float variable
	g_f = str_obdh_data.f_gs_reference_rev_no-(float)str_obdh_data.us_gs_reference_rev_no;

	//Multiply the non-integer part to integer in sec value (offset) to the revolution
	g_f *= (float)str_obdh_par.us_rev_time_s;

	//Change auto orbit following the new referred orbit & set the orbiting time to zero again
	//Add the non-integer part to the satellite revolution offset
	str_obdh_data.us_auto_reference_rev_no = str_obdh_data.us_gs_reference_rev_no;
	str_obdh_data.us_rev_offset_time = (unsigned short)g_f;
}

//------------------------------------------------------------------------------------
// To listen to GDSN command and proceeds with the validation
//------------------------------------------------------------------------------------
void groundCommandListener (void)
{
	#if (ISIS_CMD_ENABLE == 1)

	unsigned short l_us_comm_error_code = EC_INIT;
	unsigned char l_uc_i = 0;

	//If the time to check the GDSN command has come, else, skips it
	if (g_uc_gsch_flag & GSCH_FLAG_INITIAL_CHECK){
		//Complete initial validation checking, reset the checking flag
		g_uc_gsch_flag &= ~GSCH_FLAG_CHECK_RESET;

		//Get IMC no of frame in the buffer every second (refer to global software timer)
		//Get AX.25 no of frame from COMM board
		l_us_comm_error_code = isisI2C(IMC_ADDR,IMC_I2C_GET_NOFR,0);			
	
		//If there is an error code, increase retry counter
		//Else, reset it to zero
		if (l_us_comm_error_code) 
			g_uc_gsch_comm_retry_counter++;
		else g_uc_gsch_comm_retry_counter = 0;
	
		//If there is no error code and when we read the command,
		//there is something in the ISIS board, check it
		if (!l_us_comm_error_code && g_uc_i2c_data_in[0]){
			//Do second-checking
			for (l_uc_i = 0; l_uc_i < 1; l_uc_i++){
				l_us_comm_error_code = isisI2C(IMC_ADDR,IMC_I2C_GET_NOFR,0);
				if (l_us_comm_error_code) break;
			}

			//If it passes the second checking, then proceeds to read the command sent
			if (!l_us_comm_error_code && g_uc_i2c_data_in[0]){
				insertGDSNCommandToGSCHVar();
				//Check if valid command is detected
				if (!g_us_gsch_command_validation_error_code){
					//Set flags for further checking the command
					g_uc_gsch_flag |= GSCH_FLAG_VALID_COMMAND;

					//Reset time elapsed since last communication with MCC time register
					g_ul_gsch_time_elapsed_since_last_communication_with_mcc = 0;
				}
			}	
		}			
	}
	
	#endif
}

//------------------------------------------------------------------------------------
// To generate groundpass request key
//------------------------------------------------------------------------------------
// Ian (2012-05-15): In the end, the request key generator uses complex method.
void groundPassRequestKeyGenerator (void){
	#if (CRC_ENABLE == 1)

	unsigned char l_uc_array[8];
	unsigned short l_us_temp_crc16;

	#if (KEY_GENERATION_METHOD == COMPLEX_METHOD)
	//Record time & orbit info in the array
	fillData(l_uc_array,0,DATA_CAT_LATEST_TIME_AND_ORBIT_TAI_FORMAT,0);

	//Generate CRC16 based on current time and orbit info (key) 
	g_us_gsch_key = createChecksum (l_uc_array, 6, CHECKSUM_CRC16_TABLE);

	//To avoid zero key generated
	if (!g_us_gsch_key) g_us_gsch_key++;

	//Put the key to the array to generate the answer
	l_uc_array[0] = g_us_gsch_key >> 8;
	l_uc_array[1] = g_us_gsch_key;

	//Generate first CRC16 of the key (first answer)
	g_us_gsch_answer_1 = createChecksum (l_uc_array, 2, CHECKSUM_CRC16_TABLE);

	//Put the first CRC16 of the key to the array in preparation of generating answers
	l_uc_array[0] = g_us_gsch_answer_1 >> 8;
	l_uc_array[1] = g_us_gsch_answer_1;

	//Generate second CRC16 of the key
	l_us_temp_crc16 = createChecksum (l_uc_array, 2, CHECKSUM_CRC16_TABLE);

	//Put the second CRC16 of the key to the array in preparation of generating answers
	l_uc_array[0] = l_us_temp_crc16 >> 8;
	l_uc_array[1] = l_us_temp_crc16;

	//Generate third CRC16 of the key
	l_us_temp_crc16 = createChecksum (l_uc_array, 2, CHECKSUM_CRC16_TABLE);

	//Put the third CRC16 of the key to the array in preparation of generating answers
	l_uc_array[0] = l_us_temp_crc16 >> 8;
	l_uc_array[1] = l_us_temp_crc16;

	//Generate fourth CRC16 of the key (second answer)
	g_us_gsch_answer_2 = createChecksum (l_uc_array, 2, CHECKSUM_CRC16_TABLE);

	#elif (KEY_GENERATION_METHOD == SIMPLE_METHOD)
	//Take the last two bytes of the current time info as key
	g_us_gsch_key = str_obdh_data.ul_obc_time_s;

	//To avoid zero key generated
	if (!g_us_gsch_key) g_us_gsch_key++;

	//Put the key in the array
	l_uc_array[0] = g_us_gsch_key >> 8;
	l_uc_array[1] = g_us_gsch_key;

	//Generate first CRC16 of the key (first answer)
	g_us_gsch_answer_1 = createChecksum (l_uc_array, 2, CHECKSUM_CRC16_TABLE);

	//Put the first CRC16 of the key to the array in preparation of generating answers
	l_uc_array[0] = g_us_gsch_answer_1 >> 8;
	l_uc_array[1] = g_us_gsch_answer_1;

	//Generate second CRC16 of the key
	l_us_temp_crc16 = createChecksum (l_uc_array, 2, CHECKSUM_CRC16_TABLE);

	//Put the second CRC16 of the key to the array in preparation of generating answers
	l_uc_array[0] = l_us_temp_crc16 >> 8;
	l_uc_array[1] = l_us_temp_crc16;

	//Generate third CRC16 of the key
	l_us_temp_crc16 = createChecksum (l_uc_array, 2, CHECKSUM_CRC16_TABLE);

	//Put the third CRC16 of the key to the array in preparation of generating answers
	l_uc_array[0] = l_us_temp_crc16 >> 8;
	l_uc_array[1] = l_us_temp_crc16;

	//Generate fourth CRC16 of the key (second answer)
	g_us_gsch_answer_2 = createChecksum (l_uc_array, 2, CHECKSUM_CRC16_TABLE);
	#endif
	
	#else
	//Take the last two bytes of the current time info as key
	g_us_gsch_key = str_obdh_data.ul_obc_time_s;

	//To avoid zero key generated
	if (!g_us_gsch_key) g_us_gsch_key++;

	//Create keys using simple methods
	g_us_gsch_answer_1 = ~g_us_gsch_key;
	g_us_gsch_answer_2 = g_us_gsch_key & 0xF0F0;

	#endif
}
#endif
#endif

#if (SENDING_ENABLE == 1)
#if (ISIS_CMD_ENABLE == 1)
#if (BEACON_HANDLER_ENABLE == 1)
//------------------------------------------------------------------------------------
// To initialize CW beacon message, content, and rate under different modes
//------------------------------------------------------------------------------------
//	This function will be called in the beacon handler, whenever beacon handler is
//	in the automatic mode, just before it wants to send CW beacon.
//------------------------------------------------------------------------------------
void groundInitCWBeacon (unsigned short l_us_cw_beacon_mode)
{
	unsigned short l_us_batt_temp;
	unsigned short l_us_batt_v;
	unsigned char l_uc_i;

	//Set parameters for idle beacon 
	if (l_us_cw_beacon_mode == BEACON_FLAG_CW_MODE_M3)
		//Set CW message length
		g_uc_cw_message_length = CW_BEACON_LENGTH_M3;
	//Set parameters for NOP SH beacon
	else if (l_us_cw_beacon_mode == BEACON_FLAG_CW_MODE_M8)
		//Set CW message length
		g_uc_cw_message_length = CW_BEACON_LENGTH_M8;
	//Set parameters for LEOP SH beacon
	else if (l_us_cw_beacon_mode == BEACON_FLAG_CW_MODE_M4_M5_M7)
		//Set CW message length
		g_uc_cw_message_length = CW_BEACON_LENGTH_M4_M5_M7;
	//Set parameters for COMM ZONE beacon
	else if (l_us_cw_beacon_mode == BEACON_FLAG_CW_MODE_COMM_ZONE)
		//Set CW message length
		g_uc_cw_message_length = CW_BEACON_LENGTH_COMM_ZONE;

	//Ian (2012-05-15): the batteries are in series
	l_us_batt_temp = ((unsigned short)str_pwrs_hk.uc_batt_temp[0] + (unsigned short)str_pwrs_hk.uc_batt_temp[1])/2;
	l_us_batt_v = str_pwrs_hk.us_batt_v[0] + str_pwrs_hk.us_batt_v[1];

	//Set satellite mode
	//Satellite mode is initialized in luxp[version]_init.c file, it should not go uninitialized
	//Default case is added, nevertheless
	//Satellite mode M1 is impossible to occur, thus removed.
	//Ian (2012-05-15): it is noted that the lookup table is an alternative way of doing it.
	if 		(str_obdh_hk.uc_sat_mode == SATELLITE_MODE_M2) g_uc_cw_message[6] = '2';
	else if (str_obdh_hk.uc_sat_mode == SATELLITE_MODE_M3) g_uc_cw_message[6] = 'G';
	else if (str_obdh_hk.uc_sat_mode == SATELLITE_MODE_M4) g_uc_cw_message[6] = '4';
	else if (str_obdh_hk.uc_sat_mode == SATELLITE_MODE_M5) g_uc_cw_message[6] = 'C';
	else if (str_obdh_hk.uc_sat_mode == SATELLITE_MODE_M6) g_uc_cw_message[6] = '6';
	else if (str_obdh_hk.uc_sat_mode == SATELLITE_MODE_M7) g_uc_cw_message[6] = 'Q';
	else if (str_obdh_hk.uc_sat_mode == SATELLITE_MODE_M8) g_uc_cw_message[6] = '8';
	else g_uc_cw_message[6] = '2';

	//Set SoC
	if 		(str_pwrs_hk.uc_batt_soc < 25)									g_uc_cw_message[7] = 'D';
	else if (str_pwrs_hk.uc_batt_soc >= 25 && str_pwrs_hk.uc_batt_soc < 40)	g_uc_cw_message[7] = 'F';
	else if (str_pwrs_hk.uc_batt_soc >= 40 && str_pwrs_hk.uc_batt_soc < 50)	g_uc_cw_message[7] = '2';
	else if (str_pwrs_hk.uc_batt_soc >= 50 && str_pwrs_hk.uc_batt_soc < 55)	g_uc_cw_message[7] = 'G';
	else if (str_pwrs_hk.uc_batt_soc >= 55 && str_pwrs_hk.uc_batt_soc < 60)	g_uc_cw_message[7] = '4';
	else if (str_pwrs_hk.uc_batt_soc >= 60 && str_pwrs_hk.uc_batt_soc < 65)	g_uc_cw_message[7] = 'C';
	else if (str_pwrs_hk.uc_batt_soc >= 65 && str_pwrs_hk.uc_batt_soc < 70)	g_uc_cw_message[7] = '6';
	else if (str_pwrs_hk.uc_batt_soc >= 70 && str_pwrs_hk.uc_batt_soc < 80)	g_uc_cw_message[7] = 'Q';
	else if (str_pwrs_hk.uc_batt_soc >= 80 && str_pwrs_hk.uc_batt_soc < 90)	g_uc_cw_message[7] = '8';
	else if (str_pwrs_hk.uc_batt_soc >= 90)									g_uc_cw_message[7] = 'Z';

	//Set battery voltage
	if 		(l_us_batt_v < 5000)						g_uc_cw_message[8] = 'D';
	else if (l_us_batt_v >= 5000 && l_us_batt_v < 6000)	g_uc_cw_message[8] = 'F';
	else if (l_us_batt_v >= 6000 && l_us_batt_v < 6500)	g_uc_cw_message[8] = '2';
	else if (l_us_batt_v >= 6500 && l_us_batt_v < 7000)	g_uc_cw_message[8] = 'G';
	else if (l_us_batt_v >= 7000 && l_us_batt_v < 7400)	g_uc_cw_message[8] = '4';
	else if (l_us_batt_v >= 7400 && l_us_batt_v < 7600)	g_uc_cw_message[8] = 'C';
	else if (l_us_batt_v >= 7600 && l_us_batt_v < 7800)	g_uc_cw_message[8] = '6';
	else if (l_us_batt_v >= 7800 && l_us_batt_v < 8000)	g_uc_cw_message[8] = 'Q';
	else if (l_us_batt_v >= 8000 && l_us_batt_v < 8200)	g_uc_cw_message[8] = '8';
	else if (l_us_batt_v >= 8200)						g_uc_cw_message[8] = 'Z';

	//Set ADCS sensor status
	if 		((str_adcs_hk.c_sensor_enable & 0x07) == 0x00)	g_uc_cw_message[9] = 'D';
	else if ((str_adcs_hk.c_sensor_enable & 0x07) == 0x01)	g_uc_cw_message[9] = 'F';
	else if ((str_adcs_hk.c_sensor_enable & 0x07) == 0x02)	g_uc_cw_message[9] = '2';
	else if ((str_adcs_hk.c_sensor_enable & 0x07) == 0x03)	g_uc_cw_message[9] = 'G';
	else if ((str_adcs_hk.c_sensor_enable & 0x07) == 0x04)	g_uc_cw_message[9] = '4';
	else if ((str_adcs_hk.c_sensor_enable & 0x07) == 0x05)	g_uc_cw_message[9] = 'C';
	else if ((str_adcs_hk.c_sensor_enable & 0x07) == 0x06)	g_uc_cw_message[9] = '6';
	else if ((str_adcs_hk.c_sensor_enable & 0x07) == 0x07)	g_uc_cw_message[9] = 'Q';

	//Full beacon set
	if (l_us_cw_beacon_mode == BEACON_FLAG_CW_MODE_M3 || l_us_cw_beacon_mode == BEACON_FLAG_CW_MODE_M4_M5_M7 || l_us_cw_beacon_mode == BEACON_FLAG_CW_MODE_COMM_ZONE){
		//Set battery temperature
		if 		(l_us_batt_temp < 30)								g_uc_cw_message[10] = 'D';
		else if (l_us_batt_temp >= 30 	&& l_us_batt_temp < 35)		g_uc_cw_message[10] = 'F';
		else if (l_us_batt_temp >= 35 	&& l_us_batt_temp < 40)		g_uc_cw_message[10] = '2';
		else if (l_us_batt_temp >= 40 	&& l_us_batt_temp < 45)		g_uc_cw_message[10] = 'G';
		else if (l_us_batt_temp >= 45 	&& l_us_batt_temp < 50)		g_uc_cw_message[10] = '4';
		else if (l_us_batt_temp >= 50 	&& l_us_batt_temp < 55)		g_uc_cw_message[10] = 'C';
		else if (l_us_batt_temp >= 55 	&& l_us_batt_temp < 60)		g_uc_cw_message[10] = '6';
		else if (l_us_batt_temp >= 60 	&& l_us_batt_temp < 65)		g_uc_cw_message[10] = 'Q';
		else if (l_us_batt_temp >= 65 	&& l_us_batt_temp < 70)		g_uc_cw_message[10] = '8';
		else if (l_us_batt_temp >= 70)								g_uc_cw_message[10] = 'Z';

		//Set ADCS MCU temperature
		if 		(str_adcs_hk.s_mcu_temp < -2000)										g_uc_cw_message[11] = 'D';
		else if (str_adcs_hk.s_mcu_temp >= -2000 	&& str_adcs_hk.s_mcu_temp < -1000)	g_uc_cw_message[11] = 'F';
		else if (str_adcs_hk.s_mcu_temp >= -1000 	&& str_adcs_hk.s_mcu_temp < 0)		g_uc_cw_message[11] = '2';
		else if (str_adcs_hk.s_mcu_temp >= 0 		&& str_adcs_hk.s_mcu_temp < 1500)	g_uc_cw_message[11] = 'G';
		else if (str_adcs_hk.s_mcu_temp >= 1500 	&& str_adcs_hk.s_mcu_temp < 3000)	g_uc_cw_message[11] = '4';
		else if (str_adcs_hk.s_mcu_temp >= 3000 	&& str_adcs_hk.s_mcu_temp < 4500)	g_uc_cw_message[11] = 'C';
		else if (str_adcs_hk.s_mcu_temp >= 4500 	&& str_adcs_hk.s_mcu_temp < 6000)	g_uc_cw_message[11] = '6';
		else if (str_adcs_hk.s_mcu_temp >= 6000 	&& str_adcs_hk.s_mcu_temp < 7500)	g_uc_cw_message[11] = 'Q';
		else if (str_adcs_hk.s_mcu_temp >= 7500 	&& str_adcs_hk.s_mcu_temp < 8500)	g_uc_cw_message[11] = '8';
		else if (str_adcs_hk.s_mcu_temp >= 8500)										g_uc_cw_message[11] = 'Z';

		//Set PWRS MCU temperature
		if 		(str_pwrs_hk.s_mcu_temp < -2000)										g_uc_cw_message[12] = 'D';
		else if (str_pwrs_hk.s_mcu_temp >= -2000 	&& str_pwrs_hk.s_mcu_temp < -1000)	g_uc_cw_message[12] = 'F';
		else if (str_pwrs_hk.s_mcu_temp >= -1000 	&& str_pwrs_hk.s_mcu_temp < 0)		g_uc_cw_message[12] = '2';
		else if (str_pwrs_hk.s_mcu_temp >= 0 		&& str_pwrs_hk.s_mcu_temp < 1500)	g_uc_cw_message[12] = 'G';
		else if (str_pwrs_hk.s_mcu_temp >= 1500 	&& str_pwrs_hk.s_mcu_temp < 3000)	g_uc_cw_message[12] = '4';
		else if (str_pwrs_hk.s_mcu_temp >= 3000 	&& str_pwrs_hk.s_mcu_temp < 4500)	g_uc_cw_message[12] = 'C';
		else if (str_pwrs_hk.s_mcu_temp >= 4500 	&& str_pwrs_hk.s_mcu_temp < 6000)	g_uc_cw_message[12] = '6';
		else if (str_pwrs_hk.s_mcu_temp >= 6000 	&& str_pwrs_hk.s_mcu_temp < 7500)	g_uc_cw_message[12] = 'Q';
		else if (str_pwrs_hk.s_mcu_temp >= 7500 	&& str_pwrs_hk.s_mcu_temp < 8500)	g_uc_cw_message[12] = '8';
		else if (str_pwrs_hk.s_mcu_temp >= 8500)										g_uc_cw_message[12] = 'Z';
		
		//Set OBDH MCU temperature
		if 		(str_obdh_hk.s_mcu_temp < -2000)										g_uc_cw_message[13] = 'D';
		else if (str_obdh_hk.s_mcu_temp >= -2000 	&& str_obdh_hk.s_mcu_temp < -1000)	g_uc_cw_message[13] = 'F';
		else if (str_obdh_hk.s_mcu_temp >= -1000 	&& str_obdh_hk.s_mcu_temp < 0)		g_uc_cw_message[13] = '2';
		else if (str_obdh_hk.s_mcu_temp >= 0 		&& str_obdh_hk.s_mcu_temp < 1500)	g_uc_cw_message[13] = 'G';
		else if (str_obdh_hk.s_mcu_temp >= 1500 	&& str_obdh_hk.s_mcu_temp < 3000)	g_uc_cw_message[13] = '4';
		else if (str_obdh_hk.s_mcu_temp >= 3000 	&& str_obdh_hk.s_mcu_temp < 4500)	g_uc_cw_message[13] = 'C';
		else if (str_obdh_hk.s_mcu_temp >= 4500 	&& str_obdh_hk.s_mcu_temp < 6000)	g_uc_cw_message[13] = '6';
		else if (str_obdh_hk.s_mcu_temp >= 6000 	&& str_obdh_hk.s_mcu_temp < 7500)	g_uc_cw_message[13] = 'Q';
		else if (str_obdh_hk.s_mcu_temp >= 7500 	&& str_obdh_hk.s_mcu_temp < 8500)	g_uc_cw_message[13] = '8';
		else if (str_obdh_hk.s_mcu_temp >= 8500)										g_uc_cw_message[13] = 'Z';

		//Set solar panels temperature
		for (l_uc_i = 0; l_uc_i < 5; l_uc_i++){
			if 		(str_pwrs_hk.uc_sp_temp[l_uc_i] < 20)												g_uc_cw_message[l_uc_i+14] = 'D';
			else if (str_pwrs_hk.uc_sp_temp[l_uc_i] >= 20 	&& str_pwrs_hk.uc_sp_temp[l_uc_i] < 40)		g_uc_cw_message[l_uc_i+14] = 'F';
			else if (str_pwrs_hk.uc_sp_temp[l_uc_i] >= 40 	&& str_pwrs_hk.uc_sp_temp[l_uc_i] < 60)		g_uc_cw_message[l_uc_i+14] = '2';
			else if (str_pwrs_hk.uc_sp_temp[l_uc_i] >= 60 	&& str_pwrs_hk.uc_sp_temp[l_uc_i] < 80)		g_uc_cw_message[l_uc_i+14] = 'G';
			else if (str_pwrs_hk.uc_sp_temp[l_uc_i] >= 80 	&& str_pwrs_hk.uc_sp_temp[l_uc_i] < 100)	g_uc_cw_message[l_uc_i+14] = '4';
			else if (str_pwrs_hk.uc_sp_temp[l_uc_i] >= 100 	&& str_pwrs_hk.uc_sp_temp[l_uc_i] < 120)	g_uc_cw_message[l_uc_i+14] = 'C';
			else if (str_pwrs_hk.uc_sp_temp[l_uc_i] >= 120 	&& str_pwrs_hk.uc_sp_temp[l_uc_i] < 140)	g_uc_cw_message[l_uc_i+14] = '6';
			else if (str_pwrs_hk.uc_sp_temp[l_uc_i] >= 140 	&& str_pwrs_hk.uc_sp_temp[l_uc_i] < 160)	g_uc_cw_message[l_uc_i+14] = 'Q';
			else if (str_pwrs_hk.uc_sp_temp[l_uc_i] >= 160 	&& str_pwrs_hk.uc_sp_temp[l_uc_i] < 180)	g_uc_cw_message[l_uc_i+14] = '8';
			else if (str_pwrs_hk.uc_sp_temp[l_uc_i] >= 180)												g_uc_cw_message[l_uc_i+14] = 'Z';
		}
	}
}

//------------------------------------------------------------------------------------
// To initialize AX25 default beacon message, content, and rate
//------------------------------------------------------------------------------------
//	This function will be called in the beacon handler, whenever beacon handler is
//	in the automatic mode, just before it wants to send AX25 beacon.
//------------------------------------------------------------------------------------
void groundInitAX25Beacon (void)
{
	//Indicates taking data from temporary buffer
	g_uc_ax25_header_package_property = 0x00;
	
	//Taking the data initialization from the command argument
	g_uc_ax25_header_subsystem = OBDH_SUBSYSTEM;
	g_uc_ax25_header_data_type = SD_REAL_TIME;

	//Change beacon interval to 10 sec
	g_uc_beacon_interval = AX25_BEACON_INTERVAL_DEFAULT;
	
	//Initialized sending data (once only)
	g_us_beacon_init_ax25_error_code = initSendData();	
}
#endif
#endif
#endif

#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)
#if (SENDING_ENABLE == 1)
#if (ISIS_CMD_ENABLE == 1)
//------------------------------------------------------------------------------------
// Common sequence to save schedule and send acknowledgement
//------------------------------------------------------------------------------------
void groundSaveScheduleAndAck (void)
{
	#if (STORING_ENABLE == 1)
	#if (SAVE_STATE_ENABLE == 1)
	//Save the current schedule if the upload of a command is successful
	if (!g_us_gsch_error_code) saveDataProtected(OBDH_SUBSYSTEM, SD_SCHEDULE, g_uc_sd_data_buffer);
	#endif	
	#endif

	//Send ACK MSG
	sendAckMsg();
}

//------------------------------------------------------------------------------------
// Common sequence to save essential data
//------------------------------------------------------------------------------------
void groundSaveEssentialAndAck (void)
{
	#if (STORING_ENABLE == 1)
	#if (SAVE_STATE_ENABLE == 1)
	//Save the current schedule if the upload of a command is successful
	if(!g_us_gsch_error_code) saveDataProtected(ALL_SUBSYSTEMS, SD_ESSENTIAL, g_uc_sd_data_buffer);
	#endif	
	#endif

	//Send ACK MSG
	sendAckMsg();
}
#endif
#endif

//------------------------------------------------------------------------------------
// Common sequence to terminate unsuccessful GSCH transitional state
//------------------------------------------------------------------------------------
void groundTerminateTransitionalState (void)
{
	//Reset transitional flag and period
	g_uc_gsch_flag &= ~GSCH_FLAG_MONITOR_TRANSITIONAL;
	g_us_gsch_monitoring_transitional_period = 0;
	
	//Reset all GSCH request related flags
	str_obdh_hk.us_beacon_flag &= ~BEACON_FLAG_GSCH;
}

//------------------------------------------------------------------------------------
// Common sequence to terminate GSCH groundpass
//------------------------------------------------------------------------------------
void groundTerminateGroundPass (void)
{
	//Terminates transitional states
	groundTerminateTransitionalState();

	//Reset all groundpass-related variables
	resetGSCHCounters();
	g_uc_gsch_flag &= ~GSCH_FLAG_GROUNDPASS_STARTED;
	g_us_groundpass_time_register = 0;

	//Go back to idle state
	str_obdh_hk.uc_gsch_state = GSCH_STATE_IDLE;

	//If the current satellite mode is M6, go back to the previous mode,
	//Else, remain in the current mode
	if (str_obdh_hk.uc_sat_mode == SATELLITE_MODE_M6){
		if (str_obdh_hk.uc_sat_mode >= SATELLITE_MODE_M5 && str_obdh_hk.uc_sat_mode <= SATELLITE_MODE_M8 && str_obdh_hk.uc_sat_mode != SATELLITE_MODE_M6)
			str_obdh_hk.uc_sat_mode = str_obdh_data.uc_previous_mode;
		else str_obdh_hk.uc_sat_mode = SATELLITE_MODE_M5;
	}

	//If there is still mission time register, enter M7
	if (g_us_mission_time_register) str_obdh_hk.uc_sat_mode = SATELLITE_MODE_M7;
}

#if (ISIS_CMD_ENABLE == 1)
#if (SENDING_ENABLE == 1)
//------------------------------------------------------------------------------------
// Common sequence to terminate GSCH send real time data
//------------------------------------------------------------------------------------
void groundTerminateSendRealTimeData (void)
{
	//Terminates transitional states
	groundTerminateTransitionalState();

	//Go back to GSCH_STATE_GROUND_PASS state
	str_obdh_hk.uc_gsch_state = GSCH_STATE_GROUND_PASS;
}

//------------------------------------------------------------------------------------
// Common sequence to terminate GSCH send stored data
//------------------------------------------------------------------------------------
void groundTerminateSendStoredData (void)
{
	//Terminates transitional states
	groundTerminateTransitionalState();

	//Reset recovery package block pointer
	str_bp.ul_package_recovery_write_p = SD_BP_PACKAGE_RECOVERY_START;

	//Go back to GSCH_STATE_GROUND_PASS state
	str_obdh_hk.uc_gsch_state = GSCH_STATE_GROUND_PASS;
}

//------------------------------------------------------------------------------------
// Common sequence to terminate GSCH send stored data with acknowledgement message
//------------------------------------------------------------------------------------
void groundTerminateSendStoredDataWithAckMsg (void)
{
	//Reset loading retry counter
	g_uc_gsch_loading_retry_counter = 0;

	//Send loading error to GDSN
	g_us_gsch_error_code = g_us_sd_load_error_code;

	//Reset recovery package block pointer
	str_bp.ul_package_recovery_write_p = SD_BP_PACKAGE_RECOVERY_START;

	//Send down ACK_MSG with error code
	sendAckMsg();
}

//------------------------------------------------------------------------------------
// Common sequence for GSCH to start claiming beacon handler
//------------------------------------------------------------------------------------
void groundStartClaimBeacon (void)
{
	//Set beacon flag to AX25 frame
	str_obdh_hk.us_beacon_flag &= ~BEACON_FLAG_GSCH_TYPE;
	
	//Starts to claim the beacon here to be used by GSCH (don't send anything automatically anymore)
	str_obdh_hk.us_beacon_flag |= BEACON_FLAG_GSCH_CLAIM;
	
	//Set transitional period flag
	g_uc_gsch_flag |= GSCH_FLAG_MONITOR_TRANSITIONAL;	

	#if (EVENT_HANDLER_ENABLE == 1)
	#if (STORING_ENABLE == 1)
	//dec22_yc .... track valid ground station cmd and processing status.
	eventHandler(EV_GSCH_MONITORING_TRANSITIONAL_FLAG_HANDLER,str_obdh_hk.uc_gsch_state,g_uc_gsch_gs_cmd_tail,
						g_uc_gsch_gs_arg[0],g_uc_gsch_gs_arg[1],g_us_gsch_gs_cmd_crc>>16,g_us_gsch_gs_cmd_crc);
	#endif
	#endif								
}

//------------------------------------------------------------------------------------
// Common sequence to enter send real time data state
//------------------------------------------------------------------------------------
void groundEnterSendRealTime (void)
{
	//Claim beaconHandler's use
	groundStartClaimBeacon();

	//Send the data
	str_obdh_hk.uc_gsch_state = GSCH_STATE_SEND_REAL_TIME_DATA;
}

//------------------------------------------------------------------------------------
// Common sequence to enter send stored data state
//------------------------------------------------------------------------------------
void groundEnterSendStored (void)
{
	//Claim beaconHandler's use
	groundStartClaimBeacon();

	//Send the data
	str_obdh_hk.uc_gsch_state = GSCH_STATE_SEND_STORED_DATA;
}

//------------------------------------------------------------------------------------
// Common sequence to start send request (to the beacon handler)
//------------------------------------------------------------------------------------
void groundStartSendRequest (void)
{
	//Reset transitional state monitoring period
	g_us_gsch_monitoring_transitional_period = 0;

	//Set beacon flag to AX25 frame
	str_obdh_hk.us_beacon_flag &= ~BEACON_FLAG_GSCH_TYPE;						

	//Set GSCH send request flag, beacon to send data next loop
	str_obdh_hk.us_beacon_flag |= BEACON_FLAG_GSCH_SEND_REQUEST;
}

//------------------------------------------------------------------------------------
// Common sequence to keep send request (to the beacon handler)
//------------------------------------------------------------------------------------
void groundKeepSendRequest (void)
{
	//Reset transitional state monitoring period
	g_us_gsch_monitoring_transitional_period = 0;

	//Clear the sending completed flag
	str_obdh_hk.us_beacon_flag &= ~BEACON_FLAG_GSCH_SEND_COMPLETED;

	//Set GSCH send request flag, beacon to send data in the next loop
	str_obdh_hk.us_beacon_flag |= BEACON_FLAG_GSCH_SEND_REQUEST;
}

//------------------------------------------------------------------------------------
// Common sequence to terminate claiming beacon 
//------------------------------------------------------------------------------------
void groundReleaseClaimBeacon (void)
{
	//Reset transitional state monitoring period
	g_us_gsch_monitoring_transitional_period = 0;

	//Clear the sending completed flag
	str_obdh_hk.us_beacon_flag &= ~BEACON_FLAG_GSCH_SEND_COMPLETED;

	//Release claim flag for now
	str_obdh_hk.us_beacon_flag &= ~BEACON_FLAG_GSCH_CLAIM;
}
#endif
#endif

#if (SATELLITE_MODE_HANDLER_ENABLE == 1)
#if (OBDH_CMD_ENABLE == 1)
//------------------------------------------------------------------------------------
// Common sequence to enter mission mode
//------------------------------------------------------------------------------------
void groundEnterMissionMode (unsigned short l_us_elapsed_time_s)
{
	#if (IDLE_MODE_HANDLER_ENABLE == 1)
	#if (PWRS_CMD_ENABLE == 1)
	//Disable IMH 
	str_obdh_hk.us_imh_flag &= ~IMH_FLAG_ENABLE;
	#endif
	#endif

	#if (BODY_RATE_HANDLER_ENABLE == 1)
	#if (ADCS_CMD_ENABLE == 1)
	#if (PWRS_CMD_ENABLE == 1)
	//Disable BRH
	str_obdh_hk.us_brh_flag &= ~BRH_FLAG_ENABLE;

	//Reset BRH end flags
	resetBRHEndFlags();
	#endif
	#endif
	#endif

	//Set mission mode time
	g_us_mission_time_register = l_us_elapsed_time_s;

	//Change satellite mode into mission mode
	str_obdh_hk.uc_sat_mode = SATELLITE_MODE_M7;
}	
#endif
#endif
#endif
