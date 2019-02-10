//------------------------------------------------------------------------------------
// Luxp_function.c
//------------------------------------------------------------------------------------
// Copyright (C) 2013 Nanyang Technological University.
//
// AUTH: IAN
// DATE: 03 APR 2013
//
// The program Luxp_function.c will be downloaded to the Pumpkin Motherboard with 
// C8051F120 uC as part of the VELOX-P Pico-Satellite Flight Software. 
//
// Target: C8051F120
//
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include <luxp_function.h>

//------------------------------------------------------------------------------------
// Call function
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
unsigned short functionModule (unsigned char *l_uc_call_function_pointer, unsigned char *l_uc_response_function_pointer)
{
	//All local variables
	//l_uc_counter is removed as it no longer been used (20120328)
	//l_us_msg_length is removed as it is only assigned once but no longer been used (20120328)
	//l_us_checksum_bytes, l_us_checksum_bytes_msg, & l_ul_checksum_bytes are no longer used and removed (20120328)
	//For design consistency, l_us_send_options_bytes is removed as earlier it is put only to ease the programmer to read
	// the option bytes separately from send option command (20120328)
	unsigned char *l_uc_arg_p;
	unsigned char l_uc_command_header_byte;
	unsigned char l_uc_command_tail_byte;
	unsigned short l_us_arg_length;
	unsigned short l_us_arg[2] = 0;
	unsigned short l_us_res_length = 0x0001;
	unsigned short l_us_counter;
	unsigned short l_us_error_code = EC_INIT;
	char SFRPAGE_SAVE = SFRPAGE;

	l_uc_command_header_byte 	= *(l_uc_call_function_pointer+1);
	l_uc_command_tail_byte 		= *(l_uc_call_function_pointer+2);
	l_us_arg_length 			= (unsigned short)(*(l_uc_call_function_pointer+3) << 8) + *(l_uc_call_function_pointer+4);
	l_uc_arg_p 					= l_uc_call_function_pointer+5;

	//Assume successful unless stated otherwise
	g_us_function_module_error_code = EC_SUCCESSFUL;

	switch (l_uc_command_header_byte){

		#if (ISIS_CMD_ENABLE == 1)
		//Communication with IMC board
		case IMC_ADDR:
			switch (l_uc_command_tail_byte){

				case IMC_I2C_RESET:
				case IMC_I2C_RMV_FR:
					g_us_error_code = isisI2C(IMC_ADDR,l_uc_command_tail_byte,DEBUGGING_FEATURE);
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case IMC_I2C_GET_NOFR:
				case IMC_I2C_GET_FR:
				case IMC_I2C_REC_DOFF:
				case IMC_I2C_REC_SSTR:
				case IMC_I2C_TRN_RPOW:
				case IMC_I2C_TRN_FPOW:
				case IMC_I2C_TRN_ICON:
				case IMC_I2C_REC_ICON:
				case IMC_I2C_POW_AMPT:
				case IMC_I2C_POW_BUSV:
				case IMC_I2C_POW_BUSV_ICON:
				case IMC_I2C_REC_UPTM:
					g_us_error_code = isisI2C(IMC_ADDR,l_uc_command_tail_byte,DEBUGGING_FEATURE);
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000F,l_uc_response_function_pointer); //Send: error code, data out, data in, time taken
					break;

				default:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
					#endif

					g_us_error_code = EC_IMC_COMMAND_NOT_FOUND;
					g_us_function_module_error_code = g_us_error_code;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					#endif										
					break;
			}
			break; 

		//Communication with ITC board
		case ITC_ADDR:
			switch (l_uc_command_tail_byte){
				case ITC_I2C_RESET:
					g_us_error_code = isisI2C(ITC_ADDR,l_uc_command_tail_byte,DEBUGGING_FEATURE);
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case ITC_I2C_SND_AX_FR_DFLT:
					#if (DEBUGGING_FEATURE == 1)
					//Overwrite AX.25 buffer
					for (l_us_counter = 0; l_us_counter < l_us_arg_length; l_us_counter++)
						g_uc_ax25_message[l_us_counter] = *(l_uc_arg_p + l_us_counter);
					#endif
					g_us_error_code = isisI2CMsg(l_uc_command_tail_byte,0,l_us_arg_length,DEBUGGING_FEATURE);
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case ITC_I2C_SND_AX_FR_OVRD:
					#if (DEBUGGING_FEATURE == 1)
					//Overwrite AX.25 FROM-TO callsign buffer
					for (l_us_counter = 0; l_us_counter < AX25_CALLSIGN_SIZE; l_us_counter++)
						g_uc_from_to_callsign[l_us_counter] = *(l_uc_arg_p + l_us_counter);
					//Overwrite AX.25 buffer
					for (l_us_counter = AX25_CALLSIGN_SIZE; l_us_counter < l_us_arg_length; l_us_counter++)
						g_uc_ax25_message[l_us_counter-AX25_CALLSIGN_SIZE] = *(l_uc_arg_p + l_us_counter);
					#endif
					g_us_error_code = isisI2CMsg(l_uc_command_tail_byte,0,l_us_arg_length-AX25_CALLSIGN_SIZE,DEBUGGING_FEATURE);
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case ITC_I2C_SND_CW_MSG:
					#if (DEBUGGING_FEATURE == 1)
					//Overwrite CW/Morse buffer
					for (l_us_counter = 0; l_us_counter < l_us_arg_length; l_us_counter++)
						g_uc_cw_message[l_us_counter] = *(l_uc_arg_p + l_us_counter);
					#endif
					g_us_error_code = isisI2CMsg(l_uc_command_tail_byte,0,l_us_arg_length,DEBUGGING_FEATURE);
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case ITC_I2C_SET_AX_BCN_DFLT:
					//Prepare beacon period
					l_us_arg[0] = (unsigned short)((*l_uc_arg_p) << 8) + *(l_uc_arg_p + 1);
					#if (DEBUGGING_FEATURE == 1)
					//Overwrite AX.25 buffer
					for (l_us_counter = 2; l_us_counter < l_us_arg_length+2; l_us_counter++)
						g_uc_ax25_message[l_us_counter-2] = *(l_uc_arg_p + l_us_counter);
					#endif
					g_us_error_code = isisI2CMsg(l_uc_command_tail_byte,l_us_arg[0],l_us_arg_length-2,DEBUGGING_FEATURE);
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case ITC_I2C_SET_AX_BCN_OVRD:
					//Prepare beacon period
					l_us_arg[0] = (unsigned short)((*l_uc_arg_p) << 8) + *(l_uc_arg_p + 1);
					#if (DEBUGGING_FEATURE == 1)
					//Overwrite AX.25 FROM-TO callsign buffer
					for (l_us_counter = 2; l_us_counter < AX25_CALLSIGN_SIZE+2; l_us_counter++)
						g_uc_from_to_callsign[l_us_counter-2] = *(l_uc_arg_p + l_us_counter);
					//Overwrite AX.25 buffer
					for (l_us_counter = AX25_CALLSIGN_SIZE+2; l_us_counter < l_us_arg_length; l_us_counter++)
						g_uc_ax25_message[l_us_counter-AX25_CALLSIGN_SIZE-2] = *(l_uc_arg_p + l_us_counter);
					#endif
					g_us_error_code = isisI2CMsg(l_uc_command_tail_byte,l_us_arg[0],l_us_arg_length-AX25_CALLSIGN_SIZE-2,DEBUGGING_FEATURE);
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case ITC_I2C_SET_CW_BCN:
					//Prepare beacon period
					l_us_arg[0] = (unsigned short)((*l_uc_arg_p) << 8) + *(l_uc_arg_p + 1);
					#if (DEBUGGING_FEATURE == 1)
					//Overwrite CW/Morse buffer
					for (l_us_counter = 2; l_us_counter < l_us_arg_length; l_us_counter++)
						g_uc_cw_message[l_us_counter-2] = *(l_uc_arg_p + l_us_counter);
					#endif
					g_us_error_code = isisI2CMsg(l_uc_command_tail_byte,l_us_arg[0],l_us_arg_length-2,DEBUGGING_FEATURE);
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case ITC_I2C_CLR_BCN:
					g_us_error_code = isisI2C(ITC_ADDR,l_uc_command_tail_byte,DEBUGGING_FEATURE);
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case ITC_I2C_SET_DFLT_AX_TO:
					#if (DEBUGGING_FEATURE == 1)
					//Change AX.25 TO callsign
					for (l_us_counter = 0; l_us_counter < 7; l_us_counter++)
						g_uc_from_to_callsign[l_us_counter+7] = *(l_uc_arg_p + l_us_counter);
					#endif
					g_us_error_code = isisI2CMsg(l_uc_command_tail_byte,0,0,DEBUGGING_FEATURE);
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case ITC_I2C_SET_DFLT_AX_FROM:
					#if (DEBUGGING_FEATURE == 1)
					//Change AX.25 FROM callsign
					for (l_us_counter = 0; l_us_counter < 7; l_us_counter++)
						g_uc_from_to_callsign[l_us_counter] = *(l_uc_arg_p + l_us_counter);
					#endif
					g_us_error_code = isisI2CMsg(l_uc_command_tail_byte,0,0,DEBUGGING_FEATURE);
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case ITC_I2C_SET_TRN_IDLE:
				case ITC_I2C_SET_TRN_OUTM:
				case ITC_I2C_SET_AX_TRN_BRT:
					//Prepare transmitter idle state/transmitter output mode/transmitter ax25 bitrate parameter
					l_us_arg[0] = *l_uc_arg_p;
					g_us_error_code = isisI2CPar(l_uc_command_tail_byte,l_us_arg[0],DEBUGGING_FEATURE);
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case ITC_I2C_SET_CW_CHAR_RT:
					//Prepare transmitter cw char rate value (dot(s)/s) parameter
					l_us_arg[0] = (unsigned short)((*l_uc_arg_p) << 8) + *(l_uc_arg_p + 1);
					g_us_error_code = isisI2CPar(l_uc_command_tail_byte,l_us_arg[0],DEBUGGING_FEATURE);
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case ITC_I2C_TRN_UPTM:
				case ITC_I2C_TRN_STATE:
					g_us_error_code = isisI2C(ITC_ADDR,l_uc_command_tail_byte,DEBUGGING_FEATURE);
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000F,l_uc_response_function_pointer); //Send: error code, data out, data in, time taken
					break;

				default:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
					#endif

					g_us_error_code = EC_ITC_COMMAND_NOT_FOUND;
					g_us_function_module_error_code = g_us_error_code;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					#endif										
					break;
			}
			break;

		#endif

		#if (ADCS_CMD_ENABLE == 1)
		//Communication with ADCS board
		case ADCS_ADDR:
			switch (l_uc_command_tail_byte){

				case ADCS_I2C_GET_HK:
				case ADCS_I2C_GET_SSCP:
				case ADCS_I2C_SET_STANDBY:
				case ADCS_I2C_SET_SELFTEST:
					g_us_error_code = adcsI2C(l_uc_command_tail_byte,DEBUGGING_FEATURE);
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000F,l_uc_response_function_pointer); //Send: error code, data out, data in, time taken
					break;

				case ADCS_I2C_SET_GPS_ON:
				case ADCS_I2C_SET_GPS_OFF:
				case ADCS_I2C_RESET_SW:
				case ADCS_I2C_RESET_HW:
					g_us_error_code = adcsI2C(l_uc_command_tail_byte,DEBUGGING_FEATURE);
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case ADCS_I2C_GET_DATA:
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = adcsGetData(*l_uc_arg_p,DEBUGGING_FEATURE);
					#else
					g_us_error_code = adcsGetData(*l_uc_arg_p);
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000F,l_uc_response_function_pointer); //Send: error code, data out, data in, time taken
					break;

				case ADCS_I2C_SET_MODE:
					l_us_arg[0] = (unsigned short)((*(l_uc_arg_p+1)) << 8) + *(l_uc_arg_p + 2);
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = adcsSetMode(*l_uc_arg_p,l_us_arg[0],DEBUGGING_FEATURE);
					#else
					g_us_error_code = adcsSetMode(*l_uc_arg_p,l_us_arg[0]);
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case ADCS_I2C_SET_SS_THRESHOLD:
					l_us_arg[0] = (unsigned short)((*l_uc_arg_p) << 8) + *(l_uc_arg_p + 1);
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = adcsSetSSThreshold((short)l_us_arg[0],DEBUGGING_FEATURE);
					#else
					g_us_error_code = adcsSetSSThreshold((short)l_us_arg[0]);
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case ADCS_I2C_SET_CGAIN:
					#if (DEBUGGING_FEATURE == 1)
					//Update ADCS Gain
					for (l_us_counter = 0; l_us_counter < NO_OF_ADCS_CGAIN; l_us_counter++)
						str_adcs_par.f_cgain[l_us_counter] = uchar2float(*(l_uc_arg_p + l_us_counter*4),*(l_uc_arg_p + l_us_counter*4 + 1),*(l_uc_arg_p + l_us_counter*4 + 2),*(l_uc_arg_p + l_us_counter*4 + 3));
					#endif
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = adcsSetCGain(DEBUGGING_FEATURE);
					#else
					g_us_error_code = adcsSetCGain();
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case ADCS_I2C_SET_TLE:
					#if (DEBUGGING_FEATURE == 1)
					//Update ADCS TLE
					str_adcs_par.f_tle_epoch = uchar2float(*l_uc_arg_p,*(l_uc_arg_p + 1),*(l_uc_arg_p + 2),*(l_uc_arg_p + 3));
					str_adcs_par.f_tle_inc = uchar2float(*(l_uc_arg_p + 4),*(l_uc_arg_p + 5),*(l_uc_arg_p + 6),*(l_uc_arg_p + 7));
					str_adcs_par.f_tle_raan = uchar2float(*(l_uc_arg_p + 8),*(l_uc_arg_p + 9),*(l_uc_arg_p + 10),*(l_uc_arg_p + 11));
					str_adcs_par.f_tle_arg_per = uchar2float(*(l_uc_arg_p + 12),*(l_uc_arg_p + 13),*(l_uc_arg_p + 14),*(l_uc_arg_p + 15));
					str_adcs_par.f_tle_ecc = uchar2float(*(l_uc_arg_p + 16),*(l_uc_arg_p + 17),*(l_uc_arg_p + 18),*(l_uc_arg_p + 19));
					str_adcs_par.f_tle_ma = uchar2float(*(l_uc_arg_p + 20),*(l_uc_arg_p + 21),*(l_uc_arg_p + 22),*(l_uc_arg_p + 23));
					str_adcs_par.us_tle_mm = (unsigned short)((*(l_uc_arg_p+24)) << 8) + *(l_uc_arg_p + 25);
					#endif
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = adcsSetTLE(DEBUGGING_FEATURE);
					#else
					g_us_error_code = adcsSetTLE();
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case ADCS_I2C_SET_TIME:
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = adcsSetTime(uchar2ulong(*l_uc_arg_p,*(l_uc_arg_p + 1),*(l_uc_arg_p + 2),*(l_uc_arg_p + 3)),DEBUGGING_FEATURE);
					#else
					g_us_error_code = adcsSetTime(uchar2ulong(*l_uc_arg_p,*(l_uc_arg_p + 1),*(l_uc_arg_p + 2),*(l_uc_arg_p + 3)));
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case ADCS_I2C_SET_VECTOR_ALIGN2SUN:
					#if (DEBUGGING_FEATURE == 1)
					//Update ADCS vector align to sun
					str_adcs_par.s_vector_2sun[0] = (unsigned short)((*l_uc_arg_p) << 8) + *(l_uc_arg_p+1);
					str_adcs_par.s_vector_2sun[1] = (unsigned short)((*(l_uc_arg_p+2)) << 8) + *(l_uc_arg_p+3);
					str_adcs_par.s_vector_2sun[2] = (unsigned short)((*(l_uc_arg_p+4)) << 8) + *(l_uc_arg_p+5);
					#endif
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = adcsSetVectorAlignToSun(DEBUGGING_FEATURE);
					#else
					g_us_error_code = adcsSetVectorAlignToSun();
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case ADCS_I2C_SET_TOGGLE_IMU:
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = adcsToggleIMU(*l_uc_arg_p,DEBUGGING_FEATURE);
					#else
					g_us_error_code = adcsToggleIMU(*l_uc_arg_p);
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case ADCS_I2C_SET_BODY_RATE:
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = adcsSetBodyRate(*l_uc_arg_p,*(l_uc_arg_p+1),*(l_uc_arg_p+2),DEBUGGING_FEATURE);
					#else
					g_us_error_code = adcsSetBodyRate(*l_uc_arg_p,*(l_uc_arg_p+1),*(l_uc_arg_p+2));
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer);
					break;

				case ADCS_I2C_SET_DGAIN:
					l_us_arg[0] = (unsigned short)((*l_uc_arg_p) << 8) + *(l_uc_arg_p+1);
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = adcsSetDGain((short)l_us_arg[0],DEBUGGING_FEATURE);
					#else
					g_us_error_code = adcsSetDGain((short)l_us_arg[0]);
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				default:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
					#endif

					g_us_error_code = EC_ADCS_COMMAND_NOT_FOUND;
					g_us_function_module_error_code = g_us_error_code;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					#endif										
					break;
			}
			break;

		#endif

		#if (PWRS_CMD_ENABLE == 1)
		//Communication with PWRS board
		case PWRS_ADDR:
			switch (l_uc_command_tail_byte){
				case PWRS_I2C_GET_HK:
				case PWRS_I2C_GET_SSCP:
					g_us_error_code = pwrsI2C(l_uc_command_tail_byte,DEBUGGING_FEATURE);
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000F,l_uc_response_function_pointer); //Send: error code, data out, data in, time taken
					break;

				case PWRS_I2C_GET_DATA:
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = pwrsGetData(*l_uc_arg_p,DEBUGGING_FEATURE);
					#else
					g_us_error_code = pwrsGetData(*l_uc_arg_p);
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000F,l_uc_response_function_pointer); //Send: error code, data out, data in, time taken
					break;

				case PWRS_I2C_SET_HT:
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = pwrsUpdateBatteryHeater(*l_uc_arg_p,DEBUGGING_FEATURE);
					#else
					g_us_error_code = pwrsUpdateBatteryHeater(*l_uc_arg_p);
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case PWRS_I2C_SET_OUTPUT:
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = pwrsSetOutput(*l_uc_arg_p,DEBUGGING_FEATURE);
					#else
					g_us_error_code = pwrsSetOutput(*l_uc_arg_p);
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case PWRS_I2C_SET_SINGLE_OUTPUT:
					//Set delay time
					l_us_arg[0] = (unsigned short)((*(l_uc_arg_p+2)) << 8) + *(l_uc_arg_p+3);
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = pwrsSetSingleOutput(*l_uc_arg_p,*(l_uc_arg_p+1),l_us_arg[0],DEBUGGING_FEATURE);
					#else
					g_us_error_code = pwrsSetSingleOutput(*l_uc_arg_p,*(l_uc_arg_p+1),l_us_arg[0]);
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case PWRS_I2C_SET_MPPT_MODE:
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = pwrsSetMPPTMode(*l_uc_arg_p,DEBUGGING_FEATURE);
					#else
					g_us_error_code = pwrsSetMPPTMode(*l_uc_arg_p);
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case PWRS_I2C_SET_PV_VOLT:
					#if (DEBUGGING_FEATURE == 1)
					//Set PV volt
					str_pwrs_data.us_pv_fv[0] = (unsigned short)((*l_uc_arg_p) << 8) + *(l_uc_arg_p+1);
					str_pwrs_data.us_pv_fv[1] = (unsigned short)((*(l_uc_arg_p+2)) << 8) + *(l_uc_arg_p+3);
					str_pwrs_data.us_pv_fv[2] = (unsigned short)((*(l_uc_arg_p+4)) << 8) + *(l_uc_arg_p+5);
					#endif
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = pwrsSetPVVolt(DEBUGGING_FEATURE);
					#else
					g_us_error_code = pwrsSetPVVolt();
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case PWRS_I2C_SET_TK_OUTPUT:
					//Set delay time
					l_us_arg[0] = (unsigned short)((*(l_uc_arg_p+2)) << 8) + *(l_uc_arg_p+3);
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = pwrsSetTKOutput(*l_uc_arg_p,*(l_uc_arg_p+1),l_us_arg[0],DEBUGGING_FEATURE);
					#else
					g_us_error_code = pwrsSetTKOutput(*l_uc_arg_p,*(l_uc_arg_p+1),l_us_arg[0]);
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case PWRS_I2C_RESET_PWRS:
					//Set delay time
					l_us_arg[0] = (unsigned short)((*l_uc_arg_p) << 8) + *(l_uc_arg_p+1);
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = pwrsResetPWRS(l_us_arg[0],DEBUGGING_FEATURE);
					#else
					g_us_error_code = pwrsResetPWRS(l_us_arg[0]);
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case PWRS_I2C_RESET_OBDH:
					//Set delay time
					l_us_arg[0] = (unsigned short)((*l_uc_arg_p) << 8) + *(l_uc_arg_p+1);
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = pwrsResetOBDH(l_us_arg[0],DEBUGGING_FEATURE);
					#else
					g_us_error_code = pwrsResetOBDH(l_us_arg[0]);
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case PWRS_I2C_SET_SOC_LIMIT:
					//Set SOC ON and OFF limit
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = pwrsSetSOCLimit(*l_uc_arg_p,*(l_uc_arg_p+1),DEBUGGING_FEATURE);
					#else
					g_us_error_code = pwrsSetSOCLimit(*l_uc_arg_p,*(l_uc_arg_p+1));
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case PWRS_I2C_SET_WDT_LIMIT:
					//Set WDT time limit
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = pwrsSetWDTLimit(uchar2ulong(*l_uc_arg_p,*(l_uc_arg_p+1),*(l_uc_arg_p+2),*(l_uc_arg_p+3)),DEBUGGING_FEATURE);
					#else
					g_us_error_code = pwrsSetWDTLimit(uchar2ulong(*l_uc_arg_p,*(l_uc_arg_p+1),*(l_uc_arg_p+2),*(l_uc_arg_p+3)));
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case PWRS_I2C_EEPROM_INIT:
					//Reset PWRS EEPROM value to its default
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = pwrsInitEeprom(DEBUGGING_FEATURE);
					#else
					g_us_error_code = pwrsInitEeprom();
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case PWRS_I2C_SET_PWRS_SDC:
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = pwrsSetSDC(*l_uc_arg_p,DEBUGGING_FEATURE);
					#else
					g_us_error_code = pwrsSetSDC(*l_uc_arg_p);
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				default:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
					#endif

					g_us_error_code = EC_PWRS_COMMAND_NOT_FOUND;
					g_us_function_module_error_code = g_us_error_code;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					#endif										
					break;
			}
			break;

		#endif

		#if (RTC_CMD_ENABLE == 1)
		//Communication with RTC
		case RTC_ADDR:
			switch (l_uc_command_tail_byte){
				case RTC_I2C_GET_TIME:
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = rtcGetTime(DEBUGGING_FEATURE);
					#else
					g_us_error_code = rtcGetTime();
					#endif
					g_us_function_module_error_code = g_us_error_code;					
					l_us_res_length = sendOptions(0x000B,l_uc_response_function_pointer); //Send: error code, data in, time taken
					break;

				case RTC_I2C_DATE_INIT:					
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = rtcDateInit(DEBUGGING_FEATURE);
					#else
					g_us_error_code = rtcDateInit();
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case RTC_I2C_UPDATE_DATE:					
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = rtcUpdateDate(*l_uc_arg_p,*(l_uc_arg_p+1),*(l_uc_arg_p+2),*(l_uc_arg_p+3),*(l_uc_arg_p+4),*(l_uc_arg_p+5),*(l_uc_arg_p+6), DEBUGGING_FEATURE);
					#else
					g_us_error_code = rtcUpdateDate(*l_uc_arg_p,*(l_uc_arg_p+1),*(l_uc_arg_p+2),*(l_uc_arg_p+3),*(l_uc_arg_p+4),*(l_uc_arg_p+5),*(l_uc_arg_p+6));
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				default:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
					#endif

					g_us_error_code = EC_RTC_COMMAND_NOT_FOUND;
					g_us_function_module_error_code = g_us_error_code;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					#endif										
					break;
			}
			break;

		#endif

		#if (ANTENNA_CMD_ENABLE == 1)
		//Communication with Antenna deployment board
		case ANTENNA_1_ADDR:
		case ANTENNA_2_ADDR:
			switch (l_uc_command_tail_byte){
				case ANTENNA_I2C_RESET:
				case ANTENNA_I2C_ARM:
				case ANTENNA_I2C_DISARM:
				case ANTENNA_I2C_CANCEL_DPLY:
					g_us_error_code = antennaI2C (l_uc_command_header_byte,l_uc_command_tail_byte,DEBUGGING_FEATURE);
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				case ANTENNA_I2C_GET_TEMP:
				case ANTENNA_I2C_GET_DPLY_STAT:
				case ANTENNA_I2C_GET_DPLY_1_COUNT:
				case ANTENNA_I2C_GET_DPLY_2_COUNT:
				case ANTENNA_I2C_GET_DPLY_3_COUNT:
				case ANTENNA_I2C_GET_DPLY_4_COUNT:
				case ANTENNA_I2C_GET_DPLY_1_TIME:
				case ANTENNA_I2C_GET_DPLY_2_TIME:
				case ANTENNA_I2C_GET_DPLY_3_TIME:
				case ANTENNA_I2C_GET_DPLY_4_TIME:
					g_us_error_code = antennaI2C (l_uc_command_header_byte,l_uc_command_tail_byte,DEBUGGING_FEATURE);
					g_us_function_module_error_code = g_us_error_code;					
					l_us_res_length = sendOptions(0x000F,l_uc_response_function_pointer); //Send: error code, data out, data in, time taken
					break;

				case ANTENNA_I2C_DPLY_1_DFLT:
				case ANTENNA_I2C_DPLY_2_DFLT:
				case ANTENNA_I2C_DPLY_3_DFLT:
				case ANTENNA_I2C_DPLY_4_DFLT:
				case ANTENNA_I2C_AUTO_DPLY:
				case ANTENNA_I2C_DPLY_1_OVRD:
				case ANTENNA_I2C_DPLY_2_OVRD:
				case ANTENNA_I2C_DPLY_3_OVRD:
				case ANTENNA_I2C_DPLY_4_OVRD:
					#if (DEBUGGING_FEATURE == 1)
					g_us_error_code = antennaI2CPar (l_uc_command_header_byte,l_uc_command_tail_byte,*l_uc_arg_p,DEBUGGING_FEATURE);
					#else
					g_us_error_code = antennaI2CPar (l_uc_command_header_byte,l_uc_command_tail_byte,*l_uc_arg_p);
					#endif
					g_us_function_module_error_code = g_us_error_code;
					l_us_res_length = sendOptions(0x000D,l_uc_response_function_pointer); //Send: error code, data out, time taken
					break;

				default:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
					#endif

					g_us_error_code = EC_ANTENNA_COMMAND_NOT_FOUND;
					g_us_function_module_error_code = g_us_error_code;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					#endif
					break;
			}
			break;

		#endif

		#if (OBDH_CMD_ENABLE == 1)
		//Basic Commands to OBC
		case OBDH_ADDR:
			switch (l_uc_command_tail_byte){

				#if (SATELLITE_SCHEDULE_HANDLER_ENABLE == 1)
				case OBDH_CMD_REMOVE_SCHEDULE:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
					#endif

					removeSchedule(*l_uc_arg_p,*(l_uc_arg_p+1));
					g_us_error_code = EC_SUCCESSFUL;
					g_us_function_module_error_code = EC_SUCCESSFUL;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					l_us_res_length = fillData (l_uc_response_function_pointer, l_us_res_length, DATA_CAT_SCHEDULE, 1);  //Send: schedule (first ten)
					//----------------
					//Debugging block
					#endif
					break;
				#endif

				case OBDH_CMD_RESET_I2C_COMMUNICATION:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
					#endif

					resetI2CCommunication();
					g_us_error_code = EC_SUCCESSFUL;
					g_us_function_module_error_code = EC_SUCCESSFUL;


					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					#endif
					break;

				#if (TASK_ENABLE == 1)
				case OBDH_CMD_RESET_TASK:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					l_us_arg[0] = *l_uc_arg_p;
					startTiming();
					//----------------
					//Debugging block
					#endif

					resetTask(*l_uc_arg_p);
					g_us_error_code = EC_SUCCESSFUL;
					g_us_function_module_error_code = EC_SUCCESSFUL;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					l_us_res_length = fillData (l_uc_response_function_pointer, l_us_res_length, DATA_CAT_SINGLE_TASK_STRUCTURE, l_us_arg[0]);  //Send: task structure (as the input)
					//----------------
					//Debugging block
					#endif
					break;

				case OBDH_CMD_COMMAND_TASK:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
					#endif

					setTaskFlags(*l_uc_arg_p,*(l_uc_arg_p+1),0xF0);
					g_us_error_code = EC_SUCCESSFUL;
					g_us_function_module_error_code = EC_SUCCESSFUL;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					#endif
					break;
				#endif

				#if (STORING_ENABLE == 1)
				case OBDH_CMD_INIT_SD:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
					#endif

					g_us_error_code = initSD();
					g_us_function_module_error_code = g_us_error_code;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					#endif
					break;

				case OBDH_CMD_SD_ERASE_SINGLE_BLOCK:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
					#endif

					g_ul = uchar2ulong(*l_uc_arg_p,*(l_uc_arg_p+1),*(l_uc_arg_p+2),*(l_uc_arg_p+3));
					
					//Only allows to erase non-essential block
					if (g_ul > SD_BP_OBDH_ORIGINAL_ESS_END && g_ul <= SD_BP_MAX)
						g_us_error_code = sdEraseSingleBlock (g_ul);
					else g_us_error_code = EC_SD_CARD_BLOCK_UNACCESSIBLE_TO_BE_ERASED;
					g_us_function_module_error_code = g_us_error_code;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					#endif
					break;
				#endif

				case OBDH_CMD_GET_HK:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
					#endif

					getHK();
					g_us_error_code = EC_SUCCESSFUL;
					g_us_function_module_error_code = g_us_error_code;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					l_us_res_length = fillData (l_uc_response_function_pointer, l_us_res_length, DATA_CAT_LATEST_HK_SAMPLING_TIME_AND_ORBIT_TAI_FORMAT, 0);
					l_us_res_length = fillData (l_uc_response_function_pointer, l_us_res_length, DATA_CAT_LAST_HK_EC, 0);
					l_us_res_length = fillData (l_uc_response_function_pointer, l_us_res_length, DATA_CAT_PWRS_HK, 0);
					l_us_res_length = fillData (l_uc_response_function_pointer, l_us_res_length, DATA_CAT_ADCS_HK, 0);
					l_us_res_length = fillData (l_uc_response_function_pointer, l_us_res_length, DATA_CAT_COMM_HK, 0);
					l_us_res_length = fillData (l_uc_response_function_pointer, l_us_res_length, DATA_CAT_OBDH_HK, 0);
					//----------------
					//Debugging block
					#endif
					break;

				#if (STORING_ENABLE == 1)
				case OBDH_CMD_STORE_HK:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
					#endif

					storeHK();
					g_us_error_code = EC_SUCCESSFUL;
					g_us_function_module_error_code = g_us_error_code;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					l_us_res_length = fillData (l_uc_response_function_pointer, l_us_res_length, DATA_CAT_BUF_SD_DATA_IN, 0);
					//----------------
					//Debugging block
					#endif
					break;
				#endif

				#if (SATELLITE_MODE_HANDLER_ENABLE == 1)
				case OBDH_CMD_ENTER_LEOP_IDLE:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
					#endif

					#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)
					groundTerminateGroundPass();
					#endif
					enterM3();
					g_us_error_code = EC_SUCCESSFUL;
					g_us_function_module_error_code = g_us_error_code;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					#endif
					break;

				case OBDH_CMD_ENTER_LEOP_PWRS_SAFE_HOLD:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
					#endif

					#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)
					groundTerminateGroundPass();
					#endif
					enterM4();
					g_us_error_code = EC_SUCCESSFUL;
					g_us_function_module_error_code = g_us_error_code;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					#endif
					break;

				case OBDH_CMD_ENTER_NOP_IDLE:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
					#endif

					#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)
					groundTerminateGroundPass();
					#endif
					enterM5();
					g_us_error_code = EC_SUCCESSFUL;
					g_us_function_module_error_code = g_us_error_code;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					#endif
					break;
				#endif

				#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)
				case OBDH_CMD_ENTER_GROUNDPASS:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
					#endif

					enterGroundPass();
					g_us_error_code = EC_SUCCESSFUL;
					g_us_function_module_error_code = g_us_error_code;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					#endif
					break;
				#endif

				#if (SATELLITE_MODE_HANDLER_ENABLE == 1)
				#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)
				case OBDH_CMD_ENTER_MISSION_MODE:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
					#endif

					#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)
					groundTerminateGroundPass();
					#endif
					l_us_arg[0] = (unsigned short)(*l_uc_arg_p << 8) + *(l_uc_arg_p+1);
					groundEnterMissionMode(l_us_arg[0]);
					g_us_error_code = EC_SUCCESSFUL;
					g_us_function_module_error_code = g_us_error_code;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					#endif
					break;
				#endif

				case OBDH_CMD_ENTER_NOP_PWRS_SAFE_HOLD:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
					#endif

					#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)
					groundTerminateGroundPass();
					#endif
					enterM8();
					g_us_error_code = EC_SUCCESSFUL;
					g_us_function_module_error_code = g_us_error_code;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					#endif
					break;
				#endif

				#if (BODY_RATE_HANDLER_ENABLE == 1)
				#if (ADCS_CMD_ENABLE == 1)
				#if (PWRS_CMD_ENABLE == 1)
				case OBDH_CMD_SET_BRH:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
					#endif

					if (*l_uc_arg_p == 0xff) 	str_obdh_hk.us_brh_flag |= BRH_FLAG_ENABLE;
					else if (*l_uc_arg_p == 0) 	str_obdh_hk.us_brh_flag &= ~BRH_FLAG_ENABLE;
					l_us_arg[0] = str_obdh_hk.us_brh_flag;
					g_us_error_code = EC_SUCCESSFUL;
					g_us_function_module_error_code = g_us_error_code;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					#endif
					break;
				#endif
				#endif
				#endif

				#if (IDLE_MODE_HANDLER_ENABLE == 1)
				#if (PWRS_CMD_ENABLE == 1)
				case OBDH_CMD_SET_IMH:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
					#endif

					if (*l_uc_arg_p == 0xff) 	str_obdh_hk.us_imh_flag |= IMH_FLAG_ENABLE;
					else if (*l_uc_arg_p == 0) 	str_obdh_hk.us_imh_flag &= ~IMH_FLAG_ENABLE;
					l_us_arg[0] = str_obdh_hk.us_imh_flag;
					g_us_error_code = EC_SUCCESSFUL;
					g_us_function_module_error_code = g_us_error_code;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					#endif
					break;
				#endif
				#endif

				#if (SATELLITE_SCHEDULE_HANDLER_ENABLE == 1)
				case OBDH_CMD_TURN_ON_ADCS:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
					#endif

					g_us_error_code = scheduleTurnOnADCS(str_obdh_data.ul_obc_time_s+2);
					g_us_function_module_error_code = g_us_error_code;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					#endif
					break;
				#endif

				#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)
				case OBDH_CMD_TERMINATE_GROUNDPASS:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
					#endif

					groundTerminateGroundPass();
					g_us_error_code = EC_SUCCESSFUL;
					g_us_function_module_error_code = EC_SUCCESSFUL;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					#endif
					break;
				#endif

				#if (EXTRA_ENABLE == 1)
				case OBDH_CMD_SW_RESET:
				case OBDH_CMD_HW_RESET:
				case OBDH_CMD_RESET_SAT:
					//Ian (20120905 11:11AM): This condition is added to further distinguish the cause of reset of the satellite
					//For many will call reset with OBDH_CMD_RESET_SAT argument, not just OBDH command
					if (OBDH_CMD_RESET_SAT == l_uc_command_tail_byte) str_obdh_data.uc_sat_rstsrc = RESET_SOURCE_OBDH_COMMAND;
					callReset(l_uc_command_tail_byte);
					break;
				#endif

				#if (OBDH_HDN_CMD_ENABLE == 1)
				#if (DEBUGGING_FEATURE == 1)
				case OBDH_HDN_CMD_WAIT_MS:
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block

					waitMs(*l_uc_arg_p);
					g_us_error_code = EC_SUCCESSFUL;
					g_us_function_module_error_code = EC_SUCCESSFUL;

					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					break;
				#endif

				#if (DEBUGGING_FEATURE == 1)
				case OBDH_HDN_CMD_WAIT_S:
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block

					waitS(*l_uc_arg_p);
					g_us_error_code = EC_SUCCESSFUL;
					g_us_function_module_error_code = EC_SUCCESSFUL;

					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					break;
				#endif

				case OBDH_HDN_CMD_PING:
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block

					g_us_error_code = EC_SUCCESSFUL;
					g_us_function_module_error_code = EC_SUCCESSFUL;

					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					break;

				#if (STORING_ENABLE == 1)
				#if (DEBUGGING_FEATURE == 1)
				case OBDH_HDN_CMD_SEARCH_BP_BY_ORBIT_INFO:
					//Debugging block
					//----------------
					g_us_obs_first_orbit_request = (unsigned short)(*(l_uc_arg_p+1) << 8) + *(l_uc_arg_p+2);
					g_us_obs_last_orbit_request = (unsigned short)(*(l_uc_arg_p+3) << 8) + *(l_uc_arg_p+4);
					startTiming();
					//----------------
					//Debugging block

					g_us_error_code = searchBPByOrbitInfo (*l_uc_arg_p);
					g_us_function_module_error_code = g_us_error_code;

					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					l_us_res_length = fillData (l_uc_response_function_pointer, l_us_res_length, DATA_CAT_OBS_VAR, 0);
					//----------------
					//Debugging block
					break;
				#endif
				#endif

				case OBDH_HDN_CMD_CHANGE_DATA:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
					#endif

					l_us_arg[0] = (unsigned short)((*l_uc_arg_p) << 8) + *(l_uc_arg_p+1);
					changeData (l_us_arg[0], *(l_uc_arg_p+2), *(l_uc_arg_p+3), *(l_uc_arg_p+4), *(l_uc_arg_p+5));
					g_us_error_code = EC_SUCCESSFUL;
					g_us_function_module_error_code = EC_SUCCESSFUL;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					#endif
					break;

				#if (TASK_ENABLE == 1)
				case OBDH_HDN_CMD_ENTER_SCRIPTLINE:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
 					l_us_arg[0] = *l_uc_arg_p;
					startTiming();
					//----------------
					//Debugging block
					#endif

					enterScriptline (*l_uc_arg_p, *(l_uc_arg_p+1), *(l_uc_arg_p+2), *(l_uc_arg_p+3), *(l_uc_arg_p+4), *(l_uc_arg_p+5));
					g_us_error_code = EC_SUCCESSFUL;
					g_us_function_module_error_code = EC_SUCCESSFUL;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					l_us_res_length = fillData (l_uc_response_function_pointer, l_us_res_length, DATA_CAT_SINGLE_SCRIPTSPACE, l_us_arg[0]);  //Send: task script (as the input)
					//----------------
					//Debugging block
					#endif
					break;
				#endif

				#if (STORING_ENABLE == 1)
				#if (DEBUGGING_FEATURE == 1)
				case OBDH_HDN_CMD_SD_READ_SINGLE_BLOCK:
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block

					l_us_arg[0] = (unsigned short)((*l_uc_arg_p) << 8) + *(l_uc_arg_p+1); //dec_yc requires *l_uc_arg_p,*(l_uc_arg_p+1)
					l_us_arg[1] = (unsigned short)((*l_uc_arg_p+2) << 8) + *(l_uc_arg_p+3); //dec_yc requires *(l_uc_arg_p+2),*(l_uc_arg_p+3)
					g_us_error_code = sdReadSingleBlock (uchar2ulong(*l_uc_arg_p,*(l_uc_arg_p+1),*(l_uc_arg_p+2),*(l_uc_arg_p+3)));
					g_us_function_module_error_code = g_us_error_code;

					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0409,l_uc_response_function_pointer); //Send: error code, time taken, SD data in
					//----------------
					//Debugging block
					break;
				#endif

				#if (DEBUGGING_FEATURE == 1)
				case OBDH_HDN_CMD_SD_WRITE_SINGLE_BLOCK:
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block

					l_us_arg[0] = (unsigned short)((*l_uc_arg_p) << 8) + *(l_uc_arg_p+1); //dec_yc requires *l_uc_arg_p,*(l_uc_arg_p+1)
					l_us_arg[1] = (unsigned short)((*l_uc_arg_p+2) << 8) + *(l_uc_arg_p+3); //dec_yc requires *(l_uc_arg_p+2),*(l_uc_arg_p+3)
					g_us_error_code = sdWriteSingleBlock (uchar2ulong(*l_uc_arg_p,*(l_uc_arg_p+1),*(l_uc_arg_p+2),*(l_uc_arg_p+3)), g_uc_sd_data_buffer, SD_CARD_BUFFER_SIZE);
					g_us_function_module_error_code = g_us_error_code;

					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0809,l_uc_response_function_pointer); //Send: error code, time taken, SD data out
					//----------------
					//Debugging block
					break;
				#endif

				#if (DEBUGGING_FEATURE == 1)
				case OBDH_HDN_CMD_SAVE_DATA_PROTECTED:
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block

					g_us_error_code = saveDataProtected (*l_uc_arg_p,*(l_uc_arg_p+1),g_uc_sd_data_buffer);
					l_us_arg[0] = (unsigned short)((*l_uc_arg_p) << 8) + *(l_uc_arg_p+1); //dec_yc requires *l_uc_arg_p,*(l_uc_arg_p+1)
					g_us_function_module_error_code = g_us_error_code; //dec_yc requires *l_uc_arg_p,*(l_uc_arg_p+1)

					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken.
					l_us_res_length = fillData (l_uc_response_function_pointer, l_us_res_length, DATA_CAT_SD_HEADER, 0);  //Send: SD card headers
					l_us_res_length = fillData (l_uc_response_function_pointer, l_us_res_length, DATA_CAT_STORING_VAR, 0);  //Send: storing variables
					l_us_res_length = fillData (l_uc_response_function_pointer, l_us_res_length, DATA_CAT_HOLD_ACTIVATION, 0);  //Send: hold activation variables
					//----------------
					//Debugging block
					break;
				#endif

				#if (DEBUGGING_FEATURE == 1)
				case OBDH_HDN_CMD_LOAD_DATA_PROTECTED:
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block

					g_us_error_code = loadDataProtected (*l_uc_arg_p,*(l_uc_arg_p+1),uchar2ulong(*(l_uc_arg_p+2),*(l_uc_arg_p+3),*(l_uc_arg_p+4),*(l_uc_arg_p+5)),uchar2ulong(*(l_uc_arg_p+6),*(l_uc_arg_p+7),*(l_uc_arg_p+8),*(l_uc_arg_p+9)));
					l_us_arg[0] = (unsigned short)((*l_uc_arg_p) << 8) + *(l_uc_arg_p+1); //dec_yc requires *l_uc_arg_p,*(l_uc_arg_p+1)
					g_us_function_module_error_code = g_us_error_code; //dec_yc requires *l_uc_arg_p,*(l_uc_arg_p+1)

					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken.
					l_us_res_length = fillData (l_uc_response_function_pointer, l_us_res_length, DATA_CAT_SD_HEADER, 0);  //Send: SD card headers
					l_us_res_length = fillData (l_uc_response_function_pointer, l_us_res_length, DATA_CAT_STORING_VAR, 0);  //Send: storing variables
					//----------------
					//Debugging block
					break;
				#endif
				#endif

				case OBDH_HDN_CMD_READ_DATA:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
 					#endif

					l_us_arg[0] = (unsigned short)(*l_uc_arg_p << 8) + *(l_uc_arg_p+1);
					l_us_arg[1] = (unsigned short)(*(l_uc_arg_p+2) << 8) + *(l_uc_arg_p+3);
					g_us = fillData (l_uc_response_function_pointer, l_us_res_length, l_us_arg[0], l_us_arg[1]);
					g_us_error_code = EC_SUCCESSFUL;
					g_us_function_module_error_code = EC_SUCCESSFUL;//dec_yc requires l_us_arg[0],l_us_arg[1]

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x4009,l_uc_response_function_pointer); //Send: error code, time taken, global multi-purpose variable: g_us
					l_us_res_length = fillData (l_uc_response_function_pointer, l_us_res_length, l_us_arg[0], l_us_arg[1]);
					//----------------
					//Debugging block
					#endif
					break;

				//To write satellite initial state into its proper SD card block (SD_BP_OBDH_ORIGINAL_ESS_START)
				//Only available during debugging, before launch
				#if (STORING_ENABLE == 1)
				#if (DEBUGGING_FEATURE == 1)
				case OBDH_HDN_CMD_WRITE_INIT_STATE:
					startTiming();
					l_us_arg[0] = *l_uc_arg_p; //To indicate offset block from SD_BP_OBDH_ORIGINAL_ESS_START
					for (l_us_counter = 0; l_us_counter < l_us_arg_length-1; l_us_counter++)
						g_uc_sd_data_buffer[l_us_counter] = *(l_uc_arg_p+l_us_counter+1);	
					for (l_us_counter = l_us_arg_length-1; l_us_counter < SD_CARD_BUFFER_SIZE; l_us_counter++)
						g_uc_sd_data_buffer[l_us_counter] = 0;	
					g_us_error_code = sdWriteSingleBlock (SD_BP_OBDH_ORIGINAL_ESS_START+l_us_arg[0],g_uc_sd_data_buffer,SD_CARD_BUFFER_SIZE);
					g_us_function_module_error_code = g_us_error_code;

					#if (SATELLITE_SCHEDULE_HANDLER_ENABLE == 1)
					//Only write this once, when offset = 0
					if (!l_us_arg[0]){					
						//Remove all items in the schedule
						removeSchedule(1,SCHEDULER_BLOCK_SIZE);
	
						//Save the empty schedule in the SD_BP_OBDH_SCHEDULE_START
						g_us_error_code = saveDataProtected (OBDH_SUBSYSTEM,SD_SCHEDULE,g_uc_sd_data_buffer);
						g_us_function_module_error_code = g_us_error_code;
		
						//Switch the scheduler's block pointer to the original
						g_ul_schedule_bp = SD_BP_OBDH_ORIGINAL_SCHEDULE_START;
						
						//Save the empty schedule in the SD_BP_OBDH_ORIGINAL_SCHEDULE_START
						g_us_error_code = saveDataProtected (OBDH_SUBSYSTEM,SD_SCHEDULE,g_uc_sd_data_buffer);
						g_us_function_module_error_code = g_us_error_code;
		
						//Switch the scheduler's block pointer back to the actual
						g_ul_schedule_bp = SD_BP_OBDH_SCHEDULE_START;
					}
					#endif

					#if (TASK_ENABLE == 1)
					//Only write this once, when offset = 0
					if (!l_us_arg[0]){					
						//Initialize the script to the genesis state
						initTaskScript();
	
						//Save the original script in the SD_BP_OBDH_SCRIPT_START
						g_us_error_code = saveDataProtected (OBDH_SUBSYSTEM,SD_SCRIPT,g_uc_sd_data_buffer);
						g_us_function_module_error_code = g_us_error_code;
		
						//Switch the script's block pointer to the original
						g_ul_script_bp = SD_BP_OBDH_ORIGINAL_SCRIPT_START;
						
						//Save the original script in the SD_BP_OBDH_ORIGINAL_SCRIPT_START
						g_us_error_code = saveDataProtected (OBDH_SUBSYSTEM,SD_SCRIPT,g_uc_sd_data_buffer);
						g_us_function_module_error_code = g_us_error_code;
		
						//Switch the script's block pointer back to the actual
						g_ul_script_bp = SD_BP_OBDH_SCRIPT_START;
					}
					#endif
					
					endTiming();
					l_us_res_length = sendOptions(0x0809,l_uc_response_function_pointer); //Send: error code, time taken, SD data out
					break;
				#endif
				#endif

				#if (STORING_ENABLE == 1)
				//To copy over satellite initial state to essential SD card block
				case OBDH_HDN_CMD_COPY_INIT_STATE_TO_ESS:					
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
 					#endif

					//Copy over *l_uc_arg_p no of SD card blocks
					//Ian (2012-05-15): the value of l_us_arg[0] is expected to be in between 0-255
					l_us_arg[0] = *l_uc_arg_p;
					for (l_us_counter = 0; l_us_counter < l_us_arg[0]; l_us_counter++){
						g_us_error_code = sdReadSingleBlock (SD_BP_OBDH_ORIGINAL_ESS_START+l_us_counter);
						if (!g_us_error_code)
							g_us_error_code = sdWriteSingleBlock (SD_BP_OBDH_ESS_START+l_us_counter, g_uc_sd_data_buffer, SD_CARD_BUFFER_SIZE);
					}
					g_us_function_module_error_code = g_us_error_code;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0809,l_uc_response_function_pointer); //Send: error code, time taken, SD data out
					//----------------
					//Debugging block
					#endif
					break;
				#endif

				//To create CRC16 of a message
				//Only available during debugging, before launch
				#if (CRC_ENABLE == 1)
				#if (DEBUGGING_FEATURE == 1)
				case OBDH_HDN_CMD_CREATE_CRC16:
					startTiming();
					g_us_checksum_register = createChecksum (l_uc_arg_p, l_us_arg_length, CHECKSUM_CRC16_TABLE);
					g_us_error_code = EC_SUCCESSFUL;
					g_us_function_module_error_code = EC_SUCCESSFUL;
					endTiming();
					l_us_res_length = sendOptions(0x0209,l_uc_response_function_pointer); //Send: error code, time taken, global multi-purpose variable: g_us.						
					break;
				#endif
				#endif

				#if (STORING_ENABLE == 1)
				//To save latest satellite state
				case OBDH_HDN_CMD_SAVE_STATE:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
 					#endif

					saveState();
					g_us_error_code = EC_SUCCESSFUL;
					g_us_function_module_error_code = EC_SUCCESSFUL;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					#endif
					break;

				//To load latest satellite state
				case OBDH_HDN_CMD_LOAD_STATE:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
 					#endif

					loadState();
					g_us_error_code = EC_SUCCESSFUL;
					g_us_function_module_error_code = EC_SUCCESSFUL;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					#endif
					break;

				//To start the very beginning state of the satellite
				case OBDH_HDN_CMD_GENESIS:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
 					#endif

					//Temporarily change the essential block pointer to the original
					g_ul_essential_bp = SD_BP_OBDH_ORIGINAL_ESS_START;

					//Load the latest SD essential block
					g_us_error_code = loadDataProtected(ALL_SUBSYSTEMS, SD_ESSENTIAL, g_ul_essential_bp, g_ul_essential_bp);

					//Change back the essential block pointer to the normal
					g_ul_essential_bp = SD_BP_OBDH_ESS_START;
					
					//Proceeds if there is no error code
					if (!g_us_error_code){
						//Set initial value for l_us_data_counter
						g_us_data_counter = SD_CARD_HEADER_SIZE;
				
						//Update OBDH essential data
						updateOBDHEssential(g_uc_sd_data_buffer);

						#if (SATELLITE_SCHEDULE_HANDLER_ENABLE == 1)
						//Remove all items in the schedule
						removeSchedule(1,SCHEDULER_BLOCK_SIZE);
						#endif
	
						#if (TASK_ENABLE == 1)
						//Re-initialize scriptline to the original
						initTaskScript();
						#endif

						//Save the genesis state
						saveState();
					}

					//Record function module error code
					g_us_function_module_error_code = g_us_error_code;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					#endif
					break;
				#endif

				#endif

				default:
					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					startTiming();
					//----------------
					//Debugging block
					#endif

					g_us_error_code = EC_OBDH_COMMAND_NOT_FOUND;
					g_us_function_module_error_code = g_us_error_code;

					#if (DEBUGGING_FEATURE == 1)
					//Debugging block
					//----------------
					endTiming();
					l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
					//----------------
					//Debugging block
					#endif
					break;
			}
			break;

		#endif

		//Default case (return error code EC_COMMAND_NOT_FOUND)
		default:
			#if (DEBUGGING_FEATURE == 1)
			//Debugging block
			//----------------
			startTiming();
			//----------------
			//Debugging block
			#endif

			g_us_error_code = EC_COMMAND_NOT_FOUND;
			g_us_function_module_error_code = g_us_error_code;

			#if (DEBUGGING_FEATURE == 1)
			//Debugging block
			//----------------
			endTiming();
			l_us_res_length = sendOptions(0x0009,l_uc_response_function_pointer); //Send: error code, time taken
			//----------------
			//Debugging block
			#endif
			break;

	}
	#if (EVENT_HANDLER_ENABLE == 1)
	#if (STORING_ENABLE == 1)
	//dec22_yc eventHandler Call  tested
	//yc deleted 20120224
	//if(g_us_function_module_error_code){
		eventHandler(EV_E_FUNCTION_MODULE+g_us_function_module_error_code,l_uc_command_header_byte,l_uc_command_tail_byte,
				l_us_arg[0]>>8,l_us_arg[0],l_us_arg[1]>>8,l_us_arg[1]);	
	//}
	#endif	
	#endif
	return l_us_res_length;
}

#if (EXTRA_ENABLE == 1)
//------------------------------------------------------------------------------------
// To call various resets on OBDH
//------------------------------------------------------------------------------------
void callReset (unsigned char l_uc_reset_type)
{
	//Mark various reset type
	if (l_uc_reset_type == OBDH_CMD_SW_RESET)
		//Mark reset cause as RESET_SOURCE_OBDH_SW_RESET 
		str_obdh_data.uc_sat_rstsrc = RESET_SOURCE_OBDH_SW_RESET;
	else if (l_uc_reset_type == OBDH_CMD_HW_RESET)
		//Mark reset cause as RESET_SOURCE_OBDH_HW_RESET 
		str_obdh_data.uc_sat_rstsrc = RESET_SOURCE_OBDH_HW_RESET;
	//Ian (20120905 11:11AM): This case is commented to further distinguish the cause of reset of the satellite
	//For many will call reset with OBDH_CMD_RESET_SAT argument, not just OBDH command
//	else if (l_uc_reset_type == OBDH_CMD_RESET_SAT)
//		//Mark reset cause as RESET_SOURCE_OBDH_COMMAND 
//		str_obdh_data.uc_sat_rstsrc = RESET_SOURCE_OBDH_COMMAND;

	//Record reset event
	#if (EVENT_HANDLER_ENABLE == 1)
	#if (STORING_ENABLE == 1)
	eventHandler(g_us_function_module_error_code,OBDH_ADDR,l_uc_reset_type,
		0,str_obdh_data.uc_sat_rstsrc,0,0); 
	#endif
	#endif

	//Trigger various reset
	if (l_uc_reset_type == OBDH_CMD_SW_RESET)
		//Software reset
		swReset();
	else if (l_uc_reset_type == OBDH_CMD_HW_RESET)
		//Hardware reset
		hwReset();					
	else if (l_uc_reset_type == OBDH_CMD_RESET_SAT)
		//Trigger satellite reset
		resetSatellite();
}
#endif
