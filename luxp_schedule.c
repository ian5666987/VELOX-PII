//------------------------------------------------------------------------------------
// Luxp_schedule.c
//------------------------------------------------------------------------------------
// Copyright (C) 2013 Nanyang Technological University.
//
// AUTH: IAN
// DATE: 03 APR 2013
//
// The program Luxp_schedule.c will be downloaded to the Pumpkin Motherboard with 
// C8051F120 uC as part of the VELOX-P Pico-Satellite Flight Software. 
//
// This file contains all scheduling functions of the Flight Software
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

#include <luxp_schedule.h>

#if (SATELLITE_SCHEDULE_HANDLER_ENABLE == 1)
//------------------------------------------------------------------------------------
// Schedule Functions
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Rearrange satellite's schedule
//------------------------------------------------------------------------------------
void rearrangeSchedule (void)
{
    unsigned char l_uc_i;
    unsigned char l_uc_swap_flag;
    
    if (str_obdh_hk.uc_scheduler_block_size){
		do {
	        l_uc_swap_flag = 0;
	        for (l_uc_i = 0; l_uc_i < str_obdh_hk.uc_scheduler_block_size-1; l_uc_i++){
	            if (str_scheduler.ul_scheduled_time[str_scheduler.uc_reindexing[l_uc_i]] > str_scheduler.ul_scheduled_time[str_scheduler.uc_reindexing[l_uc_i+1]]){
	               swapScheduleIndex(l_uc_i,l_uc_i+1);
	               l_uc_swap_flag = 1;
	            }
	        }
	    }while (l_uc_swap_flag);
	}
}

//------------------------------------------------------------------------------------
// Swap schedule index
//------------------------------------------------------------------------------------
void swapScheduleIndex (unsigned char l_uc_index_1, unsigned char l_uc_index_2)
{
    unsigned char l_uc_temp_index;
    
    l_uc_temp_index          					= str_scheduler.uc_reindexing[l_uc_index_1];
    str_scheduler.uc_reindexing[l_uc_index_1] 	= str_scheduler.uc_reindexing[l_uc_index_2];
    str_scheduler.uc_reindexing[l_uc_index_2] 	= l_uc_temp_index;
}

//------------------------------------------------------------------------------------
// Remove schedule
//------------------------------------------------------------------------------------
//	The valid removal start point here is one (1) or above, not (0) or above
//	since the starting point of the schedule is declared as one (1), not zero (0)
//------------------------------------------------------------------------------------
void removeSchedule (unsigned char l_uc_removal_start_point, unsigned char l_uc_removal_end_point)
{
    unsigned char l_uc_index_value_removed;
    unsigned char l_uc_i;
    unsigned char l_uc_j;
	unsigned char l_uc_no_of_to_be_removed_schedule;

	//Prevent user input error
	if (l_uc_removal_end_point >= l_uc_removal_start_point){	
		//Set the number of removed schedule as what is being input
		l_uc_no_of_to_be_removed_schedule = l_uc_removal_end_point-l_uc_removal_start_point+1;
	
		//Check if the removal starting point is less than or equal to the scheduler's size
		//And whether there is still schedule to be removed
		while (l_uc_removal_start_point <= str_obdh_hk.uc_scheduler_block_size && l_uc_no_of_to_be_removed_schedule && l_uc_removal_start_point != 0){    
		    //Set the index value removed as stated by the starting point
			l_uc_index_value_removed = str_scheduler.uc_reindexing[l_uc_removal_start_point-1];
	    
		    //Value must be raised up from the removed index onwards
		    for (l_uc_i = l_uc_index_value_removed; l_uc_i < str_obdh_hk.uc_scheduler_block_size-1; l_uc_i++){
		        str_scheduler.ul_scheduled_time[l_uc_i] = str_scheduler.ul_scheduled_time[l_uc_i+1];
				str_scheduler.us_cmd[l_uc_i] 			= str_scheduler.us_cmd[l_uc_i+1];
				str_scheduler.uc_arg_length[l_uc_i] 	= str_scheduler.uc_arg_length[l_uc_i+1];
				for (l_uc_j = 0; l_uc_j < MAX_SCHEDULER_ARG; l_uc_j++)
					str_scheduler.uc_arg[l_uc_i][l_uc_j] = str_scheduler.uc_arg[l_uc_i+1][l_uc_j];
			}
		
		    //Re-indexing must be raised up from l_uc_removal_start_point onwards
		    for (l_uc_i = (l_uc_removal_start_point-1); l_uc_i < str_obdh_hk.uc_scheduler_block_size-1; l_uc_i++)
		        str_scheduler.uc_reindexing[l_uc_i] = str_scheduler.uc_reindexing[l_uc_i+1];
	
			//Checked from the beginning, whenever there is index which is greater than what is removed, 
			//the index will be minus by 1
			for (l_uc_i = 0; l_uc_i < str_obdh_hk.uc_scheduler_block_size-1; l_uc_i++)
		        if (str_scheduler.uc_reindexing[l_uc_i] > l_uc_index_value_removed)
		            str_scheduler.uc_reindexing[l_uc_i]--;
		    
		    //Size is reduced by one
		    str_obdh_hk.uc_scheduler_block_size--;
			l_uc_no_of_to_be_removed_schedule--;
		
		    //Delete the outdated re-indexing and the last schedule
		    str_scheduler.uc_reindexing[str_obdh_hk.uc_scheduler_block_size] 		= 0;
		    str_scheduler.ul_scheduled_time[str_obdh_hk.uc_scheduler_block_size] 	= 0;
			str_scheduler.us_cmd[str_obdh_hk.uc_scheduler_block_size] 				= 0;
			str_scheduler.uc_arg_length[str_obdh_hk.uc_scheduler_block_size] 		= 0;
			for (l_uc_j = 0; l_uc_j < MAX_SCHEDULER_ARG; l_uc_j++)
				str_scheduler.uc_arg[str_obdh_hk.uc_scheduler_block_size][l_uc_j] 	= 0;
		}
	}    
}

//------------------------------------------------------------------------------------
// To add new schedule in the bottom of the block
//------------------------------------------------------------------------------------
unsigned short addNewSchedule (void)
{
	unsigned short l_us_error_code = EC_INIT;
	unsigned char l_uc_i;

	//Invalid argument length will be rejected, this is to prevent an undesired scheduled event
	if (str_obdh_hk.uc_scheduler_block_size < SCHEDULER_BLOCK_SIZE){
		if (str_scheduler.uc_arg_length_register <= MAX_SCHEDULER_ARG){
			if (str_scheduler.ul_scheduled_time_register >= str_obdh_data.ul_obc_time_s){
				//Give the new schedule last index first
				str_scheduler.uc_reindexing[str_obdh_hk.uc_scheduler_block_size]	 	= str_obdh_hk.uc_scheduler_block_size;
			
				//Add the scheduled item
				str_scheduler.ul_scheduled_time[str_obdh_hk.uc_scheduler_block_size] 	= str_scheduler.ul_scheduled_time_register;
				str_scheduler.us_cmd[str_obdh_hk.uc_scheduler_block_size] 			 	= str_scheduler.us_cmd_register;
				str_scheduler.uc_arg_length[str_obdh_hk.uc_scheduler_block_size] 	 	= str_scheduler.uc_arg_length_register;
				for (l_uc_i = 0; l_uc_i < MAX_SCHEDULER_ARG; l_uc_i++)
					str_scheduler.uc_arg[str_obdh_hk.uc_scheduler_block_size][l_uc_i] 	= str_scheduler.uc_arg_register[l_uc_i];
			
			    //Increase scheduler block size
				str_obdh_hk.uc_scheduler_block_size++;
			
				//The scheduler lost its arrangement whenever a new schedule is added
				g_uc_evha_flag &= ~EVENT_FLAG_SCHEDULE_IS_REARRANGED;
				
				//Return successful
				l_us_error_code = EC_SUCCESSFUL;
			}
			else l_us_error_code = EC_SCHEDULED_TIME_HAS_ALREADY_PASSED;
		}
		else l_us_error_code = EC_SCHEDULE_ARGUMENT_TOO_LONG;
	}
	else l_us_error_code = EC_SCHEDULE_FULL;

	#if (EVENT_HANDLER_ENABLE == 1)
	#if (STORING_ENABLE == 1)
    eventHandler(EV_ADD_SCHEDULE+l_us_error_code,
				str_scheduler.us_cmd_register >> 8,str_scheduler.us_cmd_register,
					str_scheduler.uc_arg_register[0],
					str_scheduler.uc_arg_register[1],
					str_scheduler.uc_arg_register[2],
					str_scheduler.uc_arg_register[3]);	//yc added 20120224
	#endif
	#endif


	return l_us_error_code;
}

//------------------------------------------------------------------------------------
// To insert schedule to turn ON ADCS on the scheduled time.
// ADS is turned ON first, then ACS TIME_DELAY_ACS_TO_ON_AFTER_ADS_ON_S seconds later.
//------------------------------------------------------------------------------------
unsigned short scheduleTurnOnADCS (unsigned long l_ul_scheduled_time)
{
	unsigned short l_us_error_code = EC_INIT;

	//It needs at least two slots of schedule to turn ON ADCS
	if (str_obdh_hk.uc_scheduler_block_size <= (SCHEDULER_BLOCK_SIZE-2)){
		//Turn ON ADS first
		l_us_error_code = scheduleSingleChannel(ADS_CHANNEL,CHANNEL_ON,l_ul_scheduled_time);
	
		//If there is an error code, it must be because the scheduled time has already passed
		if (l_us_error_code) return l_us_error_code;
	
		//Turn ON ACS TIME_DELAY_ACS_TO_ON_AFTER_ADS_ON_S seconds after
		l_us_error_code = scheduleSingleChannel(ACS_CHANNEL,CHANNEL_ON,l_ul_scheduled_time+TIME_DELAY_ACS_TO_ON_AFTER_ADS_ON_S);	
	}
	else l_us_error_code = EC_NOT_ENOUGH_SCHEDULE_SLOT;

	return l_us_error_code;
}

//------------------------------------------------------------------------------------
// To schedule to turn single channel to a desired state on a scheduled time.
//------------------------------------------------------------------------------------
unsigned short scheduleSingleChannel (unsigned char l_uc_channel, unsigned char l_uc_state, unsigned long l_ul_scheduled_time)
{
	unsigned short l_us_error_code = EC_INIT;
	unsigned char l_uc_i;

	//Turn ON/OFF desired channel
	str_scheduler.ul_scheduled_time_register 	= l_ul_scheduled_time;
	str_scheduler.us_cmd_register 				= (unsigned short)(PWRS_ADDR << 8) + PWRS_I2C_SET_SINGLE_OUTPUT;
	str_scheduler.uc_arg_length_register 		= PWRS_I2C_SET_SINGLE_OUTPUT_WR-1;
	str_scheduler.uc_arg_register[0] 			= l_uc_channel;
	str_scheduler.uc_arg_register[1] 			= l_uc_state;
	str_scheduler.uc_arg_register[2] 			= TIME_LIMIT_SET_RESET_CHANNEL_MS >> 8;
	str_scheduler.uc_arg_register[3] 			= TIME_LIMIT_SET_RESET_CHANNEL_MS;
	for (l_uc_i = str_scheduler.uc_arg_length_register; l_uc_i < MAX_SCHEDULER_ARG; l_uc_i++)
		str_scheduler.uc_arg_register[l_uc_i] 	= 0;
	l_us_error_code = addNewSchedule();

	return l_us_error_code;
}

#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)
#if (ISIS_CMD_ENABLE == 1)
//------------------------------------------------------------------------------------
// To schedule satellite to enter mission mode (the start time is in OBC time, not TAI)
//------------------------------------------------------------------------------------
unsigned short scheduleMissionMode (unsigned long l_ul_start_time, unsigned short l_us_elapsed_time)
{
	unsigned short l_us_error_code = EC_INIT;
	unsigned char l_uc_i;

	str_scheduler.ul_scheduled_time_register 	= l_ul_start_time;
	str_scheduler.us_cmd_register 				= (unsigned short)(OBDH_ADDR << 8) + OBDH_CMD_ENTER_MISSION_MODE;
	str_scheduler.uc_arg_length_register 		= 2;
	str_scheduler.uc_arg_register[0] 			= l_us_elapsed_time >> 8;
	str_scheduler.uc_arg_register[1] 			= l_us_elapsed_time;
	for (l_uc_i = str_scheduler.uc_arg_length_register; l_uc_i < MAX_SCHEDULER_ARG; l_uc_i++)
		str_scheduler.uc_arg_register[l_uc_i] 	= 0;
	l_us_error_code = addNewSchedule();

	return l_us_error_code;
}
//------------------------------------------------------------------------------------
// To insert schedule for logging data mission
//------------------------------------------------------------------------------------
void scheduleLogData (void)
{
	unsigned char l_uc_i;

	//Insert schedule to start the log data task on the scheduled time
	str_scheduler.ul_scheduled_time_register 	= uchar2ulong(g_uc_gsch_gs_arg[0],g_uc_gsch_gs_arg[1],g_uc_gsch_gs_arg[2],g_uc_gsch_gs_arg[3]) - SECONDS_FROM_TAI_TO_KILL_SWITCH_DEACTIVATION;
	str_scheduler.us_cmd_register 				= (unsigned short)(OBDH_ADDR << 8) + OBDH_CMD_COMMAND_TASK;
	str_scheduler.uc_arg_length_register 		= 2;
	str_scheduler.uc_arg_register[0] 			= TASK_LOG_ST_DATA;
	str_scheduler.uc_arg_register[1] 			= TASK_CMD_RESTART;
	for (l_uc_i = str_scheduler.uc_arg_length_register; l_uc_i < MAX_SCHEDULER_ARG; l_uc_i++)
		str_scheduler.uc_arg_register[l_uc_i] 	= 0;
	g_us_gsch_error_code = addNewSchedule();

	//Insert the wanted start time and duration to log data
	str_obdh_data.ul_log_data_start_time = str_scheduler.ul_scheduled_time_register;
	str_obdh_data.us_log_data_duration_s = (unsigned short)(g_uc_gsch_gs_arg[4] << 8) + g_uc_gsch_gs_arg[5];

	//Limit the wanted duration to log data
	if (str_obdh_data.us_log_data_duration_s > TIME_LIMIT_LOG_DATA_S)
		str_obdh_data.us_log_data_duration_s = TIME_LIMIT_LOG_DATA_S;
}

//------------------------------------------------------------------------------------
// To insert schedule sun tracking mission
//------------------------------------------------------------------------------------
void scheduleSunTracking (void)
{
	unsigned char l_uc_i;

	//Make use of global multipurpose variable to divide elapsed time for ADCS by 10
	g_us = ((unsigned short)(g_uc_gsch_gs_arg[4] << 8) + g_uc_gsch_gs_arg[5])/10;

	//Insert schedule to start the wanted task on the scheduled time +20s
	str_scheduler.ul_scheduled_time_register 	= uchar2ulong(g_uc_gsch_gs_arg[0],g_uc_gsch_gs_arg[1],g_uc_gsch_gs_arg[2],g_uc_gsch_gs_arg[3])-SECONDS_FROM_TAI_TO_KILL_SWITCH_DEACTIVATION+20;
	str_scheduler.us_cmd_register 				= (unsigned short)(ADCS_ADDR << 8) + ADCS_I2C_SET_MODE;
	str_scheduler.uc_arg_length_register 		= ADCS_I2C_SET_MODE_WR-1;
	str_scheduler.uc_arg_register[0] 			= ADCS_MODE_SUN_TRACK;
	str_scheduler.uc_arg_register[1] 			= g_us >> 8;
	str_scheduler.uc_arg_register[2] 			= g_us;
	for (l_uc_i = str_scheduler.uc_arg_length_register; l_uc_i < MAX_SCHEDULER_ARG; l_uc_i++)
		str_scheduler.uc_arg_register[l_uc_i] 	= 0;
	g_us_gsch_error_code = addNewSchedule();
}
#endif
#endif

#if (EXTRA_ENABLE == 1)
//------------------------------------------------------------------------------------
// To schedule to reset PWRS/OBDH within 50ms
//------------------------------------------------------------------------------------
unsigned short scheduleResetPWRSOBDH (unsigned long l_ul_scheduled_time, unsigned char l_uc_subsystem)
{
	unsigned short l_us_error_code = EC_INIT;
	unsigned char l_uc_i;

	//Schedule to turn ON/OFF PWRS/OBDH
	//Ian (2012-05-15): it is noted that in the final implementations, l_ul_scheduled_time = g_ul for all cases.
	str_scheduler.ul_scheduled_time_register 	= l_ul_scheduled_time;

	//Check which subsystem to reset
	if (l_uc_subsystem == PWRS_ADDR){
		str_scheduler.us_cmd_register 			= (unsigned short)(PWRS_ADDR << 8) + PWRS_I2C_RESET_PWRS;
		str_scheduler.uc_arg_length_register 	= PWRS_I2C_RESET_PWRS_WR-1;
	}
	else if (l_uc_subsystem == OBDH_ADDR){
		//Ian (2012-05-15): it is confirmed that PWRS_ADDR is to be written here instead of OBDH_ADDR.
		//					because we want to use PWRS command to reset OBDH.
		str_scheduler.us_cmd_register 			= (unsigned short)(PWRS_ADDR << 8) + PWRS_I2C_RESET_OBDH;
		str_scheduler.uc_arg_length_register 	= PWRS_I2C_RESET_OBDH_WR-1;
	}

	str_scheduler.uc_arg_register[0] 			= TIME_LIMIT_SET_RESET_CHANNEL_MS >> 8;
	str_scheduler.uc_arg_register[1] 			= TIME_LIMIT_SET_RESET_CHANNEL_MS;
	for (l_uc_i = str_scheduler.uc_arg_length_register; l_uc_i < MAX_SCHEDULER_ARG; l_uc_i++)
		str_scheduler.uc_arg_register[l_uc_i] = 0;
	l_us_error_code = addNewSchedule();

	return l_us_error_code;
}

//------------------------------------------------------------------------------------
// To schedule PWRS to turn OFF all subsystems at once
//------------------------------------------------------------------------------------
unsigned short scheduleOffAllSubsystems (unsigned long l_ul_scheduled_time)
{
	unsigned short l_us_error_code = EC_INIT;
	unsigned char l_uc_i;

	//Schedule to turn OFF all subsystems at once
	str_scheduler.ul_scheduled_time_register 	= l_ul_scheduled_time;

	//Check which subsystem to reset
	str_scheduler.us_cmd_register 				= (unsigned short)(PWRS_ADDR << 8) + PWRS_I2C_SET_OUTPUT;
	str_scheduler.uc_arg_length_register 		= PWRS_I2C_SET_OUTPUT_WR-1;
	str_scheduler.uc_arg_register[0] 			= 0;
	for (l_uc_i = str_scheduler.uc_arg_length_register; l_uc_i < MAX_SCHEDULER_ARG; l_uc_i++)
		str_scheduler.uc_arg_register[l_uc_i] = 0;
	l_us_error_code = addNewSchedule();

	return l_us_error_code;
}
#endif
#endif

