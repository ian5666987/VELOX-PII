//------------------------------------------------------------------------------------
// Luxp_extra.c
//------------------------------------------------------------------------------------
// Copyright (C) 2013 Nanyang Technological University.
//
// AUTH: IAN
// DATE: 03 APR 2013
//
// The program Luxp_extra.c will be downloaded to the Pumpkin Motherboard with 
// C8051F120 uC as part of the VELOX-P Pico-Satellite Flight Software. 
//
//------------------------------------------------------------------------------------

#include <luxp_extra.h>

//------------------------------------------------------------------------------------
// Extra functions library
//------------------------------------------------------------------------------------

#if (BEACON_HANDLER_ENABLE == 1)
#if (ISIS_CMD_ENABLE == 1)
#if (SENDING_ENABLE == 1)
//------------------------------------------------------------------------------------
// To recover from an extremely long data transmission
//------------------------------------------------------------------------------------
unsigned short overlyLongTransmissionRecovery (void)
{
	unsigned short l_us_error_code[4] = EC_INIT;
	unsigned char l_uc_i = 0;

	//Reset ITC board
	l_us_error_code[0] = isisI2C(ITC_ADDR,ITC_I2C_RESET,0);

	//Wait for 3s (hopefully this is sufficient)
	waitS(3);

	//If there is no error code
	if (!l_us_error_code[0]){
		//Set beacon as it is supposed to.
		g_uc_ax25_message[l_uc_i++] = OBDH_SUBSYSTEM;
		g_uc_ax25_message[l_uc_i++] = SD_RCVRY_MSG;
		g_uc_ax25_message[l_uc_i++] = 0x02;
		g_uc_ax25_message[l_uc_i++] = 0x01;
		g_uc_ax25_message[l_uc_i++] = 0x08;
		g_uc_ax25_message[l_uc_i++] = 0x49;
		g_uc_ax25_message[l_uc_i++] = 0x5A;
		g_uc_ax25_message[l_uc_i++] = 0x59;

		#if (CRC_ENABLE == 1)
		//Create CRC16 bytes for the message
		insertCRC16(g_uc_ax25_message,l_uc_i);
		#endif
		
		//Set AX.25 beacon
		l_us_error_code[0] = isisI2CMsg(ITC_I2C_SET_AX_BCN_DFLT,30,l_uc_i+2,0);

		//Wait for 500ms
		waitMs(200);
		waitMs(200);
		waitMs(100);
		
		//If there is no error code, continues with clearing the beacon and do
		//re-initialization of the beacon.
		if (!l_us_error_code[0]){
			//Initialize CW beacon rate to default
			l_us_error_code[0] = isisI2CPar(ITC_I2C_SET_CW_CHAR_RT,str_obdh_hk.us_cw_char_rate_par,0);
		
			//Clear beacon
			l_us_error_code[1] = isisI2C(ITC_ADDR,ITC_I2C_CLR_BCN,0);

			//Set the beacon's TO callsigns
			l_us_error_code[2] = isisI2CMsg(ITC_I2C_SET_DFLT_AX_TO,0,0,0);
		
			//Set the beacon's FROM callsigns
			l_us_error_code[3] = isisI2CMsg(ITC_I2C_SET_DFLT_AX_FROM,0,0,0);		
		}
	}

	return l_us_error_code[0];
}
#endif
#endif
#endif

#if (EXTRA_ENABLE == 1)
#if (UPDATE_HANDLER_ENABLE == 1)
//-----------------------------------------------------------------------------
// convertA2D
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters:
//   1) unsigned char l_uc_ad_ch - 
//
// This function convert the voltage at the selected A/D input
//
//-----------------------------------------------------------------------------
//There is no Convert_AD0() function in OBDH software, there is only one function
//convertA2D() in itand thus declared here.
unsigned short convertA2D (unsigned char l_uc_ad_ch)
{
	//Save the current SFR page
  	char SFRPAGE_SAVE = SFRPAGE;

	unsigned short	l_us_ad_results;

	//Switch to ADC0
	SFRPAGE = ADC0_PAGE;

	//Select the ADC mux input
	AMX0SL = l_uc_ad_ch;

	//lsl : 03 May 2011 : 1719hrs
	//100us delay is required
	//While AD0INT is down, it has not finished converting.
	waitUs(100);

	//Step 1: Clear AD0INT
	AD0INT = 0;

	//Step 2: Set AD0BUSY
	AD0BUSY = 1;

	//Set start flag for polling later
	g_uc_spit0ad0_flag |= AD0INT_FLAG_START;

	//Step 3: Poll ADOINT for 1
	while (!AD0INT && !(g_uc_spit0ad0_flag & AD0INT_FLAG_TIMEOUT))

	//Reset start and timeout flag
	g_uc_spit0ad0_flag &= ~AD0INT_FLAG_START;
	g_uc_spit0ad0_flag &= ~AD0INT_FLAG_TIMEOUT;

	//Store conversion results
	l_us_ad_results = ADC0;

	//Clear ADC conversion complete indicator
	AD0INT = 0;

	//Restore the SFRPAGE
	SFRPAGE = SFRPAGE_SAVE;

	return (l_us_ad_results);
}
#endif

//------------------------------------------------------------------------------------
// To ensure satellite reset to occur
//------------------------------------------------------------------------------------
void resetSatellite (void)
{
	unsigned short l_us_error_code = EC_INIT;
	unsigned long l_ul_scheduled_time;

	//Set the flag to indicate that the satellite reset is initialized
	g_uc_evha_flag |= EVENT_FLAG_RESET_SAT_INITIALIZED;

	#if (STORING_ENABLE == 1)
	//Save satellite's last state
	saveState();
	#endif

	#if (SATELLITE_SCHEDULE_HANDLER_ENABLE == 1)
	//Remove the bottom three schedules (if any, ensuring that there are at least three spaces in the scheduler to reset the satellite)
	removeSchedule(SCHEDULER_BLOCK_SIZE-2,SCHEDULER_BLOCK_SIZE);

	//Record scheduled OBC time value to reset before being used
	l_ul_scheduled_time = str_obdh_data.ul_obc_time_s+2;

	//Schedule PWRS to turn OFF all subsystems at once
	scheduleOffAllSubsystems(l_ul_scheduled_time);

	//Schedule to reset PWRS 1 second after all other subsystems are turned OFF
	scheduleResetPWRSOBDH(l_ul_scheduled_time+1, PWRS_ADDR);

	//Schedule to reset OBDH 2 seconds after PWRS subsystem is reset
	scheduleResetPWRSOBDH(l_ul_scheduled_time+3, OBDH_ADDR);
	#endif
}

#if (UPDATE_HANDLER_ENABLE == 1)
//------------------------------------------------------------------------------------
// To read MCU temperature in float and converts it to short and update it to the
// pre-defined global variable.
//------------------------------------------------------------------------------------
void readMCUTemperature (void)
{
	unsigned short	l_us_adc_bits;
	long			l_l_adc_mv;
	float			l_f_mcu_temperature;

	l_us_adc_bits 			= convertA2D(8);
	l_l_adc_mv 				= (((long)l_us_adc_bits * 2430) >> 12);
	l_f_mcu_temperature 	= ((l_l_adc_mv - 776)/2.86);
	str_obdh_hk.s_mcu_temp 	= (short)(l_f_mcu_temperature*100);
}
#endif

#if (BODY_RATE_HANDLER_ENABLE == 1)
#if (ADCS_CMD_ENABLE == 1)
#if (PWRS_CMD_ENABLE == 1)
//------------------------------------------------------------------------------------
// To update body rate variable values
//------------------------------------------------------------------------------------
// Ian (2012-05-15): It is noted that there is a preferable way with less memory and
//					 and computational time to calculate average value, that is by
//					 weighting the previous averaga value and the recent measurements
void updateBodyRateAverage (void)
{
	static unsigned char l_uc_sample_point_index = 0;
	unsigned char l_uc_i = 0;
	short l_s_br_register[3] = {0,0,0};
	unsigned long l_ul_accumulator[3] = {0,0,0};

	//Get the latest body rate value
	g_s_br_eci[0][l_uc_sample_point_index] = str_adcs_hk.s_br_eci[0];
	g_s_br_eci[1][l_uc_sample_point_index] = str_adcs_hk.s_br_eci[1];
	g_s_br_eci[2][l_uc_sample_point_index] = str_adcs_hk.s_br_eci[2];

	//Calculate the average of the ten samples
	for (l_uc_i = 0; l_uc_i < BR_ECI_SAMPLE_NO; l_uc_i++){
		//Ian (2012-05-15): This step is meant to take absolute values of the measurements before they are accumulated
		//Take the absolute value of the samples
		l_s_br_register[0] = (g_s_br_eci[0][l_uc_i] >= 0) ? g_s_br_eci[0][l_uc_i] : (0-g_s_br_eci[0][l_uc_i]);
		l_s_br_register[1] = (g_s_br_eci[1][l_uc_i] >= 0) ? g_s_br_eci[1][l_uc_i] : (0-g_s_br_eci[1][l_uc_i]);
		l_s_br_register[2] = (g_s_br_eci[2][l_uc_i] >= 0) ? g_s_br_eci[2][l_uc_i] : (0-g_s_br_eci[2][l_uc_i]);

		//Accumulate the absolute values
		l_ul_accumulator[0] += l_s_br_register[0];
		l_ul_accumulator[1] += l_s_br_register[1];
		l_ul_accumulator[2] += l_s_br_register[2];
	}

	//Ian (2012-05-15): Since the values are always non-negative. It is noted that the data type is changed to be unsigned short.
	//					It was first declared as short to strictly follow ADCS data type declaration for this data.
	//Get the average
	str_obdh_data.us_br_eci_avg[0] = l_ul_accumulator[0]/BR_ECI_SAMPLE_NO;
	str_obdh_data.us_br_eci_avg[1] = l_ul_accumulator[1]/BR_ECI_SAMPLE_NO;
	str_obdh_data.us_br_eci_avg[2] = l_ul_accumulator[2]/BR_ECI_SAMPLE_NO;

	//If the sample point index is less than BR_ECI_SAMPLE_NO-1, increase it
	if (l_uc_sample_point_index < (BR_ECI_SAMPLE_NO-1))
		l_uc_sample_point_index++;
	//Else, reset it to zero
	else l_uc_sample_point_index = 0;

	//Update BRH_FLAG_BR_HI
	//Ian (2012-05-15): Euclidean norm is not used for this case since high body rate measurement 
	//					over a period of time for one axis of the satellite's ECI frame is the criteria to say
	//					that satellite is rotating too fast. Thus, max norm is used.
	if ((str_obdh_data.us_br_eci_avg[0] >= str_obdh_par.us_br_threshold_high[0]) || (str_obdh_data.us_br_eci_avg[1] >= str_obdh_par.us_br_threshold_high[1]) || (str_obdh_data.us_br_eci_avg[2] >= str_obdh_par.us_br_threshold_high[2]))
		str_obdh_hk.us_brh_flag |= BRH_FLAG_BR_HI;
	else str_obdh_hk.us_brh_flag &= ~BRH_FLAG_BR_HI;

	//Update BRH_FLAG_BR_LOW
	//Ian (2012-05-15): If neither BRH_FLAG_BR_HI nor BRH_FLAG_BR_LOW are set, it means the satellite
	//				    is neither rotating too fast not too slow. An example for this case is when the satellite
	//					has reached low body rate and thus the control mechanism is cut to save power. And so, the
	//					BRH_FLAG_BR_LOW flag is set. And afterwards the body rate gets higher, but not high 
	//					enough to make the control mechanism activated. Yet, BRH_FLAG_BR_LOW flag is reset.
	//					But when the body rate gets high more than the threshold, the BRH_FLAG_BR_HI is set and
	//					control mechanism is triggered to be activated again, making body rate lower and therefore
	//					BRH_FLAG_BR_HI is reset. But this will not cut the control mechanism unless body rate is
	//					found to be sufficiently low and thus BRH_FLAG_BR_LOW is raised. By this, it prevents
	//					oscillatory behavior of the control mechanism.
	if ((str_obdh_data.us_br_eci_avg[0] <= str_obdh_par.us_br_threshold_low[0]) && (str_obdh_data.us_br_eci_avg[1] <= str_obdh_par.us_br_threshold_low[1]) && (str_obdh_data.us_br_eci_avg[2] <= str_obdh_par.us_br_threshold_low[2]))
		str_obdh_hk.us_brh_flag |= BRH_FLAG_BR_LOW;
	else str_obdh_hk.us_brh_flag &= ~BRH_FLAG_BR_LOW;
}
#endif
#endif
#endif

//----------------------------------------------------------
// This function is to do SW reset on OBDH 
//----------------------------------------------------------
//Ian (2012-05-15): It is noted that swReset and hwReset shares the same sequence of code but different constants.
void swReset (void)
{
	#if (STORING_ENABLE == 1)
	//Save satellite's last state
	saveState();
	#endif

	SFRPAGE = LEGACY_PAGE;
	RSTSRC |= 0x10;
}

//----------------------------------------------------------
// This function is to do HW reset on OBDH
//----------------------------------------------------------
void hwReset (void)
{
	#if (STORING_ENABLE == 1)
	//Save satellite's last state
	saveState();
	#endif

	SFRPAGE = LEGACY_PAGE;
	RSTSRC |= 0x01;
}
#endif

////----------------------------------------------------------
//// This function is to toggle antenna uC address
////----------------------------------------------------------
//void switchAntennaAddress (void)
//{
//	g_uc_antenna_addr = (g_uc_antenna_addr == ANTENNA_1_ADDR) ? ANTENNA_2_ADDR : ANTENNA_1_ADDR;
//}