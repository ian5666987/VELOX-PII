//------------------------------------------------------------------------------------
// Luxp_storing.c
//------------------------------------------------------------------------------------
// Copyright (C) 2013 Nanyang Technological University.
//
// AUTH: IAN
//		 CHEW YAN CHONG
// DATE: 03 APR 2013
//
// The program Luxp_storing.c will be downloaded to the Pumpkin Motherboard with 
// C8051F120 uC as part of the VELOX-P Pico-Satellite Flight Software. 
//
// This file contains all storing functions of the Flight Software
//
//------------------------------------------------------------------------------------
// Overall functional test status
//------------------------------------------------------------------------------------
// Partially tested
//
//------------------------------------------------------------------------------------
// Overall documentation status
//------------------------------------------------------------------------------------
// Minimally documented
//------------------------------------------------------------------------------------

#include <luxp_storing.h>

#if (RTC_CMD_ENABLE == 1)
//------------------------------------------------------------------------------------
// RTC storing functions
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// To store RTC time bytes
//------------------------------------------------------------------------------------
void storeRTC (void)
{
	str_rtc_data.uc_ths 		= g_uc_i2c_data_in[0];
	str_rtc_data.uc_s 			= g_uc_i2c_data_in[1];
	str_rtc_data.uc_min 		= g_uc_i2c_data_in[2];
	str_rtc_data.uc_cenh 		= g_uc_i2c_data_in[3];
	str_rtc_data.uc_day 		= g_uc_i2c_data_in[4];
	str_rtc_data.uc_date 		= g_uc_i2c_data_in[5];
	str_rtc_data.uc_month 		= g_uc_i2c_data_in[6];
	str_rtc_data.uc_year 		= g_uc_i2c_data_in[7];
}
#endif

#if (ISIS_CMD_ENABLE == 1)
//------------------------------------------------------------------------------------
// ISIS (COMM Subsystem) storing functions
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// To store all ISIS parameters
//------------------------------------------------------------------------------------
void storeISIS (void)
{
	switch (g_uc_i2c_target){
		case IMC_ADDR:
			switch (g_uc_i2c_msg_out){
				case IMC_I2C_GET_NOFR:
					str_imc_hk.uc_receiver_no_of_frame = g_uc_i2c_data_in[0];
					break;

				case IMC_I2C_REC_DOFF:
					str_imc_hk.us_doppler_offset = (unsigned short)(g_uc_i2c_data_in[1] << 8) + g_uc_i2c_data_in[0];
					break;
		
				case IMC_I2C_REC_SSTR:
					str_imc_hk.us_receiver_signal_strength = (unsigned short)(g_uc_i2c_data_in[1] << 8) + g_uc_i2c_data_in[0];
					break;
		
				case IMC_I2C_TRN_RPOW:
					str_imc_hk.us_transmitter_reflected_power = (unsigned short)(g_uc_i2c_data_in[1] << 8) + g_uc_i2c_data_in[0];
					break;
		
				case IMC_I2C_TRN_FPOW:
					str_imc_hk.us_transmitter_forward_power = (unsigned short)(g_uc_i2c_data_in[1] << 8) + g_uc_i2c_data_in[0];
					break;
		
				case IMC_I2C_TRN_ICON:
					str_imc_hk.us_transmitter_current_consumption = (unsigned short)(g_uc_i2c_data_in[1] << 8) + g_uc_i2c_data_in[0];
					break;
		
				case IMC_I2C_REC_ICON:
					str_imc_hk.us_receiver_current_consumption = (unsigned short)(g_uc_i2c_data_in[1] << 8) + g_uc_i2c_data_in[0];
					break;
		
				case IMC_I2C_POW_AMPT:
					str_imc_hk.us_power_amplifier_temperature = (unsigned short)(g_uc_i2c_data_in[1] << 8) + g_uc_i2c_data_in[0];
					break;
		
				case IMC_I2C_POW_BUSV:
					str_imc_hk.us_power_bus_voltage = (unsigned short)(g_uc_i2c_data_in[1] << 8) + g_uc_i2c_data_in[0];
					break;
		
				case IMC_I2C_POW_BUSV_ICON:
					str_imc_hk.us_power_bus_voltage 				= (unsigned short)(g_uc_i2c_data_in[1] << 8) + g_uc_i2c_data_in[0];
					str_imc_hk.us_receiver_current_consumption 		= (unsigned short)(g_uc_i2c_data_in[3] << 8) + g_uc_i2c_data_in[2];
					str_imc_hk.us_transmitter_current_consumption 	= (unsigned short)(g_uc_i2c_data_in[5] << 8) + g_uc_i2c_data_in[4];
					break;
		
				case IMC_I2C_REC_UPTM:
					str_imc_hk.uc_receiver_uptime_minutes 		= g_uc_i2c_data_in[0];
					str_imc_hk.uc_receiver_uptime_hours 		= g_uc_i2c_data_in[1];
					str_imc_hk.uc_receiver_uptime_days 			= g_uc_i2c_data_in[2];
					break;
		
				default:
					break;
			}
			break;

		case ITC_ADDR:
			switch (g_uc_i2c_msg_out){
				case ITC_I2C_TRN_UPTM:
					str_itc_hk.uc_transmitter_uptime_minutes 	= g_uc_i2c_data_in[0];
					str_itc_hk.uc_transmitter_uptime_hours 		= g_uc_i2c_data_in[1];
					str_itc_hk.uc_transmitter_uptime_days 		= g_uc_i2c_data_in[2];
					break;
		
				case ITC_I2C_TRN_STATE:
					str_itc_hk.uc_transmitter_state				= g_uc_i2c_data_in[0];
					break;
		
				default:
					break;
			}
			break;

		default:
			break;
	}
}
#endif

#if (ANTENNA_CMD_ENABLE == 1)
//------------------------------------------------------------------------------------
// Antenna deployment subsystem storing functions
//------------------------------------------------------------------------------------
void storeAntenna (void)
{
	switch (g_uc_i2c_msg_out){
		case ANTENNA_I2C_GET_DPLY_1_COUNT:
		case ANTENNA_I2C_GET_DPLY_2_COUNT:
		case ANTENNA_I2C_GET_DPLY_3_COUNT:
		case ANTENNA_I2C_GET_DPLY_4_COUNT:
			str_antenna_hk.uc_actv_count[g_uc_i2c_msg_out-ANTENNA_I2C_GET_DPLY_1_COUNT] = g_uc_i2c_data_in[0];
			break;

		case ANTENNA_I2C_GET_DPLY_1_TIME:
		case ANTENNA_I2C_GET_DPLY_2_TIME:
		case ANTENNA_I2C_GET_DPLY_3_TIME:
		case ANTENNA_I2C_GET_DPLY_4_TIME:
			str_antenna_hk.uc_actv_time[g_uc_i2c_msg_out-ANTENNA_I2C_GET_DPLY_1_TIME] = g_uc_i2c_data_in[0];
			break;

		//The reading of antenna temperature is noticed to be unused
		case ANTENNA_I2C_GET_TEMP:
			str_antenna_hk.us_temp = (unsigned short)(g_uc_i2c_data_in[0] << 8) + g_uc_i2c_data_in[1];
			break;

		//The order of the byte is as specified by the hardware
		case ANTENNA_I2C_GET_DPLY_STAT:
			str_antenna_hk.us_deployment_status = (unsigned short)(g_uc_i2c_data_in[1] << 8) + g_uc_i2c_data_in[0];
			break;

		default:
			break;
	}
}
#endif 

#if (PWRS_CMD_ENABLE == 1)
//------------------------------------------------------------------------------------
// PWRS Subsystem storing functions
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// To store PWRS data obtained from a given array
//------------------------------------------------------------------------------------
void storePWRS (unsigned char *l_uc_array_p)
{
	unsigned char l_uc_i;

	switch (g_uc_i2c_msg_out){
		case PWRS_I2C_GET_HK:
			for (l_uc_i = 0; l_uc_i < 5; l_uc_i++)
				str_pwrs_hk.uc_sp_temp[l_uc_i] 		= *(l_uc_array_p+g_us_data_counter++);
			for (l_uc_i = 0; l_uc_i < 3; l_uc_i++)
				str_pwrs_hk.us_pv_v[l_uc_i] 		= insertArrayToShortGC(l_uc_array_p);
			for (l_uc_i = 0; l_uc_i < 3; l_uc_i++)
				str_pwrs_hk.us_pv_i[l_uc_i] 		= insertArrayToShortGC(l_uc_array_p);
			str_pwrs_hk.us_batt_v[0] 				= insertArrayToShortGC(l_uc_array_p);
			str_pwrs_hk.us_batt_v[1] 				= insertArrayToShortGC(l_uc_array_p);
			str_pwrs_hk.us_batt_i 					= insertArrayToShortGC(l_uc_array_p);
			str_pwrs_hk.uc_batt_soc 				= *(l_uc_array_p+g_us_data_counter++);
			str_obdh_data.uc_batt_soc				= str_pwrs_hk.uc_batt_soc;
			str_pwrs_hk.uc_batt_temp[0] 			= *(l_uc_array_p+g_us_data_counter++);
			str_pwrs_hk.uc_batt_temp[1] 			= *(l_uc_array_p+g_us_data_counter++);
			str_pwrs_hk.us_sys_i 					= insertArrayToShortGC(l_uc_array_p);
			str_pwrs_hk.us_pcc_v 					= insertArrayToShortGC(l_uc_array_p);
			for (l_uc_i = 0; l_uc_i < 4; l_uc_i++)
				str_pwrs_hk.us_load_v[l_uc_i] 		= insertArrayToShortGC(l_uc_array_p);
			for (l_uc_i = 0; l_uc_i < 4; l_uc_i++)
				str_pwrs_hk.us_load_i[l_uc_i] 		= insertArrayToShortGC(l_uc_array_p);
			for (l_uc_i = 0; l_uc_i < 4; l_uc_i++)
				str_pwrs_hk.uc_latch_count[l_uc_i]	= *(l_uc_array_p+g_us_data_counter++);
			str_pwrs_hk.uc_rst_count 				= *(l_uc_array_p+g_us_data_counter++);
			str_pwrs_hk.uc_rst_cause 				= *(l_uc_array_p+g_us_data_counter++);
			str_pwrs_hk.uc_mode 					= *(l_uc_array_p+g_us_data_counter++);

			//Requested by PWRS subsystem to re-check the battery SOC percentage. (PWRS subsystem requested - 20120321)
			if ((str_pwrs_hk.uc_batt_soc >= 100) && str_pwrs_hk.uc_mode == PWRS_BATT_CTRL_ON)
				str_obdh_data.uc_batt_soc = 99;

			//Reset PWRS SOC to 100% only when the V_CELL reaches maximum. (PWRS subsystem requested - 20120321)
			if (str_pwrs_hk.uc_mode == PWRS_BATT_CTRL_OFF)
				str_obdh_data.uc_batt_soc = 100;

			str_pwrs_hk.s_mcu_temp 					= insertArrayToShortGC(l_uc_array_p);
			str_pwrs_hk.c_mcu_rstsrc 				= *(l_uc_array_p+g_us_data_counter++);
			break;

		case PWRS_I2C_GET_SSCP:
			str_pwrs_par.uc_mppt_mode 				= *(l_uc_array_p+g_us_data_counter++);
			str_pwrs_par.uc_batt_heater 			= *(l_uc_array_p+g_us_data_counter++);
			str_pwrs_par.uc_batt_charge_upp_limit 	= *(l_uc_array_p+g_us_data_counter++);
			str_pwrs_par.uc_batt_charge_low_limit 	= *(l_uc_array_p+g_us_data_counter++);
			break;

		case PWRS_I2C_GET_DATA:
			if(g_uc_i2c_data_out[1] == (DATA_PWRS_UC_CHANNEL_STATUS & 0x00FF))
				str_pwrs_data.uc_channel_status = *(l_uc_array_p+g_us_data_counter+3);
			else if(g_uc_i2c_data_out[1] == (DATA_PWRS_US_PV_FV_1 & 0x00FF))
				str_pwrs_data.us_pv_fv[0] = ((unsigned short)(*(l_uc_array_p+g_us_data_counter+2) << 8) + *(l_uc_array_p+g_us_data_counter+3));
			else if(g_uc_i2c_data_out[1] == (DATA_PWRS_US_PV_FV_2 & 0x00FF))
				str_pwrs_data.us_pv_fv[1] = ((unsigned short)(*(l_uc_array_p+g_us_data_counter+2) << 8) + *(l_uc_array_p+g_us_data_counter+3));
			else if(g_uc_i2c_data_out[1] == (DATA_PWRS_US_PV_FV_3 & 0x00FF))
				str_pwrs_data.us_pv_fv[2] = ((unsigned short)(*(l_uc_array_p+g_us_data_counter+2) << 8) + *(l_uc_array_p+g_us_data_counter+3));
			break;
		
		default:
			break;
	}
}
#endif

#if (ADCS_CMD_ENABLE == 1)
//------------------------------------------------------------------------------------
// ADCS Subsystem storing functions
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// To store all ADCS data
//------------------------------------------------------------------------------------
void storeADCS (unsigned char *l_uc_array_p)
{
	unsigned char l_uc_i;

	switch (g_uc_i2c_msg_out){
		case ADCS_I2C_GET_HK:
			for (l_uc_i = 0; l_uc_i < 3; l_uc_i++)
				str_adcs_hk.f_sunv[l_uc_i] 			= insertArrayToFloatGC(l_uc_array_p);
			for (l_uc_i = 0; l_uc_i < 4; l_uc_i++)
				str_adcs_hk.f_qeb[l_uc_i] 			= insertArrayToFloatGC(l_uc_array_p);
			for (l_uc_i = 0; l_uc_i < 3; l_uc_i++)
				str_adcs_hk.s_br_eci[l_uc_i] 		= insertArrayToShortGC(l_uc_array_p);
			for (l_uc_i = 0; l_uc_i < 3; l_uc_i++)
				str_adcs_hk.f_posv[l_uc_i] 			= insertArrayToFloatGC(l_uc_array_p);
			str_adcs_hk.c_sensor_enable 			= *(l_uc_array_p+g_us_data_counter++);
			str_adcs_hk.c_sensor_credit 			= *(l_uc_array_p+g_us_data_counter++);
			for (l_uc_i = 0; l_uc_i < 3; l_uc_i++)
				str_adcs_hk.c_ac_m[l_uc_i] 			= *(l_uc_array_p+g_us_data_counter++);
			for (l_uc_i = 0; l_uc_i < 3; l_uc_i++)
				str_adcs_hk.c_mt_i[l_uc_i] 			= *(l_uc_array_p+g_us_data_counter++);
			str_adcs_hk.c_actuator_enable 			= *(l_uc_array_p+g_us_data_counter++);
			str_adcs_hk.c_mode 						= *(l_uc_array_p+g_us_data_counter++);
			str_adcs_hk.s_mcu_temp 					= insertArrayToShortGC(l_uc_array_p);
			str_adcs_hk.c_mcu_rstsrc 				= *(l_uc_array_p+g_us_data_counter++);
			break;

		case ADCS_I2C_GET_SSCP:
			str_adcs_par.s_ss_threshold 			= insertArrayToShortGC(l_uc_array_p);
			for (l_uc_i = 0; l_uc_i < NO_OF_ADCS_CGAIN; l_uc_i++)
				str_adcs_par.f_cgain[l_uc_i] 		= insertArrayToFloatGC(l_uc_array_p);
			str_adcs_par.f_tle_epoch 				= insertArrayToFloatGC(l_uc_array_p);
			str_adcs_par.f_tle_inc 					= insertArrayToFloatGC(l_uc_array_p);
			str_adcs_par.f_tle_raan 				= insertArrayToFloatGC(l_uc_array_p);
			str_adcs_par.f_tle_arg_per 				= insertArrayToFloatGC(l_uc_array_p);
			str_adcs_par.f_tle_ecc 					= insertArrayToFloatGC(l_uc_array_p);
			str_adcs_par.f_tle_ma 					= insertArrayToFloatGC(l_uc_array_p);
			str_adcs_par.us_tle_mm 					= insertArrayToShortGC(l_uc_array_p);
			str_adcs_par.ul_time 					= insertArrayToLongGC(l_uc_array_p);
			for (l_uc_i = 0; l_uc_i < 3; l_uc_i++)
				str_adcs_par.s_vector_2sun[l_uc_i] 	= insertArrayToShortGC(l_uc_array_p);
			for (l_uc_i = 0; l_uc_i < 3; l_uc_i++)
				str_adcs_par.c_ctrl_br[l_uc_i] 		= (char)*(l_uc_array_p+g_us_data_counter++);
			str_adcs_par.s_dgain 					= insertArrayToShortGC(l_uc_array_p);
			break;

		default:
			break;
	}
}
#endif

#if (STORING_ENABLE == 1)
//------------------------------------------------------------------------------------
// This function handles initialization for different type of data saving
// The initialization includes:
//	1) Data sets to be saved
//	2) No of data sets to be saved
//	3) Initial SD card block pointers
//------------------------------------------------------------------------------------
// Data sets property:
//	Bit0 - variable(1) / non-variable (0) pointer
//	Bit1 - looping(1) / non-looping (0) memory space
//	Bit7-2 - reserved
//------------------------------------------------------------------------------------
unsigned short initSDData(void)
{
	unsigned short l_us_error_code = EC_INIT;

	//Initiates SD card block writing variables for different cases
	//------------------------------------------------------------------------------------
	if ((g_uc_sd_header_subsystem == ALL_SUBSYSTEMS) && (g_uc_sd_header_data_type == SD_ESSENTIAL)){
		g_uc_data_type_saved_buffer[0] 	= DATA_CAT_PWRS_SSCP;
		g_uc_data_type_saved_buffer[1] 	= DATA_CAT_ADCS_SSCP;
		g_uc_data_type_saved_buffer[2] 	= DATA_CAT_OBDH_SSCP;
		g_uc_data_type_saved_buffer[3] 	= DATA_CAT_OBDH_SELECTED_HK;
		g_uc_data_type_saved_buffer[4] 	= DATA_CAT_OBDH_SELECTED_DATA;
		g_uc_data_type_saved_buffer[5] 	= DATA_CAT_OBDH_BP;
		g_uc_data_type_saved_buffer[6] 	= DATA_CAT_SAT_DATE_INIT;
		g_uc_data_type_saved_buffer[7] 	= DATA_CAT_FROM_TO_CALLSIGN;
		g_uc_no_of_data_saved 			= 8;
		g_ul_initial_sd_card_bp 		= g_ul_essential_bp;
		g_uc_no_of_copies				= NO_OF_COPIES_SD_ESSENTIAL;

		//Assigning data sets property
		g_uc_data_sets_property			= 0x00; //Non-variable pointer, non-looping memory space

		//If the essential block pointer is normal block pointer
		if (g_ul_essential_bp == SD_BP_OBDH_ESS_START){
			//For constant initial block pointers
			g_ul_memory_address_lower_limit	= SD_BP_OBDH_ESS_START;
			g_ul_memory_address_upper_limit	= SD_BP_OBDH_ESS_END;
			g_ul_memory_address_end			= SD_BP_OBDH_ESS_END;

			//Assign successful saving data request
			l_us_error_code 				= EC_SUCCESSFUL;
		}
		//If the essential block pointer is very first (original) essential block pointer
		else if (g_ul_essential_bp == SD_BP_OBDH_ORIGINAL_ESS_START){
			//For constant initial block pointers
			g_ul_memory_address_lower_limit	= SD_BP_OBDH_ORIGINAL_ESS_START;
			g_ul_memory_address_upper_limit	= SD_BP_OBDH_ORIGINAL_ESS_END;
			g_ul_memory_address_end			= SD_BP_OBDH_ORIGINAL_ESS_END;

			//Assign successful saving data request
			l_us_error_code 				= EC_SUCCESSFUL;
		}
		//Other than them will be unsuccessful
		else
			//Assign unsuccessful saving data request
			l_us_error_code 				= EC_INVALID_SAVING_DATA_REQUEST;
		
	}
	else if ((g_uc_sd_header_subsystem == ALL_SUBSYSTEMS) && (g_uc_sd_header_data_type == SD_HK)){
		g_uc_data_type_saved_buffer[0] 	= DATA_CAT_LATEST_HK_SAMPLING_TIME_AND_ORBIT_TAI_FORMAT;
		g_uc_data_type_saved_buffer[1] 	= DATA_CAT_LAST_HK_EC;
		g_uc_data_type_saved_buffer[2] 	= DATA_CAT_PWRS_HK;
		g_uc_data_type_saved_buffer[3] 	= DATA_CAT_ADCS_HK;
		g_uc_data_type_saved_buffer[4] 	= DATA_CAT_COMM_HK;
		g_uc_data_type_saved_buffer[5] 	= DATA_CAT_OBDH_HK;
		g_uc_no_of_data_saved 			= 6;
		g_ul_initial_sd_card_bp 		= str_bp.ul_hk_write_p;
		g_uc_no_of_copies				= NO_OF_COPIES_SD_HK;

		//For varied initial block pointers
		g_ul_initial_sd_card_bp_address = &str_bp.ul_hk_write_p;
		g_uc_no_of_overwritten_p		= &str_bp.uc_hk_overwritten;
		g_ul_memory_address_lower_limit	= SD_BP_HK_START;
		g_ul_memory_address_upper_limit	= SD_BP_HK_LIMIT;
		g_ul_memory_address_end			= SD_BP_HK_END;

		//Assigning data sets property
		g_uc_data_sets_property			= 0x03; //Variable pointer, looping memory space

		//Assign successful saving data request
		l_us_error_code 				= EC_SUCCESSFUL;
	}
	else if ((g_uc_sd_header_subsystem == OBDH_SUBSYSTEM) && (g_uc_sd_header_data_type == SD_BP)){
		g_uc_data_type_saved_buffer[0] 	= DATA_CAT_OBDH_BP;
		g_uc_no_of_data_saved 			= 1;
		g_uc_no_of_copies				= NO_OF_COPIES_SD_BP;
		g_ul_initial_sd_card_bp 		= SD_BP_OBDH_ORBIT_START + str_obdh_data.us_current_rev_no*(g_uc_no_of_copies+1);

		//For non-constant initial block pointers
		g_uc_no_of_overwritten_p		= &str_bp.uc_orbit_indexing_overwritten;
		g_ul_memory_address_lower_limit	= SD_BP_OBDH_ORBIT_START;
		g_ul_memory_address_upper_limit	= SD_BP_OBDH_ORBIT_LIMIT;
		g_ul_memory_address_end			= SD_BP_OBDH_ORBIT_END;
		
		//Assigning data sets property
		g_uc_data_sets_property			= 0x02; //Non-variable pointer, looping memory space

		//Assign successful saving data request
		l_us_error_code 				= EC_SUCCESSFUL;
	}
	else if ((g_uc_sd_header_subsystem == OBDH_SUBSYSTEM) && (g_uc_sd_header_data_type == SD_SCHEDULE)){
		g_uc_data_type_saved_buffer[0] 	= DATA_CAT_SCHEDULE_PAR;
		g_uc_data_type_saved_buffer[1] 	= DATA_CAT_SCHEDULE_1_TO_10;
		g_uc_data_type_saved_buffer[2] 	= DATA_CAT_SCHEDULE_11_TO_20;
		g_uc_data_type_saved_buffer[3] 	= DATA_CAT_SCHEDULE_21_TO_30;
		g_uc_data_type_saved_buffer[4] 	= DATA_CAT_SCHEDULE_31_TO_40;
		g_uc_data_type_saved_buffer[5] 	= DATA_CAT_SCHEDULE_41_TO_50;
		g_uc_data_type_saved_buffer[6] 	= DATA_CAT_SCHEDULE_51_TO_60;
		g_uc_data_type_saved_buffer[7] 	= DATA_CAT_SCHEDULE_61_TO_70;
		g_uc_no_of_data_saved 			= 8;
		g_ul_initial_sd_card_bp 		= g_ul_schedule_bp;
		g_uc_no_of_copies				= NO_OF_COPIES_SD_SCHEDULE;

		//Assigning data sets property
		g_uc_data_sets_property			= 0x00; //Non-variable pointer, non-looping memory space

		//If the schedule block pointer is normal block pointer
		if (g_ul_schedule_bp == SD_BP_OBDH_SCHEDULE_START){
			//For constant initial block pointers
			g_ul_memory_address_lower_limit	= SD_BP_OBDH_SCHEDULE_START;
			g_ul_memory_address_upper_limit	= SD_BP_OBDH_SCHEDULE_END;
			g_ul_memory_address_end			= SD_BP_OBDH_SCHEDULE_END;

			//Assign successful saving data request
			l_us_error_code 				= EC_SUCCESSFUL;
		}
		//If the schedule block pointer is very first (original) schedule block pointer
		else if (g_ul_schedule_bp == SD_BP_OBDH_ORIGINAL_SCHEDULE_START){
			//For constant initial block pointers
			g_ul_memory_address_lower_limit	= SD_BP_OBDH_ORIGINAL_SCHEDULE_START;
			g_ul_memory_address_upper_limit	= SD_BP_OBDH_ORIGINAL_SCHEDULE_END;
			g_ul_memory_address_end			= SD_BP_OBDH_ORIGINAL_SCHEDULE_END;

			//Assign successful saving data request
			l_us_error_code 				= EC_SUCCESSFUL;
		}
		//Other than them will be unsuccessful
		else
			//Assign unsuccessful saving data request
			l_us_error_code 				= EC_INVALID_SAVING_DATA_REQUEST;		
	}
	else if ((g_uc_sd_header_subsystem == OBDH_SUBSYSTEM) && (g_uc_sd_header_data_type == SD_SCRIPT)){
		g_uc_data_type_saved_buffer[0] 	= DATA_CAT_SCRIPTSPACE_PAR;
		g_uc_data_type_saved_buffer[1] 	= DATA_CAT_SCRIPTSPACE_1;
		g_uc_data_type_saved_buffer[2] 	= DATA_CAT_SCRIPTSPACE_2;
		g_uc_data_type_saved_buffer[3] 	= DATA_CAT_SCRIPTSPACE_3;
		g_uc_data_type_saved_buffer[4] 	= DATA_CAT_SCRIPTSPACE_4;
		g_uc_data_type_saved_buffer[5] 	= DATA_CAT_SCRIPTSPACE_5;
		g_uc_data_type_saved_buffer[6] 	= DATA_CAT_SCRIPTSPACE_6;
		g_uc_data_type_saved_buffer[7] 	= DATA_CAT_SCRIPTSPACE_7;
		g_uc_data_type_saved_buffer[8] 	= DATA_CAT_SCRIPTSPACE_8;
		g_uc_data_type_saved_buffer[9] 	= DATA_CAT_SCRIPTSPACE_9;
		g_uc_data_type_saved_buffer[10] = DATA_CAT_SCRIPTSPACE_10;
		g_uc_no_of_data_saved 			= 11;
		g_ul_initial_sd_card_bp 		= g_ul_script_bp;
		g_uc_no_of_copies				= NO_OF_COPIES_SD_SCRIPT;

		//Assigning data sets property
		g_uc_data_sets_property			= 0x00; //Non-variable pointer, non-looping memory space

		//If the script block pointer is normal block pointer
		if (g_ul_script_bp == SD_BP_OBDH_SCRIPT_START){
			//For constant initial block pointers
			g_ul_memory_address_lower_limit	= SD_BP_OBDH_SCRIPT_START;
			g_ul_memory_address_upper_limit	= SD_BP_OBDH_SCRIPT_END;
			g_ul_memory_address_end			= SD_BP_OBDH_SCRIPT_END;

			//Assign successful saving data request
			l_us_error_code 				= EC_SUCCESSFUL;
		}
		//If the script block pointer is very first (original) script block pointer
		else if (g_ul_script_bp == SD_BP_OBDH_ORIGINAL_SCRIPT_START){
			//For constant initial block pointers
			g_ul_memory_address_lower_limit	= SD_BP_OBDH_ORIGINAL_SCRIPT_START;
			g_ul_memory_address_upper_limit	= SD_BP_OBDH_ORIGINAL_SCRIPT_END;
			g_ul_memory_address_end			= SD_BP_OBDH_ORIGINAL_SCRIPT_END;

			//Assign successful saving data request
			l_us_error_code 				= EC_SUCCESSFUL;
		}
		//Other than them will be unsuccessful
		else
			//Assign unsuccessful saving data request
			l_us_error_code 				= EC_INVALID_SAVING_DATA_REQUEST;		
	}
	else if ((g_uc_sd_header_subsystem == ALL_SUBSYSTEMS) && (g_uc_sd_header_data_type == SD_LOG_DATA)){
		g_uc_data_type_saved_buffer[0] 	= DATA_CAT_LOG_DATA_BUFFER;
		g_uc_no_of_data_saved 			= 1;
		g_ul_initial_sd_card_bp 		= str_bp.ul_datalog_write_p;
		g_uc_no_of_copies				= NO_OF_COPIES_SD_LOG_DATA;

		//For varied initial block pointers
		g_ul_initial_sd_card_bp_address = &str_bp.ul_datalog_write_p;
		g_ul_memory_address_lower_limit	= SD_BP_DATALOG_START;
		g_ul_memory_address_upper_limit	= SD_BP_DATALOG_LIMIT;
		g_ul_memory_address_end			= SD_BP_DATALOG_END;

		//Assigning data sets property
		g_uc_data_sets_property			= 0x01; //Variable pointer, non-looping memory space

		//Assign successful saving data request
		l_us_error_code 				= EC_SUCCESSFUL;
	}
	else if ((g_uc_sd_header_subsystem == ALL_SUBSYSTEMS) && (g_uc_sd_header_data_type == SD_RECOVERY_PACKAGE)){
		g_uc_data_type_saved_buffer[0] 	= DATA_CAT_FILLED_AX25_FRAME;
		g_uc_no_of_data_saved 			= 1;
		g_ul_initial_sd_card_bp 		= str_bp.ul_package_recovery_write_p;
		g_uc_no_of_copies				= NO_OF_COPIES_SD_RECOVERY_PACKAGE;

		//For varied initial block pointers
		g_ul_initial_sd_card_bp_address = &str_bp.ul_package_recovery_write_p;
		g_ul_memory_address_lower_limit	= SD_BP_PACKAGE_RECOVERY_START;
		g_ul_memory_address_upper_limit	= SD_BP_PACKAGE_RECOVERY_LIMIT;
		g_ul_memory_address_end			= SD_BP_PACKAGE_RECOVERY_END;

		//Assigning data sets property
		g_uc_data_sets_property			= 0x01; //Variable pointer, non-looping memory space

		//Assign successful saving data request
		l_us_error_code 				= EC_SUCCESSFUL;
	}
	//dec22_yc eventlog storing
	else if ((g_uc_sd_header_subsystem == ALL_SUBSYSTEMS) && (g_uc_sd_header_data_type == SD_EVENTLOG_DATA)){
		g_uc_data_type_saved_buffer[0] 	= DATA_CAT_EVENTLOG_DATA_BUFFER;
		g_uc_no_of_data_saved 			= 1;
		g_ul_initial_sd_card_bp 		= str_bp.ul_eventlog_write_p;
		g_uc_no_of_copies				= NO_OF_COPIES_SD_EVENTLOG_DATA;

		//For varied initial block pointers
		g_ul_initial_sd_card_bp_address = &str_bp.ul_eventlog_write_p;
		g_ul_memory_address_lower_limit	= SD_BP_EVENTLOG_START;
		g_ul_memory_address_upper_limit	= SD_BP_EVENTLOG_LIMIT;
		g_ul_memory_address_end			= SD_BP_EVENTLOG_END;

		//Assigning data sets property
		g_uc_data_sets_property			= 0x03; //Variable pointer, looping memory space

		//Assign successful saving data request
		l_us_error_code 				= EC_SUCCESSFUL;
	}
	else {
		//Assign unsuccessful saving data request
		l_us_error_code 				= EC_INVALID_SAVING_DATA_REQUEST;
	}		
	//------------------------------------------------------------------------------------

	//Set SD card write block pointer as specified by the input first
	//Error code condition needs not to be added for this case. 
	//But in case g_ul_initial_sd_card_bp is not assigned (that is, as to make 
	//g_ul_sd_header_sd_write_block value 'random', the error code in this
	//function should indicate that SD data storing initialization is unsuccessful.
	g_ul_sd_header_sd_write_block = g_ul_initial_sd_card_bp;
	
	return l_us_error_code;
}

//------------------------------------------------------------------------------------
// To save data from the sets of data given to be saved
// This function will handle increment of block pointer for a set of data to save
// WARNING:
//	1) 	g_uc_sd_header_subsystem AND g_uc_sd_header_data_type must be declared
//		outside of this function!!!
//------------------------------------------------------------------------------------
unsigned short saveData (unsigned char *l_uc_array_p)
{
	unsigned short l_us_error_code;
    unsigned char l_uc_counter = 0;
    unsigned short l_us_current_data_set_length = 0;
    unsigned short l_us_total_to_be_data_length = 0;
	unsigned short l_us_buffer_size = SD_CARD_BUFFER_SIZE; //Initialize as SD card buffer size unless specified otherwise
    unsigned char *l_uc_temporary_data_buffer_p;
	unsigned char *l_uc_data_type_saved_buffer_p;

    //Initialize data sets to be saved buffer pointer
	l_uc_data_type_saved_buffer_p 	= g_uc_data_type_saved_buffer;
    
    //Initialize temporary test buffer and SD card data buffer
	l_uc_temporary_data_buffer_p	= g_uc_temporary_data_buffer;

	//Check other buffer size (other than normal SD_CARD_BUFFER_SIZE)
	if (l_uc_array_p == g_uc_temporary_data_buffer) l_us_buffer_size = MAX_SD_CARD_BLOCK_CONTENT_SIZE;

	//Set current block to 1
	g_us_sd_header_no_of_this_block = 1;

	//Set block property to be on-going data set now, so long as this function does not go out from the main loop,
	//the block property will always be on-going
	g_us_sd_header_block_property = 0x0100;

	//Initialize SD card block data size as SD_CARD_HEADER_SIZE, which is the initial length for any SD card block data writing
	g_us_sd_header_this_block_data_size = SD_CARD_HEADER_SIZE;
    
	//Main loop to save every data type given
	for (l_uc_counter = 0; l_uc_counter < g_uc_no_of_data_saved; l_uc_counter++){

		//Test case to calculate length of a set of data
		l_us_current_data_set_length = fillData(l_uc_temporary_data_buffer_p,0,*(l_uc_data_type_saved_buffer_p+l_uc_counter),0);
		l_us_total_to_be_data_length = g_us_sd_header_this_block_data_size + l_us_current_data_set_length;

		//If the total to be data length is not within allowed length: MAX_SD_CARD_BLOCK_CONTENT_SIZE + SD_CARD_HEADER_SIZE
		if (l_us_total_to_be_data_length > MAX_SD_CARD_BLOCK_CONTENT_SIZE + SD_CARD_HEADER_SIZE){
			//Save the data stored in the buffer to the specified SD card block
			l_us_error_code = storeInSDCard(l_uc_array_p,l_us_buffer_size);

			//Increase no of this block for the next saving
			g_us_sd_header_no_of_this_block++;

			//Reset current block data size to the original size = SD_CARD_HEADER_SIZE
			//The size of g_us_sd_header_this_block_data_size would be changed when 'else' case occurs.
			//Therefore, here g_us_sd_header_this_block_data_size is reset.
			g_us_sd_header_this_block_data_size = SD_CARD_HEADER_SIZE;

			//Fill SD card data buffer with this current data set, update its array index pointer
			g_us_sd_header_this_block_data_size = fillData(l_uc_array_p,g_us_sd_header_this_block_data_size,*(l_uc_data_type_saved_buffer_p+l_uc_counter),0);
		}
		else {//If the total to be data length is within the allowed length
			//Fill SD card data buffer with this current data set, update its array index pointer
			g_us_sd_header_this_block_data_size = fillData(l_uc_array_p,g_us_sd_header_this_block_data_size,*(l_uc_data_type_saved_buffer_p+l_uc_counter),0);
		}
	}

	//When the loop ends, all data must have been stored in the SD card, except the last one in the SD card buffer
	//Update block property to be end data set now
	g_us_sd_header_block_property = 0x0200;

	//Save the data stored in the buffer to the specified SD card block
    l_us_error_code = storeInSDCard(l_uc_array_p, l_us_buffer_size);
	//dec28_yc in case write unable to end properly, it is still possible to continue, as long as SD card does not hang.
	l_us_error_code=initSD();

	//Return no of this block, which is the number of block used for the whole saving process
	return l_us_error_code;
}

//------------------------------------------------------------------------------------
// To store data of a subsystem in the SD card
//------------------------------------------------------------------------------------
// This function is to save a set of data to the SD card. There are several types of 
// data that we can save in the SD card. They are as follow:
//	1) Housekeeping data: OBDH, PWRS, ADCS, COMM, PAYL
//	2) OBDH Housekeeping block pointers: will be saved per orbit AND per latest state
//	3) OBDH latest state (periodical saving)
//	4) OBDH schedule
//
// Storing steps:
//	1) Write main information to the SD card buffer
//	2) Write header to the SD card buffer
//	3) Calculate and write CRC16 to the SD card buffer
//	4) Write zero for the remaining elements in the SD card buffer
//	5) Write SD card buffer to SD card block
//------------------------------------------------------------------------------------
unsigned short storeInSDCard(unsigned char *l_uc_array_p, unsigned short l_us_buffer_size)
{
	unsigned short l_us_error_code;
	unsigned char l_uc_i;
	unsigned short l_us_counter = 0;
	unsigned short l_us_data_counter = 0x0000;

	//------------------------------------------------------------------------------------
	//Create header & checksum
	//Store block information
	//Store no of block used
	//Store no of this block
	//Store msg ended information
	//Store first block pointer address
	//Store satellite time & orbit when this block is saved
	//Store original block address (also in the copy block, only original block address is stored)
	//But there is another useful information, that is
	//Time and orbit when the latest housekeeping is taken, but that should be
	//provided in the content of the SD card, not the header
	l_us_data_counter = fillData(l_uc_array_p,l_us_data_counter,DATA_CAT_SD_HEADER,0);

	//Set the unused header bytes to zero
	for (l_us_counter = l_us_data_counter; l_us_counter < 0x0020; l_us_counter++)
		*(l_uc_array_p + l_us_counter) = 0x00;
	//------------------------------------------------------------------------------------

	#if (CRC_ENABLE == 1)
	//------------------------------------------------------------------------------------
	//Add checksum bytes
	if (CHECKSUM_DEFAULT & 0x40)	//If checksum default method is 16-bit
		insertCRC16(l_uc_array_p,g_us_sd_header_this_block_data_size);

	//Set all other bytes in the write buffer to zero
	for (l_us_counter = g_us_sd_header_this_block_data_size+2; l_us_counter < l_us_buffer_size; l_us_counter++)
		*(l_uc_array_p + l_us_counter) = 0x00;
	#else
	for (l_us_counter = g_us_sd_header_this_block_data_size; l_us_counter < l_us_buffer_size; l_us_counter++)
		*(l_uc_array_p + l_us_counter) = 0x00;
	#endif
		
	//------------------------------------------------------------------------------------

	//------------------------------------------------------------------------------------
	//Write to SD card
	l_us_error_code = sdWriteSingleBlock(g_ul_sd_header_sd_write_block,l_uc_array_p,l_us_buffer_size);
	g_ul_sd_header_sd_write_block++;

	//Copy multiple times
	for (l_uc_i = 0; l_uc_i < g_uc_no_of_copies; l_uc_i++){
		//Change the block property to indicate copy
		*(l_uc_array_p+3) = l_uc_i+1;

		#if (CRC_ENABLE == 1)
		//Write new checksum bytes for copy	block
		if (CHECKSUM_DEFAULT & 0x40)	//If checksum default method is 16-bit
			insertCRC16(l_uc_array_p,g_us_sd_header_this_block_data_size);
		#endif

		//Write the copy to the SD card block
		l_us_error_code = sdWriteSingleBlock(g_ul_sd_header_sd_write_block,l_uc_array_p,l_us_buffer_size);
		g_ul_sd_header_sd_write_block++;
	}  
	
	return l_us_error_code;
}

//------------------------------------------------------------------------------------
// To save data with protection feature	(creating multiple copies of the data)
//------------------------------------------------------------------------------------
unsigned short saveDataProtected (unsigned char l_uc_subsystem, unsigned char l_uc_data_type, unsigned char *l_uc_array_p)
{
	unsigned short l_us_error_code;
	unsigned char l_uc_previous_sd_header_subsystem;
	unsigned char l_uc_previous_sd_header_data_type;

	//If storing data hold is not activated, proceeds. Else, returns error code
	if (!(g_uc_strsnd_flag & STRNG_FLAG_SAVE_DATA_HOLD)){
		//Save the previous SD header subsystem and data type
		//This is to prevent lose of data when loading
		//Hence saving process can NEVER use g_sd_header_subsystem 
		//and g_uc_sd_header_data_type as inputs
		l_uc_previous_sd_header_subsystem = g_uc_sd_header_subsystem;
		l_uc_previous_sd_header_data_type = g_uc_sd_header_data_type;

		//Initialize global variable to save the data
		g_uc_sd_header_subsystem = l_uc_subsystem;
		g_uc_sd_header_data_type = l_uc_data_type;	
	
		//Call init save data to handle data allocation
		l_us_error_code = initSDData();
	
		//Proceeds if there is no error code in data saving
		if (!l_us_error_code){
			//Save data based on initialization
			//Get the number of block used to save the data
			l_us_error_code = saveData(l_uc_array_p);
		
			//To handle block pointer updates after saving data process
			saveDataEndHandler(g_ul_sd_header_sd_write_block);
		}

		//Restore the previous g_uc_sd_header_subsystem and g_uc_sd_header_data_type
		g_uc_sd_header_subsystem = l_uc_previous_sd_header_subsystem;
		g_uc_sd_header_data_type = l_uc_previous_sd_header_data_type;	
	}
	else l_us_error_code = EC_SAVING_DATA_IS_DISABLED;

	return l_us_error_code;
}

//------------------------------------------------------------------------------------
// To handle block pointer updates after saving process
//------------------------------------------------------------------------------------
void saveDataEndHandler (unsigned long l_ul_new_bp)
{
	unsigned char l_uc_overwriting_flag = 0;

	//To handle block pointer handler after saving process for various data sets property
	//------------------------------------------------------------------------------------
	if (g_uc_data_sets_property & 0x01){	//If the write pointer is a variable
		//Update the value of that variable to follow the new bp value
		*g_ul_initial_sd_card_bp_address = l_ul_new_bp;

		//If the updated value is beyond allowed, rise the overwriting flag and 
		//Set the pointer to its original value
		if ((*g_ul_initial_sd_card_bp_address) > g_ul_memory_address_upper_limit){
			*g_ul_initial_sd_card_bp_address = g_ul_memory_address_lower_limit;
			l_uc_overwriting_flag = 0x01;
		}
	}
	//------------------------------------------------------------------------------------

	//------------------------------------------------------------------------------------
	if (g_uc_data_sets_property & 0x02)	//If the memory space is looping
		if (l_uc_overwriting_flag) //If overwriting flag is raised, increase overwritten value
			(*g_uc_no_of_overwritten_p)++;
	//------------------------------------------------------------------------------------	
}

//------------------------------------------------------------------------------------
// Attempt to save the current state of the satellite
//------------------------------------------------------------------------------------
void saveState (void)
{
	#if (SAVE_STATE_ENABLE == 1)
	//dec28_yc
	unsigned short l_us_error_code = EC_SUCCESSFUL;
	if(!l_us_error_code) l_us_error_code = saveDataProtected(ALL_SUBSYSTEMS, SD_ESSENTIAL, g_uc_sd_data_buffer);
	if(!l_us_error_code) l_us_error_code = saveDataProtected(OBDH_SUBSYSTEM, SD_SCHEDULE, g_uc_sd_data_buffer);
	if(!l_us_error_code) l_us_error_code = saveDataProtected(OBDH_SUBSYSTEM, SD_SCRIPT, g_uc_sd_data_buffer);
	#if (EVENT_HANDLER_ENABLE == 1)
	eventHandler(EV_SAVE_STATE+l_us_error_code,0,0,0,0,0,0);
	#endif
	#endif
}

//------------------------------------------------------------------------------------
// To load data with protection feature	(creating multiple copies of the data)
//------------------------------------------------------------------------------------
// returns non CRC-Free SDdata of relevant pg to g_uc_sd_data_buffer
// if (g_uc_strsnd_flag & STRNG_FLAG_SAVE_DATA_HOLD), 
// allows for l_ul_start_block_num = 0, l_ul_end_block_num = 0 
//------------------------------------------------------------------------------------
unsigned short loadDataProtected (unsigned char l_uc_subsystem, unsigned char l_uc_data_type, unsigned long l_ul_start_block_num, unsigned long l_ul_end_block_num)
{
	unsigned short l_us_error_code = EC_INIT;
	unsigned char l_uc_i = 0;
	unsigned short l_us_msg_length;		

	//Initialize global variable to load the data
	//Differs from saveDataProtected, loadDataProtected inputs,
	//although ideally should not NEVER be put as g_uc_sd_header_subsystem
	//and g_uc_sd_header_data_type respectively, can handle the case
	g_uc_sd_header_subsystem = l_uc_subsystem;
	g_uc_sd_header_data_type = l_uc_data_type;
	
	//Call init data to handle data allocation
	if (initSDData()) return EC_INVALID_LOADING_DATA_REQUEST;

	//If storing data hold is not activated, proceeds. Else, returns error code
	if(g_uc_sd_load_data_timeout_counter >= TIME_LIMIT_LOAD_DATA_S){			
	//	this is NEW read. update to global variables
		g_uc_sd_load_data_timeout_counter		= 0;
		g_ul_start_sd_card_load_bp 		= l_ul_start_block_num;
		g_ul_end_sd_card_load_bp 		= l_ul_end_block_num;
		g_uc_strsnd_flag |= (g_ul_end_sd_card_load_bp < g_ul_start_sd_card_load_bp)? (g_uc_data_sets_property & STRNG_FLAG_SD_LOAD_OVERWRITTEN):0;	//If the memory space is looping
		g_uc_strsnd_flag &= ~STRNG_FLAG_SD_LOAD_COMPLETED;
	}	

	//Continue...  check weather valid g_ul_start_sd_card_load_bp. CAN BE INVALID if we are continuing read but calling Func asks for different l_uc_subsystem set.
	if (g_ul_start_sd_card_load_bp < g_ul_memory_address_lower_limit || g_ul_start_sd_card_load_bp > g_ul_memory_address_end)	//invalid
		return EC_INVALID_LOADING_DATA_REQUEST;
		
	//Continue...  check weather valid g_ul_end_sd_card_load_bp. CAN BE INVALID if we are continuing read but calling Func asks for different l_uc_subsystem set.
	if (g_ul_end_sd_card_load_bp < g_ul_memory_address_lower_limit || g_ul_end_sd_card_load_bp > g_ul_memory_address_end)	//invalid
		return EC_INVALID_LOADING_DATA_REQUEST;

	//Continue if no error
	for (l_uc_i = 0; l_uc_i <= g_uc_no_of_copies; l_uc_i++){
		//next block to try to read
		//Ian (2012-05-15): l_ul_read_address is removed
		l_us_error_code = sdReadSingleBlock(g_ul_start_sd_card_load_bp + l_uc_i);

		l_us_msg_length = (unsigned short)(g_uc_sd_data_buffer[6] << 8) + g_uc_sd_data_buffer[7] + 1;

		//If this block data size is greater than allowed, return error code
		if (l_us_msg_length > MAX_SD_CARD_BLOCK_CONTENT_SIZE + SD_CARD_HEADER_SIZE) 
			l_us_error_code = EC_NO_DATA_STORED_IN_THIS_BLOCK;

		//Check if msg length is less than SD card header size, return error code
		if (l_us_msg_length < SD_CARD_HEADER_SIZE)
			l_us_error_code = EC_NO_DATA_STORED_IN_THIS_BLOCK;

		#if (CRC_ENABLE == 1)
		//check for data integrity
		if (!l_us_error_code)
			if(checkCRC16Match(g_uc_sd_data_buffer,l_us_msg_length) == EC_SUCCESSFUL) 	
				break;
		#endif

		//If it reaches this point, it means the data must have been all corrupted
		if (l_uc_i == g_uc_no_of_copies) l_us_error_code = EC_SD_ALL_STORED_DATA_IS_CORRUPTED;
	}//exit from loop due to no more copies to try or sucessful. Might contain error if last try not sucessful. But better wrong data than no data (YC).
	//Ian: No, of course no data is safer than wrong data =.='
	//If CRC fails, the loop will break. But if all data are corrupted, the loop will continue until the end and indicates that
	//all stored data are corrupted
					
	//update g_ul_start_sd_card_load_bp to next reading for the original blocks, skip the copies
	g_ul_start_sd_card_load_bp += (g_uc_no_of_copies + 1);
	
	//over size limit condition && //check g_us_sd_header_block_property for end
	if (g_ul_start_sd_card_load_bp > g_ul_memory_address_upper_limit && ((g_uc_sd_data_buffer[2] & 0x02) == 0x02)){	
		g_ul_start_sd_card_load_bp = g_ul_memory_address_lower_limit;
		g_uc_strsnd_flag &= ~STRNG_FLAG_SD_LOAD_OVERWRITTEN;	
	}		

	//Reset storing data hold activation timeout whenever this function is called within 30 seconds
	g_uc_sd_load_data_timeout_counter = 0;
	
	//FULL READ completed... reload load data timeout value
	//check g_us_sd_header_block_property for end
	if(g_ul_start_sd_card_load_bp > g_ul_end_sd_card_load_bp && !(g_uc_strsnd_flag & STRNG_FLAG_SD_LOAD_OVERWRITTEN) && ((g_uc_sd_data_buffer[2] & 0x02) == 0x02)){	
		g_uc_sd_load_data_timeout_counter = TIME_LIMIT_LOAD_DATA_S;
		g_uc_strsnd_flag |= STRNG_FLAG_SD_LOAD_COMPLETED;
	}					

	return l_us_error_code;
} 

//------------------------------------------------------------------------------------
// To load the latest state
//------------------------------------------------------------------------------------
void loadState (void)
{
	#if (LOAD_STATE_ENABLE == 1)
	
	unsigned short l_us_error_code = EC_INIT;
	unsigned char *l_uc_array_p;

	//Set pointer to SD data buffer
	l_uc_array_p = g_uc_sd_data_buffer;

	//Load the latest SD essential block
	l_us_error_code = loadDataProtected(ALL_SUBSYSTEMS, SD_ESSENTIAL, SD_BP_OBDH_ESS_START, SD_BP_OBDH_ESS_START);

	//Proceeds if there is no error code
	if (!l_us_error_code){
		//Set initial value for l_us_data_counter
		g_us_data_counter = SD_CARD_HEADER_SIZE;

		//Update OBDH essential data
		updateOBDHEssential(l_uc_array_p);

		//Next step, load schedule
		l_us_error_code = loadSchedule(l_uc_array_p);  //dec28_yc

		//Next step, load scripspace
		if (!l_us_error_code) l_us_error_code = loadScriptspace(l_uc_array_p);//dec28_yc
	}
	#if (EVENT_HANDLER_ENABLE == 1)
	eventHandler(EV_LOAD_STATE+l_us_error_code,0,0,0,0,0,0); //dec28_yc
	#endif

	#endif
}

#if (LOAD_STATE_ENABLE == 1)
//------------------------------------------------------------------------------------
// To load schedule with better efficiency
//------------------------------------------------------------------------------------
//dec28_yc changes to loadSchedule & loadScriptspace to include return EC_CODES
unsigned short loadSchedule (unsigned char *l_uc_array_p)
{
	unsigned short l_us_error_code = EC_INIT; //dec28_yc
	unsigned char l_uc_no_of_loading = 0;
	unsigned char l_uc_i;
	unsigned char l_uc_j;
	unsigned char l_uc_k;
	unsigned char l_uc_last_no_of_schedule_loaded;
	unsigned char l_uc_no_of_schedule_loaded;

	//Depends on how many maximum schedule is there, we can calculate no of loading needed
	l_uc_no_of_loading = (SCHEDULER_BLOCK_SIZE+19)/20;

	//Get the no of schedule needs to be loaded for the last time loading
	l_uc_last_no_of_schedule_loaded = SCHEDULER_BLOCK_SIZE%20;

	//Load the SD card that amount of time
	for (l_uc_i = 0; l_uc_i < l_uc_no_of_loading; l_uc_i++){
		//Load block of the schedule, redundant computation SD_BP_OBDH_SCHEDULE_START+l_uc_i*(NO_OF_COPIES_SD_SCHEDULE+1) is noticed.
		l_us_error_code = loadDataProtected(OBDH_SUBSYSTEM, SD_SCHEDULE, SD_BP_OBDH_SCHEDULE_START+l_uc_i*(NO_OF_COPIES_SD_SCHEDULE+1), SD_BP_OBDH_SCHEDULE_START+l_uc_i*(NO_OF_COPIES_SD_SCHEDULE+1));
	
		//if there is no error code, proceeds
		if (!l_us_error_code){
			//Reset global data counter
			g_us_data_counter = SD_CARD_HEADER_SIZE;

			//Get the schdule's block size (only for the 1st time)
			if (l_uc_i == 0) str_obdh_hk.uc_scheduler_block_size = *(l_uc_array_p+g_us_data_counter++);

			//Get 20 schedule or less at once
			l_uc_no_of_schedule_loaded = (l_uc_i < l_uc_no_of_loading-1) ? 20 : l_uc_last_no_of_schedule_loaded; 

			//Loop to get the schedule
			for (l_uc_j = 0; l_uc_j < l_uc_no_of_schedule_loaded; l_uc_j++){
				str_scheduler.uc_reindexing[l_uc_i*20+l_uc_j] = *(l_uc_array_p+g_us_data_counter++);
				str_scheduler.ul_scheduled_time[l_uc_i*20+l_uc_j] = insertArrayToLongGC(l_uc_array_p);
				str_scheduler.us_cmd[l_uc_i*20+l_uc_j] = insertArrayToShortGC(l_uc_array_p);
				str_scheduler.uc_arg_length[l_uc_i*20+l_uc_j] = *(l_uc_array_p+g_us_data_counter++);
				for (l_uc_k = 0; l_uc_k < MAX_SCHEDULER_ARG; l_uc_k++)
					str_scheduler.uc_arg[l_uc_i*20+l_uc_j][l_uc_k] = *(l_uc_array_p+g_us_data_counter++);
			}
		}
	} 
	return l_us_error_code;		//dec28_yc
}

//------------------------------------------------------------------------------------
// To load scriptspace
//------------------------------------------------------------------------------------
unsigned short loadScriptspace (unsigned char *l_uc_array_p)
{
	unsigned short l_us_error_code = EC_INIT;	//dec28_yc
	unsigned short l_us_no_of_data_byte = 0;
	unsigned char l_uc_i;
	unsigned char l_uc_j;
	unsigned char l_uc_k;

	//Depends on how many maximum scriptspace is there, we can calculate no of loading needed
	l_us_no_of_data_byte = (SCRIPTSPACE_SIZE*MAX_SCRIPTLINE_SIZE*MAX_SCRIPT_LENGTH) + TASK_SIZE;

	//So long as the no of data byte is less than certain threshold, this condition will always be true
	//Else, more code will be needed
	if (l_us_no_of_data_byte <= MAX_SD_CARD_BLOCK_CONTENT_SIZE){
		//Load block of the scriptspace
		l_us_error_code = loadDataProtected(OBDH_SUBSYSTEM, SD_SCRIPT, SD_BP_OBDH_SCRIPT_START, SD_BP_OBDH_SCRIPT_START);
	
		//if there is no error code, proceeds
		if (!l_us_error_code){
			//Reset global data counter
			g_us_data_counter = SD_CARD_HEADER_SIZE;

			//Get the scriptspace of the tasks
			for (l_uc_i = 0; l_uc_i < TASK_SIZE; l_uc_i++)
				str_task[l_uc_i].uc_scriptspace =  *(l_uc_array_p+g_us_data_counter++);

			//Fill the scriptspaces
			//The scriptspace is written when saveDataProtected is called with inputs
			//(g_uc_sd_header_subsystem == OBDH_SUBSYSTEM) && (g_uc_sd_header_data_type == SD_SCRIPT)
			for (l_uc_i = 0; l_uc_i < SCRIPTSPACE_SIZE; l_uc_i++)
				for (l_uc_j = 0; l_uc_j < MAX_SCRIPTLINE_SIZE; l_uc_j++)
					for (l_uc_k = 0; l_uc_k < MAX_SCRIPT_LENGTH; l_uc_k++)
						str_scriptspace[l_uc_i].uc_script[l_uc_j][l_uc_k] = *(l_uc_array_p + g_us_data_counter++);
		}
	}
	return l_us_error_code;	  //dec28_yc
}
#endif

//------------------------------------------------------------------------------------
// To update OBDH essential from an array using global data counter
//------------------------------------------------------------------------------------
// It is noted that the design of updateOBDHEssential() allows input from any array
//------------------------------------------------------------------------------------
void updateOBDHEssential (unsigned char *l_uc_array_p)
{
	unsigned short l_us_counter_i;

	#if (PWRS_CMD_ENABLE == 1)
	//Set as if there is I2C msg to get PWRS SSCP
	g_uc_i2c_msg_out = PWRS_I2C_GET_SSCP;

	//Load PWRS SSCP
	storePWRS(l_uc_array_p);
	#endif

	#if (ADCS_CMD_ENABLE == 1)
	//Set as if there is I2C msg to get ADCS SSCP
	g_uc_i2c_msg_out = ADCS_I2C_GET_SSCP;

	//Load ADCS SSCP
	storeADCS(l_uc_array_p);
	#endif

	//Load OBDH various data
	updateOBDHSSCP(l_uc_array_p);
	updateOBDHSelectedHK(l_uc_array_p);
	updateOBDHSelectedData(l_uc_array_p);
	updateOBDHBP(l_uc_array_p);

	#if (TIME_ENABLE == 1)
	//Load SAT_DATE_INIT
	str_sat_date.uc_init_day_of_month = *(l_uc_array_p + g_us_data_counter++);
	str_sat_date.uc_init_month = *(l_uc_array_p + g_us_data_counter++);
	str_sat_date.uc_init_year = *(l_uc_array_p + g_us_data_counter++);
	str_sat_date.uc_init_hour = *(l_uc_array_p + g_us_data_counter++);
	str_sat_date.uc_init_min = *(l_uc_array_p + g_us_data_counter++);
	str_sat_date.uc_init_s = *(l_uc_array_p + g_us_data_counter++);
	str_sat_date.us_init_ms = insertArrayToShortGC(l_uc_array_p);
	#endif

	//Load from to callsign
	for (l_us_counter_i = 0; l_us_counter_i < AX25_CALLSIGN_SIZE; l_us_counter_i++)	
		g_uc_from_to_callsign[l_us_counter_i] = *(l_uc_array_p + g_us_data_counter++);

}

//------------------------------------------------------------------------------------
// To update OBDH SSCP from an array using global data counter
//------------------------------------------------------------------------------------
void updateOBDHSSCP (unsigned char *l_uc_array_p)
{
	unsigned short l_us_counter_i;

	str_obdh_par.uc_manual_control_enabled = *(l_uc_array_p + g_us_data_counter++);
	str_obdh_par.uc_no_of_interrupt_per_s = *(l_uc_array_p + g_us_data_counter++);
	str_obdh_par.uc_gsch_time_window_10s = *(l_uc_array_p + g_us_data_counter++);
	for (l_us_counter_i = 0; l_us_counter_i < NO_OF_SOC_THRESHOLD_PAR; l_us_counter_i++)			
		str_obdh_par.uc_soc_threshold[l_us_counter_i] = *(l_uc_array_p + g_us_data_counter++);
	str_obdh_par.us_rev_time_s = insertArrayToShortGC(l_uc_array_p);
	for (l_us_counter_i = 0; l_us_counter_i < MAX_NO_OF_DATA_TYPE_LOGGED; l_us_counter_i++)
		str_obdh_par.us_log_data_id[l_us_counter_i] = insertArrayToShortGC(l_uc_array_p);
	for (l_us_counter_i = 0; l_us_counter_i < 3; l_us_counter_i++)
		str_obdh_par.us_br_threshold_low[l_us_counter_i] = insertArrayToShortGC(l_uc_array_p);
	for (l_us_counter_i = 0; l_us_counter_i < 3; l_us_counter_i++)
		str_obdh_par.us_br_threshold_high[l_us_counter_i] = insertArrayToShortGC(l_uc_array_p);
	for (l_us_counter_i = 0; l_us_counter_i < NO_OF_BEACON_INTERVAL; l_us_counter_i++)
		str_obdh_par.uc_beacon_interval[l_us_counter_i] = *(l_uc_array_p + g_us_data_counter++);
	str_obdh_par.ul_det_uptime_limit_s = insertArrayToLongGC(l_uc_array_p);
}

//------------------------------------------------------------------------------------
// To update OBDH Selected HK from an array using global data counter
//------------------------------------------------------------------------------------
void updateOBDHSelectedHK (unsigned char *l_uc_array_p)
{
	str_obdh_hk.uc_sat_mode = *(l_uc_array_p + g_us_data_counter++);
	str_obdh_hk.ul_wod_sample_count = insertArrayToLongGC(l_uc_array_p);
	str_obdh_hk.uc_scheduler_block_size = *(l_uc_array_p + g_us_data_counter++);
	str_obdh_hk.us_beacon_flag = insertArrayToShortGC(l_uc_array_p);
	str_obdh_hk.us_cw_char_rate_par = insertArrayToShortGC(l_uc_array_p);
	str_obdh_hk.uc_ax25_bit_rate_par = *(l_uc_array_p + g_us_data_counter++);
	str_obdh_hk.uc_mcu_rst_count = *(l_uc_array_p + g_us_data_counter++);
	str_obdh_hk.ul_data_byte_sent = insertArrayToLongGC(l_uc_array_p);

	#if (SATELLITE_MODE_HANDLER_ENABLE == 1)
	//Only M2, M3, and M5 modes are valid for the first initialization
	if (str_obdh_hk.uc_sat_mode >= SATELLITE_MODE_M5) enterM5();
	else if ((str_obdh_hk.uc_sat_mode == SATELLITE_MODE_M3) || (str_obdh_hk.uc_sat_mode == SATELLITE_MODE_M4)) enterM3();
	#endif

	//Keep the auto-beacon disabling flag. Reset the rests.
	str_obdh_hk.us_beacon_flag &= BEACON_FLAG_AUTO_BEACON_DISABLE;
}

//------------------------------------------------------------------------------------
// To update OBDH Selected Data from an array using global data counter
//------------------------------------------------------------------------------------
void updateOBDHSelectedData (unsigned char *l_uc_array_p)
{
	unsigned short l_us_counter_i;

	str_obdh_data.uc_sat_rstsrc = *(l_uc_array_p + g_us_data_counter++);
	str_obdh_hk.uc_sat_rstsrc = str_obdh_data.uc_sat_rstsrc;
	str_obdh_data.uc_sat_rstsrc = RESET_SOURCE_UNKNOWN;
	str_obdh_data.ul_obc_time_s = insertArrayToLongGC(l_uc_array_p)+5; //Includes 5 seconds delay
	str_obdh_data.us_current_rev_no = insertArrayToShortGC(l_uc_array_p);
	str_obdh_data.us_rev_offset_time = insertArrayToShortGC(l_uc_array_p);
	for (l_us_counter_i = 0; l_us_counter_i < OBDH_DATA_INTERVAL_PERIOD_SIZE; l_us_counter_i++)	
		str_obdh_data.us_interval_period[l_us_counter_i] = insertArrayToShortGC(l_uc_array_p);
	str_obdh_data.uc_previous_mode = *(l_uc_array_p + g_us_data_counter++);	
	str_obdh_data.f_gs_reference_rev_no = insertArrayToFloatGC(l_uc_array_p);
	str_obdh_data.ul_gs_ref_time_s = insertArrayToLongGC(l_uc_array_p);
	str_obdh_data.us_auto_reference_rev_no = insertArrayToShortGC(l_uc_array_p);
	str_obdh_data.us_gs_reference_rev_no = insertArrayToShortGC(l_uc_array_p);
	str_obdh_data.uc_orbit_no_changed_counter = *(l_uc_array_p + g_us_data_counter++);

	//Set the previous mode following the current mode
	str_obdh_data.uc_previous_mode = str_obdh_hk.uc_sat_mode;
}

//------------------------------------------------------------------------------------
// To update OBDH block pointers from an array using global data counter
//------------------------------------------------------------------------------------
void updateOBDHBP (unsigned char *l_uc_array_p)
{
	str_bp.ul_hk_write_p = insertArrayToLongGC(l_uc_array_p);
	str_bp.ul_datalog_write_p = insertArrayToLongGC(l_uc_array_p);
	if (str_bp.ul_datalog_write_p != SD_BP_DATALOG_START) 
		g_us_log_data_block_size = (str_bp.ul_datalog_write_p-SD_BP_DATALOG_START)/(1+NO_OF_COPIES_SD_LOG_DATA);
//	str_bp.ul_package_recovery_write_p = insertArrayToLongGC(l_uc_array_p);
	//Loading of the datalog write pointer and package recovery block pointer are not needed
	//Therefore, they are skipped.
	g_us_data_counter += 4;
	str_bp.ul_eventlog_write_p = insertArrayToLongGC(l_uc_array_p);
	str_bp.uc_hk_overwritten = *(l_uc_array_p + g_us_data_counter++);
	str_bp.uc_orbit_indexing_overwritten = *(l_uc_array_p + g_us_data_counter++);
}

//------------------------------------------------------------------------------------
// To search correct block pointer
//------------------------------------------------------------------------------------
unsigned short searchBPByOrbitInfo (unsigned char l_uc_data_type)
{   //To search the g_ul_obs_initial_bp & g_ul_obs_end_bp
	unsigned short l_us_error_code = EC_SUCCESSFUL;
	unsigned long l_uc_search_block;
	unsigned long l_ul_next_orbit_bp;
	unsigned char l_uc_search_byte_num;
	unsigned char l_uc_no_of_copies;
	 
	//get the l_uc_search_byte_num. Please refer to fillData()::(l_us_data_type == DATA_CAT_OBDH_BP) 
	// data type very related to this ... (g_uc_data_sets_property & 0x01)

	switch (l_uc_data_type){  
		//case SD_ESSENTIAL:				return EC_INVALID_INPUT; 							break;
		case SD_HK:							
			l_uc_no_of_copies = 2;	
			l_uc_search_byte_num = SD_CARD_HEADER_SIZE + 0; 		
			break;
		//case SD_IMAGE:					l_uc_search_byte_num=SD_CARD_HEADER_SIZE+X;			break;
		//case SD_FLASH_PROGRAM:			return EC_INVALID_INPUT; 							break;
		//case SD_BP:						return EC_INVALID_INPUT; 							break;
		case SD_EVENTLOG_DATA:							
			l_uc_no_of_copies = 2;	
			l_uc_search_byte_num = SD_CARD_HEADER_SIZE + 12; 		
			break;
		//case SD_SCHEDULE:					return EC_INVALID_INPUT; 							break;
		//case SD_SCRIPT:					return EC_INVALID_INPUT; 							break;
		//case SD_LOG_DATA:					l_uc_search_byte_num=SD_CARD_HEADER_SIZE+X; 		break;
		//case SD_COMPLETE_TASK_STRUCTURE:	return EC_INVALID_INPUT; 							break;								
	   	default: 							
			l_us_error_code = EC_INVALID_SEARCHING_INPUT;							
			break;
	}
	
	if (l_us_error_code) return l_us_error_code;

	//Check validity of the orbit request, the request must be within the current orbit
	if ((g_us_obs_first_orbit_request <= str_obdh_data.us_current_rev_no) && (g_us_obs_last_orbit_request <= str_obdh_data.us_current_rev_no)){
		// Search g_ul_load_start_address	 
		l_uc_search_block 	= SD_BP_OBDH_ORBIT_START + g_us_obs_first_orbit_request * (l_uc_no_of_copies+1);
		l_us_error_code		= loadDataProtected(OBDH_SUBSYSTEM,SD_BP,l_uc_search_block,l_uc_search_block); //Subjected to change for data with greater block no	for block pointer search
		if (l_us_error_code) return l_us_error_code;			
		
		//Continue if there is no error code
		g_ul_obs_initial_bp = uchar2ulong(g_uc_sd_data_buffer[l_uc_search_byte_num],g_uc_sd_data_buffer[l_uc_search_byte_num+1],g_uc_sd_data_buffer[l_uc_search_byte_num+2],g_uc_sd_data_buffer[l_uc_search_byte_num+3]);
		 
		// Search g_ul_load_end_address
		if(g_us_obs_last_orbit_request >= str_obdh_data.us_current_rev_no)	
			g_ul_obs_end_bp = str_bp.ul_hk_write_p;
		else{ //SD_BP_OBDH_ORBIT_START + str_obdh_data.us_current_rev_no*(g_uc_no_of_copies+1);
			l_uc_search_block 	= SD_BP_OBDH_ORBIT_START + (g_us_obs_last_orbit_request+1) * (l_uc_no_of_copies+1);
			l_us_error_code		= loadDataProtected(OBDH_SUBSYSTEM,SD_BP,l_uc_search_block,l_uc_search_block);
			if(l_us_error_code) return l_us_error_code;
			l_ul_next_orbit_bp = uchar2ulong(g_uc_sd_data_buffer[l_uc_search_byte_num],g_uc_sd_data_buffer[l_uc_search_byte_num+1],g_uc_sd_data_buffer[l_uc_search_byte_num+2],g_uc_sd_data_buffer[l_uc_search_byte_num+3]);
			//If the next orbit block pointer is the same as the current one, just get it
			if (l_ul_next_orbit_bp == g_ul_obs_initial_bp)
				g_ul_obs_end_bp = l_ul_next_orbit_bp;				
			//Ideal case would be this, if the bp is greater than the current one
			else if (l_ul_next_orbit_bp > g_ul_obs_initial_bp)
				g_ul_obs_end_bp = l_ul_next_orbit_bp - (l_uc_no_of_copies + 1);
			//The most problematic case would be if the next orbit bp is less than. This means, overflowing happens
			//The case is taken care by loading function. Do nothing here.
		}
	
		return EC_SUCCESSFUL;
	} 
	//If the orbit request is invalid, return error code
	else return EC_RECORD_FOR_REQUESTED_ORBIT_DOES_NOT_EXIST;

	return l_us_error_code;
}
#endif
