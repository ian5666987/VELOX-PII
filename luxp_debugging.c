//------------------------------------------------------------------------------------
// Luxp_debugging.c
//------------------------------------------------------------------------------------
// Copyright (C) 2013 Nanyang Technological University.
//
// AUTH: IAN
// DATE: 03 APR 2013
//
// The program Luxp_debugging.c will be downloaded to the Pumpkin Motherboard with 
// C8051F120 uC as part of the VELOX-P Pico-Satellite Flight Software. 
//
// There is one main function contained in this file, namely
// 1) Change Data
//
//------------------------------------------------------------------------------------

#include <luxp_debugging.h>

//------------------------------------------------------------------------------------
// To change data of a variable
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
void changeData (unsigned short l_us_data_code, unsigned char l_uc_data_1, unsigned char l_uc_data_2, unsigned char l_uc_data_3, unsigned char l_uc_data_4)
{
	unsigned char l_uc_data_code_header;

	l_uc_data_code_header = l_us_data_code >> 8;

	switch (l_uc_data_code_header){

		#if (PWRS_CMD_ENABLE == 1)

		//PWRS data
		case PWRS_ADDR:
			switch (l_us_data_code){
				case DATA_PWRS_HK_UC_MODE:
					str_pwrs_hk.uc_mode = l_uc_data_4;
					break;

				case DATA_PWRS_PAR_UC_BATT_CHARGE_UPP_LIMIT:
					str_pwrs_par.uc_batt_charge_upp_limit = l_uc_data_4;
					break;

				case DATA_PWRS_PAR_UC_BATT_CHARGE_LOW_LIMIT:
					str_pwrs_par.uc_batt_charge_low_limit = l_uc_data_4;
					break;

				case DATA_PWRS_US_PV_FV_1:
				case DATA_PWRS_US_PV_FV_2:
				case DATA_PWRS_US_PV_FV_3:
					str_pwrs_data.us_pv_fv[l_us_data_code-DATA_PWRS_US_PV_FV_1] = (unsigned short)(l_uc_data_3 << 8) + l_uc_data_4;
					break;

				default:
					break;
			}
			break;

		#endif
		
		#if (ADCS_CMD_ENABLE == 1)

		//ADCS data
		case ADCS_ADDR:
			switch (l_us_data_code){
				case DATA_ADCS_PAR_S_SS_THRESHOLD:
					str_adcs_par.s_ss_threshold = (unsigned short)(l_uc_data_3 << 8) + l_uc_data_4;
					break;

				case DATA_ADCS_PAR_F_CGAIN_1:
				case DATA_ADCS_PAR_F_CGAIN_2:
				case DATA_ADCS_PAR_F_CGAIN_3:
				case DATA_ADCS_PAR_F_CGAIN_4:
				case DATA_ADCS_PAR_F_CGAIN_5:
				case DATA_ADCS_PAR_F_CGAIN_6:
				case DATA_ADCS_PAR_F_CGAIN_7:
				case DATA_ADCS_PAR_F_CGAIN_8:
				case DATA_ADCS_PAR_F_CGAIN_9:
				case DATA_ADCS_PAR_F_CGAIN_10:
				case DATA_ADCS_PAR_F_CGAIN_11:
				case DATA_ADCS_PAR_F_CGAIN_12:
				case DATA_ADCS_PAR_F_CGAIN_13:
				case DATA_ADCS_PAR_F_CGAIN_14:
				case DATA_ADCS_PAR_F_CGAIN_15:
				case DATA_ADCS_PAR_F_CGAIN_16:
				case DATA_ADCS_PAR_F_CGAIN_17:
				case DATA_ADCS_PAR_F_CGAIN_18:
					str_adcs_par.f_cgain[l_us_data_code-DATA_ADCS_PAR_F_CGAIN_1] = uchar2float(l_uc_data_1,l_uc_data_2,l_uc_data_3,l_uc_data_4);
					break;

				case DATA_ADCS_PAR_F_TLE_EPOCH:
					str_adcs_par.f_tle_epoch = uchar2float(l_uc_data_1,l_uc_data_2,l_uc_data_3,l_uc_data_4);
					break;

				case DATA_ADCS_PAR_F_TLE_INC:
					str_adcs_par.f_tle_inc = uchar2float(l_uc_data_1,l_uc_data_2,l_uc_data_3,l_uc_data_4);
					break;

				case DATA_ADCS_PAR_F_TLE_RAAN:
					str_adcs_par.f_tle_raan = uchar2float(l_uc_data_1,l_uc_data_2,l_uc_data_3,l_uc_data_4);
					break;

				case DATA_ADCS_PAR_F_TLE_ARG_PER:
					str_adcs_par.f_tle_arg_per = uchar2float(l_uc_data_1,l_uc_data_2,l_uc_data_3,l_uc_data_4);
					break;

				case DATA_ADCS_PAR_F_TLE_ECC:
					str_adcs_par.f_tle_ecc = uchar2float(l_uc_data_1,l_uc_data_2,l_uc_data_3,l_uc_data_4);
					break;

				case DATA_ADCS_PAR_F_TLE_MA:
					str_adcs_par.f_tle_ma = uchar2float(l_uc_data_1,l_uc_data_2,l_uc_data_3,l_uc_data_4);
					break;

				case DATA_ADCS_PAR_US_TLE_MM:
					str_adcs_par.us_tle_mm = (unsigned short)(l_uc_data_3 << 8) + l_uc_data_4;
					break;

				case DATA_ADCS_PAR_UL_TIME:
					str_adcs_par.ul_time = uchar2ulong(l_uc_data_1,l_uc_data_2,l_uc_data_3,l_uc_data_4);
					break;

				case DATA_ADCS_PAR_S_VECTOR_2SUN_X:
				case DATA_ADCS_PAR_S_VECTOR_2SUN_Y:
				case DATA_ADCS_PAR_S_VECTOR_2SUN_Z:
					str_adcs_par.s_vector_2sun[l_us_data_code-DATA_ADCS_PAR_S_VECTOR_2SUN_X] = (unsigned short)(l_uc_data_3 << 8) + l_uc_data_4;
					break;

				case DATA_ADCS_PAR_C_CTRL_BR_X:
				case DATA_ADCS_PAR_C_CTRL_BR_Y:
				case DATA_ADCS_PAR_C_CTRL_BR_Z:
					str_adcs_par.c_ctrl_br[l_us_data_code-DATA_ADCS_PAR_C_CTRL_BR_X] = l_uc_data_4;
					break;

				case DATA_ADCS_PAR_S_DGAIN:
					str_adcs_par.s_dgain = (unsigned short)(l_uc_data_3 << 8) + l_uc_data_4;
					break;

				default:
					break;
			}
			break;

		#endif
		
		#if (OBDH_CMD_ENABLE == 1)

		//OBDH data
		//Ian (2012-05-15): comments are removed
		case OBDH_ADDR:
			switch (l_us_data_code){
				case DATA_OBDH_PAR_UC_MANUAL_CONTROL_ENABLED:
					str_obdh_par.uc_manual_control_enabled = l_uc_data_4;
					break;

				case DATA_OBDH_PAR_UC_NO_OF_INTERRUPT_PER_S:
					str_obdh_par.uc_no_of_interrupt_per_s = l_uc_data_4;
					break;

				case DATA_OBDH_PAR_UC_GSCH_TIME_WINDOW_10S:
					str_obdh_par.uc_gsch_time_window_10s = l_uc_data_4;
					break;

				case DATA_OBDH_PAR_UC_SOC_THRESHOLD_1:
				case DATA_OBDH_PAR_UC_SOC_THRESHOLD_2:
				case DATA_OBDH_PAR_UC_SOC_THRESHOLD_3:
				case DATA_OBDH_PAR_UC_SOC_THRESHOLD_4:
				case DATA_OBDH_PAR_UC_SOC_THRESHOLD_5:
				case DATA_OBDH_PAR_UC_SOC_THRESHOLD_6:
				case DATA_OBDH_PAR_UC_SOC_THRESHOLD_7:
				case DATA_OBDH_PAR_UC_SOC_THRESHOLD_8:
					str_obdh_par.uc_soc_threshold[l_us_data_code-DATA_OBDH_PAR_UC_SOC_THRESHOLD_1] = l_uc_data_4;
					break;

				case DATA_OBDH_PAR_US_REV_TIME_S:
					str_obdh_par.us_rev_time_s = (unsigned short)(l_uc_data_3 << 8) + l_uc_data_4;
					break;

				case DATA_OBDH_PAR_US_LOG_DATA_ID_1:
				case DATA_OBDH_PAR_US_LOG_DATA_ID_2:
				case DATA_OBDH_PAR_US_LOG_DATA_ID_3:
				case DATA_OBDH_PAR_US_LOG_DATA_ID_4:
				case DATA_OBDH_PAR_US_LOG_DATA_ID_5:
				case DATA_OBDH_PAR_US_LOG_DATA_ID_6:
				case DATA_OBDH_PAR_US_LOG_DATA_ID_7:
				case DATA_OBDH_PAR_US_LOG_DATA_ID_8:
					str_obdh_par.us_log_data_id[l_us_data_code-DATA_OBDH_PAR_US_LOG_DATA_ID_1] = (unsigned short)(l_uc_data_3 << 8) + l_uc_data_4;
					break;

				case DATA_OBDH_PAR_US_BR_X_THRESHOLD_LOW:
				case DATA_OBDH_PAR_US_BR_Y_THRESHOLD_LOW:
				case DATA_OBDH_PAR_US_BR_Z_THRESHOLD_LOW:
					str_obdh_par.us_br_threshold_low[l_us_data_code-DATA_OBDH_PAR_US_BR_X_THRESHOLD_LOW] = (unsigned short)(l_uc_data_3 << 8) + l_uc_data_4;
					break;

				case DATA_OBDH_PAR_US_BR_X_THRESHOLD_HIGH:
				case DATA_OBDH_PAR_US_BR_Y_THRESHOLD_HIGH:
				case DATA_OBDH_PAR_US_BR_Z_THRESHOLD_HIGH:
					str_obdh_par.us_br_threshold_high[l_us_data_code-DATA_OBDH_PAR_US_BR_X_THRESHOLD_HIGH] = (unsigned short)(l_uc_data_3 << 8) + l_uc_data_4;
					break;

				case DATA_OBDH_PAR_UC_BEACON_INTERVAL_M3:
				case DATA_OBDH_PAR_UC_BEACON_INTERVAL_M4_M5_M7:
				case DATA_OBDH_PAR_UC_BEACON_INTERVAL_M8:
				case DATA_OBDH_PAR_UC_BEACON_INTERVAL_COMM_ZONE:
					str_obdh_par.uc_beacon_interval[l_us_data_code-DATA_OBDH_PAR_UC_BEACON_INTERVAL_M3] = l_uc_data_4;
					break;

				case DATA_OBDH_PAR_UL_DET_UPTIME_LIMIT:
					str_obdh_par.ul_det_uptime_limit_s = uchar2ulong(l_uc_data_1,l_uc_data_2,l_uc_data_3,l_uc_data_4);
					break;

				#if (DEBUGGING_FEATURE == 1)
				case DATA_OBDH_UC_SCM_HOLD_ACTIVATION:
					g_uc_evha_flag &= ~HACVTN_FLAG_SCM;
					g_uc_evha_flag |= (l_uc_data_4 & HACVTN_FLAG_SCM);
					break;

				case DATA_OBDH_UC_TM_HOLD_ACTIVATION:
					g_uc_evha_flag &= ~HACVTN_FLAG_TM;
					g_uc_evha_flag |= (l_uc_data_4 & HACVTN_FLAG_TM);
					break;

				case DATA_OBDH_UC_AUTO_BEACON_ACTIVATION:
					if (l_uc_data_4) 
						str_obdh_hk.us_beacon_flag &= ~BEACON_FLAG_AUTO_BEACON_DISABLE;
					else str_obdh_hk.us_beacon_flag |= BEACON_FLAG_AUTO_BEACON_DISABLE;					 					
					break;
				#endif

				case DATA_OBDH_US_INTERVAL_PERIOD_1:
				case DATA_OBDH_US_INTERVAL_PERIOD_2:
				case DATA_OBDH_US_INTERVAL_PERIOD_3:
				case DATA_OBDH_US_INTERVAL_PERIOD_4:
				case DATA_OBDH_US_INTERVAL_PERIOD_5:
				case DATA_OBDH_US_INTERVAL_PERIOD_6:
				case DATA_OBDH_US_INTERVAL_PERIOD_7:
				case DATA_OBDH_US_INTERVAL_PERIOD_8:
				case DATA_OBDH_US_INTERVAL_PERIOD_9:
				case DATA_OBDH_US_INTERVAL_PERIOD_10:
				case DATA_OBDH_US_INTERVAL_PERIOD_11:
				case DATA_OBDH_US_INTERVAL_PERIOD_12:
				case DATA_OBDH_US_INTERVAL_PERIOD_13:
				case DATA_OBDH_US_INTERVAL_PERIOD_14:
				case DATA_OBDH_US_INTERVAL_PERIOD_15:
				case DATA_OBDH_US_INTERVAL_PERIOD_16:
					str_obdh_data.us_interval_period[l_us_data_code-DATA_OBDH_US_INTERVAL_PERIOD_1] = (unsigned short)(l_uc_data_3 << 8) + l_uc_data_4;
					break;

				case DATA_OBDH_G_UC_FROM_TO_CALLSIGN_1:
				case DATA_OBDH_G_UC_FROM_TO_CALLSIGN_2:
				case DATA_OBDH_G_UC_FROM_TO_CALLSIGN_3:
				case DATA_OBDH_G_UC_FROM_TO_CALLSIGN_4:
				case DATA_OBDH_G_UC_FROM_TO_CALLSIGN_5:
				case DATA_OBDH_G_UC_FROM_TO_CALLSIGN_6:
				case DATA_OBDH_G_UC_FROM_TO_CALLSIGN_7:
				case DATA_OBDH_G_UC_FROM_TO_CALLSIGN_8:
				case DATA_OBDH_G_UC_FROM_TO_CALLSIGN_9:
				case DATA_OBDH_G_UC_FROM_TO_CALLSIGN_10:
				case DATA_OBDH_G_UC_FROM_TO_CALLSIGN_11:
				case DATA_OBDH_G_UC_FROM_TO_CALLSIGN_12:
				case DATA_OBDH_G_UC_FROM_TO_CALLSIGN_13:
				case DATA_OBDH_G_UC_FROM_TO_CALLSIGN_14:
					g_uc_from_to_callsign[l_us_data_code-DATA_OBDH_G_UC_FROM_TO_CALLSIGN_1] = l_uc_data_4;
					break;

				//Ian, 19-JUL-2012: to support satellite time reset
				case DATA_OBDH_G_UL_OBC_TIME:
					str_obdh_data.ul_obc_time_s = uchar2ulong(l_uc_data_1,l_uc_data_2,l_uc_data_3,l_uc_data_4);
					break;

				default:
					break;
			}
			break;
		
		#endif

		default:
			break;

	}
}

