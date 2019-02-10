//------------------------------------------------------------------------------------
// Luxp_interrupts.c
//------------------------------------------------------------------------------------
// Copyright (C) 2013 Nanyang Technological University.
//
// AUTH: IAN
// DATE: 03 APR 2013
//
// The program Luxp_interrupts.c will be downloaded to the Pumpkin Motherboard with 
// C8051F120 uC as part of the VELOX-P Pico-Satellite Flight Software. 
//
// This file contains all functions related to interrupts of the Flight Software
//
//------------------------------------------------------------------------------------
// Overall functional test status
//------------------------------------------------------------------------------------
// Completely tested, partially functional with warnings
//
//------------------------------------------------------------------------------------
// Overall documentation status
//------------------------------------------------------------------------------------
// Averagely documented
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// List of functions				| Test status					| Documentation
//------------------------------------------------------------------------------------
// ISRTimer1						| Tested, fully functional		| Average	
// ISRTimer2						| Tested, fully functional		| Average	
// ISRTimer3						| Tested, fully functional		| Average	
// ISRTimer4						| Tested, fully functional		| Average	
// ISRSMBus							| Tested, functional (w)		| Average	
// ISRUART0							| Tested, fully functional 		| Average	
// ISRUART1							| Tested, fully functional 		| Average	
// ISRSPI							| Tested, fully functional 		| Average	
//------------------------------------------------------------------------------------

#include <luxp_interrupts.h>

//------------------------------------------------------------------------------------
// Interrupt Service Routine Functions
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Timer3 Interrupt
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
void ISRTimer3 (void) interrupt 14
{
	char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

	SFRPAGE = SMB0_PAGE;

	SMB0CN &= ~0x40;                    // Disable SMBus
	SMB0CN |= 0x40;                     // Re-enable SMBus

	SFRPAGE = SFRPAGE_SAVE;             // Switch back to the Timer3 SFRPAGE

	TF3 = 0;                            // Clear Timer3 interrupt-pending flag
}

//------------------------------------------------------------------------------------
// Timer4 Interrupt
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
void ISRTimer4 (void) interrupt 16
{
	//Static variables
	static unsigned char l_uc_no_of_beacon_check = 0;
	static unsigned char l_uc_beacon_time_interrupt_counter = 0;

	#if (BODY_RATE_HANDLER_ENABLE == 1)
	#if (ADCS_CMD_ENABLE == 1)
	#if (PWRS_CMD_ENABLE == 1)
	static unsigned char l_uc_brh_counter_s = 0;
	#endif
	#endif
	#endif

    #if (IDLE_MODE_HANDLER_ENABLE == 1)
    #if (PWRS_CMD_ENABLE == 1)
	static unsigned char l_uc_imh_counter_s = 0;
	#endif
	#endif

	//Non-static local variables
	unsigned char l_uc_i;

	//Enable Watchdog timer (when enabled)
	#if (WDT_ENABLE == 1)
	unsigned char SFRPAGE_SAVE = SFRPAGE;

	SFRPAGE = CONFIG_PAGE; 
	WDTCN = 0x07;		//setting WDT interval bit7=0 bits[2:0] set to 111	
	WDTCN = 0xA5;		//Enable/Reset WDT
	SFRPAGE = SFRPAGE_SAVE;
	#endif

	//For function timing
	if (g_uc_combn_flag & STMR_FLAG_COUNT_START) 	
		g_us_timer_flag_counter++;
	else g_us_timer_flag_counter = 0;

	//The "elegant" way to program the timeout mechanism is not yet implemented
	//Due to uncertainty of the requirement of final program. But with certainty,
	//more "elegant" and "code-size-saving" method is preferred.

	//To set I2C bus lines free in the event of error (due to stuck)
	if (g_us_i2c_flag & I2C_FLAG_BUS_RECOVERY_ATTEMPT){
		g_uc_i2c_bus_sda_held_low_recovery_timeout_counter++;
		if (g_uc_i2c_bus_sda_held_low_recovery_timeout_counter > TIME_LIMIT_I2C_BUS_RECOVERY_OFL){
			g_uc_i2c_bus_sda_held_low_recovery_timeout_counter = 0;
			g_us_i2c_flag |= I2C_FLAG_BUS_SDA_HELD_LOW_RECOVERY_TIMEOUT;
		}
	}
	else g_uc_i2c_bus_sda_held_low_recovery_timeout_counter = 0;

	//To finish I2C bus SCL line held high if it does not give positive response
	if (g_us_i2c_flag & I2C_FLAG_BUS_SCL_HELD_HIGH_RECOVERY_ATTEMPT){
		g_uc_i2c_bus_scl_held_high_recovery_timeout_counter++;
		if(g_uc_i2c_bus_scl_held_high_recovery_timeout_counter > TIME_LIMIT_I2C_BUS_SCL_HIGH_RECOVERY_OFL){
			g_uc_i2c_bus_scl_held_high_recovery_timeout_counter = 0;
			g_us_i2c_flag |= I2C_FLAG_BUS_SCL_HELD_HIGH_RECOVERY_TIMEOUT;
		}
	}
	else g_uc_i2c_bus_scl_held_high_recovery_timeout_counter = 0;

	//To give time limit for SPI busy status
	if (g_uc_spit0ad0_flag & SPI_FLAG_BUSY_START){
		g_uc_spi_busy_timeout_counter++;
		if(g_uc_spi_busy_timeout_counter > TIME_LIMIT_SPI_BUS_BUSY_OFL){
			g_uc_spi_busy_timeout_counter = 0;
			g_uc_spit0ad0_flag |= SPI_FLAG_BUSY_TIMEOUT;
		}
	}
	else g_uc_spi_busy_timeout_counter = 0;

	//To give time limit for SPI transmission status
	if (g_uc_spit0ad0_flag & SPI_FLAG_TRANSMISSION_START){
		g_uc_spi_transmission_timeout_counter++;
		if(g_uc_spi_transmission_timeout_counter > TIME_LIMIT_SPI_BUS_TRANSMISSION_OFL){
			g_uc_spi_transmission_timeout_counter = 0;
			g_uc_spit0ad0_flag |= SPI_FLAG_TRANSMISSION_TIMEOUT;
		}
	}
	else g_uc_spi_transmission_timeout_counter = 0;

	//Set UART Busy timeout
	if (g_uc_combn_flag & UART_FLAG_RX_BUSY){							//If g_b_uart_rx_busy is set, that is, there is receiving UART transmission occurring
		if (g_uc_uart_busy_timeout_counter < TIME_LIMIT_UART_BUSY_OFL)					
			g_uc_uart_busy_timeout_counter++;		//If UART waiting is still within time limit, increase the counter
		else {										//else set UART timeout and release the UART line
			g_uc_combn_flag |= UART_FLAG_RX_BUSY_TIMEOUT;			//Set UART time out flag
			g_uc_uart_busy_timeout_counter = 0;		//Reset UART time out counter
			g_uc_combn_flag &= ~UART_FLAG_RX_BUSY;	//Release the UART line
			g_uc_combn_flag |= USIP_FLAG_READY;
		}
	}
	else g_uc_uart_busy_timeout_counter = 0;		//If UART_FLAG_RX_BUSY is not set, keep the counter zero

	//Set SMB transmission timeout (maximum time OBDH may send data to other subsystem)
	if (g_us_i2c_flag & I2C_FLAG_TRANSMISSION_START){				//When a waiting for transmission from other subsystem occurs
		if (g_uc_i2c_transmission_timeout_counter < TIME_LIMIT_I2C_TRANSMISSION_OFL)	
			g_uc_i2c_transmission_timeout_counter++;
		else {
			g_us_i2c_flag |= I2C_FLAG_TRANSMISSION_TIMEOUT;
			g_uc_i2c_transmission_timeout_counter = 0;
		}
	}
	else g_uc_i2c_transmission_timeout_counter = 0;	//If there is no SMB transmission, keep g_uc_i2c_transmission_timeout_counter to zero

	//Set SMB busy timeout (maximum time OBDH waiting for SM Bus to be freed)
	if (g_us_i2c_flag & I2C_FLAG_BUSY_START){	//When a waiting for SM Bus line to be freed to initiates a transmission
		if (g_uc_i2c_busy_timeout_counter < TIME_LIMIT_I2C_BUSY_OFL)	
			g_uc_i2c_busy_timeout_counter++;
		else {
			g_us_i2c_flag |= I2C_FLAG_BUSY_TIMEOUT;
			g_uc_i2c_busy_timeout_counter = 0;
		}
	}
	else g_uc_i2c_busy_timeout_counter = 0;	//If there is no waiting for SM Bus to be freed keep g_uc_i2c_busy_timeout_counter to zero

	//Set SMB receiving timeout (maximum time a subsystem may send data and halt OBDH program)
	if (g_us_i2c_flag & I2C_FLAG_RECEIVING_START){	//When a waiting for transmission from other subsystem occurs
		if (g_uc_i2c_receiving_timeout_counter < TIME_LIMIT_I2C_RECEIVING_OFL)	
			g_uc_i2c_receiving_timeout_counter++;
		else {
			g_us_i2c_flag |= I2C_FLAG_RECEIVING_TIMEOUT;
			g_uc_i2c_receiving_timeout_counter = 0;
		}
	}
	else g_uc_i2c_receiving_timeout_counter = 0;	//If there is no SMB transmission, keep g_uc_i2c_transmission_timeout_counter to zero

	//If timer0 is running 
	if (g_uc_spit0ad0_flag & TIMER0_FLAG_WAIT){
		if (g_uc_timer0_stuck_counter < TIME_LIMIT_TIMER0_STUCK_OFL)
			g_uc_timer0_stuck_counter++;
		else{
			g_uc_spit0ad0_flag |= TIMER0_FLAG_WAIT_TIMEOUT;
			g_uc_timer0_stuck_counter = 0;
		}
	}
	else g_uc_timer0_stuck_counter = 0;

	//If A2D conversion is running for too long, raise timeout flag
	if (g_uc_spit0ad0_flag & AD0INT_FLAG_START){
		if (g_uc_ad0int_counter < TIME_LIMIT_A2D_CONVERSION)
			g_uc_ad0int_counter++;
		else{
			g_uc_spit0ad0_flag |= AD0INT_FLAG_TIMEOUT;
			g_uc_ad0int_counter = 0;
		}
	}
	else g_uc_ad0int_counter = 0;

	//For task timing 
	//If there is any flag tasks counting flag is raised, increase the counter value for it
	//If the counter rises to more than SOFTWARE_TIMER_RATE_MS/5, increase the timer value
	//Else, keeps the flag tasks counter and timer to zero
	for (l_uc_i = 0; l_uc_i < SCRIPTLINE_TIMER_SIZE; l_uc_i++){
		if (str_scriptline_timer[l_uc_i].uc_started){
			str_scriptline_timer[l_uc_i].uc_counter++;
			//By default, every 100 ms, timer value will be increased
			//Ian (2012-05-15): this interrupt is called once every 5 miliseconds. There the magic constant 5 comes from.
			if (str_scriptline_timer[l_uc_i].uc_counter >= SOFTWARE_TIMER_RATE_MS/5){
				str_scriptline_timer[l_uc_i].uc_counter = 0;
				str_scriptline_timer[l_uc_i].ul_timer++;
			}
		}
	}

	//For beacon status checking
	if (str_obdh_hk.us_beacon_flag & BEACON_FLAG_ENABLE){
		//Increase beacon time interrupt counter
		l_uc_beacon_time_interrupt_counter++;

		//If the interrupt counter has shown that 200ms has passed
		if (l_uc_beacon_time_interrupt_counter >= (str_obdh_par.uc_no_of_interrupt_per_s/NO_OF_BEACON_CHECK_PER_S)){
			//Reset beacon time interrupt counter
			l_uc_beacon_time_interrupt_counter = 0;

			//Set beacon checking flag
			str_obdh_hk.us_beacon_flag |= BEACON_FLAG_CHECK;

			//Increase checking interrupt
			l_uc_no_of_beacon_check++;

			//Increase the time how long it has been since last checking here
			if (l_uc_no_of_beacon_check >= NO_OF_BEACON_CHECK_PER_S){
				l_uc_no_of_beacon_check = 0;
				g_uc_beacon_time_since_last_timeout_sending++;
				if (g_uc_beacon_time_since_last_timeout_sending >= g_uc_beacon_interval){
					//Limit the time that its value should not blow up
					g_uc_beacon_time_since_last_timeout_sending = g_uc_beacon_interval;
					//Raise up beacon timeout flag when it reaches timeout
					str_obdh_hk.us_beacon_flag |= BEACON_FLAG_TIMEOUT;
				}
			}
		}
	}
	//Else, reset all counters
	else {
		l_uc_beacon_time_interrupt_counter = 0;
		l_uc_no_of_beacon_check = 0;
		g_uc_beacon_time_since_last_timeout_sending = 0;
	}

	//For constant satellite tick timing (ticking every interrupt)
	str_obdh_data.uc_timer4_interrupt_counter++;

	//If one second has passed, this loop acts as a global software timer
	if (str_obdh_data.uc_timer4_interrupt_counter >= str_obdh_par.uc_no_of_interrupt_per_s){
		str_obdh_data.uc_timer4_interrupt_counter = 0;

		//Update time elapsed after last auto orbit change every second
		str_obdh_data.us_rev_offset_time++;

		//Updata time elapsed since last communication with MCC, only valid if COMM is ON
		if (str_pwrs_data.uc_channel_status & COMM_CHANNEL_BIT) g_ul_gsch_time_elapsed_since_last_communication_with_mcc++;

		//Increase time lapse counter for all subsystems update (in systemUpdateAndMonitor) since the last update
		g_uc_adcs_time_lapse_from_last_update_s++;
		g_uc_pwrs_time_lapse_from_last_update_s++;
		g_uc_rtc_time_lapse_from_last_update_s++;
		g_uc_pwrs_time_lapse_from_last_ch_stat_update_s++;
		g_uc_comm_time_lapse_from_last_update_s++;
		g_uc_antenna_status_check_from_last_update_s++;
		g_us_obdh_time_lapse_from_last_save_state_s++;

		//So long as we are in the safe mode, or we are in the Groundpass while the previous mode is safe-mode
		//this counter will be counting up every second. Else, it will be reset every second.
		if (str_obdh_hk.uc_sat_mode == SATELLITE_MODE_M4 || str_obdh_hk.uc_sat_mode == SATELLITE_MODE_M8 || (str_obdh_hk.uc_sat_mode == SATELLITE_MODE_M6 && str_obdh_data.uc_previous_mode == SATELLITE_MODE_M8)) 
			g_ul_smh_time_register++;
		else g_ul_smh_time_register = 0;

		//So long as the satellite is in the idle mode, this counter will be increased every second
		if (str_obdh_hk.uc_sat_mode == SATELLITE_MODE_M3 || str_obdh_hk.uc_sat_mode == SATELLITE_MODE_M5 || (str_obdh_hk.uc_sat_mode == SATELLITE_MODE_M6 && str_obdh_data.uc_previous_mode == SATELLITE_MODE_M5)) 
			g_ul_idle_time_register++;
		else g_ul_idle_time_register = 0;

		//To calculate ADS uptime (only valid when latest channel update is successful)
		if (!g_us_latest_ch_stat_update_ec && (str_pwrs_data.uc_channel_status & ADS_CHANNEL_BIT))
			//Increase its value
			g_ul_ads_uptime_s++;
		//If ADS is OFF, of course the uptime will always be zero
		else if (!g_us_latest_ch_stat_update_ec && !(str_pwrs_data.uc_channel_status & ADS_CHANNEL_BIT))
			g_ul_ads_uptime_s = 0;
		
		#if (BODY_RATE_HANDLER_ENABLE == 1)
		#if (ADCS_CMD_ENABLE == 1)
		#if (PWRS_CMD_ENABLE == 1)
		//To calculate detumbling mode uptime 
		//only valid when BRH is ready and detumbling is found to be ON
		if ((str_obdh_hk.us_brh_flag & BRH_FLAG_READY) && (str_obdh_hk.us_brh_flag & BRH_FLAG_DET_ON)){
			//Increase Detumbling mode uptime value
			g_ul_det_uptime_s++;

			//If the Detumbling mode uptime has reached its limit
			if (g_ul_det_uptime_s >= str_obdh_par.ul_det_uptime_limit_s)
				//Set BRH_FLAG_BR_ST_TO flag
				str_obdh_hk.us_brh_flag |= BRH_FLAG_BR_ST_TO;
		}
		//If BRH is not ready, keep the uptime be zero
		else g_ul_det_uptime_s = 0;
		
		//For BRH checking
		if (str_obdh_hk.us_brh_flag & BRH_FLAG_ENABLE){
			//Increase BRH counter
			l_uc_brh_counter_s++;

			//If it is time to check BRH, raise its checking flag
			if (l_uc_brh_counter_s > BRH_CHECK_PERIOD){
				l_uc_brh_counter_s = 0;
				str_obdh_hk.us_brh_flag |= BRH_FLAG_CHECK;
			}			
		}
		//Else, reset its counter and its check flag
		else {
			l_uc_brh_counter_s = 0;
			str_obdh_hk.us_brh_flag &= ~BRH_FLAG_CHECK;
		}

		//If currently BRH is waiting for Detumbling to be turned to wanted state
		if ((str_obdh_hk.us_brh_flag & BRH_FLAG_DET_WT) && (g_uc_brh_wait_det_time_s < TIME_LIMIT_WAITING_DET_S))
			//Increase Detumbling mode change waiting time
			g_uc_brh_wait_det_time_s++;
		//Else if currently BRH is waiting for ADCS detumbling mode to be turned to wanted state for too long
		else if ((str_obdh_hk.us_brh_flag & BRH_FLAG_DET_WT) && (g_uc_brh_wait_det_time_s >= TIME_LIMIT_WAITING_DET_S))
			//Set Detumbling state timeout flag
			str_obdh_hk.us_brh_flag |= BRH_FLAG_DET_TO;
		//Else, if currently waiting flag is not set
		else
			//Reset Detumbling mode waiting time
			g_uc_brh_wait_det_time_s = 0;
		#endif
		#endif
		#endif

		#if (IDLE_MODE_HANDLER_ENABLE == 1)
		#if (PWRS_CMD_ENABLE == 1)
		//For IMH checking
		if (str_obdh_hk.us_imh_flag & IMH_FLAG_ENABLE){
			//Increase IMH counter
			l_uc_imh_counter_s++;

			//If it is time to check IMH, raise its checking flag
			if (l_uc_imh_counter_s > IMH_CHECK_PERIOD){
				l_uc_imh_counter_s = 0;
				str_obdh_hk.us_imh_flag |= IMH_FLAG_CHECK;
			}			
		}
		//Else, reset its counter and its check flag
		else {
			l_uc_imh_counter_s = 0;
			str_obdh_hk.us_imh_flag &= ~IMH_FLAG_CHECK;
		}

		//If currently IMH is waiting for ADS to be turned to wanted state
		if ((str_obdh_hk.us_imh_flag & IMH_FLAG_ADS_WT) && (g_uc_imh_wait_ads_time_s < TIME_LIMIT_WAITING_ADS_S))
			//Increase ADS waiting time
			g_uc_imh_wait_ads_time_s++;
		//Else if currently IMH is waiting for ADS to be turned to wanted state for too long
		else if ((str_obdh_hk.us_imh_flag & IMH_FLAG_ADS_WT) && (g_uc_imh_wait_ads_time_s >= TIME_LIMIT_WAITING_ADS_S))
			//Set ADS turn ON timeout flag
			str_obdh_hk.us_imh_flag |= IMH_FLAG_ADS_TO;
		//Else, if currently waiting flag is not set
		else
			//Reset ADS waiting time
			g_uc_imh_wait_ads_time_s = 0;
		#endif
		#endif

		//If it has passed the time to revolute once, update the reference orbit no.
		if (str_obdh_data.us_rev_offset_time >= str_obdh_par.us_rev_time_s){
			str_obdh_data.us_rev_offset_time = 0;
			str_obdh_data.us_auto_reference_rev_no++;
		}

		//Update OBC time every second
		str_obdh_data.ul_obc_time_s++;

		//Time to update the satellite time
		g_uc_evha_flag |= EVENT_FLAG_SAT_INFO_TO_UPDATE;

		//To decrease mission mode time register value
		if (g_us_mission_time_register) g_us_mission_time_register--;

		//Check beacon handler error (if any)
		if (g_uc_beacon_error_counter <= TIME_LIMIT_BEACON_ERROR_S && (str_obdh_hk.us_beacon_flag & BEACON_FLAG_ERROR))
			g_uc_beacon_error_counter++;

		//Time to check GDSN command buffer, if currently there is no checking happening
		if (!(g_uc_gsch_flag & GSCH_FLAG_CHECK_RESET))
			g_uc_gsch_flag |= GSCH_FLAG_INITIAL_CHECK;	//Initial validation checking

		//If Groundpass state has been started, update g_us_groundpass_time_register
		if((g_uc_gsch_flag & GSCH_FLAG_GROUNDPASS_STARTED) && (g_us_groundpass_time_register < TIME_LIMIT_GROUND_PASS_S))
			g_us_groundpass_time_register++;

		//If transitional state is occuring
		if (g_uc_gsch_flag & GSCH_FLAG_MONITOR_TRANSITIONAL)
			g_us_gsch_monitoring_transitional_period++;
		else g_us_gsch_monitoring_transitional_period = 0;

		//g_uc_sd_load_data_timeout_counter update	  
		if(g_uc_sd_load_data_timeout_counter < TIME_LIMIT_LOAD_DATA_S)
			g_uc_sd_load_data_timeout_counter++;
		
		//If (g_uc_strsnd_flag & STRNG_FLAG_SAVE_DATA_HOLD) && g_uc_save_data_hold_timeout_counter < TIME_LIMIT_SAVE_DATA_HOLD_S
		if ((g_uc_strsnd_flag & STRNG_FLAG_SAVE_DATA_HOLD) && g_uc_save_data_hold_timeout_counter < TIME_LIMIT_SAVE_DATA_HOLD_S)
			g_uc_save_data_hold_timeout_counter++;		
		else if(g_uc_save_data_hold_timeout_counter >= TIME_LIMIT_SAVE_DATA_HOLD_S){
			g_uc_strsnd_flag &= ~STRNG_FLAG_SAVE_DATA_HOLD;
			g_uc_save_data_hold_timeout_counter = 0;
		}

		//Check log data task's running flag, and its second script's (log data script) running flag
		if ((str_task[TASK_LOG_ST_DATA].uc_status & TASK_RUNNING_FLAG) && g_us_log_data_elapsed_time_s < TIME_LIMIT_LOG_DATA_S && (str_task[TASK_LOG_ST_DATA].uc_script_running_flags & 0x02))
			g_us_log_data_elapsed_time_s++;

		//Check thermal knife task's running flag and its fourth script's (thermal knife enable) running flag
		if ((str_task[TASK_TEST_TK].uc_status & TASK_RUNNING_FLAG) && (str_task[TASK_TEST_TK].uc_script_running_flags & 0x08)){
			if (g_uc_tk_waiting_s < TIME_LIMIT_TK_WAITING_S+5)
				g_uc_tk_waiting_s++;
		}
		else g_uc_tk_waiting_s = 0;

		//Check thermal knife task's running flag and its fifth script's (thermal knife cut) running flag
		if ((str_task[TASK_TEST_TK].uc_status & TASK_RUNNING_FLAG) && (str_task[TASK_TEST_TK].uc_script_running_flags & 0x10)){
			if (g_uc_tk_uptime_s < TIME_LIMIT_TK_CUTTING_S+5)
				g_uc_tk_uptime_s++;
		}
		else g_uc_tk_uptime_s = 0;

		//Check antenna deloyment task's running flag and its second script's (PWRS channel enable) or sixth script's (PWRS channel enable) running flag
		if ((str_task[TASK_DEPLOY_ANTENNA].uc_status & TASK_RUNNING_FLAG) && ((str_task[TASK_DEPLOY_ANTENNA].uc_script_running_flags & 0x02) || (str_task[TASK_DEPLOY_ANTENNA].uc_script_running_flags & 0x20))){
			if (g_uc_deployment_channel_waiting_s < TIME_LIMIT_DPLY_WAIITING_S+5)
				g_uc_deployment_channel_waiting_s++;
		}
		//Check thermal knife task's running flag and its second script's (PWRS channel enable) or eight script's (PWRS channel enable) running flag
		else if ((str_task[TASK_TEST_TK].uc_status & TASK_RUNNING_FLAG) && ((str_task[TASK_TEST_TK].uc_script_running_flags & 0x02) || (str_task[TASK_TEST_TK].uc_script_running_flags & 0x80))){
			if (g_uc_deployment_channel_waiting_s < TIME_LIMIT_DPLY_WAIITING_S+5)
				g_uc_deployment_channel_waiting_s++;
		}
		else g_uc_deployment_channel_waiting_s = 0;

		//Check antenna deployment's task running flag and its fourth script's (antenna deployed) running flag
		if ((str_task[TASK_DEPLOY_ANTENNA].uc_status & TASK_RUNNING_FLAG) && (str_task[TASK_DEPLOY_ANTENNA].uc_script_running_flags & 0x08)){
			if (g_uc_antenna_deployment_waiting_s < (TIME_LIMIT_ANTENNA_WAIITING_S*4+30))
				g_uc_antenna_deployment_waiting_s++;
		}
		else g_uc_antenna_deployment_waiting_s = 0;

		//Check sending time flag
		if ((g_uc_gsch_flag & GSCH_FLAG_TOTAL_SENDING) && g_us_gsch_total_sending_time < TIME_LIMIT_GROUND_PASS_S)
			g_us_gsch_total_sending_time++;
	}

	TF4 = 0; // Clear Timer4 interrupt-pending flag
}

//------------------------------------------------------------------------------------
// SMBus Interrupt
//------------------------------------------------------------------------------------
// Function's test result: functional, but without being tested in all possible cases
//------------------------------------------------------------------------------------
void ISRSMBus (void) interrupt 7
{
	unsigned char l_uc_fail = 0;	// Used by the ISR to flag failed transfers

	static unsigned char l_uc_sent_byte_counter = 1; //This does not use standard c indexing way for adjustment with g_us_i2c_transferred_byte_out purpose. If this uses standard c indexing way, g_us_i2c_transferred_byte_out would need to be adjusted to g_us_i2c_transferred_byte_out-1. Either way an adjustment will be needed.
	static unsigned char l_uc_rec_byte_counter = 1; //This does not use standard c indexing way for adjustment with g_us_i2c_transferred_byte_in purpose. If this uses standard c indexing way, g_us_i2c_transferred_byte_in would need to be adjusted to g_us_i2c_transferred_byte_in-1 (and g_us_i2c_transferred_byte_in-1 to g_us_i2c_transferred_byte_in-2). Either way an adjustment will be needed.
	static unsigned char l_uc_no_of_trial = 0;

	// Status code for the SMBus (SMB0STA register)
	switch (SMB0STA)
	{
		// Master Transmitter/Receiver: START condition transmitted.
		// Load SMB0DAT with slave device address.
		case SMB_START:

		// Master Transmitter/Receiver: repeated START condition transmitted.
		// Load SMB0DAT with slave device address
		case SMB_RP_START:
			SMB0DAT = g_uc_i2c_target;		// Load address of the slave.
			SMB0DAT &= 0xFE;			// Clear the LSB of the address for the R/W bit
			SMB0DAT |= (unsigned char)(g_us_i2c_flag & I2C_FLAG_RW);		// Load R/W bit
			STA = 0;					// Manually clear STA bit

			l_uc_rec_byte_counter = 1;	// Reset the counter
			l_uc_sent_byte_counter = 1;	// Reset the counter

			break;

		// Master Transmitter: Slave address + WRITE transmitted.  ACK received.
		// For a READ: N/A
		//
		// For a WRITE: Send the first data byte to the slave.
		case SMB_MTADDACK:
		
			//Fill SMB0DAT with first data byte, that is g_uc_i2c_data_out[0]
			SMB0DAT = g_uc_i2c_data_out[l_uc_sent_byte_counter-1]; //send data byte

			//Increase l_uc_sent_byte_counter to 2
			l_uc_sent_byte_counter++;

			//Reset l_uc_no_of_trial counter
			l_uc_no_of_trial = 0;

			break;

		//Master Transmitter: Slave address + WRITE transmitted. NACK received.
		//Restart the transfer.
        case SMB_MTADDNACK:
			l_uc_no_of_trial++;
			if (l_uc_no_of_trial < I2C_RESTART_TRIAL_LIMIT) STA = 1;  //Restart transfer, if it is within no of trial limit
			else {
				l_uc_no_of_trial = 0; 	//reset no of trial
				STO = 1;			//stops the I2C transmission
				g_us_i2c_flag &= ~I2C_FLAG_BUSY;       //free SMBus interface
				g_us_i2c_isr_error_code = EC_I2C_ISR_SLAVE_ADDR_NACK;	//Indicate that slave address is not acknowledged
				g_us_i2c_flag |= I2C_FLAG_TRANSMISSION_TIMEOUT;
			}
			break;

		//Master Transmitter: Data byte transmitted.  ACK received.
		//For a READ: N/A
		//
		//For a WRITE: Send all data.  After the last data byte, send the stop bit.
		case SMB_MTDBACK:

			//If l_uc_sent_byte_counter <= g_uc_i2c_transferred_byte_out, keep sending
			if (l_uc_sent_byte_counter <= g_uc_i2c_transferred_byte_out)
			{
				//transfer data
				SMB0DAT = g_uc_i2c_data_out[l_uc_sent_byte_counter-1];

				//Increase the l_uc_sent_byte_counter for the next data byte
				l_uc_sent_byte_counter++;
			}

			else
			{
				STO = 1;					//Set STO to terminate transfer
				g_us_i2c_flag &= ~I2C_FLAG_BUSY;				//And free SMBus interface
				g_us_i2c_isr_error_code = EC_SUCCESSFUL;	//Indicates that I2C ISR transmission is successful
			}

			//Reset l_uc_no_of_trial counter
			l_uc_no_of_trial = 0;

			break;

		// Master Transmitter: Data byte transmitted.  NACK received.
		// Restart the transfer.
		case SMB_MTDBNACK:
			l_uc_no_of_trial++;	//Increase l_uc_no_of_trial counter
			STA = 1;  //Restart transfer, if it is within no of trial limit
			break;

		// Master Receiver: Slave address + READ transmitted.  ACK received.
		// For a READ: check if this is a one-byte transfer. if so, set the
		//  NACK after the data byte is received to end the transfer. if not,
		//  set the ACK and receive the other data bytes.
		//
		// For a WRITE: N/A
		case SMB_MRADDACK: 			
			if (l_uc_rec_byte_counter == g_uc_i2c_transferred_byte_in)
			{		 
				AA = 0;		// Only one byte in this transfer,
			}
			else
			{
				AA = 1; 	// More than one byte in this transfer, send ACK after byte is received
			}

			//Reset l_uc_no_of_trial counter
			l_uc_no_of_trial = 0;

			break;

		// Master Receiver: Slave address + READ transmitted.  NACK received.
		// Restart the transfer.
		case SMB_MRADDNACK:
			l_uc_no_of_trial++;
			if (l_uc_no_of_trial < I2C_RESTART_TRIAL_LIMIT) STA = 1;  //Restart transfer, if it is within no of trial limit
			else {
				l_uc_no_of_trial = 0; 	//reset l_uc_no_of_trial counter
				STO = 1;				//stops the I2C transmission
				g_us_i2c_flag &= ~I2C_FLAG_BUSY;       //free SMBus interface
				g_us_i2c_isr_error_code = EC_I2C_ISR_SLAVE_ADDR_NACK;	//Indicate that slave address is not acknowledged
				g_us_i2c_flag |= I2C_FLAG_TRANSMISSION_TIMEOUT;	//Set SMB transmission timeout flag
			}
			break;

		// Master Receiver: Data byte received.  ACK transmitted.
		// For a READ: receive each byte from the slave.  if this is the last
		// byte, send a NACK and set the STOP bit.
		//
		// For a WRITE: N/A
		case SMB_MRDBACK:
			if (l_uc_rec_byte_counter < (g_uc_i2c_transferred_byte_in-1))
			{
				g_uc_i2c_data_in[l_uc_rec_byte_counter-1] = SMB0DAT; // Store received byte	
				AA = 1;	// Send ACK to indicate byte received
				l_uc_rec_byte_counter++;	// Increment the byte counter				
			}
			else
			{
				g_uc_i2c_data_in[l_uc_rec_byte_counter-1] = SMB0DAT; // Store received byte	
				AA = 0;	// Send NACK to indicate last byte of this transfer
				l_uc_rec_byte_counter++;	// Increment the byte counter		
			}

			//Reset l_uc_no_of_trial counter
			l_uc_no_of_trial = 0;											   
			
			break;

		// Master Receiver: Data byte received.  NACK transmitted.
		// For a READ: Read operation has completed.  Read data register and
		//  send STOP.
		//
		// For a WRITE: N/A
		case SMB_MRDBNACK:
			g_uc_i2c_data_in[l_uc_rec_byte_counter-1] = SMB0DAT; //Store received byte
			STO = 1;					//Generate stop bit
			AA = 1;						//Set AA for next transfer
			g_us_i2c_flag &= ~I2C_FLAG_BUSY;			//Free SMBus line

			//Reset l_uc_no_of_trial counter
			l_uc_no_of_trial = 0;
			g_us_i2c_isr_error_code = EC_SUCCESSFUL;	//Indicates that I2C ISR transmission is successful											   

			break;
			
		// Master Transmitter: Arbitration lost.
		case SMB_MTARBLOST:		
			l_uc_fail = 1;	// Indicate failed transfer and handle at end of ISR
			break;
			
		// Slave Receiver: Slave address + WRITE received.  ACK transmitted.
		// Fall through.
		case SMB_SROADACK:
			
		// Slave Receiver: General call address received.  ACK transmitted.
		case SMB_SRGADACK:
			l_uc_sent_byte_counter = 1;        // Reinitialize the data counters
			l_uc_rec_byte_counter = 1;
			break;

		// Slave Receiver: Data byte received after addressed by general
		// call address + WRITE.
		// ACK transmitted.  Fall through.
		case SMB_SRGDBACK:

		// Slave Receiver: Data byte received after addressed by own
		// slave address + WRITE.
		// ACK transmitted.
		// Receive each byte from the master.
		// Ian (2012-05-15): AA = 1 is moved out of the if-else segment.
		case SMB_SRODBACK:
			if (l_uc_rec_byte_counter < g_uc_i2c_transferred_byte_in)
			{
				g_uc_i2c_data_in[l_uc_rec_byte_counter-1] = SMB0DAT;	// Store incoming data
				l_uc_rec_byte_counter++;			//Increase received byte counter				
			}
			else
			{
				//Ian (2012-05-15): this case does not increase the number of byte received.
				g_uc_i2c_data_in[l_uc_rec_byte_counter-1] = SMB0DAT; //Store incoming data (last byte)
                g_us_i2c_flag |= I2C_FLAG_DATA_RECEIVED; //Indicate new data fully received
            }
			AA = 1;						//ACK received data
    
			break;

		// Slave Receiver: Data byte received while addressed as slave.
		// NACK transmitted. Should not occur since AA will not be cleared
		// as slave.  Fall through to next state.
		case SMB_SRODBNACK:

		// Slave Receiver: Data byte received while addressed by general call.
		// NACK transmitted.
		// Should not occur since AA will not be cleared as slave.
		case SMB_SRGDBNACK:
			AA = 1;
			break;

		// Slave Receiver: STOP or repeated START received while addressed as
		// slave.
		case SMB_SRSTOP:
			break;

		//All other status codes invalid.  Reset communication.
		default:
			l_uc_fail = 1;
			break;
	}

	if (l_uc_fail)							//If the transmission failed
	{
		g_us_i2c_isr_error_code = EC_I2C_ISR_COMMUNICATION_FAIL;	//Indicates that I2C ISR communication is failed
		resetI2CCommunication();		//Reset I2C Communication
	}

	SI = 0;                             // Clear interrupt flag
}

#if (DEBUGGING_FEATURE == 1)
#if (UART_LINE == 1)
//------------------------------------------------------------------------------------
// UART1 Interrupt
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
void ISRUART1 (void) interrupt 20
{
	unsigned char l_uc_byte;

	if (RI1 && !(g_uc_combn_flag & USIP_FLAG_READY))		//If we receive something from UART1 and the input is received when neither ready_to_process and ready_to_clear flag is set
	{
		g_uc_combn_flag |= USIP_FLAG_START;	//Raise user input start flag, will not be cleared until it is manually cleared by the software
		g_uc_combn_flag |= UART_FLAG_RX_BUSY;		//UART receive something, the software flag indicating that is set, it will be reseted if the buffersize is full or the time out is reached
		g_uc_uart_busy_timeout_counter = 0;	//UART Time out counter is reseted every time this RI1 is called (that is, there is a data received)

		if(g_us_uart_buffer_filled == 0)	//If there is no data in the buffer 
			g_us_uart_input_first = 0;	//Point the first input to the array 0, implies that this is the first time we input the data

		RI1 = 0;		//Manually reset receive UART flag
		l_uc_byte = SBUF1;	//Set Byte variable to whatever character received in UART1

		if (g_us_uart_buffer_filled < UART_BUFFER_SIZE)		//If there is a space in the buffer
		{
			g_uc_uart_buffer[g_us_uart_input_first] = l_uc_byte;	//Insert the byte to the space
			g_us_uart_buffer_filled++;						//Indicate increase in buffer size as a data has just been inputed
			g_us_uart_input_first++;						//Increase pointer to the next coming data
		}
		else	//If the buffer is full, g_b_uart_rx_busy flag shall be reseted
			g_uc_combn_flag &= ~UART_FLAG_RX_BUSY;
	}
	else RI1 = 0;

	if(TI1) TI1 = 0;

}

#elif (UART_LINE == 0)
//------------------------------------------------------------------------------------
// UART0 Interrupt
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
void ISRUART0 (void) interrupt 4
{
	unsigned char l_uc_byte;

	if (RI0 && !(g_uc_combn_flag & USIP_FLAG_READY))		//If we receive something from UART0 and the input is received when neither ready_to_process and ready_to_clear flag is set
	{
		g_uc_combn_flag |= USIP_FLAG_START;	//Raise user input start flag, will not be cleared until it is manually cleared by the software
		g_uc_combn_flag |= UART_FLAG_RX_BUSY;		//UART receive something, the software flag indicating that is set, it will be reseted if the buffersize is full or the time out is reached
		g_uc_uart_busy_timeout_counter = 0;	//UART Time out counter is reseted every time this RI0 is called (that is, there is a data received)

		if(g_us_uart_buffer_filled == 0)	//If there is no data in the buffer 
			g_us_uart_input_first = 0;	//Point the first input to the array 0, implies that this is the first time we input the data

		RI0 = 0;		//Manually reset receive UART flag
		l_uc_byte = SBUF0;	//Set Byte variable to whatever character received in UART0

		if (g_us_uart_buffer_filled < UART_BUFFER_SIZE)		//If there is a space in the buffer
		{
			g_uc_uart_buffer[g_us_uart_input_first] = l_uc_byte;	//Insert the byte to the space
			g_us_uart_buffer_filled++;						//Indicate increase in buffer size as a data has just been inputed
			g_us_uart_input_first++;						//Increase pointer to the next coming data
		}
		else	//If the buffer is full, UART_FLAG_RX_BUSY flag shall be reseted
			g_uc_combn_flag &= ~UART_FLAG_RX_BUSY;
	}
	else RI0 = 0;

	if(TI0) TI0 = 0;

}

#else
#endif
#endif

//------------------------------------------------------------------------------------
// SPI Interrupt Routine
//------------------------------------------------------------------------------------
void ISRSPI (void) interrupt 6
{
	if(g_uc_spit0ad0_flag & SPI_FLAG_TRANSMISSION_TIMEOUT)
		SPIF = 0;

	if(WCOL | MODF)
	{
		SPIEN = 0;
		SPIEN = 1;
	}
}
