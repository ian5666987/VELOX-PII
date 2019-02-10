//------------------------------------------------------------------------------------
// Luxp_task.c
//------------------------------------------------------------------------------------
// Copyright (C) 2013 Nanyang Technological University.
//
// AUTH: IAN
// DATE: 03 APR 2013
//
// The program Luxp_task.c will be downloaded to the Pumpkin Motherboard with 
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

#include <luxp_task.h>

#if (TASK_ENABLE == 1)
//------------------------------------------------------------------------------------
// Task Module (TM)
//------------------------------------------------------------------------------------
// Task Module (TM) have the following important properties:
//	-> It will always produce response length
//	-> The response length is always started as '1'
//	-> If there is read flag raised, it will fill the output buffer and increas the
//	   response length accordingly
//------------------------------------------------------------------------------------
// Each task in TM works in the following sequence:
//	-> Trace its current task
//	-> Trace its reading format
//	-> Check whether the conditions for the current task to be run are satistifed
//		The conditions are:
//		1) The current task's initialization and running task flags are set
//		2) The current task's completed & error task flags are not set
//	-> If the conditions to run are satisfied, runs the task, if not, skips it
//	-> Check the conditions for task's reading
//		The conditions are:
//		1) The current task's status read flags is set, OR
//		2) The current task's summary read flags is set
//	-> If any of the task's read flag is set
//		1) Put the task's reading in the buffer
//		2) Increase the output buffer pointer accordingly
//------------------------------------------------------------------------------------
// Each subtask in TM works in the following sequence:
//	-> Trace its current subtask
//	-> Check whether compulsory conditions for the subtask to run are satistifed
//		The compulsory conditions are:
//		1) The current subtask's running flag is raised
//		2) The current subtask's completed & error flags are not raised
//	-> Check whether exceptional conditions for the subtask to run are satisfied
//		Some possible exceptional conditions are as follow:
//		1) Other subtask's flag
//		2) Satellite's status
//		3) Subtask's timer
//		4) Etc
//	-> If the conditions to run are satisfied, runs the subtask, if not, skips it
//	-> Check the task's detail read flag
//	-> If the task's detail read flag is set
//		1) Put the subtask's reading in the buffer
//		2) Increase the output buffer pointer accordingly
//------------------------------------------------------------------------------------
void taskModule (unsigned char l_uc_current_task, unsigned char *l_uc_response_task_buffer_p)
{
	unsigned char	l_uc_i;
	unsigned char 	l_uc_current_reading_format = 0xF0;

	//Trace current task's reading format
	l_uc_current_reading_format = getReadingFormat(l_uc_current_task);

	//If there is any reading
	//The underline assumption for this task reading is that it may only occur if there is 
	//a command captured by commandControlModule to read task. Thus, g_us_res_length will
	//always be initialized as 1 by commandControlModule before this condition is true.
	//See g_us_res_length in commandControlModule.
	if (l_uc_current_reading_format != READ_TASK_NO_READING){
		*(l_uc_response_task_buffer_p + g_us_res_length++) = l_uc_current_task;
		*(l_uc_response_task_buffer_p + g_us_res_length++) = l_uc_current_reading_format;
	}
		
	//Check current task's running condition, runs the tasks if the conditions are met, else skips it
	//The conditions are:
	//1) The current task's initialization and running task flags are set
	//2) The current task's completed task flag is not set
	if ((str_task[l_uc_current_task].uc_status & TASK_STAT_RUNNING_FLTR) == TASK_STAT_RUNNING){
	
		//----------------------------------------------------------------	
		// Current task's running block starts
		//----------------------------------------------------------------	

		//Running only one subtask at a time, if that subtask is meant to be run
			//Check compulsory conditions to run
			//The compulsory conditions are:
			// 1) The current subtask's running flag is raised
			// 2) The current subtask's completed & error flags are not raised
				//Check whether exceptional conditions for the subtask to run are satisfied
				//Some possible exceptional conditions are as follow:
				// 1) Other subtask's flag
				// 2) Satellite's status
				// 3) Subtask's timer
				// 4) Etc
		for (l_uc_i = 0; l_uc_i < 8; l_uc_i++)
			if ((str_task[l_uc_current_task].uc_script_running_flags & (unsigned short)(1 << l_uc_i)) && !(str_task[l_uc_current_task].uc_script_completed_flags & (unsigned short)(1 << l_uc_i)) && !(str_task[l_uc_current_task].uc_script_error_flags & (unsigned short)(1 << l_uc_i)))
				g_us_res_length = readScriptline(l_uc_current_task,(unsigned short)(1 << l_uc_i),g_us_res_length);	
	
		//----------------------------------------------------------------
		// Current task's running block ends
		//----------------------------------------------------------------
	}

	//Check current task's reading condition, put the reading into the buffer if the conditions are met
	//Else, skips it
	//The conditions are:
	// 1) The current task's status read flags is set, OR
	// 2) The current task's summary read flags is set
	//If any of the task's read flag is set
	// 1) Put the task's reading in the buffer
	// 2) Increase the output buffer pointer accordingly

	//----------------------------------------------------------------	
	// Current task's reading block starts
	//----------------------------------------------------------------	
	if (l_uc_current_reading_format != READ_TASK_NO_READING)
		g_us_res_length = fillData(l_uc_response_task_buffer_p,g_us_res_length,DATA_CAT_SINGLE_TASK_UC_STATUS,(unsigned short)l_uc_current_task);

	if ((l_uc_current_reading_format == READ_TASK_FORMAT_2))
		g_us_res_length = fillData(l_uc_response_task_buffer_p,g_us_res_length,DATA_CAT_SINGLE_TASK_STRUCTURE,(unsigned short)l_uc_current_task);
	//----------------------------------------------------------------	
	// Current task's reading block ends
	//----------------------------------------------------------------	
}

//------------------------------------------------------------------------------------
// Set task flags functions
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
void setTaskFlags (unsigned char l_uc_task_chosen, unsigned char l_uc_task_cmd, unsigned char l_uc_task_arg)
{
	unsigned char l_uc_i;
	unsigned char l_uc_latest_scriptline;
	unsigned char l_uc_retry_counter_no;
	unsigned short l_us_error_code=EC_SUCCESSFUL;//dec22_yc
	//---------------------------------------------
	// Handle task's command block starts here
	//---------------------------------------------
	#if (EVENT_HANDLER_ENABLE == 1)
	#if (STORING_ENABLE == 1)
	if (l_uc_task_chosen >= TASK_SIZE){//dec_yc invalid l_uc_task_chosen
	l_us_error_code=EV_SET_TASK_FLAG+EC_INVALID_TASK_NO;		
		eventHandler(l_us_error_code,l_uc_task_chosen,l_uc_task_cmd,
			0,0,0,0);
		return;
	}
	#endif
	#endif
	//---------------------------------------------
	//	a) TASK_CMD_TERMINATE
	//---------------------------------------------
	//	To change a task to termination (uninitialized state), the following change will
	//	take place when this command is received:
	//	-> Release all timer(s) used by the task
	//	-> Release all retry counter(s) used by the task
	//	-> Task's status, subtask flags (running, error, & completed), and timer flags 
	//	   (owning and running) will be reset
	//---------------------------------------------
	if (l_uc_task_cmd == TASK_CMD_TERMINATE)
	{
		scriptspaceReleaseAllTimer(l_uc_task_chosen);
		scriptspaceReleaseAllRetryCounter(l_uc_task_chosen);
		resetTask(l_uc_task_chosen);
	}

	//---------------------------------------------
	//	b) TASK_CMD_RUN
	//---------------------------------------------
	//	To run a terminated/stopped task. The following sequence will take place:
	//	-> If the task is initially completed or errorneous
	//		1) Checks the latest subtask that was being run
	//		2) Runs it again
	//	-> Reset no of retry used by the latest subtask 
	//	-> Set task initialization flag
	// 	-> Reset the task completed and errorneous flags
	//	-> Set task running flag
	//	-> Starts all timer(s) used by the task
	//---------------------------------------------
	else if (l_uc_task_cmd == TASK_CMD_RUN)
	{
		l_uc_latest_scriptline = str_task[l_uc_task_chosen].uc_script_completed_flags || str_task[l_uc_task_chosen].uc_script_error_flags || str_task[l_uc_task_chosen].uc_script_running_flags; 

		if ((str_task[l_uc_task_chosen].uc_status & TASK_COMPLETED_FLAG) || (str_task[l_uc_task_chosen].uc_status & TASK_ERROR_FLAG)){
			for (l_uc_i = 0; l_uc_i < MAX_SCRIPTLINE_SIZE; l_uc_i++) if (!(l_uc_latest_scriptline & (1 << l_uc_i))) {l_uc_i--; break;}
			str_task[l_uc_task_chosen].uc_script_completed_flags 	&= ~(1 << l_uc_i);		
			str_task[l_uc_task_chosen].uc_script_error_flags 		&= ~(1 << l_uc_i);		
			str_task[l_uc_task_chosen].uc_script_running_flags 		|= (1 << l_uc_i);
		}

		l_uc_retry_counter_no = scriptlineClaimRetryCounter(l_uc_task_chosen,(1 << l_uc_i));
		#if (EVENT_HANDLER_ENABLE == 1)
		#if (STORING_ENABLE == 1)
		if(! g_us_scriptline_retry_counter_error_code) {
			str_scriptline_retry_counter[l_uc_retry_counter_no].uc_counter = 0x00;//dec22_yc			
			}
		else{
			l_us_error_code=EV_SET_TASK_FLAG+l_us_error_code;
			eventHandler(l_us_error_code,l_uc_task_chosen,l_uc_task_cmd,
			str_task[l_uc_task_chosen].uc_status,str_task[l_uc_task_chosen].uc_script_error_flags,
			str_task[l_uc_task_chosen].uc_script_running_flags,str_task[l_uc_task_chosen].uc_script_completed_flags);
			return;
		}
		#endif
		#endif

		str_task[l_uc_task_chosen].uc_status	|= TASK_INITIALIZED_FLAG;
		str_task[l_uc_task_chosen].uc_status	&= ~TASK_COMPLETED_FLAG;		
		str_task[l_uc_task_chosen].uc_status	&= ~TASK_ERROR_FLAG;				
		str_task[l_uc_task_chosen].uc_status 	|= TASK_RUNNING_FLAG;
		scriptspaceStartAllTimer(l_uc_task_chosen);
	}

	//---------------------------------------------
	//	c) TASK_CMD_STOP
	//---------------------------------------------
	//	To stopped a task. The following sequence will take place:
	//	-> Stops all timer(s) used by the task
	//	-> Reset task running flag
	//---------------------------------------------
	else if (l_uc_task_cmd == TASK_CMD_STOP)
	{
		scriptspaceStopAllTimer(l_uc_task_chosen);
		str_task[l_uc_task_chosen].uc_status 	&= ~TASK_RUNNING_FLAG;
	}

	//---------------------------------------------
	//	d) TASK_CMD_RESTART
	//---------------------------------------------
	//	To reset a task and re-runs it. The following sequence will take place:
	//	-> Release all timer(s) used by the task
	//	-> Release all retry counter(s) used by the task
	//	-> Task's status, subtask flags (running, error, & completed), and timer flags 
	//	   (owning and running) will be reset
	//	-> Set task initialization flag
	//	-> Set task running flag
	//---------------------------------------------
	else if (l_uc_task_cmd == TASK_CMD_RESTART)
	{
		scriptspaceReleaseAllTimer(l_uc_task_chosen);
		scriptspaceReleaseAllRetryCounter(l_uc_task_chosen);
		resetTask(l_uc_task_chosen);
		str_task[l_uc_task_chosen].uc_status 	|= TASK_INITIALIZED_FLAG;
		str_task[l_uc_task_chosen].uc_status 	|= TASK_RUNNING_FLAG;
	}

	//---------------------------------------------
	//	e) TASK_CMD_DO_NOTHING
	//---------------------------------------------
	//	As the name suggests
	//---------------------------------------------
	else if (l_uc_task_cmd == TASK_CMD_DO_NOTHING);

	//---------------------------------------------
	// Handle task's command block ends here
	//---------------------------------------------

	//---------------------------------------------
	// Handle task's argument block ends here
	//---------------------------------------------

	//---------------------------------------------
	//	a) READ_TASK_FORMAT_1
	//---------------------------------------------
	//	To read status for a task. The following sequence will take place:
	//	-> The task's status read flag will be set
	//---------------------------------------------
	if (l_uc_task_arg == READ_TASK_FORMAT_1)
		str_task[l_uc_task_chosen].uc_status	|= TASK_STATUS_READ_FLAG;

	//---------------------------------------------
	//	b) READ_TASK_FORMAT_2
	//---------------------------------------------
	//	To read summary status for a task. The following sequence will take place:
	//	-> The task's status read flag will be set
	//	-> The task's summary read flag will be set
	//---------------------------------------------
	else if (l_uc_task_arg == READ_TASK_FORMAT_2)
		str_task[l_uc_task_chosen].uc_status	|= TASK_SUMMARY_READ_FLAG;

	#if (EVENT_HANDLER_ENABLE == 1)
	#if (STORING_ENABLE == 1)
	eventHandler(EV_SET_TASK_FLAG,l_uc_task_chosen,l_uc_task_cmd,
			str_task[l_uc_task_chosen].uc_status,str_task[l_uc_task_chosen].uc_script_error_flags,
			str_task[l_uc_task_chosen].uc_script_running_flags,str_task[l_uc_task_chosen].uc_script_completed_flags);
	#endif
	#endif
}

//------------------------------------------------------------------------------------
// Get reading format function
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
unsigned char getReadingFormat(unsigned char l_uc_task_chosen)
{
	unsigned char l_uc_reading_format = READ_TASK_NO_READING;

	if (str_task[l_uc_task_chosen].uc_status & TASK_SUMMARY_READ_FLAG)	l_uc_reading_format = READ_TASK_FORMAT_2;
	else if (str_task[l_uc_task_chosen].uc_status & TASK_STATUS_READ_FLAG)	l_uc_reading_format = READ_TASK_FORMAT_1;

	//Clear all reading flags
	str_task[l_uc_task_chosen].uc_status &= 0x8F;

	return l_uc_reading_format;
}

//------------------------------------------------------------------------------------
// To enter values of a scriptline
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
void enterScriptline (unsigned char l_uc_scriptspace, unsigned char l_uc_scriptline, unsigned char l_uc_function_code, unsigned char l_uc_property, unsigned char l_uc_end_doing, unsigned char l_uc_argument)
{
	str_scriptspace[l_uc_scriptspace].uc_script[l_uc_scriptline][0] = l_uc_function_code;
	str_scriptspace[l_uc_scriptspace].uc_script[l_uc_scriptline][1] = l_uc_property;
	str_scriptspace[l_uc_scriptspace].uc_script[l_uc_scriptline][2] = l_uc_end_doing;
	str_scriptspace[l_uc_scriptspace].uc_script[l_uc_scriptline][3] = l_uc_argument;	
}

//------------------------------------------------------------------------------------
// Scriptline end handler: to handle end of a scriptline
//  Bit7: timed/non-timed flag
//  Bit6: last/non-last flag
//  Bit5: progress/regress flag
//	Bit4: progress/regress 3
//	Bit3: progress/regress 2
//	Bit2: progress/regress 1
//	Bit1-0: end result
//		01: non-critical error
//		10: critical error
//		11: successful
//------------------------------------------------------------------------------------
void scriptlineEndHandler (unsigned char l_uc_task, unsigned char l_uc_scriptline_bit, unsigned char l_uc_end_type, unsigned char l_uc_retry_counter_no)
{
	unsigned char l_uc_timer_no;

	if ((l_uc_end_type & 0x03) == SCRIPTLINE_END_NON_CRITICAL_ERROR){
		str_task[l_uc_task].uc_script_error_flags 		|= l_uc_scriptline_bit;  //Marks the current scriptline as errorneous
		str_task[l_uc_task].uc_script_running_flags 	&= ~l_uc_scriptline_bit; //Stops the current scriptline
		str_task[l_uc_task].uc_status					|= TASK_WARNING_FLAG; 	 //Marks the current task as having warning
		scriptlineReleaseRetryCounter(l_uc_task,l_uc_retry_counter_no);
	}

	else if ((l_uc_end_type & 0x03) == SCRIPTLINE_END_CRITICAL_ERROR){
		str_task[l_uc_task].uc_script_error_flags 		|= l_uc_scriptline_bit;  //Marks the current scriptline as errorneous
		str_task[l_uc_task].uc_script_running_flags 	&= ~l_uc_scriptline_bit; //Stops the current scriptline
		str_task[l_uc_task].uc_status					|= TASK_ERROR_FLAG; 	  //Marks the current task as errorneous
		str_task[l_uc_task].uc_status					&= ~TASK_RUNNING_FLAG; 	  //Stops the current task
		scriptlineReleaseRetryCounter(l_uc_task,l_uc_retry_counter_no);
	}
	
	else if ((l_uc_end_type & 0x03) == SCRIPTLINE_END_SUCCESSFUL){
		str_task[l_uc_task].uc_script_completed_flags 	|= l_uc_scriptline_bit;
		str_task[l_uc_task].uc_script_running_flags 	&= ~l_uc_scriptline_bit;
		scriptlineReleaseRetryCounter(l_uc_task,l_uc_retry_counter_no);
	}	

	//Release the current timer used (if any)
	if (l_uc_end_type & SCRIPTLINE_END_TIMED_FLAG){ 
		l_uc_timer_no = scriptlineClaimTimer(l_uc_task,l_uc_scriptline_bit);
		scriptlineReleaseTimer(l_uc_task,l_uc_timer_no);
	}

	//If this is a last scriptline
	if (l_uc_end_type & SCRIPTLINE_END_LAST_FLAG){ 
		str_task[l_uc_task].uc_status |= TASK_COMPLETED_FLAG;
		str_task[l_uc_task].uc_status &= ~TASK_RUNNING_FLAG;							
	}

	//If the end of the task brings certain progress
	//It is possible for the three flags to have been all raised
	if (l_uc_end_type & SCRIPTLINE_END_PROGRESS_FLAG){
		if (l_uc_end_type & SCRIPTLINE_END_ONE_SHIFT_FLAG){
			str_task[l_uc_task].uc_script_running_flags 	|= (l_uc_scriptline_bit << 1);
			str_task[l_uc_task].uc_script_completed_flags 	&= ~(l_uc_scriptline_bit << 1);
		}
		if (l_uc_end_type & SCRIPTLINE_END_TWO_SHIFT_FLAG){
			str_task[l_uc_task].uc_script_running_flags 	|= (l_uc_scriptline_bit << 2);		
			str_task[l_uc_task].uc_script_completed_flags 	&= ~(l_uc_scriptline_bit << 2);
		}
		if (l_uc_end_type & SCRIPTLINE_END_THREE_SHIFT_FLAG){
			str_task[l_uc_task].uc_script_running_flags 	|= (l_uc_scriptline_bit << 3);	
			str_task[l_uc_task].uc_script_completed_flags 	&= ~(l_uc_scriptline_bit << 3);
		}
	}
	else { //Treats it as brings certain regress
		if (l_uc_end_type & SCRIPTLINE_END_ONE_SHIFT_FLAG){
			str_task[l_uc_task].uc_script_running_flags 	|= (l_uc_scriptline_bit >> 1);
			str_task[l_uc_task].uc_script_completed_flags 	&= ~(l_uc_scriptline_bit >> 1);
		}
		if (l_uc_end_type & SCRIPTLINE_END_TWO_SHIFT_FLAG){
			str_task[l_uc_task].uc_script_running_flags 	|= (l_uc_scriptline_bit >> 2);
			str_task[l_uc_task].uc_script_completed_flags 	&= ~(l_uc_scriptline_bit >> 2);
		}
		if (l_uc_end_type & SCRIPTLINE_END_THREE_SHIFT_FLAG){
			str_task[l_uc_task].uc_script_running_flags 	|= (l_uc_scriptline_bit >> 3);
			str_task[l_uc_task].uc_script_completed_flags 	&= ~(l_uc_scriptline_bit >> 3);
		}
	}
	#if (EVENT_HANDLER_ENABLE == 1)
	#if (STORING_ENABLE == 1)
	//dec28_yc
	eventHandler(EV_SCRIPTLINE_END_FLAG,l_uc_task,l_uc_end_type,
			str_task[l_uc_task].uc_status,str_task[l_uc_task].uc_script_error_flags,
			str_task[l_uc_task].uc_script_running_flags,str_task[l_uc_task].uc_script_completed_flags);
	#endif
	#endif
}

//------------------------------------------------------------------------------------
// To get a timer for a scriptline, reset and started
//	The procedure is as follow:
//	-> Find an available timer by exhausive method
//	-> When an available timer is found, claims its ownership, resets it, & starts it. 
//	-> Indicate the sub-task as owning timer
//	-> Indicate the sub-task as running the owned timer
//	-> Indicate the task as having which timer
//------------------------------------------------------------------------------------
unsigned char scriptlineGetTimer (unsigned char l_uc_task, unsigned char l_uc_scriptline_bit)
{
	unsigned char l_uc_i;
	unsigned char l_uc_timer_no = 0xFF;

	//Check task no given
	if (l_uc_task >= TASK_SIZE){
		g_us_scriptline_timer_error_code = EC_INVALID_TASK_NO;
		return l_uc_timer_no;
	}

	//Check scriptspace no given
	if (str_task[l_uc_task].uc_scriptspace >= SCRIPTSPACE_SIZE){
		g_us_scriptline_timer_error_code = EC_INVALID_SCRIPTSPACE_NO;
		return l_uc_timer_no;
	}

	//Adjust scriptline bit given
	//Ian (2012-05-15): The function to adjust scriptline bit is made
	l_uc_scriptline_bit = checkScriptlineBit(l_uc_scriptline_bit);

	//Search for unused task scriptline timer
	for (l_uc_i = 0; l_uc_i < SCRIPTLINE_TIMER_SIZE; l_uc_i++){
		if ((str_scriptline_timer[l_uc_i].uc_scriptspace_owner == SP_SCRIPTSPACE_EMPTY) && !str_scriptline_timer[l_uc_i].uc_scriptline_owner){
			l_uc_timer_no = l_uc_i;
			str_scriptline_timer[l_uc_i].uc_scriptspace_owner 		= str_task[l_uc_task].uc_scriptspace;
			str_scriptline_timer[l_uc_i].uc_scriptline_owner 		= l_uc_scriptline_bit;
			str_scriptline_timer[l_uc_i].uc_counter 				= 0x00;
			str_scriptline_timer[l_uc_i].ul_timer 					= 0x00000000;
			str_scriptline_timer[l_uc_i].uc_started 				= 0x01;
			str_task[l_uc_task].uc_script_timer_owning_flags 		|= l_uc_scriptline_bit;
			str_task[l_uc_task].uc_script_timer_running_flags 		|= l_uc_scriptline_bit;
			break;
		}
	}

	//If an unused task scriptline timer is not found, update error
	if (l_uc_timer_no != 0xFF)
		g_us_scriptline_timer_error_code = EC_SUCCESSFUL;
	else g_us_scriptline_timer_error_code = EC_ALL_SCRIPTLINE_TIMER_IS_USED;

	return l_uc_timer_no;
}

//------------------------------------------------------------------------------------
// To get timer no used by a scriptline
//	The procedure is as follow:
//	-> by exhausive search, check the timer no owned by the scriptline
//	-> if it is found, return the number, else, return 0xFF
//------------------------------------------------------------------------------------
unsigned char scriptlineReadTimerNo (unsigned char l_uc_task, unsigned char l_uc_scriptline_bit)
{
	unsigned char l_uc_i;
	unsigned char l_uc_timer_no = 0xFF;

	//Check scriptspace no given
	if (str_task[l_uc_task].uc_scriptspace >= SCRIPTSPACE_SIZE){
		g_us_scriptline_timer_error_code = EC_INVALID_SCRIPTSPACE_NO;
		return l_uc_timer_no;
	}

	//Adjust scriptline bit given
	l_uc_scriptline_bit = checkScriptlineBit(l_uc_scriptline_bit);

	//Read timer no
	for (l_uc_i = 0; l_uc_i < SCRIPTLINE_TIMER_SIZE; l_uc_i++){
		if ((str_scriptline_timer[l_uc_i].uc_scriptspace_owner == str_task[l_uc_task].uc_scriptspace) && (str_scriptline_timer[l_uc_i].uc_scriptline_owner == l_uc_scriptline_bit)){
			l_uc_timer_no = l_uc_i;
			break;
		}
	}

	//If an reading task scriptline timer is not found, update error
	if (l_uc_timer_no != 0xFF)
		g_us_scriptline_timer_error_code = EC_SUCCESSFUL;
	else g_us_scriptline_timer_error_code = EC_FAIL_TO_READ_SCRIPTLINE_TIMER;

	return l_uc_timer_no;
}

//------------------------------------------------------------------------------------
// To claim a timer for a scriptline
//------------------------------------------------------------------------------------
unsigned char scriptlineClaimTimer(unsigned char l_uc_task, unsigned char l_uc_scriptline_bit)
{
	unsigned char l_uc_timer_no = 0xFF;

	//Check if this scriptline does not have a timer
	//If it does not have, claims a timer, if it has, continues
	if (!(str_task[l_uc_task].uc_script_timer_owning_flags & l_uc_scriptline_bit)){
		l_uc_timer_no = scriptlineGetTimer (l_uc_task,l_uc_scriptline_bit);
		g_uc_evha_flag |= EVENT_FLAG_SCPTLN_FIRST_TIME_GET_TIMER;
	}
	else {
		l_uc_timer_no = scriptlineReadTimerNo (l_uc_task,l_uc_scriptline_bit);
		g_uc_evha_flag &= ~EVENT_FLAG_SCPTLN_FIRST_TIME_GET_TIMER;
	}

	return l_uc_timer_no;

}

//------------------------------------------------------------------------------------
// To start a task flag's timer (unreset)
//	The procedure is as follow:
//	-> Get the task & scriptline owner of the timer
//	-> Set the timer's scriptline owner's timer running flag
//	-> Starts the timer
//------------------------------------------------------------------------------------
void scriptlineStartTimer (unsigned char l_uc_task, unsigned char l_uc_timer_no)
{
	unsigned char l_uc_scriptline_bit;

	//Check for timer no validity
	if (l_uc_timer_no >= SCRIPTLINE_TIMER_SIZE)
		g_us_scriptline_timer_error_code = EC_INVALID_SCRIPTLINE_TIMER_NO;
	else{
		l_uc_scriptline_bit = str_scriptline_timer[l_uc_timer_no].uc_scriptline_owner;
	
		str_task[l_uc_task].uc_script_running_flags 	|= l_uc_scriptline_bit;
		str_scriptline_timer[l_uc_timer_no].uc_started 	= 0x01;

		g_us_scriptline_timer_error_code = EC_SUCCESSFUL;
	}
}

//------------------------------------------------------------------------------------
// To stop a task flag's timer (unreset)
//	The procedure is as follow:
//	-> Get the task & scriptline owner of the timer
//	-> Reset the timer's scriptline owner's timer running flag
//	-> Stops the timer
//------------------------------------------------------------------------------------
void scriptlineStopTimer (unsigned char l_uc_task, unsigned char l_uc_timer_no)
{
	unsigned char l_uc_scriptline_bit;

	//Check for timer no validity
	if (l_uc_timer_no >= SCRIPTLINE_TIMER_SIZE)
		g_us_scriptline_timer_error_code = EC_INVALID_SCRIPTLINE_TIMER_NO;
	else{
		l_uc_scriptline_bit = str_scriptline_timer[l_uc_timer_no].uc_scriptline_owner;
	
		str_task[l_uc_task].uc_script_timer_running_flags 	&= ~l_uc_scriptline_bit;
		str_scriptline_timer[l_uc_timer_no].uc_started 		= 0x00;

		g_us_scriptline_timer_error_code = EC_SUCCESSFUL;
	}
}

//------------------------------------------------------------------------------------
// To release a timer from a scriptline's possession (unreset)
//	The procedure is as follow:
//	-> Get the task & sub-task owner of the timer
//	-> Reset the timer's sub-task owner's timer owning & running flags
//	-> Reset the timer's task owner's timer owned flags
//	-> Stops the timer and set its owner to be nobody
//------------------------------------------------------------------------------------
void scriptlineReleaseTimer (unsigned char l_uc_task, unsigned char l_uc_timer_no)
{
	unsigned char l_uc_scriptline_bit;

	//Check for timer no validity
	if (l_uc_timer_no >= SCRIPTLINE_TIMER_SIZE)
		g_us_scriptline_timer_error_code = EC_INVALID_SCRIPTLINE_TIMER_NO;
	else{
		l_uc_scriptline_bit = str_scriptline_timer[l_uc_timer_no].uc_scriptline_owner;
	
		str_task[l_uc_task].uc_script_timer_owning_flags 	&= ~l_uc_scriptline_bit;
		str_task[l_uc_task].uc_script_timer_running_flags 	&= ~l_uc_scriptline_bit;
	
		str_scriptline_timer[l_uc_timer_no].uc_started 				= 0x00;
		str_scriptline_timer[l_uc_timer_no].uc_scriptspace_owner 	= SP_SCRIPTSPACE_EMPTY;
		str_scriptline_timer[l_uc_timer_no].uc_scriptline_owner 	= 0x0000;	

		g_us_scriptline_timer_error_code = EC_SUCCESSFUL;
	}
}

//------------------------------------------------------------------------------------
// To release all timers from a task's possession (unreset)
//	The procedure is as follow:
//	-> For every timer, check whether it is possessed by a task
//	-> If it is, release it
//------------------------------------------------------------------------------------
void scriptspaceReleaseAllTimer (unsigned char l_uc_task)
{
	unsigned char l_uc_i;

	//Check task no given
	if (l_uc_task >= TASK_SIZE)
		g_us_scriptline_timer_error_code = EC_INVALID_TASK_NO;
	else for (l_uc_i = 0; l_uc_i < SCRIPTLINE_TIMER_SIZE; l_uc_i++)
		if (str_scriptline_timer[l_uc_i].uc_scriptspace_owner == str_task[l_uc_task].uc_scriptspace)
			scriptlineReleaseTimer(l_uc_task,l_uc_i);
}

//------------------------------------------------------------------------------------
// To start all timers from of a task (unreset)
//	The procedure is as follow:
//	-> For every timer, check whether it is possessed by a task
//	-> If it is, starts it
//------------------------------------------------------------------------------------
void scriptspaceStartAllTimer (unsigned char l_uc_task)
{
	unsigned char l_uc_i;

	//Check task no given
	if (l_uc_task >= TASK_SIZE)
		g_us_scriptline_timer_error_code = EC_INVALID_TASK_NO;
	else for (l_uc_i = 0; l_uc_i < SCRIPTLINE_TIMER_SIZE; l_uc_i++)
		if (str_scriptline_timer[l_uc_i].uc_scriptspace_owner == str_task[l_uc_task].uc_scriptspace)
			scriptlineStartTimer(l_uc_task,l_uc_i);
}

//------------------------------------------------------------------------------------
// To stop all timers from of a task (unreset)
//	The procedure is as follow:
//	-> For every timer, check whether it is possessed by a task
//	-> If it is, stops it
//------------------------------------------------------------------------------------
void scriptspaceStopAllTimer (unsigned char l_uc_task)
{
	unsigned char l_uc_i;

	//Check task no given
	if (l_uc_task >= TASK_SIZE)
		g_us_scriptline_timer_error_code = EC_INVALID_TASK_NO;
	else for (l_uc_i = 0; l_uc_i < SCRIPTLINE_TIMER_SIZE; l_uc_i++)
		if (str_scriptline_timer[l_uc_i].uc_scriptspace_owner == str_task[l_uc_task].uc_scriptspace)
			scriptlineStopTimer(l_uc_task,l_uc_i);
}

//------------------------------------------------------------------------------------
// To reset a scriptline's timer
//------------------------------------------------------------------------------------
void resetScriptlineTimer (unsigned char l_uc_timer_no)
{
	//Check for timer no validity
	if (l_uc_timer_no >= SCRIPTLINE_TIMER_SIZE)
		g_us_scriptline_timer_error_code = EC_INVALID_SCRIPTLINE_TIMER_NO;
	else{
		str_scriptline_timer[l_uc_timer_no].uc_counter 		= 0x00;
		str_scriptline_timer[l_uc_timer_no].ul_timer 		= 0x00000000;
		g_us_scriptline_timer_error_code = EC_SUCCESSFUL;
	}
}

//------------------------------------------------------------------------------------
// To get a retry counter
//------------------------------------------------------------------------------------
unsigned char scriptlineGetRetryCounter (unsigned char l_uc_task, unsigned char l_uc_scriptline_bit)
{
	unsigned char l_uc_retry_counter_no = 0xFF;
	unsigned char l_uc_i;

	//Check task no given
	if (l_uc_task >= TASK_SIZE){
		g_us_scriptline_retry_counter_error_code = EC_INVALID_TASK_NO;
		return l_uc_retry_counter_no;
	}

	//Adjust scriptline bit given
	l_uc_scriptline_bit = checkScriptlineBit(l_uc_scriptline_bit);

	//Get retry counter
	for (l_uc_i = 0; l_uc_i < SCRIPTLINE_RETRY_COUNTER_SIZE; l_uc_i++){
		if ((str_scriptline_retry_counter[l_uc_i].uc_scriptspace_owner == SP_SCRIPTSPACE_EMPTY) && !str_scriptline_retry_counter[l_uc_i].uc_scriptline_owner){
			l_uc_retry_counter_no 										= l_uc_i;
			str_scriptline_retry_counter[l_uc_i].uc_scriptspace_owner 	= str_task[l_uc_task].uc_scriptspace;
			str_scriptline_retry_counter[l_uc_i].uc_scriptline_owner 	= l_uc_scriptline_bit;
			str_scriptline_retry_counter[l_uc_i].uc_counter 			= 0x00;
			str_task[l_uc_task].uc_script_retry_counter_owning_flags 	|= l_uc_scriptline_bit;
			break;
		}
	}

	//If an task scriptline retry counter cannot be obtained, update error
	if (l_uc_retry_counter_no != 0xFF)
		g_us_scriptline_retry_counter_error_code = EC_SUCCESSFUL;
	else g_us_scriptline_retry_counter_error_code = EC_ALL_RETRY_COUNTER_IS_USED;

	return l_uc_retry_counter_no;
}

//------------------------------------------------------------------------------------
// To read a retry counter value
//------------------------------------------------------------------------------------
unsigned char scriptlineReadRetryCounter (unsigned char l_uc_task, unsigned char l_uc_scriptline_bit)
{
	unsigned char l_uc_retry_counter_no = 0xFF;
	unsigned char l_uc_i;

	//Check scriptspace no given
	if (str_task[l_uc_task].uc_scriptspace >= SCRIPTSPACE_SIZE){
		g_us_scriptline_timer_error_code = EC_INVALID_SCRIPTSPACE_NO;
		return l_uc_retry_counter_no;
	}

	//Adjust scriptline bit given
	l_uc_scriptline_bit = checkScriptlineBit(l_uc_scriptline_bit);

	//Read retry counter used
	for (l_uc_i = 0; l_uc_i < SCRIPTLINE_RETRY_COUNTER_SIZE; l_uc_i++){
		if ((str_scriptline_retry_counter[l_uc_i].uc_scriptspace_owner == str_task[l_uc_task].uc_scriptspace) && (str_scriptline_retry_counter[l_uc_i].uc_scriptline_owner == l_uc_scriptline_bit)){
			l_uc_retry_counter_no = l_uc_i;
			break;
		}
	}

	//If an task scriptline retry counter cannot be read, update error
	if (l_uc_retry_counter_no != 0xFF)
		g_us_scriptline_retry_counter_error_code = EC_SUCCESSFUL;
	else g_us_scriptline_retry_counter_error_code = EC_FAIL_TO_READ_SCRIPTLINE_RETRY_COUNTER;

	return l_uc_retry_counter_no;
}

//------------------------------------------------------------------------------------
// To claim a retry counter
//------------------------------------------------------------------------------------
unsigned char scriptlineClaimRetryCounter (unsigned char l_uc_task, unsigned char l_uc_scriptline_bit)
{
	unsigned char l_uc_retry_counter_no = 0xFF;

	//Check if this scriptline does not have a retry counter
	//If it does not have, gets a retry counter, if it has, reads its value
	if (!(str_task[l_uc_task].uc_script_retry_counter_owning_flags & l_uc_scriptline_bit))
		l_uc_retry_counter_no = scriptlineGetRetryCounter (l_uc_task,l_uc_scriptline_bit);
	else l_uc_retry_counter_no = scriptlineReadRetryCounter (l_uc_task,l_uc_scriptline_bit);

	return l_uc_retry_counter_no;
}

//------------------------------------------------------------------------------------
// To release a retry counter
//------------------------------------------------------------------------------------
void scriptlineReleaseRetryCounter (unsigned char l_uc_task, unsigned char l_uc_retry_counter_no)
{
	unsigned char l_uc_scriptline_bit;

	//Check for l_uc_retry_counter_no validity
	if (l_uc_retry_counter_no >= SCRIPTLINE_RETRY_COUNTER_SIZE)
		g_us_scriptline_retry_counter_error_code = EC_INVALID_SCRIPTLINE_RETRY_COUNTER_NO;
	else{
		l_uc_scriptline_bit = str_scriptline_retry_counter[l_uc_retry_counter_no].uc_scriptline_owner;
	
		str_task[l_uc_task].uc_script_retry_counter_owning_flags 					&= ~l_uc_scriptline_bit;
	
		str_scriptline_retry_counter[l_uc_retry_counter_no].uc_scriptspace_owner 	= SP_SCRIPTSPACE_EMPTY;
		str_scriptline_retry_counter[l_uc_retry_counter_no].uc_scriptline_owner 	= 0x00;

		g_us_scriptline_retry_counter_error_code = EC_SUCCESSFUL;
	}
}

//------------------------------------------------------------------------------------
// To release all retry counter owned by a task
//------------------------------------------------------------------------------------
void scriptspaceReleaseAllRetryCounter (unsigned char l_uc_task)
{
	unsigned char l_uc_i;

	//Check task no given
	if (l_uc_task >= TASK_SIZE)
		g_us_scriptline_retry_counter_error_code = EC_INVALID_TASK_NO;
	else for (l_uc_i = 0; l_uc_i < SCRIPTLINE_RETRY_COUNTER_SIZE; l_uc_i++)
		if(str_scriptline_retry_counter[l_uc_i].uc_scriptspace_owner == str_task[l_uc_task].uc_scriptspace) 
			scriptlineReleaseRetryCounter(l_uc_task,l_uc_i);
}

//------------------------------------------------------------------------------------
// To read a scriptline
//------------------------------------------------------------------------------------
unsigned short readScriptline (unsigned char l_uc_task, unsigned char l_uc_scriptline_bit, unsigned short l_us_res_length)
{
	unsigned short 	l_us_error_code 				= EC_SUCCESSFUL;	//dec22_yc EC_INIT
	unsigned char 	l_uc_timer_check 				= 0;	//Initialize as failed
	unsigned char 	l_uc_timer_no 					= 0;
	unsigned char 	l_uc_interval_period_used_no 	= 0;
	unsigned char 	l_uc_retry_counter_no 			= 0;
	unsigned char 	l_uc_scriptline_end_handler 	= 0;
	unsigned char 	l_uc_complete_flag 				= 0;
	unsigned char 	l_uc_error_flag 				= 0;
	unsigned char 	l_uc_scriptline_no				= 0;
	unsigned char 	l_uc_function_code				= 0;
	unsigned char 	l_uc_property					= 0;
	unsigned char 	l_uc_end_doing					= 0;
	unsigned char 	l_uc_argument					= 0;
	unsigned char	l_uc_i							= 0;

	//Gets scriptline no
	for (l_uc_i = 0; l_uc_i < 8; l_uc_i++)
		if ((l_uc_scriptline_bit >> l_uc_i) & 0x01){
			l_uc_scriptline_no = l_uc_i;
			break;
		}

	#if (EVENT_HANDLER_ENABLE == 1)
	#if (STORING_ENABLE == 1)
	if (l_uc_task >= TASK_SIZE){//dec22_yc invalid l_uc_task_chosen tracks error only, not event...		
		eventHandler(EV_TASK_MODULE+EC_INVALID_TASK_NO,l_uc_task,l_uc_scriptline_no,0,0,0,0);
		return l_us_res_length;
	}
	#endif
	#endif

	//Initialize the readScriptline
	l_uc_function_code 	= str_scriptspace[str_task[l_uc_task].uc_scriptspace].uc_script[l_uc_scriptline_no][0];
	l_uc_property 		= str_scriptspace[str_task[l_uc_task].uc_scriptspace].uc_script[l_uc_scriptline_no][1];
	l_uc_end_doing 		= str_scriptspace[str_task[l_uc_task].uc_scriptspace].uc_script[l_uc_scriptline_no][2];
	l_uc_argument 		= str_scriptspace[str_task[l_uc_task].uc_scriptspace].uc_script[l_uc_scriptline_no][3];

	//Get the timer & interval period used first, then compare the timer with the wanted interval
	if (l_uc_property & 0x80){
		l_uc_timer_no = scriptlineClaimTimer(l_uc_task,l_uc_scriptline_bit);
		l_uc_interval_period_used_no = l_uc_property & 0x1F;

		//Special case is if this is the first time the task script get the timer,
		//Runs the script within maximum one second (later) after first time got timer
		if ((g_uc_evha_flag & EVENT_FLAG_SCPTLN_FIRST_TIME_GET_TIMER) && !g_us_scriptline_timer_error_code){
			str_scriptline_timer[l_uc_timer_no].ul_timer = (str_obdh_data.us_interval_period[l_uc_interval_period_used_no] >= 10) ? str_obdh_data.us_interval_period[l_uc_interval_period_used_no]-10 : 0;
			g_uc_evha_flag &= ~EVENT_FLAG_SCPTLN_FIRST_TIME_GET_TIMER;
		}
	
		//Check whether the timer has shown time which exceeds the interval
		if ((str_scriptline_timer[l_uc_timer_no].ul_timer >= str_obdh_data.us_interval_period[l_uc_interval_period_used_no]) && !g_us_scriptline_timer_error_code){
			//Reset the timer for preparation for the next cycle
			resetScriptlineTimer(l_uc_timer_no);
	
			//Indicates this as having completed timer check
			l_uc_timer_check = 1;
		}
	}
	else //Indicates this as having completed timer check
		l_uc_timer_check = 1;

	//If it passes timer check, runs the scriptline
	if (l_uc_timer_check){
		switch (l_uc_function_code){
			//This scriptline only rise up/put down flags of its own task as given by l_uc_argument
			//Since this scriptline will not use timer, the 5 timer-bits (scriptline's property last 5 bits) are free to be used
			//The 5-bits will be used to choose mode of the task flags manipulation. After the use of the argument, this scriptline
			//will automatically set its own complete flag (there is no error) and provide its necessary end handler type
			// Mode 1: normal running flags (00000)
			//	In this mode, the running flags will be changed according to what is referred in l_uc_argument
			//	and that's all
			// Mode 2: forced running flags (00001)
			//	In this mode, the running flags will be changed according to what is referred in l_uc_argument
			//	and the error flags and completed flags will be put down (on the running tasks)
			case SCRIPT_CMD_CHANGE_TASK_FLAGS:
				if ((l_uc_property & 0x1F) == 0x00){
					str_task[l_uc_task].uc_script_running_flags 	= l_uc_argument;
				}
				else if ((l_uc_property & 0x1F) == 0x01){
					str_task[l_uc_task].uc_script_running_flags 	= l_uc_argument;
					str_task[l_uc_task].uc_script_completed_flags 	&= ~str_task[l_uc_task].uc_script_running_flags;
					str_task[l_uc_task].uc_script_error_flags 		&= ~str_task[l_uc_task].uc_script_running_flags;
				}
				l_uc_complete_flag = 1;
				l_uc_property = 0x00;
				l_uc_end_doing = 0x00;
				break;

			//Script command to get housekeeping data
			case SCRIPT_CMD_GET_HK:
				getHK();
				break;

			#if (STORING_ENABLE == 1)
			//Script command to store housekeeping data
			case SCRIPT_CMD_STORE_HK:
				storeHK();
				break;
			#endif

			//Script command to get SSCP data
			case SCRIPT_CMD_GET_SSCP:
				getSSCP();
				break;

			//Script command to initialize data logging mission
			case SCRIPT_CMD_INIT_LOG_DATA:
				initLogData();
				l_uc_complete_flag = 1;
				break;

			//Script command to do data logging mission
			//If at any point of time, one of the following two conditions happen:
			// 1) BP reset occurs
			// 2) g_us_log_data_elapsed_time_s has exceeded str_obdh_data.us_log_data_duration_s
			//Then this scriptline is completed
			case SCRIPT_CMD_LOG_DATA:
				logData();
				if (g_us_log_data_elapsed_time_s >= str_obdh_data.us_log_data_duration_s)
					l_uc_complete_flag = 1;
				break;

			//Script command to terminate data logging mission
			case SCRIPT_CMD_TERMINATE_LOG_DATA:
				terminateLogData();
				l_uc_complete_flag = 1;
				break;

			#if (PWRS_CMD_ENABLE == 1)
			//Script command to set/reset thermal knife channel (ON/OFF)
			case SCRIPT_CMD_SET_TK_CHANNEL:
				#if (DEBUGGING_FEATURE == 1)
				l_us_error_code = pwrsSetTKOutput (TK_TEST_CHANNEL, (l_uc_argument & 0x01), TIME_LIMIT_SET_RESET_CHANNEL_MS, 0);				
				#else
				l_us_error_code = pwrsSetTKOutput (TK_TEST_CHANNEL, (l_uc_argument & 0x01), TIME_LIMIT_SET_RESET_CHANNEL_MS);				
				#endif
				if (l_us_error_code) l_uc_error_flag = 1;
				else l_uc_complete_flag = 1;
				break;

			//Script command to check thermal knife channel (whether it has been turned as desired or not)
			case SCRIPT_CMD_CHECK_TK_CHANNEL:
				//Get PWRS' thermal knife register data
				#if (DEBUGGING_FEATURE == 1)
				l_us_error_code = pwrsGetData (DATA_PWRS_C_THERMAL_KNIFE, 0);
				#else
				l_us_error_code = pwrsGetData (DATA_PWRS_C_THERMAL_KNIFE);
				#endif

				//Only valid when there is no error code
				if (!l_us_error_code){
					//Check if enable flag has been as expected
					if ((g_uc_i2c_data_in[3] & TEST_TK_ENABLE_FLAG) == l_uc_argument)
						//If it is, considers the scriptline as completed
						l_uc_complete_flag = 1;
				}

				//If timeout occurs at any point of time
				if (g_uc_tk_waiting_s >= TIME_LIMIT_TK_WAITING_S)
					//Considers the scriptline as errorneous
					l_uc_error_flag = 1;
				break;

			//Script command to check thermal knife, whether it has been cut (with timeout checking)
			case SCRIPT_CMD_CHECK_TK_CUT:
				//Get PWRS' thermal knife register data
				#if (DEBUGGING_FEATURE == 1)
				l_us_error_code = pwrsGetData (DATA_PWRS_C_THERMAL_KNIFE, 0);
				#else
				l_us_error_code = pwrsGetData (DATA_PWRS_C_THERMAL_KNIFE);
				#endif

				//Only valid when there is no error code
				if (!l_us_error_code){
					//Check if cutting flag has been as expected
					if ((g_uc_i2c_data_in[3] & TEST_TK_CUT_FLAG) == l_uc_argument)
						//If it is, considers the scriptline as completed
						l_uc_complete_flag = 1;
				}

				//If timeout occurs at any point of time
				if (g_uc_tk_uptime_s >= TIME_LIMIT_TK_CUTTING_S)
					//Considers the scriptline as errorneous
					l_uc_error_flag = 1;
				break;

			//Script command to set single PWRS channel
			case SCRIPT_CMD_SET_SINGLE_PWRS_CHANNEL:
				//PWRS' set single output command is called
				#if (DEBUGGING_FEATURE == 1)
				l_us_error_code = pwrsSetSingleOutput ((l_uc_argument & 0x7F),((l_uc_argument & 0x80) >> 7),TIME_LIMIT_SET_RESET_CHANNEL_MS,DEBUGGING_FEATURE);
				#else
				l_us_error_code = pwrsSetSingleOutput ((l_uc_argument & 0x7F),((l_uc_argument & 0x80) >> 7),TIME_LIMIT_SET_RESET_CHANNEL_MS);
				#endif
				if (l_us_error_code) l_uc_error_flag = 1;
				else l_uc_complete_flag = 1;
				break;

			//Script command to check single PWRS channel, whether it has been as expected
			case SCRIPT_CMD_CHECK_SINGLE_PWRS_CHANNEL:
				//If successful occurs at any point of time
				if (!g_us_latest_ch_stat_update_ec)
					if ((((l_uc_argument & 0x80) >> 7) && (str_pwrs_data.uc_channel_status & (l_uc_argument & 0x7F))) || (((!((l_uc_argument & 0x80) >> 7))) && (~(str_pwrs_data.uc_channel_status | ~(l_uc_argument & 0x7F)))))
						l_uc_complete_flag = 1;

				//If timeout occurs at any point of time
				if (g_uc_deployment_channel_waiting_s >= TIME_LIMIT_DPLY_WAIITING_S)
					//Considers the scriptline as errorneous
					l_uc_error_flag = 1;
				break;
			#endif

			#if (ANTENNA_CMD_ENABLE == 1)
			//Script command to execute antenna auto-deployment mechanism
			case SCRIPT_CMD_ANTENNA_AUTO_DPLY:
				//Arms the antenna first, then wait for 50 ms
				l_us_error_code = antennaI2C (g_uc_antenna_addr, ANTENNA_I2C_ARM, 0);
				waitMs(50);
				
				//Only proceeds if there is no error code
				if (!l_us_error_code){
					//Antenna auto-deployment mechanism is called
					#if (DEBUGGING_FEATURE == 1)
					l_us_error_code = antennaI2CPar (g_uc_antenna_addr,ANTENNA_I2C_AUTO_DPLY,TIME_LIMIT_ANTENNA_WAIITING_S,DEBUGGING_FEATURE);
					#else
					l_us_error_code = antennaI2CPar (g_uc_antenna_addr,ANTENNA_I2C_AUTO_DPLY,TIME_LIMIT_ANTENNA_WAIITING_S);
					#endif
				}

				if (l_us_error_code) l_uc_error_flag = 1;
				else l_uc_complete_flag = 1;
				break;

			//Script command to check antenna auto-deployment mechanism
			case SCRIPT_CMD_CHECK_ANTENNA_DPLY_STAT:
				//Get antenna deployment status
				l_us_error_code = antennaI2C (g_uc_antenna_addr, ANTENNA_I2C_GET_DPLY_STAT, 0);

				//Proceeds if there is no error code
				//Check antenna deployment status byte, only consider the case as successful if all antennas are deployed
				if ((!l_us_error_code) && (!(str_antenna_hk.us_deployment_status & INITIAL_ANTENNA_STATUS)))
					//Considers the scriptline as successful
					l_uc_complete_flag = 1;

				//If timeout occurs at any point of time or if there is an error code
				if ((g_uc_antenna_deployment_waiting_s >= TIME_LIMIT_ANTENNA_WAIITING_S*4+20) || l_us_error_code)
					//Considers the scriptline as errorneous
					l_uc_error_flag = 1;
				break;
			#endif

			default:
				break;
		}	
	}

	//Handle completed scriptline
	if (l_uc_complete_flag){
		//Adds the scriptline end handler timed & last flags
		l_uc_scriptline_end_handler |= (l_uc_property & 0xC0);
		
		//Adds the scriptline progress/regress flags for completed scriptline
		l_uc_scriptline_end_handler |= (l_uc_end_doing & 0xF0) >> 2;

		//Marks the scriptline as successful
		l_uc_scriptline_end_handler |= 0x03;

		//Runs scriptline end handler
		scriptlineEndHandler(l_uc_task,l_uc_scriptline_bit,l_uc_scriptline_end_handler,l_uc_retry_counter_no);
	}

	//Handle erroneous scriptline
	if (l_uc_error_flag){
		//Adds the scriptline end handler timed & last flags
		l_uc_scriptline_end_handler |= (l_uc_property & 0xC0);
		
		//Adds the scriptline progress/regress flags for errorneous scriptline
		l_uc_scriptline_end_handler |= (l_uc_end_doing & 0x0F) << 2;

		//If the task is a critical task
		if (l_uc_property & 0x20)
			//Marks the scriptline as critically erroneous
			l_uc_scriptline_end_handler |= 0x02;
		else
			//Marks the scriptline as non-critically erroneous
			l_uc_scriptline_end_handler |= 0x01;

		//Runs scriptline end handler
		scriptlineEndHandler(l_uc_task,l_uc_scriptline_bit,l_uc_scriptline_end_handler,l_uc_retry_counter_no);
	}

	return l_us_res_length;
}
#endif

//------------------------------------------------------------------------------------
// To get housekeeping data
//------------------------------------------------------------------------------------
void getHK (void)
{
	#if (ADCS_CMD_ENABLE == 1)
	//Only get housekeeping from ADCS if it has been turned ON for at least	TIME_DELAY_ACS_TO_ON_AFTER_ADS_ON_S
	if (g_ul_ads_uptime_s >= TIME_DELAY_ACS_TO_ON_AFTER_ADS_ON_S){
		//Get housekeeping from ADCS
		g_us_latest_adcs_hk_ec = adcsI2C(ADCS_I2C_GET_HK,0);
	
		#if (BODY_RATE_HANDLER_ENABLE == 1)
		#if (ADCS_CMD_ENABLE == 1)
		#if (PWRS_CMD_ENABLE == 1)
		//If no error code, update body rate control variables
		//If ADCS is not turned ON, therefore, this body rate will not be updated
		if (!g_us_latest_adcs_hk_ec) {
			#if (EXTRA_ENABLE == 1)
			updateBodyRateAverage();
			#endif
	
			//Update ADCS detumble mode flag for BRH
			if (str_adcs_hk.c_mode == ADCS_MODE_DETUMBLE)
				str_obdh_hk.us_brh_flag |= BRH_FLAG_DET_ON;
			else str_obdh_hk.us_brh_flag &= ~BRH_FLAG_DET_ON;
		}
		//If ADCS latest housekeeping reading is not available, then ADS must not be ON
		//In that case BRH will be considered not ready by systemUpdateAndMonitor already, no need to
		//bother here.
		#endif
		#endif
		#endif
	}
	//Else
	else g_us_latest_adcs_hk_ec = EC_ADCS_NOT_READY;
	#else
	g_us_latest_adcs_hk_ec = EC_ADCS_CMD_DISABLED;
	#endif

	#if (PWRS_CMD_ENABLE == 1)
	//Get housekeeping from PWRS
	g_us_latest_pwrs_hk_ec = pwrsI2C(PWRS_I2C_GET_HK,0);
	#else
	g_us_latest_pwrs_hk_ec = EC_PWRS_CMD_DISABLED;
	#endif

	#if (ISIS_CMD_ENABLE == 1)
	//Get housekeeping from COMM
	#if (DEBUGGING_FEATURE == 1)
	if (str_pwrs_data.uc_channel_status & COMM_CHANNEL_BIT)
		isisGetHousekeeping(0);	//Returning error code here is removed, as it is redundant with what is in luxp[version]_isis.c (20120328)
	else g_us_latest_comm_hk_ec = EC_COMM_IS_OFF;
	#else
	if (str_pwrs_data.uc_channel_status & COMM_CHANNEL_BIT)
		g_us_latest_comm_hk_ec = isisGetHousekeeping();
	else g_us_latest_comm_hk_ec = EC_COMM_IS_OFF;
	#endif
	#else
	g_us_latest_comm_hk_ec = EC_ISIS_CMD_DISABLED;
	#endif

	#if (TIME_ENABLE == 1)
	//Get housekeeping time and orbit stamp
	getHKTimeAndOrbit();
	#endif

}

#if (STORING_ENABLE == 1)
//------------------------------------------------------------------------------------
// To store housekeeping
//------------------------------------------------------------------------------------
void storeHK (void)
{
	#if (TASK_ENABLE == 1)

	//To store all housekeeping data
	g_us_latest_saving_hk_ec = saveDataProtected (ALL_SUBSYSTEMS, SD_HK, g_uc_sd_data_buffer);

	//Increase no of HK data stored taken
	if (!g_us_latest_saving_hk_ec) str_obdh_hk.ul_wod_sample_count++;

	#if (EVENT_HANDLER_ENABLE == 1)
	else{
		//dec22_yc event log tested i/p dec21_yc	
		eventHandler(EV_TASK_MODULE+SCRIPT_CMD_STORE_HK,g_us_latest_saving_hk_ec>>8,g_us_latest_saving_hk_ec,0,0,0,0);
		//dec22_yc event log catches other errors
	}
	#endif

	#endif
}
#endif

#if (TASK_ENABLE == 1)
//------------------------------------------------------------------------------------
// To get SSCP
//------------------------------------------------------------------------------------
void getSSCP (void)
{
	#if (ADCS_CMD_ENABLE == 1)
	//To get latest SSCP of the subsystem
	//Only get SSCP from ADCS if it has been turned ON for at least	TIME_DELAY_ACS_TO_ON_AFTER_ADS_ON_S
	if (g_ul_ads_uptime_s >= TIME_DELAY_ACS_TO_ON_AFTER_ADS_ON_S)
		//Get SSCP from ADCS
		g_us_latest_adcs_sscp_ec = adcsI2C(ADCS_I2C_GET_SSCP,0);
	//Else
	else g_us_latest_adcs_sscp_ec = EC_ADCS_NOT_READY;
	#else
	g_us_latest_adcs_sscp_ec = EC_ADCS_CMD_DISABLED;
	#endif

	#if (PWRS_CMD_ENABLE == 1)
	//Get SSCP from PWRS
	g_us_latest_pwrs_sscp_ec = pwrsI2C(PWRS_I2C_GET_SSCP,0);
	#else
	g_us_latest_pwrs_sscp_ec = EC_PWRS_CMD_DISABLED;
	#endif
}

//------------------------------------------------------------------------------------
// To initialize logging of the declared data type
//------------------------------------------------------------------------------------
void initLogData (void)
{
	//Initialize memory space
	str_bp.ul_datalog_write_p = SD_BP_DATALOG_START;

	//Initialize elapsed time
	g_us_log_data_elapsed_time_s = 0;

	//Initialize log data buffer index
	g_us_log_data_buffer_index = 0;

	//Initialize log data block size
	g_us_log_data_block_size = 0;

	//To put initial data set in the log data buffer
	g_us_log_data_buffer_index = fillData (&g_uc_log_data_buffer, g_us_log_data_buffer_index, DATA_CAT_LOG_DATA_INIT_SET, 0);

	#if (EVENT_HANDLER_ENABLE == 1)
	#if (STORING_ENABLE == 1)
	//dec22_yc event log	
	eventHandler(EV_TASK_MODULE+SCRIPT_CMD_INIT_LOG_DATA,0,0,0,0,0,0);
	//dec22_yc event log catches other errors
	#endif
	#endif

}

//------------------------------------------------------------------------------------
// To log the declared data type
//------------------------------------------------------------------------------------
//	To log every data declared and put them into the log data set
//
//	If the log data buffer still has space. What is put in the log data set will be
//	put in the log data buffer. 
//
//	If it does not, the log data buffer will first be saved (and therefore, cleared)
//	Then the log data set will be put in the log data buffer
//
//	The elapsed time is taken before data is obtained
//
//	The error code is accumulated into one single (char) error code
//------------------------------------------------------------------------------------
void logData (void)
{
	unsigned char l_uc_i = 0;
	unsigned char l_uc_j = 0;
	unsigned char l_uc_data_set_counter = 1;
	unsigned char l_uc_subsystem;
	unsigned char l_uc_data_type;
	unsigned short l_us_indv_error_code;
	unsigned char l_uc_error_code = 0;	//Assuming all correct at first
	unsigned short l_us_log_data_elapsed_time_s;

	//This local variable is created to eliminate elapsing time recording error
	//Due to interrupt update while recording the elapsed time
	l_us_log_data_elapsed_time_s = g_us_log_data_elapsed_time_s;

	//Insert elapsed time to the log data set buffer
	g_uc_log_data_set[l_uc_data_set_counter++] = l_us_log_data_elapsed_time_s >> 8;
	g_uc_log_data_set[l_uc_data_set_counter++] = l_us_log_data_elapsed_time_s;

	//Loop to collect data, based on the data type request
	for (l_uc_i	= 0; l_uc_i < MAX_NO_OF_DATA_TYPE_LOGGED; l_uc_i++){
		//Get the data type requested
		l_uc_subsystem = str_obdh_par.us_log_data_id[l_uc_i] >> 8;
		l_uc_data_type = str_obdh_par.us_log_data_id[l_uc_i];

		//Check the data type requested, get the data
		if (l_uc_subsystem == ADCS_ADDR){
			#if (ADCS_CMD_ENABLE == 1)
			#if (DEBUGGING_FEATURE == 1)
			l_us_indv_error_code = adcsGetData(l_uc_data_type,0);
			#else
			l_us_indv_error_code = adcsGetData(l_uc_data_type);
			#endif
			#else
			l_us_indv_error_code = EC_ADCS_CMD_DISABLED;
			#endif
		}
		else if (l_uc_subsystem == PWRS_ADDR){
			#if (PWRS_CMD_ENABLE == 1)
			#if (DEBUGGING_FEATURE == 1)
			l_us_indv_error_code = pwrsGetData(l_uc_data_type,0);
			#else
			l_us_indv_error_code = pwrsGetData(l_uc_data_type);
			#endif
			#else
			l_us_indv_error_code = EC_PWRS_CMD_DISABLED;
			#endif
		}
		else l_us_indv_error_code = EC_SUBSYSTEM_TO_LOG_DATA_UNDEFINED;

		//Update the l_uc_error_code
		if (l_us_indv_error_code) 
			l_uc_error_code |= (0x80 >> l_uc_i);
		//Obsolete line is removed (20120328)

		//Insert the data in g_uc_i2c_data_in buffer
		//Constant is replaced by unique PWRS_I2C_GET_DATA_RD (20120328)
		for (l_uc_j	= 0; l_uc_j < PWRS_I2C_GET_DATA_RD; l_uc_j++)
			g_uc_log_data_set[l_uc_data_set_counter++] = g_uc_i2c_data_in[l_uc_j];
	}

	//Insert the error code to the log data set buffer
	g_uc_log_data_set[0] = l_uc_error_code;

	//Check if the log data buffer still has space
	if (g_us_log_data_buffer_index+LOG_DATA_SET_SIZE <= LOG_DATA_BUFFER_SIZE)
		//Put the data set in the log data buffer
		g_us_log_data_buffer_index = fillData (&g_uc_log_data_buffer, g_us_log_data_buffer_index, DATA_CAT_LOG_DATA_SET, 0);
	//If the log data buffer does not have space anymore
	else{
		#if (STORING_ENABLE == 1)
		//Save the logged data to the SD card
		l_us_indv_error_code = saveDataProtected (ALL_SUBSYSTEMS, SD_LOG_DATA, g_uc_sd_data_buffer);
		#endif

		//Increase the log data block size upon successful logging
		if (!l_us_indv_error_code) g_us_log_data_block_size++;

		//Reset the data buffer index to zero
		g_us_log_data_buffer_index = 0;

		//Put the data set in the log data buffer
		g_us_log_data_buffer_index = fillData (&g_uc_log_data_buffer, g_us_log_data_buffer_index, DATA_CAT_LOG_DATA_SET, 0);
	}

	#if (EVENT_HANDLER_ENABLE == 1)
	#if (STORING_ENABLE == 1)
	//dec22_yc event log	
	eventHandler(EV_TASK_MODULE+SCRIPT_CMD_LOG_DATA,l_uc_error_code,0,0,0,0,0);
	//dec22_yc event log catches other errors
	#endif
	#endif
}

//------------------------------------------------------------------------------------
// To terminates logging mission
//------------------------------------------------------------------------------------
void terminateLogData (void)
{
	#if (STORING_ENABLE == 1)
	unsigned short l_us_error_code;

	//Save the latest logged data to the SD card
	l_us_error_code = saveDataProtected (ALL_SUBSYSTEMS, SD_LOG_DATA, g_uc_sd_data_buffer);

	#if (EVENT_HANDLER_ENABLE == 1)
	//dec22_yc event log	
	eventHandler(EV_TASK_MODULE+SCRIPT_CMD_TERMINATE_LOG_DATA,l_us_error_code>>8,l_us_error_code,0,0,0,0);
	//dec22_yc event log catches other errors
	#endif
	#endif
}

//------------------------------------------------------------------------------------
// Adjust scriptline bit given
//------------------------------------------------------------------------------------
unsigned char checkScriptlineBit (unsigned char l_uc_scriptline_bit)
{
	unsigned char l_uc_scriptline_bit_ret;

	//Adjust scriptline bit given
	if (l_uc_scriptline_bit & 0x01)			l_uc_scriptline_bit_ret = 0x01;
	else if (l_uc_scriptline_bit & 0x02) 	l_uc_scriptline_bit_ret = 0x02;
	else if (l_uc_scriptline_bit & 0x04) 	l_uc_scriptline_bit_ret = 0x04;
	else if (l_uc_scriptline_bit & 0x08) 	l_uc_scriptline_bit_ret = 0x08;
	else if (l_uc_scriptline_bit & 0x10) 	l_uc_scriptline_bit_ret = 0x10;
	else if (l_uc_scriptline_bit & 0x20) 	l_uc_scriptline_bit_ret = 0x20;
	else if (l_uc_scriptline_bit & 0x40) 	l_uc_scriptline_bit_ret = 0x40;
	else l_uc_scriptline_bit_ret = 0x80;

	return l_uc_scriptline_bit_ret;
}
#endif
