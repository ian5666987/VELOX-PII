//------------------------------------------------------------------------------------
// Luxp_main.c
//------------------------------------------------------------------------------------
// Copyright (C) 2013 Nanyang Technological University.
//
// AUTH: IAN
// DATE: 03 APR 2013
//
// The program Luxp_main.c will be downloaded to the Pumpkin Motherboard with 
// C8051F120 uC as the main program of VELOX-P Pico-Satellite Flight Software. 
//
// Starts from version 0.3, LUX-P software uses code banking. LUX-P software version
// 0.3b is identical with version 0.3 except that it does not use code banking. 
//
// Starts from version 0.7l, running multiple commands in one loop is implemented.
// Version 0.7k is the last version in which we can give commands longer than
// 16 bytes and are still executed. Starts from version 0.7l, no command longer
// than 16 bytes can be executed. But, every single command in the command queue 
// will now be first executed before SCM is run again.
//
// Version 0.7x is the optimized and organized version of 0.7w. Some potentially
// unused codes and data are removed here. Besides, some data are collected together.
// There is no main functional change between two versions. Code banking is not
// supported in the version. The organization and optimization is half-way done.
// Thus in the later version, more optimization and organization will be needed.
//
// Additional minor functional changes in version 0.7x and above:
//	1) Maximum time to send a data down now is limited to 15 minutes.
//	2) Checking transmitter current is more than one time
//		before we decide if it is ok to start new transmission.
//	3) No of bytes transmitted in single transmission is limited down to 235 instead 
//		of 237.
//	4) Receiver checking is done more than one time to determine if there is really
//		anything in the receiver buffer.
//	5) Recovery attempt for overly long single transmission is added in the beacon
//		handler
//
// Changes in version 0.8e and above:
//	1) idleModeHandler now does not handle ACS to ON/OFF. It only turn OFF ADS.
//	2) When bodyRateHandler finish its stabilization, it turn OFF ADS but not ACS.
//
// This is LUX-P Software version 1.1
//
// Changes from version 1.0
//  1) Optimization of the SD card timing, allowing faster satellite's response
//     in the event of SD card failure.
//
// Target: C8051F120
//
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include <luxp_main.h>

//-----------------------------------------------------------------------------
// 16-bit SFR declarations
//-----------------------------------------------------------------------------
sbit SDA = P0^6;                       // SMBus on P0.6	(SDA)
sbit SCL = P0^7;                       // and P0.7 (SCL)

//-----------------------------------------------------------------------------
// Structures for OBDH
//-----------------------------------------------------------------------------
struct OBDHHKStruct					str_obdh_hk;
struct OBDHParStruct				str_obdh_par;
struct OBDHDataStruct				str_obdh_data;
struct SchedulerStruct				str_scheduler;
struct ScriptlineTimerStruct		str_scriptline_timer[SCRIPTLINE_TIMER_SIZE];
struct TaskStruct					str_task[TASK_SIZE];
struct ScriptlineRetryCounterStruct	str_scriptline_retry_counter[SCRIPTLINE_RETRY_COUNTER_SIZE];
struct ScriptspaceStruct			str_scriptspace[SCRIPTSPACE_SIZE];

//-----------------------------------------------------------------------------
// Structures for data storing
//-----------------------------------------------------------------------------
struct IMCHKStruct 		str_imc_hk;
struct ITCHKStruct 		str_itc_hk;
struct AntennaHKStruct	str_antenna_hk;
struct RTCDataStruct	str_rtc_data;
struct PWRSHKStruct		str_pwrs_hk;
struct PWRSParStruct	str_pwrs_par;
struct PWRSDataStruct	str_pwrs_data;
struct ADCSHKStruct		str_adcs_hk;
struct ADCSParStruct	str_adcs_par;
struct BPStruct			str_bp;

//------------------------------------------------------------------------------------
// Structures for date and time
//------------------------------------------------------------------------------------
struct SatTimeStruct 			str_sat_time;
struct SatDateStruct 			str_sat_date;
struct DateUpdateFromRefStruct 	str_date_update;

//-----------------------------------------------------------------------------
// Oscillator variables (parameters)
//-----------------------------------------------------------------------------
#if (SYSCLK_USED == LEVEL_3_SYSCLK)
unsigned long OSC_FREQUENCY = EXT_OSC_FREQUENCY;
unsigned long SYSCLK 		= EXT_SYSCLK;
#elif (SYSCLK_USED == LEVEL_2_SYSCLK)
unsigned long OSC_FREQUENCY = EXT_OSC_FREQUENCY;
unsigned long SYSCLK 		= EXT_SYSCLK;
#elif (SYSCLK_USED == LEVEL_1_SYSCLK)
unsigned long OSC_FREQUENCY = INT_OSC_FREQUENCY;
unsigned long SYSCLK 		= INT_SYSCLK;
#endif

#if (SYSCLK_USED == LEVEL_3_SYSCLK)
unsigned char PLL_MUL 		= EXT_PLL_MUL;
unsigned char PLL_DIV 		= EXT_PLL_DIV;
unsigned long PLL_DIV_CLK 	= EXT_PLL_DIV_CLK;
unsigned long PLL_OUT_CLK 	= EXT_PLL_OUT_CLK;
unsigned char FLSCL_FLRT	= EXT_FLSCL_FLRT;
unsigned char PLLFLT_ICO	= EXT_PLLFLT_ICO;
unsigned char PLLFLT_LOOP	= EXT_PLLFLT_LOOP;
#endif

//-----------------------------------------------------------------------------
// Global variables for main loop
//-----------------------------------------------------------------------------
unsigned char 	g_uc_obdh_firmware_version[OBDH_FIRMWARE_VERSION_LENGTH] = "LUX-P Version 1.1 ";
unsigned char	g_uc_call_task_buffer[CALL_TASK_BUFFER_SIZE];
unsigned short 	g_us_error_code = EC_INIT;
unsigned short	g_us_res_length = 0x0001; //This will be produced by task flags, default is 1
unsigned short 	g_us_checksum_register = 0x0000;
unsigned short	g_us_function_module_error_code;
unsigned short	g_us_data_counter = 0;	//This variable act as global data counter

#if (CQUEUE_ENABLE == 1)
//-----------------------------------------------------------------------------
// Command queue variables
//-----------------------------------------------------------------------------
unsigned char	g_uc_command_queue[COMMAND_QUEUE_SIZE][COMMAND_LENGTH];
unsigned char 	g_uc_command_queue_size = 0;	//At first, there is nothing in the queue
unsigned char 	g_uc_command_queue_pointer = 0;	//At first, the pointer points the first command in queue
unsigned char 	g_uc_command_entry[COMMAND_LENGTH] = 0; //At first, nothing is here
#endif

//-----------------------------------------------------------------------------
// CRC variables
//-----------------------------------------------------------------------------
unsigned short g_us_crc16_table[CRC_TABLE_SIZE];

//-----------------------------------------------------------------------------
// A2D conversion software flags, counters, and variables
//-----------------------------------------------------------------------------
unsigned char 	g_uc_ad0int_counter = 0x00;

//-----------------------------------------------------------------------------
// Global variables for ISR
//-----------------------------------------------------------------------------
unsigned char g_uc_i2c_data_out[I2C_BUFFER_SIZE];	//To write data out on SMBus
unsigned char g_uc_i2c_data_in[I2C_BUFFER_SIZE];	//To read data in from SMBus
unsigned char g_uc_i2c_transferred_byte_out = 1;	//Initiates g_uc_i2c_transferred_byte_out variable
unsigned char g_uc_i2c_transferred_byte_in = 1;		//Initiates transferred_Byte_in variable
unsigned char g_uc_i2c_target;						//To specify g_uc_i2c_target slave's address
unsigned char g_uc_uart_buffer[UART_BUFFER_SIZE];	//Provides UART interface buffer
unsigned short g_us_i2c_isr_error_code = EC_INIT;
unsigned char g_uc_i2c_msg_out = EMPTY_MSG;			//Initiates type of inter-subsystem's message
unsigned short g_us_uart_buffer_filled = 0;
unsigned short g_us_uart_input_first = 0;
unsigned short g_us_uart_output_first = 0;

//-----------------------------------------------------------------------------
// Global variables for ISIS
//-----------------------------------------------------------------------------
//Some global variables are noticed to be unnecessary, as they cannot be
//extracted by PC debugging software and they only be used in one-two functions.
//Some variables are earlier assigned as global in the testing phase.
//More favorable approach, though not a must, would be to put them as 
//static local variable in the respective functions to avoid misrepresentation.
unsigned char g_uc_cw_message[CW_BUFFER_SIZE] 			= "VELOXP\0\0\0\0\0\0\0\0\0\0\0\0\0";
unsigned char g_uc_cw_message_length 					= CW_BEACON_LENGTH_M3;
unsigned char g_uc_beacon_interval 						= CW_BEACON_INTERVAL_M3;
unsigned char g_uc_from_to_callsign[AX25_CALLSIGN_SIZE] = "VELOXP\09V1SGS\0";
unsigned char g_uc_ax25_message[AX25_BUFFER_SIZE] 		= "This is AX.25 default message";
unsigned char g_uc_itc_snd_ax_fr_dflt_wr 				= 0;
unsigned char g_uc_itc_snd_ax_fr_ovrd_wr 				= 0;
unsigned char g_uc_itc_snd_cw_msg_wr 					= 0;
unsigned char g_uc_itc_set_ax_bcn_dflt_wr 				= 0;
unsigned char g_uc_itc_set_ax_bcn_ovrd_wr 				= 0;
unsigned char g_uc_itc_set_cw_bcn_wr 					= 0;

//-----------------------------------------------------------------------------
// Global variable for timeout counters
//-----------------------------------------------------------------------------
//Some global variables are noticed to be unnecessary, as they cannot be
//extracted by PC debugging software and they only be used in one-two functions.
//Some variables are earlier assigned as global in the testing phase.
//More favorable approach, though not a must, would be to put them as 
//static local variable in the respective functions to avoid misrepresentation.
unsigned char g_uc_uart_busy_timeout_counter 						= 0;
unsigned char g_uc_i2c_transmission_timeout_counter 				= 0;
unsigned char g_uc_i2c_busy_timeout_counter	 						= 0;
unsigned char g_uc_i2c_receiving_timeout_counter 					= 0;
unsigned char g_uc_i2c_bus_sda_held_low_recovery_timeout_counter 	= 0;
unsigned char g_uc_i2c_bus_scl_held_high_recovery_timeout_counter 	= 0;
unsigned char g_uc_spi_busy_timeout_counter							= 0;
unsigned char g_uc_spi_transmission_timeout_counter					= 0;
unsigned char g_uc_timer0_stuck_counter								= 0;

//-----------------------------------------------------------------------------
// Global variables for SD Card
//-----------------------------------------------------------------------------
// The variables which earlier declared to write/read multiple blocks from SD
// card are removed, for in the latest software, the SD card is write/read
// block by block, not in multiple blocks.
unsigned char 	g_uc_sd_data_buffer[SD_CARD_BUFFER_SIZE];

//-----------------------------------------------------------------------------
// Global variables for SD Card header
//-----------------------------------------------------------------------------
// When SaveDataProtected is called, the g_uc_sd_header_subsystem and 
// g_uc_sd_header_data_type variables are "initialized" - or rather "captured"
// There is no need to intialize the g_uc_sd_header_subsystem and
// g_uc_sd_header_data_type here considering the whole program. 
// Yet, for "safety standard reason" these two would be initialized as 
// "OBDH_SUBSYSTEM" and "SD_NULL"
unsigned char	g_uc_sd_header_subsystem = OBDH_SUBSYSTEM;
unsigned char	g_uc_sd_header_data_type = SD_NULL;
unsigned short	g_us_sd_header_block_property;
unsigned short	g_us_sd_header_no_of_this_block;
unsigned long	g_ul_sd_header_sd_write_block;
unsigned short	g_us_sd_header_this_block_data_size;

//-----------------------------------------------------------------------------
// Global variables for AX.25 frame package buffer
//-----------------------------------------------------------------------------
unsigned char	g_uc_ax25_header_subsystem;
unsigned char	g_uc_ax25_header_data_type;
unsigned char	g_uc_ax25_header_package_property;
unsigned char	g_uc_ax25_header_no_of_this_package;
unsigned char	g_uc_ax25_header_this_package_size = AX25_HEADER_SIZE;

//-----------------------------------------------------------------------------
// Orbital-based search (OBS) variables
// Some unused global variables have been removed
// First and last orbit request are noticed to be redundant for VELOX-P use as 
// there could be only one orbit requested at one time.
//-----------------------------------------------------------------------------
unsigned long	g_ul_obs_initial_bp; //Initial BP of the requested orbit
unsigned long	g_ul_obs_end_bp; //End BP of the requested orbit
unsigned short	g_us_obs_first_orbit_request; //Global variable for ground station request
unsigned short	g_us_obs_last_orbit_request; //Global variable for ground station request

//-----------------------------------------------------------------------------
// Global variables for data storing
// for *g_ul_initial_sd_card_bp_address, g_uc_no_of_copies, and  
// *g_uc_no_of_overwritten_p, read the functions in luxp[version]_storing.c
// initSDData() and saveDataEndHandler() for understanding their data type assignment.
// Just like g_uc_sd_header_subsystem and g_uc_sd_header_data_type, they are not
// necessarily initialized here.
//-----------------------------------------------------------------------------
unsigned char 	g_uc_no_of_data_saved;
unsigned char 	g_uc_data_type_saved_buffer[MAX_NO_OF_DATA_TYPE_TO_BE_SAVED];
unsigned long 	g_ul_initial_sd_card_bp;
unsigned long 	*g_ul_initial_sd_card_bp_address;
unsigned char 	g_uc_no_of_copies;
unsigned char 	*g_uc_no_of_overwritten_p;
unsigned char	g_uc_data_sets_property;
unsigned long	g_ul_memory_address_lower_limit;
unsigned long	g_ul_memory_address_upper_limit;
unsigned long 	g_ul_memory_address_end;
unsigned long	g_ul_start_sd_card_load_bp;		   
unsigned long   g_ul_end_sd_card_load_bp; //for load SDCARD to tell end of block
unsigned char   g_uc_sd_load_data_timeout_counter = TIME_LIMIT_LOAD_DATA_S;  
unsigned short	g_us_sd_load_error_code;
unsigned char	g_uc_save_data_hold_timeout_counter;

//-----------------------------------------------------------------------------
// Global variables for data sending
//-----------------------------------------------------------------------------
unsigned char 	g_uc_no_of_data_sent;
unsigned char	g_uc_data_type_sent_buffer[MAX_NO_OF_DATA_TYPE_TO_BE_SENT];
unsigned short	g_us_sd_card_meaningful_data_size = 0;
unsigned char	g_uc_current_data_sent_counter = 0;
unsigned short	g_us_beacon_send_error_code = EC_SUCCESSFUL;
unsigned char 	g_uc_schedule_sent_counter = 0;
unsigned short	g_us_expected_no_of_itn_checking_per_transmission = 0;

//-----------------------------------------------------------------------------
// Shared global variables for data storing and sending
//-----------------------------------------------------------------------------
unsigned char 	g_uc_temporary_data_buffer[MAX_SD_CARD_BLOCK_CONTENT_SIZE];

//------------------------------------------------------------------------------------
// Global variables for time functions
//------------------------------------------------------------------------------------
unsigned char 	g_uc_timer_value_low[RECORD_BLOCK_SIZE] 			= 0;
unsigned char 	g_uc_timer_value_high[RECORD_BLOCK_SIZE] 			= 0;
unsigned short 	g_us_timer_flag_counter_value[RECORD_BLOCK_SIZE] 	= 0;
unsigned short 	g_us_timer_flag_counter 							= 0;
unsigned short 	g_us_timer_value[RECORD_BLOCK_SIZE] 				= 0;

//------------------------------------------------------------------------------------
// Global variables for scriptline
//------------------------------------------------------------------------------------
unsigned short	g_us_scriptline_timer_error_code = EC_SUCCESSFUL;
unsigned short	g_us_scriptline_retry_counter_error_code = EC_SUCCESSFUL;

//------------------------------------------------------------------------------------
// Global variables for ground station command handler
//------------------------------------------------------------------------------------
unsigned short	g_us_groundpass_time_register = 0;

//------------------------------------------------------------------------------------
// Global variables for essential error codes
// Some unused global variables are removed
//------------------------------------------------------------------------------------
unsigned short	g_us_latest_obdh_hk_ec = EC_SUCCESSFUL;	//always successful, need to be changed for future use if the condition is no longer true. This condition is true for VELOX-P software, but not necessary for other software.
unsigned short	g_us_latest_adcs_hk_ec = EC_INIT;
unsigned short	g_us_latest_pwrs_hk_ec = EC_INIT;
unsigned short	g_us_latest_comm_hk_ec = EC_INIT;
unsigned short	g_us_latest_rtc_time_update_ec = EC_INIT; //Noticed to be unnecessary to be global variable anymore, as it cannot be read by PC. Static local variable is noticed to be more favorable
unsigned short	g_us_latest_ch_stat_update_ec = EC_INIT;
unsigned short	g_us_latest_saving_hk_ec = EC_INIT; //Noticed to be unnecessary to be global variable anymore, as it cannot be read by PC. Static local variable is noticed to be more favorable
unsigned short	g_us_latest_gdsn_to_rtc_update_ec = EC_INIT;
unsigned short	g_us_latest_obdh_sscp_ec = EC_SUCCESSFUL;	//always successful, need to be changed for future use if the condition is no longer true. This condition is true for VELOX-P software, but not necessary for other software.
unsigned short	g_us_latest_adcs_sscp_ec = EC_INIT;
unsigned short	g_us_latest_pwrs_sscp_ec = EC_INIT;

//------------------------------------------------------------------------------------
// Global multi-purpose registers
//------------------------------------------------------------------------------------
unsigned char 	g_uc;
unsigned short	g_us;
short			g_s;
//Ian (2012-05-15): It is confirmed that g_ul is not used in the interrupts and immediately
//					used to do any calculation before another function is called to prevent
//					change of value in g_ul.
unsigned long	g_ul;
float			g_f;

//------------------------------------------------------------------------------------
// Global variables for beacon handler
//------------------------------------------------------------------------------------
//	Beacon flag reading:
//	1) Bit0 -> Init/uninit (1/0)
//	2) Bit1 -> Beacon enable/disable (1/0)
//	3) Bit2 -> Check/not check (1/0)
//	4) Bit3 -> Timeout/not timeout (1/0)
//	5) Bit4 -> Monitor/not monitor (1/0)
//	6) Bit5 -> CW/AX.25 (1/0)
//	7) Bit7-6 -> CW sending mode (00 - Default, 01 - Alternate 1, 10 - Alternate 2, 11 - Alternate 3)
//	8) Bit8 -> Beacon error/not error (1/0)
//	9) Bit9 -> GSCH claim/GSCH release (1/0)
//	10) Bit10 -> GSCH send request/no request (1/0)
//	12) Bit11 -> GSCH request ack/nack (1/0)
//	13) Bit12 -> GSCH send request completed/incompleted (1/0)
//	14) Bit13 -> GSCH send request type CW/AX.25 (1/0)
//	15) Bit14 -> Beacon in recovery/normal phase (1/0)
//	16) Bit15 -> Auto-beacon disabled/enabled (1/0)
//------------------------------------------------------------------------------------
unsigned char	g_uc_beacon_time_since_last_timeout_sending = 0;
unsigned char	g_uc_beacon_timeout_value;
unsigned char 	g_uc_beacon_retry_counter = 0;
unsigned char 	g_uc_beacon_timeout_retry_counter = 0;
unsigned short	g_us_beacon_init_ax25_error_code;
unsigned short	g_us_beacon_monitoring_time = 0;
unsigned char 	g_uc_beacon_error_counter = 0;

//------------------------------------------------------------------------------------
// Global variables for GSCH
//------------------------------------------------------------------------------------
//Variables for GSCH error codes
unsigned short g_us_gsch_error_code = EC_INIT;

//Variables for further GSCH monitoring
unsigned short g_us_gsch_command_validation_error_code = EC_INVALID_GDSN_COMMAND; //Assume command to be invalid first

//Retry counter in each state
unsigned char g_uc_gsch_comm_retry_counter = 0;
unsigned char g_uc_gsch_loading_retry_counter = 0;

//Counter for monitoring GSCH transitional period
unsigned short g_us_gsch_monitoring_transitional_period = 0;

//Monitoring time for DEAD state and RECOVERY state
unsigned short g_us_gsch_monitoring_period = 0;

//Variables to store generated key and answers
unsigned short g_us_gsch_key = 0;
unsigned short g_us_gsch_answer_1 = 0;
unsigned short g_us_gsch_answer_2 = 0;

//Variables for GSCH command frame
unsigned char g_uc_gsch_gs_header;
unsigned char g_uc_gsch_gs_cmd_header;
unsigned char g_uc_gsch_gs_cmd_tail;
unsigned char g_uc_gsch_gs_arg_len;
unsigned char g_uc_gsch_gs_arg[MAX_GSCH_CMD_ARG_LEN];
unsigned short g_us_gsch_gs_cmd_crc;

//Variables to store subsystem and data type to load as commanded by GDSN
unsigned char g_uc_gsch_subsystem_to_load;
unsigned char g_uc_gsch_data_type_to_load;
unsigned char g_uc_gsch_sd_block_property;
unsigned char g_uc_gsch_no_of_loading = 0; //Always initialize this as zero
unsigned char g_uc_gsch_sampling_step = 1;	

//Register for scheduled command given
unsigned char g_uc_gsch_sch_register[MAX_SCHEDULER_ARG+SCHEDULER_NON_ARG_SIZE+2];
unsigned short g_us_gsch_total_sending_time = 0;

//Register for GSCH watchdog timer
unsigned long g_ul_gsch_time_elapsed_since_last_communication_with_mcc = 0;

//-----------------------------------------------------------------------------
// Datalog
//-----------------------------------------------------------------------------
unsigned char g_uc_log_data_buffer[LOG_DATA_BUFFER_SIZE] = 0;
unsigned char g_uc_log_data_set[LOG_DATA_SET_SIZE] = 0;
unsigned short g_us_log_data_elapsed_time_s = 0;
unsigned short g_us_log_data_buffer_index = 0;
unsigned short g_us_log_data_block_size = 0;

//-----------------------------------------------------------------------------
// Eventlog //dec22_yc
//-----------------------------------------------------------------------------
unsigned char g_uc_eventlog_data_buffer[EVENTLOG_DATA_BUFFER_SIZE] = 0;
unsigned short g_us_no_of_eventlog_this_orbit = 0;
unsigned char g_uc_eventlog_status_flag = 0;//missing logs... full logs, time to save log to SD

//-----------------------------------------------------------------------------
// Idle / Mission time variables
//-----------------------------------------------------------------------------
unsigned short g_us_mission_time_register = 0;
unsigned long g_ul_idle_time_register = 0;

//-----------------------------------------------------------------------------
// Body rate control variables
//-----------------------------------------------------------------------------
unsigned char g_uc_desired_adcs_mode = ADCS_MODE_DETUMBLE;
unsigned char g_uc_brh_retry_counter = 0;
unsigned char g_uc_brh_timeout_retry_counter = 0;
unsigned char g_uc_brh_wait_det_time_s = 0;
unsigned long g_ul_ads_uptime_s = 0;
unsigned long g_ul_det_uptime_s = 0;
short g_s_br_eci[3][BR_ECI_SAMPLE_NO];

//-----------------------------------------------------------------------------
// Idle mode variables
//-----------------------------------------------------------------------------
unsigned char g_uc_imh_retry_counter = 0;
unsigned char g_uc_imh_acs_timeout_retry_counter = 0;
unsigned char g_uc_imh_ads_timeout_retry_counter = 0;
unsigned char g_uc_imh_wait_acs_time_s = 0;
unsigned char g_uc_imh_wait_ads_time_s = 0;

//-----------------------------------------------------------------------------
// Safe mode variables
//-----------------------------------------------------------------------------
unsigned long g_ul_smh_time_register = 0;
unsigned long g_ul_smh_saved_time = 0;
unsigned char g_uc_soc_reading_when_entering_safe_mode = THRESHOLD_SOC_M3_TO_M4_DEFAULT;
unsigned char g_uc_soc_after_one_orbit_in_safe_mode = THRESHOLD_SOC_M3_TO_M4_DEFAULT;

//-----------------------------------------------------------------------------
// Flags collection
//-----------------------------------------------------------------------------
//Though might be modified in more than one interrupt function as well as the normal
//code, the i2c flag set/reset is carefully assigned. Based on testing, no unpredictable
//outcome is observed.
unsigned short g_us_i2c_flag 		= 0x0000;	//I2C flags
unsigned char g_uc_spit0ad0_flag 	= 0x00;		//SPI, Timer0, ADC0 flags
unsigned char g_uc_combn_flag 		= 0x00;		//Command, user input, software timer, UART flags
unsigned char g_uc_strsnd_flag 		= 0x00;		//Storing and sending flags
unsigned char g_uc_gsch_flag 		= 0x00;		//GSCH flags
unsigned char g_uc_evha_flag 		= 0x00;		//Event and hold activation flags

//-----------------------------------------------------------------------------
// Update handler's timers
//-----------------------------------------------------------------------------
unsigned char g_uc_adcs_time_lapse_from_last_update_s = 0;
unsigned char g_uc_pwrs_time_lapse_from_last_update_s = 0;
unsigned char g_uc_rtc_time_lapse_from_last_update_s = 0;
unsigned char g_uc_pwrs_time_lapse_from_last_ch_stat_update_s = 0;
unsigned char g_uc_comm_time_lapse_from_last_update_s = 50;
unsigned char g_uc_antenna_status_check_from_last_update_s = 0;
unsigned short g_us_obdh_time_lapse_from_last_save_state_s = 0;

//-----------------------------------------------------------------------------
// Thermal knife task's variables
//-----------------------------------------------------------------------------
unsigned char g_uc_tk_waiting_s = 0;
unsigned char g_uc_tk_uptime_s = 0;

//-----------------------------------------------------------------------------
// Antenna deployment task's variables
//-----------------------------------------------------------------------------
unsigned char g_uc_deployment_channel_waiting_s = 0;
unsigned char g_uc_antenna_deployment_waiting_s = 0;
unsigned char g_uc_antenna_addr	= ANTENNA_1_ADDR;

//-----------------------------------------------------------------------------
// Special block pointers
//-----------------------------------------------------------------------------
unsigned long g_ul_essential_bp = SD_BP_OBDH_ESS_START; 		//This is a very special block pointer, used to get satellite initialization data in the SD card, it is never been changed unless in a very special circumstance (Genesis)
unsigned long g_ul_schedule_bp = SD_BP_OBDH_SCHEDULE_START; 	//This is a very special block pointer, used to get satellite initialization data in the SD card, it is never been changed unless in a very special circumstance (Genesis)
unsigned long g_ul_script_bp = SD_BP_OBDH_SCRIPT_START;			//This is a very special block pointer, used to get satellite initialization data in the SD card, it is never been changed unless in a very special circumstance (Genesis)

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void)
{
	//All local variables
	//Ian (2012-05-15): l_uc_counter is removed.
	unsigned char *l_uc_call_task_pointer;
	unsigned char *l_uc_response_task_pointer;
	unsigned short l_us_msg_length = 0x0000;
	unsigned short l_us_counter = 0x0000;
	unsigned short l_us_error_code = EC_INIT;
	char SFRPAGE_SAVE = SFRPAGE;

	//In the beginning, it is necessary to initialize all hardware settings.
	//Hardware and variable initialization
	initSystem();

	initSD(); //Init SD card before starting the system
	waitS(2); //Give delay after initializing SD card

	#if (STORING_ENABLE == 1)
	//Load the latest state
	loadState();

	//Increases reset count
	str_obdh_hk.uc_mcu_rst_count++;

	//Store initialization events
	#if (EVENT_HANDLER_ENABLE == 1)
	eventHandler(EV_INIT_COMPLETED,str_obdh_hk.uc_sat_rstsrc,g_uc_evha_flag,0,0,0,0); //dec28_yc
	#endif

	//Save the initial state
	saveState();

	//Initialize orbital search for orbit zero content
	if (str_obdh_data.us_current_rev_no == 0)
		l_us_error_code = saveDataProtected(OBDH_SUBSYSTEM,SD_BP,g_uc_sd_data_buffer);
	#endif

	//Clear buffers
	clearUARTBuffer();
	clearCallTaskBuffer();

	//Initialize call task and response task pointer as internal call task buffer
	l_uc_call_task_pointer = g_uc_call_task_buffer;
	l_uc_response_task_pointer = g_uc_call_task_buffer;

	#if (TASK_ENABLE == 1)
	//Starts housekeeping task
	setTaskFlags(TASK_GET_HK,TASK_CMD_RESTART,READ_TASK_NO_READING);
	#endif

	//Enable Watchdog timer
	#if (WDT_ENABLE == 1)

	SFRPAGE_SAVE = SFRPAGE;
	SFRPAGE = CONFIG_PAGE; 
	WDTCN = 0x07;		//setting WDT interval bit7=0 bits[2:0] set to 111	
	WDTCN = 0xA5;		//Enable/Reset WDT
	SFRPAGE = SFRPAGE_SAVE;

	#endif

	//Infinite loop
	while (1) 
	{
		#if (DEBUGGING_FEATURE == 1)
		//See if SCM is held
		if (!(g_uc_evha_flag & HACVTN_FLAG_SCM))
			//Satellite control module is run when it is not held
			satelliteControlModule();

		//Reset response bit
		g_uc_combn_flag &= ~CMD_FLAG_RESPONSE;

		//If SOFTWARE_LOOPHOLD_MODE is activated, then hold the software here until there is valid input to be processed
		//Ian (2012-05-15): software mode choice is disabled,
		//if (str_obdh_data.uc_software_mode == SOFTWARE_LOOPHOLD_MODE)
		//	while(!(g_uc_combn_flag & USIP_FLAG_READY));

		//If there is an input from UART buffer which is ready to be processed, switch the current
		//call and response buffer to UART buffers
		if (g_uc_combn_flag & USIP_FLAG_READY){
			l_uc_call_task_pointer = g_uc_uart_buffer;
			l_uc_response_task_pointer = g_uc_uart_buffer;

			//Reset invalid command bit
			g_uc_combn_flag &= ~CMD_FLAG_INVALID;
	
			//Clear user input bits
			g_uc_combn_flag &= ~UART_FLAG_RX_BUSY_TIMEOUT;
			g_uc_combn_flag &= ~USIP_FLAG_START;

			//Check command input here	
			//Check first byte of the input buffer
			if(*l_uc_call_task_pointer != 0xCC && *l_uc_call_task_pointer != 0xBB) //If neither Debugging Module nor Satellite Control Module gives any task, consider invalid
				g_uc_combn_flag |= CMD_FLAG_INVALID; //This is to prevent any interrupt occurs from this point onwards to run any function
			else {	//If it is a valid header
				//claims the command by putting response byte header equal to the caller header byte
				*l_uc_response_task_pointer = *l_uc_call_task_pointer;
			}
	
			//If the command is valid so far, check for length validity
			if (!(g_uc_combn_flag & CMD_FLAG_INVALID)){
				//Check message length validity
				l_us_msg_length = ((unsigned short)(*(l_uc_call_task_pointer+3) << 8) + *(l_uc_call_task_pointer+4)) + 5;
				//Ian (2012-05-15): the value 598 is changed to (UART_BUFFER_SIZE-2) 
				if (l_us_msg_length > (UART_BUFFER_SIZE-2)) g_uc_combn_flag |= CMD_FLAG_INVALID;		
			}
	
			//If the command is valid, proceeds to Command Control Module, if not, continue to task module
			if(!(g_uc_combn_flag & CMD_FLAG_INVALID)) commandControlModule(l_uc_call_task_pointer,l_uc_response_task_pointer);

			//If it is asked for response and the caller is not Satellite Control Module
			//return response to the caller + CRC bytes, if not skip it
			if (g_uc_combn_flag & CMD_FLAG_RESPONSE){
				#if (CRC_ENABLE == 1)
				//Insert CRC16 bytes
				insertCRC16(l_uc_response_task_pointer,g_us_res_length);
				#endif
		
				//If the header shows that the caller is Debugging Module, sends the output buffer to Debugging Module through defined UART line
				//Else, keep it in the buffer to be processed by Satellite Control Module
				#if (UART_LINE == 1)
					sendToUART1(l_uc_response_task_pointer,0,g_us_res_length+2);
				#elif (UART_LINE == 0)
					sendToUART0(l_uc_response_task_pointer,0,g_us_res_length+2);
				#else
				#endif
			}	

			//Clear UART input buffer header
			g_uc_uart_buffer[0] 			= 0x00;

			//Clear UART buffer indicators
			g_us_uart_buffer_filled 		= 0;
			g_us_uart_input_first 			= 0;
			g_uc_combn_flag 				&= ~USIP_FLAG_READY;
		}
		else{ //Else, the call and response buffers are taken from internal call task buffer
			l_uc_call_task_pointer = g_uc_call_task_buffer;
			l_uc_response_task_pointer = g_uc_call_task_buffer;

			#if (CQUEUE_ENABLE == 1)
			//So long as there is command in the queue, execute it
			//If a command is invalid, however, it will still be put
			//in the buffer but will not be executed
			while (g_uc_command_queue_size){
				//Reset invalid command bit
				g_uc_combn_flag &= ~CMD_FLAG_INVALID;
		
				//To handle commands generated by SCM
				commandQueueHandler();
		
				//Check command input here	
				//Check first byte of the input buffer
				if(*l_uc_call_task_pointer != 0xCC && *l_uc_call_task_pointer != 0xBB) //If neither Debugging Module nor Satellite Control Module gives any task, consider invalid
					g_uc_combn_flag |= CMD_FLAG_INVALID; //This is to prevent any interrupt occurs from this point onwards to run any function
				else {	//If it is a valid header
					//claims the command by putting response byte header equal to the caller header byte
					*l_uc_response_task_pointer = *l_uc_call_task_pointer;
				}
		
				//If the command is valid so far, check for length validity
				if (!(g_uc_combn_flag & CMD_FLAG_INVALID)){
					//Check message length validity
					l_us_msg_length = ((unsigned short)(*(l_uc_call_task_pointer+3) << 8) + *(l_uc_call_task_pointer+4)) + 5;
					//Ian (2012-05-15): the value 598 is changed to (UART_BUFFER_SIZE-2) 
					if (l_us_msg_length > (UART_BUFFER_SIZE-2)) g_uc_combn_flag |= CMD_FLAG_INVALID;		
				}
		
				//If the command is valid, proceeds to Command Control Module, if not, continue to task module
				if(!(g_uc_combn_flag & CMD_FLAG_INVALID)) commandControlModule(l_uc_call_task_pointer,l_uc_response_task_pointer);

				//Clear call task buffer header
				g_uc_call_task_buffer[0] = 0x00;
			}
			#else
			#if (SATELLITE_SCHEDULE_HANDLER_ENABLE == 1)
			//Execute all scheduled commands
			satelliteScheduleHandler();
			#endif
			#endif
		}

		#elif (DEBUGGING_FEATURE == 0)

		//Satellite control module is always running
		satelliteControlModule();

		//Reset response bit
		g_uc_combn_flag &= ~CMD_FLAG_RESPONSE;

		#if (CQUEUE_ENABLE == 1)
		//So long as there is command in the queue, execute it
		//If a command is invalid, however, it will still be put
		//in the buffer but will not be executed
		while (g_uc_command_queue_size){
			//Reset invalid command bit
			g_uc_combn_flag &= ~CMD_FLAG_INVALID;
	
			//To handle commands generated by SCM
			commandQueueHandler();
	
			//Check command input here	
			//Check first byte of the input buffer
			if(*l_uc_call_task_pointer != 0xCC && *l_uc_call_task_pointer != 0xBB) //If neither Debugging Module nor Satellite Control Module gives any task, consider invalid
				g_uc_combn_flag |= CMD_FLAG_INVALID; //This is to prevent any interrupt occurs from this point onwards to run any function
			else {	//If it is a valid header
				//claims the command by putting response byte header equal to the caller header byte
				*l_uc_response_task_pointer = *l_uc_call_task_pointer;
			}
	
			//If the command is valid so far, check for length validity
			if (!(g_uc_combn_flag & CMD_FLAG_INVALID)){
				//Check message length validity
				l_us_msg_length = ((unsigned short)(*(l_uc_call_task_pointer+3) << 8) + *(l_uc_call_task_pointer+4)) + 5;
				//Ian (2012-05-15): the value 598 is changed to (UART_BUFFER_SIZE-2) 
				if (l_us_msg_length > (UART_BUFFER_SIZE-2)) g_uc_combn_flag |= CMD_FLAG_INVALID;			
			}
	
			//If the command is valid, proceeds to Command Control Module, if not, continue to task module
			if(!(g_uc_combn_flag & CMD_FLAG_INVALID)) commandControlModule(l_uc_call_task_pointer,l_uc_response_task_pointer);

			//Clear call task buffer header
			g_uc_call_task_buffer[0] = 0x00;
		}
		#else
		#if (SATELLITE_SCHEDULE_HANDLER_ENABLE == 1)
		//Execute all scheduled commands
		satelliteScheduleHandler();
		#endif
		#endif

		#else
		#endif
		
		#if (TASK_ENABLE == 1)
		//Only run the task module if reset is not being initiated
		if (!(g_uc_evha_flag & EVENT_FLAG_RESET_SAT_INITIALIZED)){
			#if (DEBUGGING_FEATURE == 1)
			//Run task's module according to the maximum number of task.
			//Task module will check whether certain task is under valid condition to run or not
			for (l_us_counter = 0; l_us_counter < TASK_SIZE; l_us_counter++){
				//If TM is NOT to be held and task status is NOT held, runs the TM
				if (!(g_uc_evha_flag & HACVTN_FLAG_TM) && !(g_uc_evha_flag & HACVTN_FLAG_TS))
					taskModule(l_us_counter,l_uc_response_task_pointer);
				//If TM is to be held but task status is NOT held, holds the task status
				else if ((g_uc_evha_flag & HACVTN_FLAG_TM) && !(g_uc_evha_flag & HACVTN_FLAG_TS)){
					str_task[l_us_counter].uc_status_held = str_task[l_us_counter].uc_status;
					scriptspaceStopAllTimer(l_us_counter);
					str_task[l_us_counter].uc_status &= ~TASK_RUNNING_FLAG;
					if (l_us_counter == TASK_SIZE-1) g_uc_evha_flag |= HACVTN_FLAG_TS; //In the last loop, update the hold task status
				}
				//If TM is NOT to be held but task status is held, release the task status, runs that task
				else if (!(g_uc_evha_flag & HACVTN_FLAG_TM) && (g_uc_evha_flag & HACVTN_FLAG_TS)){
					str_task[l_us_counter].uc_status = str_task[l_us_counter].uc_status_held;
					scriptspaceStartAllTimer(l_us_counter);
					//Ian (2012-05-15): the sequence of the next two lines are deemed to be correct. As soon as the flag
					//					is no more held, the tasks are run immediately.
					if (l_us_counter == TASK_SIZE-1) g_uc_evha_flag &= ~HACVTN_FLAG_TS; //In the last loop, update the hold task status
					taskModule(l_us_counter,l_uc_response_task_pointer);
				}
				//If TM is to be held, so does the task status, then do nothing, just count.
			}
			#else
			//Run task's module according to the maximum number of task.
			//Task module will check whether certain task is under valid condition to run or not
			for (l_us_counter = 0; l_us_counter < TASK_SIZE; l_us_counter++)
				taskModule(l_us_counter,l_uc_response_task_pointer);
			#endif
		}
		#endif
	}
}

//------------------------------------------------------------------------------------
// Satellite Control Module (SCM)
//------------------------------------------------------------------------------------
//	Satellite control module has four main routines:
//	 1) To check satellite's power status
//	 2) To check satellite's time and schedule
//	 3) To check satellite's state
//	 4) To check ground station commands
//
//	Based on the combination of the checking results, the satellite would be executing
//	certain commands.
//
//------------------------------------------------------------------------------------
void satelliteControlModule (void)
{
	//If reset satellite is initialized, do not do the following
	if (!(g_uc_evha_flag & EVENT_FLAG_RESET_SAT_INITIALIZED)){	
		#if (SATELLITE_MODE_HANDLER_ENABLE == 1)
		//To handle various satellite modes
		satelliteModeHandler();
		#endif
	
		//Peripheral error handler for OBDH	(whenever any recovery attempt is possible)
		peripheralErrorHandler();
	
		//To handle all updates and changes in the satellite and in the OBDH states
		systemUpdateAndMonitor();
	}

	#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)
	//To handle ground station command inputs
	groundStationCommandHandler();
	#endif

	#if (SENDING_ENABLE == 1)
	#if (ISIS_CMD_ENABLE == 1)
	#if (BEACON_HANDLER_ENABLE == 1)
	//To handle beacon modes (auto-beacon could be disabled forever, if wanted)	
	beaconHandler();
	#endif
	#endif
	#endif

	#if (CQUEUE_ENABLE == 1)
	#if (SATELLITE_SCHEDULE_HANDLER_ENABLE == 1)
	//To handle satellite schedule
	satelliteScheduleHandler();
	#endif
	#endif
}

//------------------------------------------------------------------------------------
// Command Control Module (CCM)
//------------------------------------------------------------------------------------
// Task	commands:
//	Either Satellite Control Module (SCM) or other interface module may send task
//	command the Command Control Module (CCM). There are several task commands which are 
//	supported by this CCM. They are listed as follow:
//	 a) TASK_CMD_TERMINATE
//	 b) TASK_CMD_RUN
//	 c) TASK_CMD_STOP
//	 d) TASK_CMD_RESTART
//	 e) TASK_CMD_DO_NOTHING
//
//	Besides command, there is a task's argument which is used to determine what kind
//	of reading will be used for certain task. The task's argument is as listed:
//	 a) READ_TASK_FORMAT_1
//	 b) READ_TASK_FORMAT_2
//
//------------------------------------------------------------------------------------
void commandControlModule (unsigned char *l_uc_call_task_buffer_pointer, unsigned char *l_uc_response_task_buffer_pointer)
{
	unsigned char l_uc_header;
	
	#if (TASK_ENABLE == 1)
	//Ian (2012-05-15): The size of l_uc_task_chosen, l_uc_task_cmd, and l_uc_task_arg arrays are reduced to 1.
	unsigned char l_uc_task_chosen;
	unsigned char l_uc_task_cmd;
	unsigned char l_uc_task_arg;
	unsigned char l_uc_no_of_task;
	#endif

	unsigned char *l_uc_arg_p;
	unsigned short l_us_command;
	unsigned short l_us_arg_length;
	unsigned short l_us_crc16_not_match = EC_SUCCESSFUL;

	//Initiates response length
	g_us_res_length = 1;	//If there is no task read, return one-byte header only

	l_uc_header 	= *l_uc_call_task_buffer_pointer;
	l_us_command 	= (unsigned short)(*(l_uc_call_task_buffer_pointer+1) << 8) + *(l_uc_call_task_buffer_pointer+2);
	l_us_arg_length = (unsigned short)(*(l_uc_call_task_buffer_pointer+3) << 8) + *(l_uc_call_task_buffer_pointer+4);
	l_uc_arg_p 		= l_uc_call_task_buffer_pointer+5;

	#if (CRC_ENABLE == 1)
	//Check the header of the call task buffer
	if (l_uc_header != 0xBB) //If the sender is not Satellite Control Module, do CRC-checking
		l_us_crc16_not_match = checkCRC16Match(l_uc_call_task_buffer_pointer,l_us_arg_length+5);		
	#endif

	//If CRC bytes do not match, do not proceed, if they match, proceeds
	if (!l_us_crc16_not_match){		
		//If this is a function, call that function
		//The magic constant is assigned based on 'minimum' I2C address (GOMSpace)
		//and its 'maximum' complement (RTC).
		if (l_us_command >= 0x0400 && l_us_command <= 0xD0FF){ 
			g_us_res_length = functionModule(l_uc_call_task_buffer_pointer,l_uc_response_task_buffer_pointer);
		}
		#if (TASK_ENABLE == 1)
		//If this is a single task call, read its task command to determine its action
		else if (l_us_command >= ((unsigned short)(TASK_ADDR << 8)) && l_us_command <= ((unsigned short)(TASK_ADDR << 8)+(TASK_SIZE-1))){	
			l_uc_no_of_task		= 1;
			l_uc_task_chosen 	= l_us_command-(unsigned short)(TASK_ADDR << 8); 
			l_uc_task_cmd 		= *l_uc_arg_p;
			l_uc_task_arg 		= *(l_uc_arg_p+1);

			setTaskFlags(l_uc_task_chosen,l_uc_task_cmd,l_uc_task_arg);

			*(l_uc_response_task_buffer_pointer + g_us_res_length++) = l_uc_no_of_task;
		}
		#endif
		g_uc_combn_flag |= CMD_FLAG_RESPONSE;
	}
}

#if (SATELLITE_MODE_HANDLER_ENABLE == 1)
//------------------------------------------------------------------------------------
// To handle satellite modes
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
void satelliteModeHandler (void)
{
	unsigned short l_us_error_code = EC_INIT;

	//Check satellite operation mode
	switch (str_obdh_hk.uc_sat_mode){
		//If currently satellite is in the Early Operation Mode
		case SATELLITE_MODE_M2:
			//During M2, auto-beacon is disabled
			//Ian (2012-05-15): This line is to ensure that whatever happens, so that the auto beacon must be disabled
			//					during M2.
			if (!(str_obdh_hk.us_beacon_flag & BEACON_FLAG_AUTO_BEACON_DISABLE))
				str_obdh_hk.us_beacon_flag |= BEACON_FLAG_AUTO_BEACON_DISABLE;

			#if (BODY_RATE_HANDLER_ENABLE == 1)
			#if (ADCS_CMD_ENABLE == 1)
			#if (PWRS_CMD_ENABLE == 1)
			//Check satellite time, if the time has already passed the limit, do necessary things
			//Before 15 mins, don't enable bodyRateHandler, but after 15 mins, enable it.
			if ((str_obdh_data.ul_obc_time_s > TIME_LIMIT_ENABLE_BRH_IN_M2) && (!(str_obdh_hk.us_brh_flag & BRH_FLAG_ENABLE))){
				#if (SATELLITE_SCHEDULE_HANDLER_ENABLE == 1)
				//If BRH is disabled, enable it
				//Turn on ADCS once
				//Turn ON ADS channel in 50 ms in str_obdh_data.ul_obc_time_s + 2 seconds
				scheduleTurnOnADCS(str_obdh_data.ul_obc_time_s+2);
				#endif

				//Reset body rate value first, assuming it is high
				resetBodyRateValue();

				//Set BRH enable flag
				str_obdh_hk.us_brh_flag |= BRH_FLAG_ENABLE;
			}

			//After 15 mins + 20 seconds
			//By this time, ADCS would have been turn ON by the scheduler too. Give buffer of 20 sec
			if (str_obdh_data.ul_obc_time_s > TIME_LIMIT_ENABLE_BRH_IN_M2+20)
				//Runs bodyRateHandler
				bodyRateHandler();
			#endif
			#endif
			#endif

			#if (TASK_ENABLE == 1)
			//After 45 mins, antenna should be deployed, deployment mechanism will be started immediately
			if ((str_obdh_data.ul_obc_time_s > TIME_LIMIT_DEPLOY_ANTENNA_START_S) && (!(str_task[TASK_DEPLOY_ANTENNA].uc_status & TASK_REC_FLAGS)))
				//Starts deploy antenna's task
				setTaskFlags(TASK_DEPLOY_ANTENNA,TASK_CMD_RESTART,READ_TASK_NO_READING);
			#endif
				
			//If the time has come to escape M2 mode, escape from it
			if (str_obdh_data.ul_obc_time_s > TIME_LIMIT_TO_STAY_IN_M2){
				//Enable auto-beacon
				//Ian (2012-05-15): This flag modification cannot be put into enterM3, because once the auto beacon is disabled
				//					by the ground station, entering M3 should not cause the auto-beacon to be enabled.
				str_obdh_hk.us_beacon_flag &= ~BEACON_FLAG_AUTO_BEACON_DISABLE;

				//Change satellite mode into M3
				enterM3();
			}				
			break;

		//If currently satellite is in the LEOP idle mode
		case SATELLITE_MODE_M3:
			//If there is no timeout and no complete stabilization flag from bodyRateHandler
			if (!(str_obdh_hk.us_brh_flag & BRH_FLAG_ST_COMPLETED) && !(str_obdh_hk.us_brh_flag & BRH_FLAG_BR_ST_TO)){
				#if (BODY_RATE_HANDLER_ENABLE == 1)
				#if (ADCS_CMD_ENABLE == 1)
				#if (PWRS_CMD_ENABLE == 1)
				//If BRH is disabled, enable it
				if (!(str_obdh_hk.us_brh_flag & BRH_FLAG_ENABLE)) {
					//Reset body rate value first, assuming it is high
					resetBodyRateValue();

					//Set BRH enable flag
					str_obdh_hk.us_brh_flag |= BRH_FLAG_ENABLE;
				}
	
				//Do body rate control whenever we are in this mode
				bodyRateHandler();
				#endif
				#endif
				#endif
			}
			//Else
			else {
				#if (BODY_RATE_HANDLER_ENABLE == 1)
				#if (ADCS_CMD_ENABLE == 1)
				#if (PWRS_CMD_ENABLE == 1)
				//If BRH is enabled, disable it
				if (str_obdh_hk.us_brh_flag & BRH_FLAG_ENABLE) str_obdh_hk.us_brh_flag &= ~BRH_FLAG_ENABLE;
				#endif
				#endif
				#endif

				#if (IDLE_MODE_HANDLER_ENABLE == 1)
				#if (PWRS_CMD_ENABLE == 1)
				//If IMH is disabled, enable it
				if (!(str_obdh_hk.us_imh_flag & IMH_FLAG_ENABLE)) str_obdh_hk.us_imh_flag |= IMH_FLAG_ENABLE;

				//Do idle mode control
				idleModeHandler();
				#endif
				#endif
			}

			//Check satellite power, see if it is time to enter safe-mode
			if (str_obdh_data.uc_batt_soc < str_obdh_par.uc_soc_threshold[0]) enterM4();

			//Check if the idle time register is exceeding TIME_LIMIT_BODY_RATE_UNCHECKED_S s
			else if (g_ul_idle_time_register > TIME_LIMIT_BODY_RATE_UNCHECKED_S)
				//Re-enter M3
				enterM3();
			break;

		//If currently satellite is in the LEOP safehold mode
		case SATELLITE_MODE_M4:
			#if (IDLE_MODE_HANDLER_ENABLE == 1)
			#if (PWRS_CMD_ENABLE == 1)
			//Make sure that ADCS is OFF at this stage, unless time to enable bodyRateHandler has come
			idleModeHandler();
			#endif
			#endif

			//Check satellite power, if it goes above the limit to escape safe-mode, escape from it
			if (str_obdh_data.uc_batt_soc >= str_obdh_par.uc_soc_threshold[1]) enterM3();

			#if (SAFE_MODE_HANDLER_ENABLE == 1)
			#if (ADCS_CMD_ENABLE == 1)
			#if (PWRS_CMD_ENABLE == 1)
			//Run safe-mode handler
			else safeModeHandler();
			#endif
			#endif
			#endif
			break;

		//If currently satellite is in the Idle mode
		case SATELLITE_MODE_M5:
			#if (IDLE_MODE_HANDLER_ENABLE == 1)
			#if (PWRS_CMD_ENABLE == 1)
			//Always run IMH here, but since there is no auto-enabling, it can be disabled by user input
			idleModeHandler();
			#endif
			#endif

			#if (BODY_RATE_HANDLER_ENABLE == 1)
			#if (ADCS_CMD_ENABLE == 1)
			#if (PWRS_CMD_ENABLE == 1)
			//Do not control BRH here, but if someone want to enable BRH, he must know the procedure...
			if (str_obdh_hk.us_brh_flag & BRH_FLAG_ENABLE) {
				//Runs bodyRateHandler
				bodyRateHandler();

				//Check if the idle time register is exceeding TIME_LIMIT_BODY_RATE_UNCHECKED_S s
				if (g_ul_idle_time_register > TIME_LIMIT_BODY_RATE_UNCHECKED_S){
					#if (SATELLITE_MODE_HANDLER_ENABLE == 1)
					//Re-enter M5
					enterM5();
					#endif

					//Enable body rate handler
					reenableBRH();
				}
			}
			#endif
			#endif
			#endif

			//Check satellite power, see if it is time to enter safe-mode
			if (str_obdh_data.uc_batt_soc < str_obdh_par.uc_soc_threshold[0]) enterM8();
			break;

		//If currently satellite is in the Groundpass mode
		case SATELLITE_MODE_M6:
			#if (SATELLITE_MODE_HANDLER_ENABLE == 1)
			//Do nothing, this state will always be over, anyway.
			//Just a fail-case here, if groundpass is not started, but we are in this state, 
			//Go back to state SATELLITE_MODE_M5
			if (!(g_uc_gsch_flag & GSCH_FLAG_GROUNDPASS_STARTED)) enterM5();
			#endif
			break;

		//If currently satellite is in the Mission mode
		case SATELLITE_MODE_M7:
			#if (SATELLITE_MODE_HANDLER_ENABLE == 1)
			//Run neither BRH nor IMH here
			//So long as mission time register is still there, keep in this state, else, go to M5			
			if (!(g_us_mission_time_register)) enterM5();
			#endif
				
			//Check satellite power, see if it is time to enter safe-mode
			else if (str_obdh_data.uc_batt_soc < str_obdh_par.uc_soc_threshold[2]){
				#if (SATELLITE_MODE_HANDLER_ENABLE == 1)
				//Change satellite mode into M8
				enterM8();
				#endif

				//Reset mission time register (terminates whatever missions we currently have)
				g_us_mission_time_register = 0;
			}
			break;

		//If currently satellite is in the Power safehold mode
		case SATELLITE_MODE_M8:
			#if (IDLE_MODE_HANDLER_ENABLE == 1)
			#if (PWRS_CMD_ENABLE == 1)
			//Make sure that ADCS is OFF at this stage, unless time to enable bodyRateHandler has come
			idleModeHandler();
			#endif
			#endif

			//Check satellite power, if it goes above the limit to escape safe-mode, escape from it
			if (str_obdh_data.uc_batt_soc >= str_obdh_par.uc_soc_threshold[1]) enterM5();

			#if (SAFE_MODE_HANDLER_ENABLE == 1)
			#if (ADCS_CMD_ENABLE == 1)
			#if (PWRS_CMD_ENABLE == 1)
			//Run safe-mode handler
			else safeModeHandler();
			#endif
			#endif
			#endif

			break;
		
		//If the satellite is not within the known modes, satellite is in errorneous condition
		default:
			//Attempt to enter stable mode (M5)
			enterM5();
			break;
	}
}
#endif

//------------------------------------------------------------------------------------
// To handle some peripheral errors (in the beginning of each loop, if any)
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
void peripheralErrorHandler (void)
{
	#if (PERIPHERAL_ERROR_HANDLER_ENABLE == 1)

	char SFRPAGE_SAVE = SFRPAGE;

	//Only in the config page, we can control XBR1
	SFRPAGE = CONFIG_PAGE;

	//To detect I2C bus error and prevent I2C bus stuck when OBDH becomes master device
	if(!SDA && !(g_us_i2c_flag & I2C_FLAG_OBDH_IN_SLAVE_MODE)){
		//Set I2C bus recovery attempt flag
		g_us_i2c_flag |= I2C_FLAG_BUS_RECOVERY_ATTEMPT;

		//If slave is holding SDA low because of an improper SMBus reset or error
		while(!SDA && !(g_us_i2c_flag & I2C_FLAG_BUS_SDA_HELD_LOW_RECOVERY_TIMEOUT))
		{
			//Provide clock pulses to allow the slave to advance out
			//of its current state. This will allow it to release SDA.
			//Enable Crossbar
			XBR1 |= 0x40;                     
			
			//Forcefully drive the clock low
			SCL = 0;                         
		  	waitMs(TIME_LIMIT_SCL_FORCED_LOW_MS);					
		  									
			//Release the clock
			SCL = 1;                       
		
			//Set I2C bus recovery attempt flag for SCL held high
			g_us_i2c_flag |= I2C_FLAG_BUS_SCL_HELD_HIGH_RECOVERY_ATTEMPT;
	
			//Wait for open-drain clock output to rise
			while(!SCL && !(g_us_i2c_flag & I2C_FLAG_BUS_SCL_HELD_HIGH_RECOVERY_TIMEOUT));                     	  								   

			//Reset I2C bus recovery attempt flag for SCL held high
			g_us_i2c_flag &= ~I2C_FLAG_BUS_SCL_HELD_HIGH_RECOVERY_ATTEMPT;
		
			//Hold the clock high
			waitTenthMs(TIME_LIMIT_SCL_FORCED_HIGH_TENTH_MS);
		
			//Disable Crossbar
			XBR1 &= ~0x40;                     
		}

		//Reset I2C bus recovery attempt flag
		g_us_i2c_flag &= ~I2C_FLAG_BUS_RECOVERY_ATTEMPT;

		//Reset both timeout flags SDA low and SCl high timeout flags to zero again
		//Retry to recover I2C lines in the next loop, if the problem still persists
		g_us_i2c_flag &= ~I2C_FLAG_BUS_SDA_HELD_LOW_RECOVERY_TIMEOUT;
		g_us_i2c_flag &= ~I2C_FLAG_BUS_SCL_HELD_HIGH_RECOVERY_TIMEOUT;
	}

	//Check the status of the I2C bus again here
	if(!SDA && !(g_us_i2c_flag & I2C_FLAG_OBDH_IN_SLAVE_MODE)){
		if(!(g_us_i2c_flag & I2C_FLAG_STATUS_NON_FUNCTIONAL) ){ //dec28_yc, only updates on transitional from !I2C_FLAG_STATUS_NON_FUNCTIONAL to I2C_FLAG_STATUS_NON_FUNCTIONAL
			//I2C lines fail
			g_us_i2c_flag |= I2C_FLAG_STATUS_NON_FUNCTIONAL;
			
			//I2C lines still fail, attempt reset, clear software flag
			resetI2CCommunication();		
			#if (EVENT_HANDLER_ENABLE == 1)
			#if (STORING_ENABLE == 1)
			//dec22_yc event log	
			eventHandler(EV_PERIPHERAL_HANDLER+g_us_i2c_flag,0,0,0,0,0,0);
			#endif
			#endif
		}
	}
	else{
		if(g_us_i2c_flag & I2C_FLAG_STATUS_NON_FUNCTIONAL){ //dec28_yc, only updates on transitional from I2C_FLAG_STATUS_NON_FUNCTIONAL to !I2C_FLAG_STATUS_NON_FUNCTIONAL
			//I2C lines are okay
			g_us_i2c_flag &= ~I2C_FLAG_STATUS_NON_FUNCTIONAL;

			//I2C lines are just recovered, reset, clear software flag
			resetI2CCommunication();		
			#if (EVENT_HANDLER_ENABLE == 1)
			#if (STORING_ENABLE == 1)
			//dec22_yc event log	
			eventHandler(EV_PERIPHERAL_HANDLER+g_us_i2c_flag,0,0,0,0,0,0);
			#endif
			#endif
		}
	}

	SFRPAGE = SFRPAGE_SAVE;

	#endif
}

//------------------------------------------------------------------------------------
// To handle updates in the beginning of each loop
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
void systemUpdateAndMonitor (void)
{
	#if (UPDATE_HANDLER_ENABLE == 1)

	unsigned short l_us_adcs_error_code = EC_INIT;
	unsigned short l_us_pwrs_error_code = EC_INIT;
	unsigned short l_us_comm_error_code = EC_INIT;
	unsigned short l_us_save_bp_error_code = EC_INIT;
	unsigned char l_uc_error_counter = 0;
	#if (RTC_CMD_ENABLE == 1)
	#if (TIME_ENABLE == 1)
	unsigned long l_ul_lower_limit;	
	unsigned long l_ul_upper_limit;
	#endif
	#endif

	//Update satellite time	(estimated update: every second)
	//In the interrupt, time to be updated is raised every second 
	if (g_uc_evha_flag & EVENT_FLAG_SAT_INFO_TO_UPDATE){
		//Reset the satellite info to update flag
		g_uc_evha_flag &= ~EVENT_FLAG_SAT_INFO_TO_UPDATE;

		#if (EXTRA_ENABLE == 1)
		//Update OBDH info
		readMCUTemperature();
		#endif

		#if (TIME_ENABLE == 1)
		#if (RTC_CMD_ENABLE == 1)
		//Get satellite time from RTC
		if (g_uc_rtc_time_lapse_from_last_update_s >= GET_RTC_TIME_UPDATE_PERIOD){
			g_uc_rtc_time_lapse_from_last_update_s = 0;			
			g_us_latest_rtc_time_update_ec = getSatTimeFromRTCInSec();
			#if (EVENT_HANDLER_ENABLE == 1)
			#if (STORING_ENABLE == 1)
			//dec22_yc tested
			eventHandler(EV_RTC_TIME_UPDATE_HANDLER+g_us_latest_rtc_time_update_ec,
				str_obdh_data.ul_rtc_ref_time_s>>24,
				str_obdh_data.ul_rtc_ref_time_s>>16,
				str_obdh_data.ul_rtc_ref_time_s>>8,
				str_obdh_data.ul_rtc_ref_time_s,0,0);
			#endif
			#endif

			//If there is no error code, take RTC updated satellite time
			//Else, keep our current time
			if (!g_us_latest_rtc_time_update_ec){
				//Lock OBC time before being used
				g_ul = str_obdh_data.ul_obc_time_s;
			
				//RTC time cannot drift more than 25% since last time updated. If it is, there must be something wrong.
				l_ul_lower_limit = (g_ul >= (GET_RTC_TIME_UPDATE_PERIOD/4)) ? g_ul-(GET_RTC_TIME_UPDATE_PERIOD/4) : 0;
				l_ul_upper_limit = g_ul + (GET_RTC_TIME_UPDATE_PERIOD/4);
				if ((str_obdh_data.ul_rtc_ref_time_s <= l_ul_upper_limit) && (str_obdh_data.ul_rtc_ref_time_s > l_ul_lower_limit))
					str_obdh_data.ul_obc_time_s = str_obdh_data.ul_rtc_ref_time_s;
				else{
					#if (RTC_CMD_ENABLE == 1)
					sec2DateUpdateFromRef(g_ul);
					#if (DEBUGGING_FEATURE == 1)
					g_us_latest_gdsn_to_rtc_update_ec = rtcUpdateDate(0,str_date_update.uc_day,str_date_update.uc_month,str_date_update.uc_year,str_date_update.uc_hour,str_date_update.uc_min,str_date_update.uc_s,DEBUGGING_FEATURE);
					#else
					g_us_latest_gdsn_to_rtc_update_ec = rtcUpdateDate(0,str_date_update.uc_day,str_date_update.uc_month,str_date_update.uc_year,str_date_update.uc_hour,str_date_update.uc_min,str_date_update.uc_s);
					#endif
					#else
					g_us_latest_gdsn_to_rtc_update_ec = EC_RTC_CMD_DISABLED;
					#endif
				}
			}
		}
		#endif
		#endif

		#if (PWRS_CMD_ENABLE == 1)
		//Get latest channel status from PWRS
		if (g_uc_pwrs_time_lapse_from_last_ch_stat_update_s >= PWRS_CH_STAT_UPDATE_PERIOD){
			g_uc_pwrs_time_lapse_from_last_ch_stat_update_s = 0;
			#if (DEBUGGING_FEATURE == 1)
			g_us_latest_ch_stat_update_ec = pwrsGetData((unsigned char)DATA_PWRS_UC_CHANNEL_STATUS,0);
			#else
			g_us_latest_ch_stat_update_ec = pwrsGetData((unsigned char)DATA_PWRS_UC_CHANNEL_STATUS);
			#endif

			//Only if there is no error code in PWRS, 
			if (!g_us_latest_ch_stat_update_ec){
				#if (IDLE_MODE_HANDLER_ENABLE == 1)
				#if (PWRS_CMD_ENABLE == 1)
				//IMH can be considered ready
				str_obdh_hk.us_imh_flag |= IMH_FLAG_READY;
				#endif
				#endif

				//COMM channel can be checked, if it is not ON, while satellite mode is not M4 or M8, turn it ON
				if ((str_obdh_hk.uc_sat_mode != SATELLITE_MODE_M4) && (str_obdh_hk.uc_sat_mode != SATELLITE_MODE_M8)){
					//Check COMM channel
					if (!(str_pwrs_data.uc_channel_status & COMM_CHANNEL_BIT)){
						#if (DEBUGGING_FEATURE == 1)
						l_us_pwrs_error_code = pwrsSetSingleOutput(COMM_CHANNEL,CHANNEL_ON,TIME_LIMIT_SET_RESET_CHANNEL_MS,0);	
						#else
						l_us_pwrs_error_code = pwrsSetSingleOutput(COMM_CHANNEL,CHANNEL_ON,TIME_LIMIT_SET_RESET_CHANNEL_MS);
						#endif
					}

					//Check ACS channel
					if (!(str_pwrs_data.uc_channel_status & ACS_CHANNEL_BIT)){
						#if (DEBUGGING_FEATURE == 1)
						l_us_pwrs_error_code = pwrsSetSingleOutput(ACS_CHANNEL,CHANNEL_ON,TIME_LIMIT_SET_RESET_CHANNEL_MS,0);	
						#else
						l_us_pwrs_error_code = pwrsSetSingleOutput(ACS_CHANNEL,CHANNEL_ON,TIME_LIMIT_SET_RESET_CHANNEL_MS);
						#endif
					}
				}
			}
            #if (IDLE_MODE_HANDLER_ENABLE == 1)
            #if (PWRS_CMD_ENABLE == 1)
			else str_obdh_hk.us_imh_flag &= ~IMH_FLAG_READY;
			#endif
			#endif

			//Only if there is no error code and the PWRS channel status shows that ADS is ON
			//(both ADS desired and actual), flag for READY in the body rate handler will be set.
			//Currently, only actual check is concerened. Also, only if latest ADCS HK is valid
			//Then BRH can be truly considered as ready.
			if (!g_us_latest_ch_stat_update_ec && (str_pwrs_data.uc_channel_status & ADS_CHANNEL_BIT) && !g_us_latest_adcs_hk_ec){
				#if (BODY_RATE_HANDLER_ENABLE == 1)
				#if (ADCS_CMD_ENABLE == 1)
				#if (PWRS_CMD_ENABLE == 1)				
				str_obdh_hk.us_brh_flag |= BRH_FLAG_READY;
				#endif
				#endif
				#endif

	            #if (IDLE_MODE_HANDLER_ENABLE == 1)
	            #if (PWRS_CMD_ENABLE == 1)
				str_obdh_hk.us_imh_flag |= IMH_FLAG_ADS_ON;
				#endif
				#endif
			}
			else {
				#if (BODY_RATE_HANDLER_ENABLE == 1)
				#if (ADCS_CMD_ENABLE == 1)
				#if (PWRS_CMD_ENABLE == 1)				
				str_obdh_hk.us_brh_flag &= ~BRH_FLAG_READY; 
				str_obdh_hk.us_brh_flag &= ~BRH_FLAG_DET_ON; //If ADS is OFF, ADCS cannot detumble.
				#endif
				#endif
				#endif

	            #if (IDLE_MODE_HANDLER_ENABLE == 1)
	            #if (PWRS_CMD_ENABLE == 1)
				str_obdh_hk.us_imh_flag &= ~IMH_FLAG_ADS_ON;
				#endif
				#endif
			}				
		}
		#endif

		#if (ADCS_CMD_ENABLE == 1)
		//If it is time to update ADCS info, update ADCS info
		if (g_uc_adcs_time_lapse_from_last_update_s >= ADCS_INFO_UPDATE_PERIOD){
			g_uc_adcs_time_lapse_from_last_update_s = 0;
			
			//Only update time if ADS has been turned ON for more than TIME_DELAY_ACS_TO_ON_AFTER_ADS_ON_S
			//This is to prevent I2C lines hanging
			if (g_ul_ads_uptime_s >= TIME_DELAY_ACS_TO_ON_AFTER_ADS_ON_S){
				#if (DEBUGGING_FEATURE == 1)
				l_us_adcs_error_code = adcsSetTime (str_obdh_data.ul_obc_time_s+SECONDS_FROM_TAI_TO_KILL_SWITCH_DEACTIVATION,0);
				#else
				l_us_adcs_error_code = adcsSetTime (str_obdh_data.ul_obc_time_s+SECONDS_FROM_TAI_TO_KILL_SWITCH_DEACTIVATION);
				#endif
				#if (EVENT_HANDLER_ENABLE == 1)
				#if (STORING_ENABLE == 1)
				if(l_us_adcs_error_code) eventHandler(EV_E_ADCS_INFO_UPDATE_HANDLER+l_us_adcs_error_code,0,0,0,0,0,0);//dec22_yc track error only  dec21_yc changed and tested i/p
				#endif
				#endif
			}
		}
		#endif

		#if (PWRS_CMD_ENABLE == 1)
		//If it is time to update PWRS info, update PWRS info
		if (g_uc_pwrs_time_lapse_from_last_update_s >= PWRS_INFO_UPDATE_PERIOD){
			g_uc_pwrs_time_lapse_from_last_update_s = 0;
		}
		#endif

		#if (ISIS_CMD_ENABLE == 1)
		//If it is time to update COMM rate, update it (if necessary, implement it)
		if ((g_uc_comm_time_lapse_from_last_update_s > COMM_INFO_UPDATE_PERIOD) && (str_pwrs_data.uc_channel_status & COMM_CHANNEL_BIT)){
			g_uc_comm_time_lapse_from_last_update_s = 0;
			
			//This is to prevent unwanted beacon sending.
			l_us_comm_error_code = isisI2C(ITC_ADDR, ITC_I2C_TRN_STATE, 0);

			//If the beacon is found to be active
			if (!l_us_comm_error_code && (g_uc_i2c_data_in[0] & 0x20))
				//Turn off the beacon
				l_us_comm_error_code = isisI2C(ITC_ADDR, ITC_I2C_CLR_BCN, 0);

			//If the transmitter is found to be not idle when not used
			if (!l_us_comm_error_code && (g_uc_i2c_data_in[0] & 0x40))
				//Turn off the beacon
				l_us_comm_error_code = isisI2CPar(ITC_I2C_SET_TRN_IDLE, IDLE_OFF, 0);

			//If the transmitter is found to be not in NOMINAL TELEMETRY mode
			if (!l_us_comm_error_code && ((g_uc_i2c_data_in[0] & 0x03) != 0x01))
				//Set transmitter to nominal telemetry mode
				l_us_comm_error_code = isisI2CPar(ITC_I2C_SET_TRN_OUTM, NOMINAL_TELEMETRY, 0);

			#if (EVENT_HANDLER_ENABLE == 1)
			#if (STORING_ENABLE == 1)
			if(l_us_comm_error_code)	eventHandler(EV_E_COMM_INFO_UPDATE_HANDLER+l_us_comm_error_code,0,0,0,0,0,0);//dec22_yc track error only	dec21_yc changed and tested i/p
			#endif
			#endif

		}
		#endif

		#if (TASK_ENABLE == 1)
		#if (ANTENNA_CMD_ENABLE == 1)
		#if (PWRS_CMD_ENABLE == 1)
		//Antenna deployment handler here. To make sure to do any possible attempt to deploy antenna
		//Only applicable when satellite is in the M3 mode
		if (str_obdh_hk.uc_sat_mode == SATELLITE_MODE_M3 && g_uc_antenna_status_check_from_last_update_s >= ANTENNA_STATUS_CHECK_PERIOD){
			g_uc_antenna_status_check_from_last_update_s = 0;

			//Check if any antenna is not deployed and antenna deployment task is currently not running
			if ((str_antenna_hk.us_deployment_status & INITIAL_ANTENNA_STATUS) && (!(str_task[TASK_DEPLOY_ANTENNA].uc_status & TASK_RUNNING_FLAG))){
				//Toggle antenna microcontroller used
				g_uc_antenna_addr = (g_uc_antenna_addr == ANTENNA_1_ADDR) ? ANTENNA_2_ADDR : ANTENNA_1_ADDR;

				//Restart deployment mechanism
				setTaskFlags(TASK_DEPLOY_ANTENNA,TASK_CMD_RESTART,READ_TASK_NO_READING);
			}
		}
		#endif
		#endif
		#endif
	
		//If there is any change in the reference orbit as compared to the current orbit
	    if (str_obdh_data.us_auto_reference_rev_no != str_obdh_data.us_current_rev_no){        
			//Case 1: reference orbit is higher than the current orbit
			//Block pointer must be kept saved until both orbits show same results
			if (str_obdh_data.us_auto_reference_rev_no > str_obdh_data.us_current_rev_no)
				str_obdh_data.uc_orbit_no_changed_counter = str_obdh_data.us_auto_reference_rev_no-str_obdh_data.us_current_rev_no;
	
			//Case 2: reference orbit is lower than or equal to the current orbit
			//Only save the block pointer once
			else str_obdh_data.uc_orbit_no_changed_counter = 1;

			#if (EVENT_HANDLER_ENABLE == 1)
			#if (STORING_ENABLE == 1)
			//dec22_yc moved to top	a bit (tested)
			eventHandler(EV_ORBIT_UPDATE_HANDLER,
				str_obdh_data.us_auto_reference_rev_no>>8,str_obdh_data.us_auto_reference_rev_no,
				str_obdh_data.us_current_rev_no>>8,str_obdh_data.us_current_rev_no,0,0);//dec_yc track orbit change
			#endif
			#endif
	
			//Adjust current orbit to be the same with the reference orbit
	        str_obdh_data.us_current_rev_no = str_obdh_data.us_auto_reference_rev_no;
	
	    }
	
		#if (STORING_ENABLE == 1)
		//Save the block pointer's SD write pointer value in the respective SD card block (based-on its current orbit number)
		//Only consider the saving is done if there is no error code in the saving process
		//Ian (2012-05-15): l_uc_error_counter++ and l_uc_error_counter < 1 are put as a way to let retry if constant 1 is increased.
		while(str_obdh_data.uc_orbit_no_changed_counter && l_uc_error_counter < 1){
			l_us_save_bp_error_code = saveDataProtected(OBDH_SUBSYSTEM,SD_BP,g_uc_sd_data_buffer);
			if (!l_us_save_bp_error_code) str_obdh_data.uc_orbit_no_changed_counter--;
			else l_uc_error_counter++;
		}
		
		//If this is the time to save the satellite's latest state, save it
		if ((g_us_obdh_time_lapse_from_last_save_state_s > OBDH_SAVE_STATE_PERIOD) && (!(g_uc_evha_flag & EVENT_FLAG_RESET_SAT_INITIALIZED))){
			//Reset OBDH time lapse from last saving state attempt
			g_us_obdh_time_lapse_from_last_save_state_s = 0;

			//Latest satellite state
			saveState();
		}
		#endif
    }	

	#endif
}

#if (SENDING_ENABLE == 1)
#if (ISIS_CMD_ENABLE == 1)
#if (BEACON_HANDLER_ENABLE == 1)
//------------------------------------------------------------------------------------
// To handle beacon modes
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
void beaconHandler (void)
{
	unsigned short l_us_error_code[4] = 0;
	unsigned short l_us_msg_length = 0;
	static unsigned char l_uc_itn_low_monitoring_counter = 0;

	//If there is no error in the beacon or the time of error has exceeded TIME_LIMIT_BEACON_ERROR_S
	if (!(str_obdh_hk.us_beacon_flag & BEACON_FLAG_ERROR) || g_uc_beacon_error_counter > TIME_LIMIT_BEACON_ERROR_S){
		//Reset beacon error counter
		g_uc_beacon_error_counter = 0;

		//If beacon is uninitialized
		if (!(str_obdh_hk.us_beacon_flag & BEACON_FLAG_INIT)){								
			//Initialize CW beacon content, rate, interval and length
			groundInitCWBeacon(BEACON_FLAG_CW_MODE_M3);
		
			//Initialize CW beacon rate to default
			l_us_error_code[0] = isisI2CPar(ITC_I2C_SET_CW_CHAR_RT,str_obdh_hk.us_cw_char_rate_par,0);
		
			//Clear CW beacon
			l_us_error_code[1] = isisI2C(ITC_ADDR,ITC_I2C_CLR_BCN,0);

			//Set the beacon's TO callsigns
			l_us_error_code[2] = isisI2CMsg(ITC_I2C_SET_DFLT_AX_TO,0,0,0);
		
			//Set the beacon's FROM callsigns
			l_us_error_code[3] = isisI2CMsg(ITC_I2C_SET_DFLT_AX_FROM,0,0,0);
		
			//If there is no error code, then considers initialization finished
			if (!l_us_error_code[0] && !l_us_error_code[1] && !l_us_error_code[2] && !l_us_error_code[3]){
				//Reset all flags except auto-beacon disabling flag
				str_obdh_hk.us_beacon_flag &= ~BEACON_FLAG_ERROR_SET_FILTER;

				//Set initialized flags
				str_obdh_hk.us_beacon_flag |= BEACON_FLAG_INITIALIZED;
		
				//Reset beacon retry counter
				g_uc_beacon_retry_counter = 0;				

				//Reset the current monitoring value
				l_uc_itn_low_monitoring_counter = 0;

				#if (EVENT_HANDLER_ENABLE == 1)
				#if (STORING_ENABLE == 1)
				//dec22_yc tested
				eventHandler(EV_BEACON_HANDLER+EC_BEACON_FLAG_ERROR,str_obdh_hk.us_beacon_flag>>8,str_obdh_hk.us_beacon_flag,
										g_us_beacon_monitoring_time>>8,g_us_beacon_monitoring_time,g_uc_beacon_retry_counter,0);//dec22_yc track prior to error only as trsistion too fast				
				#endif
				#endif
			}
			//Else, increase retry counter
			else g_uc_beacon_retry_counter++;	
		}
	}

	//If the beacon is enabled and its checking flag is raised
	if ((str_obdh_hk.us_beacon_flag & BEACON_FLAG_CHECKING_FILTER) == BEACON_FLAG_CHECKING_CONDITION){
		//Reset check flag
		str_obdh_hk.us_beacon_flag &= ~BEACON_FLAG_CHECK;

		//Update beacon's automatic type here, based on some considerations
		if (str_obdh_hk.uc_gsch_state == GSCH_STATE_GROUND_PASS || str_obdh_hk.uc_gsch_state == GSCH_STATE_SEND_REAL_TIME_DATA || str_obdh_hk.uc_gsch_state == GSCH_STATE_SEND_STORED_DATA){
			str_obdh_hk.us_beacon_flag &= ~BEACON_FLAG_TYPE;
			//Change the beacon interval when entering Groundpass state
			//Change beacon interval to 10 sec
			g_uc_beacon_interval = AX25_BEACON_INTERVAL_DEFAULT;
		}
		//Else if the current GSCH state is waiting for the groundpass key
		else if (str_obdh_hk.uc_gsch_state == GSCH_STATE_WAITING_FOR_KEY_REPLY){
			//Set the beacon type to CW
			str_obdh_hk.us_beacon_flag |= BEACON_FLAG_TYPE;
	
			//Change CW beacon mode flag to COMM_ZONE flag
			//Change the beacon interval accordingly
			str_obdh_hk.us_beacon_flag &= ~BEACON_FLAG_CW_MODE;
			str_obdh_hk.us_beacon_flag |= BEACON_FLAG_CW_MODE_COMM_ZONE;
			g_uc_beacon_interval = str_obdh_par.uc_beacon_interval[3];
		}
		//Other than that, assigning each individual beacon 
		else {
			//Set the beacon type to CW
			str_obdh_hk.us_beacon_flag |= BEACON_FLAG_TYPE;
	
			//Set the beacon type and interval based on satellite mode
			//If Satellite is in M3 -> long, 1 min
			if (str_obdh_hk.uc_sat_mode == SATELLITE_MODE_M3){
				//Set auto-beacon for M3 mode
				str_obdh_hk.us_beacon_flag &= ~BEACON_FLAG_CW_MODE;
				str_obdh_hk.us_beacon_flag |= BEACON_FLAG_CW_MODE_M3;
				g_uc_beacon_interval = str_obdh_par.uc_beacon_interval[0];
			}
			//If the satellite is in M4, M5, or M7 -> long, 2 mins
			else if (str_obdh_hk.uc_sat_mode == SATELLITE_MODE_M4 || str_obdh_hk.uc_sat_mode == SATELLITE_MODE_M5 || str_obdh_hk.uc_sat_mode == SATELLITE_MODE_M7){
				//Set auto-beacon for M4, M5, and M7 mode
				str_obdh_hk.us_beacon_flag &= ~BEACON_FLAG_CW_MODE;
				str_obdh_hk.us_beacon_flag |= BEACON_FLAG_CW_MODE_M4_M5_M7;
				g_uc_beacon_interval = str_obdh_par.uc_beacon_interval[1];
			}
			//If the satellite is in M8 -> short, 2 mins
			else if (str_obdh_hk.uc_sat_mode == SATELLITE_MODE_M8){
				//Set auto-beacon for M8 mode
				str_obdh_hk.us_beacon_flag &= ~BEACON_FLAG_CW_MODE;
				str_obdh_hk.us_beacon_flag |= BEACON_FLAG_CW_MODE_M8;
				g_uc_beacon_interval = str_obdh_par.uc_beacon_interval[2];
			}
		}
	
		//Check its monitor flag
		if (str_obdh_hk.us_beacon_flag & BEACON_FLAG_MONITOR){
			//If currently the beacon is in the monitoring state
			//Check ISIS' transmitter current
			l_us_error_code[0] = isisI2C(IMC_ADDR,IMC_I2C_TRN_ICON,0);

			//If the communication with ISIS is successful, read I2C data in
			if (!l_us_error_code[0]){
				//Reset retry counter value
				g_uc_beacon_retry_counter = 0;

				//Increase monitoring time
				g_us_beacon_monitoring_time++;
	
				//If very little current reading for ISIS transmitter, considers the transmitter off
				if (g_uc_i2c_data_in[0] <= ISIS_TRN_ICON_LIMIT_WHEN_OFF && g_uc_i2c_data_in[1] == 0){
					//If we never do current monitoring before
					if (l_uc_itn_low_monitoring_counter < (ITN_CHECK-1))
						//Increase the current monitoring value
						l_uc_itn_low_monitoring_counter++;	   

					//If we have done current monitoring sufficiently before
					else if (l_uc_itn_low_monitoring_counter >= (ITN_CHECK-1)){
						//Reset the current monitoring value
						l_uc_itn_low_monitoring_counter = 0;

						//Reset the monitoring flag
						str_obdh_hk.us_beacon_flag &= ~BEACON_FLAG_MONITOR;
	
						//Reset recovery flag
						str_obdh_hk.us_beacon_flag &= ~BEACON_FLAG_RECOVERY;

						//Reset monitoring time
						g_us_beacon_monitoring_time = 0;
	
						//If there is a request from GSCH
						if (str_obdh_hk.us_beacon_flag & BEACON_FLAG_GSCH_SEND_ACK){
							//Reset ACK flag
							str_obdh_hk.us_beacon_flag &= ~BEACON_FLAG_GSCH_SEND_ACK;
	
							//Set GSCH complete flag
							str_obdh_hk.us_beacon_flag |= BEACON_FLAG_GSCH_SEND_COMPLETED;
						}
					}
				}
				//If not completed, maybe needs to see if there is a need for a timeout
				//If there is a transmission which is too long, try to do the recovery attempt first
				//If recovery attempt is not successful, terminates the monitoring, marks it as erroneous
				else if (g_us_beacon_monitoring_time > (2*g_us_expected_no_of_itn_checking_per_transmission)){					
					//If recovery attempt has never been done
					if (!(str_obdh_hk.us_beacon_flag & BEACON_FLAG_RECOVERY)){
						//Set recovery flag
						str_obdh_hk.us_beacon_flag |= BEACON_FLAG_RECOVERY;

						//Recovery attempt
						l_us_error_code[0] = overlyLongTransmissionRecovery();

						//If recovery attempt is unsuccessful, set the beacon as erroneous
						if (l_us_error_code[0]){

							#if (EVENT_HANDLER_ENABLE == 1)
							#if (STORING_ENABLE == 1)
							eventHandler(EV_BEACON_HANDLER+EC_BEACON_FLAG_ERROR,str_obdh_hk.us_beacon_flag>>8,str_obdh_hk.us_beacon_flag,
								g_us_beacon_monitoring_time>>8,g_us_beacon_monitoring_time,g_uc_beacon_retry_counter,(unsigned char)l_us_error_code[0]);//dec22_yc track prior to error only as trsistion too fast
							#endif
							#endif
					
							//Reset the current monitoring value
							l_uc_itn_low_monitoring_counter = 0;
		
							//Reset retry counter
							g_uc_beacon_retry_counter = 0;
		
							//Reset monitoring time
							g_us_beacon_monitoring_time = 0;
		
							//Set beacon to ERROR
							str_obdh_hk.us_beacon_flag &= ~BEACON_FLAG_ERROR_SET_FILTER;
							str_obdh_hk.us_beacon_flag |= BEACON_FLAG_ERROR;
						}
						//Else, if recovery attempt is successful, delay the checking for at most 6 seconds
						if (g_us_beacon_monitoring_time > (6*NO_OF_BEACON_CHECK_PER_S))
							g_us_beacon_monitoring_time -= 6*NO_OF_BEACON_CHECK_PER_S;
						else g_us_beacon_monitoring_time = 0;
					}
					//If the recovery attempt has been done and it is still unsuccessful,
					//set the beacon as erroneous
					else {

						#if (EVENT_HANDLER_ENABLE == 1)
						#if (STORING_ENABLE == 1)
						eventHandler(EV_BEACON_HANDLER+EC_BEACON_FLAG_ERROR,str_obdh_hk.us_beacon_flag>>8,str_obdh_hk.us_beacon_flag,
							g_us_beacon_monitoring_time>>8,g_us_beacon_monitoring_time,g_uc_beacon_retry_counter,0);//dec24_yc track prior to error only as trsistion too fast														
						#endif
						#endif

						//Reset the current monitoring value
						l_uc_itn_low_monitoring_counter = 0;
	
						//Reset retry counter
						g_uc_beacon_retry_counter = 0;
	
						//Reset monitoring time
						g_us_beacon_monitoring_time = 0;
	
						//Set beacon to ERROR
						str_obdh_hk.us_beacon_flag &= ~BEACON_FLAG_ERROR_SET_FILTER;
						str_obdh_hk.us_beacon_flag |= BEACON_FLAG_ERROR;
					}
				}
				//If the current level is high and it is not a timeout
				else 
					//Reset current monitor counter
					l_uc_itn_low_monitoring_counter = 0;
			}
	
			//Else, increase retry counter
			else g_uc_beacon_retry_counter++;
		}

		//If currently it is not monitoring a sending or previous monitoring has just been completed
		//and there is something to send due to GSCH
		if ((str_obdh_hk.us_beacon_flag & BEACON_FLAG_REQUEST_FILTER) == BEACON_FLAG_REQUEST_CONDITION){

			#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)

			//Check its GSCH sending type, whether it is CW sending
			if (str_obdh_hk.us_beacon_flag & BEACON_FLAG_GSCH_TYPE){
				//Send CW beacon
				g_us_beacon_send_error_code = isisI2CMsg(ITC_I2C_SND_CW_MSG,0,g_uc_cw_message_length,0);
			}

			//Or it is AX.25 frame sending
			else
				//Send whatever data that has been initialized
				g_us_beacon_send_error_code = sendData();
		
			//Check error code
			if (!g_us_beacon_send_error_code){
				//If there is no error code, considers the sending as successful
				//Reset retry counter
				g_uc_beacon_retry_counter = 0;

				//Reset GSCH send request flag
				str_obdh_hk.us_beacon_flag &= ~BEACON_FLAG_GSCH_SEND_REQUEST;

				//Set monitoring flag
				str_obdh_hk.us_beacon_flag |= BEACON_FLAG_MONITOR;

				//Reset monitoring time
				g_us_beacon_monitoring_time = 0;

				//Set ACK flag
				str_obdh_hk.us_beacon_flag |= BEACON_FLAG_GSCH_SEND_ACK;

			}
			//Else, increase retry counter
			else g_uc_beacon_retry_counter++;
		
			#endif					
		}

		//If the beacon is currently not in use by GSCH and the timeout flag is raised and auto-beacon is enabled, send beacon
		else if ((str_obdh_hk.us_beacon_flag & BEACON_FLAG_AUTO_BEACON_FILTER) == BEACON_FLAG_AUTO_BEACON_CONDITION){

			//Check beacon type
			//If it is CW
			if (str_obdh_hk.us_beacon_flag & BEACON_FLAG_TYPE){
				//If it is CW beacon sending, check its mode to initialize certain things
				groundInitCWBeacon(str_obdh_hk.us_beacon_flag & BEACON_FLAG_CW_MODE);

				//Send CW beacon
				g_us_beacon_send_error_code = isisI2CMsg(ITC_I2C_SND_CW_MSG,0,g_uc_cw_message_length,0);

			}

			//Else, if it is AX25
			else{
				//Init AX25 beacon sending
				groundInitAX25Beacon();
			
				//Sending data as requested
				g_us_beacon_send_error_code = sendData();					
			}

			//Check error code
			if (!g_us_beacon_send_error_code){
				//If there is no error code, considers the sending as successful
				//Reset retry counter
				g_uc_beacon_retry_counter = 0;

				//Reset time since last sending value
				g_uc_beacon_time_since_last_timeout_sending	= 0;

				//Reset timeout flag
				str_obdh_hk.us_beacon_flag &= ~BEACON_FLAG_TIMEOUT;

				//Set monitoring flag
				str_obdh_hk.us_beacon_flag |= BEACON_FLAG_MONITOR;

				//Reset monitoring time
				g_us_beacon_monitoring_time = 0;
			}
			//Else, increase retry counter
			else g_uc_beacon_retry_counter++;
		}
	}

	//If for whatever reason, beacon retry counter is exceeding the limit,
	if (g_uc_beacon_retry_counter > BEACON_COMM_FAIL_LIMIT){
		
		#if (EVENT_HANDLER_ENABLE == 1)
		#if (STORING_ENABLE == 1)
		//dec22_yc tested
		eventHandler(EV_BEACON_HANDLER+EC_BEACON_FLAG_ERROR,str_obdh_hk.us_beacon_flag>>8,str_obdh_hk.us_beacon_flag,
								g_us_beacon_monitoring_time>>8,g_us_beacon_monitoring_time,g_uc_beacon_retry_counter,0);//dec22_yc track prior to error only as trsistion too fast				
		#endif
		#endif

		//Reset retry counter
		g_uc_beacon_retry_counter = 0;

		//Reset monitoring time
		g_us_beacon_monitoring_time = 0;

		//Reset the current monitoring value
		l_uc_itn_low_monitoring_counter = 0;

		//Set beacon to ERROR
		str_obdh_hk.us_beacon_flag &= ~BEACON_FLAG_ERROR_SET_FILTER;
		str_obdh_hk.us_beacon_flag |= BEACON_FLAG_ERROR;
	}	
}
#endif
#endif
#endif

#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)
//------------------------------------------------------------------------------------
// To handle ground station command event
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
//This funciton is noticed to have over 1600 lines, probably the most amusing function on VELOX-P code.
void groundStationCommandHandler (void)
{
	#if (ISIS_CMD_ENABLE == 1)

	unsigned short l_us_beacon_status;
	unsigned short l_us_reply_1;
	unsigned short l_us_reply_2;
	unsigned short l_us_data_id;
	unsigned short l_us_init_send_error_code = EC_INIT;
	unsigned short l_us_i;
	unsigned short l_us_comm_error_code = EC_INIT;

	#if (ADCS_CMD_ENABLE == 1)
	unsigned short l_us_dummy;
	#endif

	//Check current GSCH state
	switch (str_obdh_hk.uc_gsch_state){
		//When GSCH is idle
		case GSCH_STATE_IDLE:
			//Listen to GDSN command
			groundCommandListener();

			//If it has passed initial GSCH checking and the initial checking request for further checking
			//Then do whatever necessary
			if (g_uc_gsch_flag & GSCH_FLAG_VALID_COMMAND){
				//Reset GSCH checking flag
				g_uc_gsch_flag &= ~GSCH_FLAG_CHECK_RESET;

				//Starts to claim the beacon here to be used by GSCH (don't send anything automatically anymore)
				str_obdh_hk.us_beacon_flag |= BEACON_FLAG_GSCH_CLAIM;

				//Go to the GSCH_STATE_GENERATING_KEY state
				str_obdh_hk.uc_gsch_state = GSCH_STATE_GENERATING_KEY;
			}

			break;

		//When GSCH is generating key
		case GSCH_STATE_GENERATING_KEY:
			//Call groundPassRequestKeyGenerator
			groundPassRequestKeyGenerator();
	
			//Go to the GSCH_STATE_SEND_GENERATED_KEY state
			str_obdh_hk.uc_gsch_state = GSCH_STATE_SEND_GENERATED_KEY;

			//Set transitional period flag
			g_uc_gsch_flag |= GSCH_FLAG_MONITOR_TRANSITIONAL;

			#if (EVENT_HANDLER_ENABLE == 1)
			#if (STORING_ENABLE == 1)
			//dec22_yc .... track valid ground station cmd and processing status.  tested dec22_yc
			eventHandler(EV_GSCH_MONITORING_TRANSITIONAL_FLAG_HANDLER,str_obdh_hk.uc_gsch_state,g_uc_gsch_gs_cmd_tail,
								g_uc_gsch_gs_arg[0],g_uc_gsch_gs_arg[1],g_us_gsch_gs_cmd_crc>>16,g_us_gsch_gs_cmd_crc);
			#endif
			#endif

			break;

		//When GSCH is sending generated key
		case GSCH_STATE_SEND_GENERATED_KEY:
			//For all transition state case, it is important to get latest beacon status
			//Only proceeds when beacon is functional
			if ((str_obdh_hk.us_beacon_flag & BEACON_FLAG_FILTER_PRIMARY) == BEACON_FUNCTIONAL){
				//If it is functional, check its case
				l_us_beacon_status = str_obdh_hk.us_beacon_flag & BEACON_FLAG_FILTER_SECONDARY;

				//Switch the case for beacon status
				switch (l_us_beacon_status){
					//There are only four valid cases
					//First, beacon is free, initiates transmission here
					case BEACON_FREE:
						#if (SENDING_ENABLE == 1)
						#if (ISIS_CMD_ENABLE == 1)
						sendKeyRequest();
						#endif
						#endif
						break;
	
					//Second, beacon is busy of other sending task, do nothing
					//Third, beacon is currently sending your request, do nothing too
					//If the sending has gone too long, however, terminates the requets
					//Go back to idle state
					case BEACON_BUSY_1:
					case BEACON_BUSY_2:
					case BEACON_SENDING:
					case BEACON_WAITING:
						break;
	
					//Fourth, beacon sending is completed					
					case BEACON_SEND_COMPLETED:
						#if (SENDING_ENABLE == 1)
						//Release beacon's claim
						groundReleaseClaimBeacon();
						#endif

						//Go to the next state, waiting for key reply
						str_obdh_hk.uc_gsch_state = GSCH_STATE_WAITING_FOR_KEY_REPLY;
						break;
	
					//Besides the above cases, something wrong with the beacon, terminates the sending
					default:
						//Terminates transitional states
						groundTerminateTransitionalState();
		
						//Go back to idle state
						str_obdh_hk.uc_gsch_state = GSCH_STATE_IDLE;
						break;
				}
			}
			//If beacon is not found to be functional at this stage, return to idle
			else {
				//Terminates transitional states
				groundTerminateTransitionalState();

				//Go back to idle state
				str_obdh_hk.uc_gsch_state = GSCH_STATE_IDLE;
			}

			//If at any point timeout for monitoring occurs
			if (g_us_gsch_monitoring_transitional_period > TIME_LIMIT_GSCH_TRANSITIONAL_STATE_S){
				//Terminates transitional states
				groundTerminateTransitionalState();

				//Go back to idle state
				str_obdh_hk.uc_gsch_state = GSCH_STATE_IDLE;
			} 
			break;

		//When GSCH is waiting for key reply
		case GSCH_STATE_WAITING_FOR_KEY_REPLY:
			//Listen to GDSN command
			groundCommandListener();

			//If it has passed initial GSCH checking and the initial checking request for further checking
			//Then do whatever necessary
			if (g_uc_gsch_flag & GSCH_FLAG_VALID_COMMAND){
				//Reset GSCH checking flag
				g_uc_gsch_flag &= ~GSCH_FLAG_CHECK_RESET;

				//Check what command is sent
				//Check the command content
				switch (g_uc_gsch_gs_cmd_tail){
					case GDSN_REQUEST_GDPASS:
						//Reset transitional state monitoring period
						g_us_gsch_monitoring_transitional_period = 0;

						//Starts to claim the beacon here to be used by GSCH (don't send anything automatically anymore)
						str_obdh_hk.us_beacon_flag |= BEACON_FLAG_GSCH_CLAIM;
		
						//Go to the GSCH_STATE_GENERATING_KEY state
						str_obdh_hk.uc_gsch_state = GSCH_STATE_GENERATING_KEY;
						break;

					case GDSN_SEND_GDPASS_CODE:
						//Reset transitional state monitoring period
						g_us_gsch_monitoring_transitional_period = 0;

						//Check the key reply
						l_us_reply_1 = (unsigned short)(g_uc_gsch_gs_arg[0] << 8) + g_uc_gsch_gs_arg[1];
						l_us_reply_2 = (unsigned short)(g_uc_gsch_gs_arg[2] << 8) + g_uc_gsch_gs_arg[3];

						//If the key reply is correct, enters ground pass state
						//Else, keeps waiting in this state
						if ((g_us_gsch_answer_1 == l_us_reply_1) && (g_us_gsch_answer_2 == l_us_reply_2)){
							//Enter Groundpass
							enterGroundPass();

							//Always successful
							g_us_gsch_error_code = EC_SUCCESSFUL;

							#if (SENDING_ENABLE == 1)
							//Send ACK MSG
							sendAckMsg();
							#endif
						}

						break;

					//Invalid command will be ignored, but monitoring period will not be reset
					default:
						break;
				}
			}

			//If at any point timeout for monitoring occurs
			if (g_us_gsch_monitoring_transitional_period > TIME_LIMIT_GSCH_TRANSITIONAL_STATE_S){
				//Terminates transitional states
				groundTerminateTransitionalState();

				//Go back to idle state
				str_obdh_hk.uc_gsch_state = GSCH_STATE_IDLE;
			} 
			break;

		//When GSCH is in the groundpass mode
		case GSCH_STATE_GROUND_PASS:
			//In the ground pass state, all sending and loading can be reinitialized
			g_uc_strsnd_flag &= ~SNDNG_FLAG_COMPLETED;
			g_uc_strsnd_flag &= ~STRNG_FLAG_SD_LOAD_COMPLETED;
			g_uc_strsnd_flag &= ~STRNG_FLAG_SAVE_DATA_HOLD;
			g_uc_sd_load_data_timeout_counter = TIME_LIMIT_LOAD_DATA_S;
			g_uc_save_data_hold_timeout_counter = 0;
			g_uc_gsch_flag &= ~GSCH_FLAG_TOTAL_SENDING;
			g_us_gsch_total_sending_time = 0;

			//If it is within the time limit, stays in this state
			if (g_us_groundpass_time_register < TIME_LIMIT_GROUND_PASS_S){
				//In this groundpass mode, commands will be received directly from the ground station.
				//For every command received, GSCH will first check its validity
				//If the command is valid, only two possible types of command will be recognized here:
				// 1) If this is command to be run by the command control module (0x0000-0x02FF && 0x0400-0xD0FF)
				// 2) If this command is special ground station command
				//
				//For the type (1) of the command, the only restriction here is if the argument length is more than 10 bytes
				// then, the command will not be executed
				//
				//For the type (2) of the command, various scenario is possible. Most difficult one will be when GDSN
				// wants to get some data sending down, then difficulties come...

				//If there is sufficient power, proceeds, else, terminates Ground Pass
				//Go to idle state
				if (str_obdh_data.uc_batt_soc >= str_obdh_par.uc_soc_threshold[3]){
					//Listen to GDSN command
					groundCommandListener();
		
					//If it has passed initial GSCH checking and the initial checking request for further checking
					//Then do whatever necessary
					if (g_uc_gsch_flag & GSCH_FLAG_VALID_COMMAND){
						//Reset GSCH checking flag
						g_uc_gsch_flag &= ~GSCH_FLAG_CHECK_RESET;

						//Reset g_us_groundpass_time_register upon receiving successful command
						g_us_groundpass_time_register = TIME_RESET_GROUND_PASS_S;
		
						//Here classification of the commands occur
						//First, if the command is to be run by command control module,
						//It will be straight forward
						//Only available when manual mode is turned ON
						if (((g_uc_gsch_gs_cmd_header >= 0x00 && g_uc_gsch_gs_cmd_header <= 0x02) || (g_uc_gsch_gs_cmd_header >= 0x04 && g_uc_gsch_gs_cmd_header <= 0xD0)) && (str_obdh_par.uc_manual_control_enabled == 0xff)){
							#if (CQUEUE_ENABLE == 1)
							//Insert whatever command given (unfiltered) to the command queue
							insertGSCHCommandToQueue();

							#if (SENDING_ENABLE == 1)
							#if (ISIS_CMD_ENABLE == 1)
							//Always successful
							g_us_gsch_error_code = EC_SUCCESSFUL;

							//Send acknowledgement message
							sendAckMsg();
							#endif
							#endif
							#else
							//Insert the commands to the call task buffer
							g_uc_call_task_buffer[0] = 0xBB;
							g_uc_call_task_buffer[1] = g_uc_gsch_gs_cmd_header;
							g_uc_call_task_buffer[2] = g_uc_gsch_gs_cmd_tail;
							g_uc_call_task_buffer[3] = 0x00;
							g_uc_call_task_buffer[4] = g_uc_gsch_gs_arg_len;
							
							//Insert command's argument (if any)
							for (l_us_i = 0; l_us_i < g_uc_gsch_gs_arg_len; l_us_i++)
								g_uc_call_task_buffer[l_us_i+COMMAND_HEADER_SIZE+1] = g_uc_gsch_gs_arg[l_us_i];

							#if (EVENT_HANDLER_ENABLE == 1)
							#if (STORING_ENABLE == 1)
							//dec22_yc here add even logging for command added to queue tested			
							eventHandler(EV_MCC_TO_COMMAND,g_uc_gsch_gs_cmd_header,g_uc_gsch_gs_cmd_tail,
									g_uc_call_task_buffer[COMMAND_HEADER_SIZE+1],g_uc_call_task_buffer[COMMAND_HEADER_SIZE+2],g_uc_call_task_buffer[COMMAND_HEADER_SIZE+3],g_uc_call_task_buffer[COMMAND_HEADER_SIZE+4]);
							#endif
							#endif
						
							//Runs the command directly by calling functionModule
							functionModule (g_uc_call_task_buffer, g_uc_call_task_buffer);

							#if (SENDING_ENABLE == 1)
							#if (ISIS_CMD_ENABLE == 1)
							//Get function module error code
							g_us_gsch_error_code = g_us_function_module_error_code;

							//Send acknowledgement message
							sendAckMsg();
							#endif
							#endif
							#endif
						}

						//Else, if the command is not to be run by command control module, but by GSCH
						//Then more complex task there will be...
						else if (g_uc_gsch_gs_cmd_header == GDSN_ADDR){
							//If the command is special GDSN command, we have a switch case here...
							//The special GDSN command will mainly deal with sending command
							//Because, if it is not sending, there is no point to access this command
							//For any valid command, there will be acknowledgement sent by the satellite (not yet)
							switch (g_uc_gsch_gs_cmd_tail){
								#if (SENDING_ENABLE == 1)
								#if (ISIS_CMD_ENABLE == 1)
								//Here, all possible cases will MCC-OBC communication will be considered
								//GDSN_REQUEST_GDPASS and GDSN_SEND_GDPASS_CODE will be ignored
								//Send current HK in the temporary buffer
								case GDSN_GET_ALL_HK:
									//Send real time HK
									sendCommonData(SD_HK);
									break;

								//Send current SSCP in the temporary buffer
								case GDSN_GET_ALL_SSCP:
									//Send real time SSCP
									sendCommonData(SD_SSCP);
									break;

								//Get whatever data sets under specified category
								case GDSN_GET_DATA_SET:
									//Always successful
									g_us_gsch_error_code = EC_SUCCESSFUL;

									//Indicates taking data from temporary buffer
									g_uc_ax25_header_package_property = 0x00;

									//Taking the data initialization from the command argument
									g_uc_ax25_header_subsystem = OBDH_SUBSYSTEM;
									g_uc_ax25_header_data_type = SD_FREE_CATEGORY;

									//Initialize sending data under free category
									g_uc_no_of_data_sent = (str_obdh_data.uc_no_of_data_cat_sent+1);

									//Just another error prevention
									if (g_uc_no_of_data_sent > MAX_DATA_CAT_TO_BE_SENT_FREELY)
										g_uc_no_of_data_sent = MAX_DATA_CAT_TO_BE_SENT_FREELY;

									//The first data set sent is always ACK_MSG
									g_uc_data_type_sent_buffer[0] = DATA_CAT_ACK_MSG;

									//Fill whatever category requested (theoretically, maximum of nine)
									for (l_us_i = 0; l_us_i < g_uc_no_of_data_sent; l_us_i++)
										g_uc_data_type_sent_buffer[l_us_i+1] = g_uc_gsch_gs_arg[l_us_i];

									//Initialized sending data (once only)
									l_us_init_send_error_code = initSendData();

									//If there is unaccepted initialization error, do not proceed (re-take new command)
									//Else, proceeds to GSCH_STATE_SEND_REAL_TIME_DATA state
									if(!l_us_init_send_error_code || l_us_init_send_error_code == EC_INVALID_SENDING_DATA_REQUEST)
										//Send real-time data
										groundEnterSendRealTime();									
									break;

								//Get whatever data under GDSN request
								//Here, we execute directly command to get data
								case GDSN_GET_DATA:
									//Get the data ID requested
									l_us_data_id = ((unsigned short)g_uc_gsch_gs_arg[0] << 8) + g_uc_gsch_gs_arg[1];

									//See whether it is PWRS or ADCS data requested
									//If it is ADCS data
									if (g_uc_gsch_gs_arg[0] == ADCS_ADDR){
										#if (ADCS_CMD_ENABLE == 1)
										#if (DEBUGGING_FEATURE == 1)
										g_us_gsch_error_code = adcsGetData(g_uc_gsch_gs_arg[1],0);
										#else
										g_us_gsch_error_code = adcsGetData(g_uc_gsch_gs_arg[1]);
										#endif
										#else
										g_us_gsch_error_code = EC_ADCS_CMD_DISABLED;
										#endif
									}

									//If it is PWRS data
									else if (g_uc_gsch_gs_arg[0] == PWRS_ADDR){
										#if (PWRS_CMD_ENABLE == 1)
										#if (DEBUGGING_FEATURE == 1)
										g_us_gsch_error_code = pwrsGetData(g_uc_gsch_gs_arg[1],0);
										#else
										g_us_gsch_error_code = pwrsGetData(g_uc_gsch_gs_arg[1]);
										#endif
										#else
										g_us_gsch_error_code = EC_PWRS_CMD_DISABLED;
										#endif
									}

									//If it is neither
									else g_us_gsch_error_code = EC_INVALID_GDSN_GET_DATA_REQUEST;

									//Send real time single data
									sendCommonData(SD_SINGLE_SUBSYSTEM_DATA);
									break;
								#endif
								#endif

								//Set an SSCP data of a subsystem
								case GDSN_SET_SSCP:
									//Time window validation check
									timeWindowCheck();

									//Only proccess the command if it passes time window validation
									if (g_uc_gsch_flag & GSCH_FLAG_TIME_WINDOW_VALID){
										//Get the data ID requested
										l_us_data_id = ((unsigned short)g_uc_gsch_gs_arg[0] << 8) + g_uc_gsch_gs_arg[1];
	
										//See whether it is OBDH, PWRS, or ADCS request
										//If it is OBDH request
										if (g_uc_gsch_gs_arg[0] == OBDH_ADDR)
											//Change whatever OBDH's SSCP desired to be changed
											changeData(l_us_data_id,g_uc_gsch_gs_arg[2],g_uc_gsch_gs_arg[3],g_uc_gsch_gs_arg[4],g_uc_gsch_gs_arg[5]);
	
										//If it is ADCS request
										else if (g_uc_gsch_gs_arg[0] == ADCS_ADDR){
											#if (ADCS_CMD_ENABLE == 1)
											//Check the data ID
											switch (l_us_data_id){
												//To set DATA_ADCS_PAR_S_SS_THRESHOLD
												case DATA_ADCS_PAR_S_SS_THRESHOLD:
													l_us_dummy = (unsigned short)(g_uc_gsch_gs_arg[4] << 8) + g_uc_gsch_gs_arg[5];
													#if (DEBUGGING_FEATURE == 1)
													g_us_gsch_error_code = adcsSetSSThreshold(l_us_dummy,0);
													#else
													g_us_gsch_error_code = adcsSetSSThreshold(l_us_dummy);
													#endif
													break;
												
												//To set DATA_ADCS_PAR_F_CGAIN_1 to DATA_ADCS_PAR_F_CGAIN_18,
												//DATA_ADCS_PAR_F_TLE_EPOCH, DATA_ADCS_PAR_F_TLE_INC,
												//DATA_ADCS_PAR_F_TLE_RAAN, DATA_ADCS_PAR_F_TLE_ARG_PER,
												//DATA_ADCS_PAR_F_TLE_ECC, DATA_ADCS_PAR_F_TLE_MA, DATA_ADCS_PAR_US_TLE_MM 
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
												case DATA_ADCS_PAR_F_TLE_EPOCH:
												case DATA_ADCS_PAR_F_TLE_INC:
												case DATA_ADCS_PAR_F_TLE_RAAN:
												case DATA_ADCS_PAR_F_TLE_ARG_PER:
												case DATA_ADCS_PAR_F_TLE_ECC:
												case DATA_ADCS_PAR_F_TLE_MA:
													changeData(l_us_data_id,g_uc_gsch_gs_arg[2],g_uc_gsch_gs_arg[3],g_uc_gsch_gs_arg[4],g_uc_gsch_gs_arg[5]);
													g_us_gsch_error_code = EC_SUCCESSFUL;
													break;
												
												case DATA_ADCS_PAR_US_TLE_MM:
													changeData(l_us_data_id,g_uc_gsch_gs_arg[2],g_uc_gsch_gs_arg[3],g_uc_gsch_gs_arg[4],g_uc_gsch_gs_arg[5]);
													g_f = (unsigned short)(g_uc_gsch_gs_arg[4] << 8) + g_uc_gsch_gs_arg[5];
													g_f = (g_f/26214.0) + 13.7;
													str_obdh_par.us_rev_time_s = NO_OF_SECOND_PER_DAY/g_f;
													g_us_gsch_error_code = EC_SUCCESSFUL;
													break;
												
												//To set DATA_ADCS_PAR_UL_TIME
												case DATA_ADCS_PAR_UL_TIME:
													#if (DEBUGGING_FEATURE == 1)
													g_us_gsch_error_code = adcsSetTime(uchar2ulong(g_uc_gsch_gs_arg[2],g_uc_gsch_gs_arg[3],g_uc_gsch_gs_arg[4],g_uc_gsch_gs_arg[5]),0);
													#else
													g_us_gsch_error_code = adcsSetTime(uchar2ulong(g_uc_gsch_gs_arg[2],g_uc_gsch_gs_arg[3],g_uc_gsch_gs_arg[4],g_uc_gsch_gs_arg[5]));
													#endif
													break;
												
												//To set DATA_ADCS_PAR_S_VECTOR_2SUN_X, DATA_ADCS_PAR_S_VECTOR_2SUN_Y
												//DATA_ADCS_PAR_S_VECTOR_2SUN_Z
												case DATA_ADCS_PAR_S_VECTOR_2SUN_X:
												case DATA_ADCS_PAR_S_VECTOR_2SUN_Y:
												case DATA_ADCS_PAR_S_VECTOR_2SUN_Z:
													changeData(l_us_data_id,0,0,g_uc_gsch_gs_arg[4],g_uc_gsch_gs_arg[5]);
													#if (DEBUGGING_FEATURE == 1)
													g_us_gsch_error_code = adcsSetVectorAlignToSun(0);
													#else
													g_us_gsch_error_code = adcsSetVectorAlignToSun();
													#endif
													break;
												
												
												//To set DATA_ADCS_PAR_C_CTRL_BR_X, DATA_ADCS_PAR_C_CTRL_BR_Y
												//DATA_ADCS_PAR_C_CTRL_BR_Z
												case DATA_ADCS_PAR_C_CTRL_BR_X:
												case DATA_ADCS_PAR_C_CTRL_BR_Y:
												case DATA_ADCS_PAR_C_CTRL_BR_Z:
													changeData(l_us_data_id,0,0,0,g_uc_gsch_gs_arg[5]);
													#if (DEBUGGING_FEATURE == 1)
													g_us_gsch_error_code = adcsSetBodyRate(str_adcs_par.c_ctrl_br[0],str_adcs_par.c_ctrl_br[1],str_adcs_par.c_ctrl_br[2],0);
													#else
													g_us_gsch_error_code = adcsSetBodyRate(str_adcs_par.c_ctrl_br[0],str_adcs_par.c_ctrl_br[1],str_adcs_par.c_ctrl_br[2]);
													#endif
													break;
												
												//To set DATA_ADCS_PAR_S_DGAIN
												case DATA_ADCS_PAR_S_DGAIN:
													str_adcs_par.s_dgain = (unsigned short)(g_uc_gsch_gs_arg[4] << 8) + g_uc_gsch_gs_arg[5];
													#if (DEBUGGING_FEATURE == 1)
													g_us_gsch_error_code = adcsSetDGain(str_adcs_par.s_dgain,0);
													#else
													g_us_gsch_error_code = adcsSetDGain(str_adcs_par.s_dgain);
													#endif
													break;
												
												//If it is not in the SSCP list, return error code
												default:
													g_us_gsch_error_code = EC_INVALID_GDSN_SET_ADCS_SSCP_REQUEST;
													break;											
											}
											#else
											g_us_gsch_error_code = EC_ADCS_CMD_DISABLED;
											#endif
										}
	
										//If it is PWRS request
										else if (g_uc_gsch_gs_arg[0] == PWRS_ADDR){
											#if (PWRS_CMD_ENABLE == 1)
											//Check the data ID
											switch (l_us_data_id){
	
												//To set DATA_PWRS_PAR_UC_MPPT_MODE
												case DATA_PWRS_PAR_UC_MPPT_MODE:
													#if (DEBUGGING_FEATURE == 1)
													g_us_gsch_error_code = pwrsSetMPPTMode(g_uc_gsch_gs_arg[5],0);
													#else
													g_us_gsch_error_code = pwrsSetMPPTMode(g_uc_gsch_gs_arg[5]);
													#endif
													break;
												
												//To set DATA_PWRS_PAR_UC_BATT_HEATER
												case DATA_PWRS_PAR_UC_BATT_HEATER:
													#if (DEBUGGING_FEATURE == 1)
													g_us_gsch_error_code = pwrsUpdateBatteryHeater(g_uc_gsch_gs_arg[5],0);
													#else
													g_us_gsch_error_code = pwrsUpdateBatteryHeater(g_uc_gsch_gs_arg[5]);
													#endif
													break;
												
												//To set DATA_PWRS_PAR_UC_BATT_CHARGE_UPP_LIMIT and DATA_PWRS_PAR_UC_BATT_CHARGE_LOW_LIMIT
												case DATA_PWRS_PAR_UC_BATT_CHARGE_UPP_LIMIT:
												case DATA_PWRS_PAR_UC_BATT_CHARGE_LOW_LIMIT:
													changeData(l_us_data_id,0,0,0,g_uc_gsch_gs_arg[5]);
													#if (DEBUGGING_FEATURE == 1)
													g_us_gsch_error_code = pwrsSetSOCLimit(str_pwrs_par.uc_batt_charge_upp_limit,str_pwrs_par.uc_batt_charge_low_limit,0);
													#else
													g_us_gsch_error_code = pwrsSetSOCLimit(str_pwrs_par.uc_batt_charge_upp_limit,str_pwrs_par.uc_batt_charge_low_limit);
													#endif
													break;

												//To set DATA_PWRS_US_PV_FV_1, DATA_PWRS_US_PV_FV_2, DATA_PWRS_US_PV_FV_3
												case DATA_PWRS_US_PV_FV_1:
												case DATA_PWRS_US_PV_FV_2:
												case DATA_PWRS_US_PV_FV_3:
													changeData(l_us_data_id,0,0,g_uc_gsch_gs_arg[4],g_uc_gsch_gs_arg[5]);
													#if (DEBUGGING_FEATURE == 1)
													g_us_gsch_error_code = pwrsSetPVVolt(0);
													#else
													g_us_gsch_error_code = pwrsSetPVVolt();
													#endif
													break;											
																							
												//If it is not in the SSCP list, return error code
												default:
													g_us_gsch_error_code = EC_INVALID_GDSN_SET_PWRS_SSCP_REQUEST;
													break;											
											}
											#else
											g_us_gsch_error_code = EC_PWRS_CMD_DISABLED;
											#endif
										}
	
										//If it is not among the possibilities
										else g_us_gsch_error_code = EC_INVALID_GDSN_SET_SSCP_REQUEST;
	
										#if (SENDING_ENABLE == 1)
										//Save essential state and send acknowledgement
										groundSaveEssentialAndAck();
										#endif
									}
										
									break;

								#if (SENDING_ENABLE == 1)
								#if (ISIS_CMD_ENABLE == 1)
								//Get WOD data based on orbit
								case GDSN_GET_WOD:
									//Search proper block pointer based on the orbit requested
									g_us_obs_first_orbit_request 	= (unsigned short)(g_uc_gsch_gs_arg[0] << 8) + g_uc_gsch_gs_arg[1];
									g_us_obs_last_orbit_request 	= g_us_obs_first_orbit_request;
									#if (STORING_ENABLE == 1)
									g_us_gsch_error_code 			= searchBPByOrbitInfo(SD_HK);
									#else
									g_us_gsch_error_code			= EC_STORING_FUNCTIONS_ARE_DISABLED;
									#endif

									//If there is initialization error, do not proceed
									//Else, proceeds to GSCH_STATE_SEND_STORED_DATA state
									if(!g_us_gsch_error_code){
										//Indicates taking data from SD card
										g_uc_ax25_header_package_property = AX25_HEADER_PROPERTY_STORED_DATA_BIT;

										//Set current package no to 1
										g_uc_ax25_header_no_of_this_package = 1;

										//Initialize subsystem and data type to load
										g_uc_gsch_subsystem_to_load = ALL_SUBSYSTEMS;
										g_uc_gsch_data_type_to_load = SD_HK;
	
										//Set the downlink rate as assigned
										g_uc_gsch_sampling_step = g_uc_gsch_gs_arg[2];

										//Check the limits
										if (g_uc_gsch_sampling_step < WOD_SAMPLE_STEP_LOWER_LIMIT) 
											g_uc_gsch_sampling_step = WOD_SAMPLE_STEP_LOWER_LIMIT;
										else if (g_uc_gsch_sampling_step > WOD_SAMPLE_STEP_UPPER_LIMIT) 
											g_uc_gsch_sampling_step = WOD_SAMPLE_STEP_UPPER_LIMIT;

										//Send stored data
										groundEnterSendStored();
									}
									else sendAckMsg();
									break;
								#endif
								#endif

								#if (SATELLITE_SCHEDULE_HANDLER_ENABLE == 1)
								//To start logging data based on GDSN request
								case GDSN_LOG_DATA:
									//Time window validation check
									timeWindowCheck();

									//Only process the command if it passes time window validation
									if (g_uc_gsch_flag & GSCH_FLAG_TIME_WINDOW_VALID){
										//Check if there is ADCS data in the log data type
										if (checkADCSDataInLogDataType()){
											//Check if there are at least 4 slots in scheduler
											//If there is sufficient slot(s) in the scheduler
											if (str_obdh_hk.uc_scheduler_block_size <= (SCHEDULER_BLOCK_SIZE-4)){ 
												//Schedule mission mode at requested time -20s with elapsed time as requested +30s
												g_us_gsch_error_code = scheduleMissionMode((uchar2ulong(g_uc_gsch_gs_arg[0],g_uc_gsch_gs_arg[1],g_uc_gsch_gs_arg[2],g_uc_gsch_gs_arg[3])-SECONDS_FROM_TAI_TO_KILL_SWITCH_DEACTIVATION-20),(unsigned short)((unsigned short)(g_uc_gsch_gs_arg[4] << 8)+g_uc_gsch_gs_arg[5]+30));
	
												//If there is no error code, it means that the scheduled time has not passed.
												if (!g_us_gsch_error_code){
													//Schedule to turn ON ADCS first
													g_us_gsch_error_code = scheduleTurnOnADCS(uchar2ulong(g_uc_gsch_gs_arg[0],g_uc_gsch_gs_arg[1],g_uc_gsch_gs_arg[2],g_uc_gsch_gs_arg[3])-SECONDS_FROM_TAI_TO_KILL_SWITCH_DEACTIVATION-20);
	
													//Insert schedule to log data
													scheduleLogData();
												}
											}
											//If there isn't sufficient slot(s) in the scheduler 
											else g_us_gsch_error_code = EC_NOT_ENOUGH_SCHEDULE_SLOT;									
										}
										//Else, we need only 2 slots for PWRS data logging
										else{
											//If there is sufficient slot(s) in the scheduler
											if (str_obdh_hk.uc_scheduler_block_size <= (SCHEDULER_BLOCK_SIZE-2)){
												//Schedule mission mode at requested time -20s with elapsed time as requested +30s
												g_us_gsch_error_code = scheduleMissionMode((uchar2ulong(g_uc_gsch_gs_arg[0],g_uc_gsch_gs_arg[1],g_uc_gsch_gs_arg[2],g_uc_gsch_gs_arg[3])-SECONDS_FROM_TAI_TO_KILL_SWITCH_DEACTIVATION-20),(unsigned short)((unsigned short)(g_uc_gsch_gs_arg[4] << 8)+g_uc_gsch_gs_arg[5]+30));
	
												//If there is no error code, it means that the scheduled time has not passed.
												if (!g_us_gsch_error_code) scheduleLogData();
											}
											//If there isn't sufficient slot(s) in the scheduler
											else g_us_gsch_error_code = EC_NOT_ENOUGH_SCHEDULE_SLOT;									
										}

										#if (SENDING_ENABLE == 1)
										//Save latest schedule and send acknowledgement message
										groundSaveScheduleAndAck();
										#endif
									}									
									break;
								#endif

								#if (SENDING_ENABLE == 1)
								#if (ISIS_CMD_ENABLE == 1)
								//To send down special data requested
								case GDSN_GET_DATA_SPEC:
									//Check if there is any data logged
									if (g_us_log_data_block_size){
										//Always successful
										g_us_gsch_error_code = EC_SUCCESSFUL;

										//Set initial and end block pointer for the wanted data
										g_ul_obs_initial_bp = SD_BP_DATALOG_START;
										g_ul_obs_end_bp = str_bp.ul_datalog_write_p-(1+NO_OF_COPIES_SD_LOG_DATA);
	
										//Logical check (it is not possible to have overwritten block here)
										if (g_ul_obs_end_bp < g_ul_obs_initial_bp)
											g_ul_obs_end_bp = g_ul_obs_initial_bp;
	
										//Indicates taking data from SD card
										g_uc_ax25_header_package_property = AX25_HEADER_PROPERTY_STORED_DATA_BIT;
	
										//Set current package no to 1
										g_uc_ax25_header_no_of_this_package = 1;
	
										//Initialize subsystem and data type to load
										g_uc_gsch_subsystem_to_load = ALL_SUBSYSTEMS;
										g_uc_gsch_data_type_to_load = SD_LOG_DATA;
	
										//Set the downlink rate as assigned (always one for this case)
										g_uc_gsch_sampling_step = 1;

										//Send stored data
										groundEnterSendStored();
									}
									//If there is no data logged yet
									else{ 
										g_us_gsch_error_code = EC_NO_DATA_LOGGED_YET;
										sendAckMsg();
									}
										
									break;

								//To send down current satellite schedule
								case GDSN_GET_SAT_SCHEDULE:
									//Always successful
									g_us_gsch_error_code = EC_SUCCESSFUL;

									//Initialize schedule sent counter
									g_uc_schedule_sent_counter = 0;

									//Send current satellite schedule
									sendCommonData(SD_SCHEDULE);
									break;

								//To send down current satellite time
								case GDSN_GET_SAT_TIME:
									//Always successful
									g_us_gsch_error_code = EC_SUCCESSFUL;

									//Send current satellite time (int TAI format)
									sendCommonData(SD_SAT_TIME);
									break;
								#endif
								#endif

								//To set current satellite time
								case GDSN_SET_SAT_TIME:
									//Update satellite time as specified by the ground station
									groundUpdateTime(uchar2ulong(g_uc_gsch_gs_arg[0],g_uc_gsch_gs_arg[1],g_uc_gsch_gs_arg[2],g_uc_gsch_gs_arg[3]));

									//Assign error code of RTC (by right, this should always be 0x0000)
									g_us_gsch_error_code = g_us_latest_gdsn_to_rtc_update_ec;

									#if (SENDING_ENABLE == 1)
									//Save essential state and send acknowledgement
									groundSaveEssentialAndAck();
									#endif
									break;

								#if (EXTRA_ENABLE == 1)
								//To reset satellite (to be developed later)
								case GDSN_RESET_SAT:
									//Time window validation check
									timeWindowCheck();

									//Only proccess the command if it passes time window validation
									if (g_uc_gsch_flag & GSCH_FLAG_TIME_WINDOW_VALID){
										//Check if satellite rest flag has already been initialized, if it is not...
										if (!(g_uc_evha_flag & EVENT_FLAG_RESET_SAT_INITIALIZED)){	
											//Mark reset cause as RESET_SOURCE_MCC_COMMAND 
											str_obdh_data.uc_sat_rstsrc = RESET_SOURCE_MCC_COMMAND;
	
											//Trigger satellite's reset
											callReset(OBDH_CMD_RESET_SAT);
	
											#if (SENDING_ENABLE == 1)
											#if (ISIS_CMD_ENABLE == 1)
											//Return successful	message
											g_us_gsch_error_code = EC_SUCCESSFUL;
	
											//Send ACK MSG
											sendAckMsg();
											#endif
											#endif
										}
										#if (SENDING_ENABLE == 1)
										#if (ISIS_CMD_ENABLE == 1)
										//If it is...
										else{										
											//Indicates that satellite reset has already been initialized
											g_us_gsch_error_code = EC_INVALID_GDSN_RESET_SAT_ALREADY_INITIALIZED;
	
											//Send ACK MSG
											sendAckMsg();
										}
										#endif
										#endif
									}
									break;
								#endif

								//To start NOP
								case GDSN_START_NOP:
									//Time window validation check
									timeWindowCheck();

									//Only proccess the command if it passes time window validation
									if (g_uc_gsch_flag & GSCH_FLAG_TIME_WINDOW_VALID){
										//Change satellite mode to NOP
										str_obdh_hk.uc_sat_mode = SATELLITE_MODE_M6;
										str_obdh_data.uc_previous_mode = SATELLITE_MODE_M5;

										#if (BODY_RATE_HANDLER_ENABLE == 1)
										#if (ADCS_CMD_ENABLE == 1)
										#if (PWRS_CMD_ENABLE == 1)				
										//Disable bodyRateHandler
										str_obdh_hk.us_brh_flag &= ~BRH_FLAG_ENABLE;
										#endif
										#endif
										#endif
						
										#if (IDLE_MODE_HANDLER_ENABLE == 1)
										#if (PWRS_CMD_ENABLE == 1)
										//Enable idleModeHandler
										str_obdh_hk.us_imh_flag |= IMH_FLAG_ENABLE;
										#endif
										#endif

										//Update SoC thresholds
										str_obdh_par.uc_soc_threshold[0] = THRESHOLD_SOC_M5_TO_M8_DEFAULT;
										str_obdh_par.uc_soc_threshold[1] = THRESHOLD_SOC_M8_TO_M5_DEFAULT;

										//Always successful
										g_us_gsch_error_code = EC_SUCCESSFUL;

										#if (SENDING_ENABLE == 1)
										//Save essential state and send acknowledgement
										groundSaveEssentialAndAck();
										#endif
									}
									break;

								//To update ADCS TLE
								case GDSN_UPDATE_TLE:
									//Time window validation check
									timeWindowCheck();

									//Only proccess the command if it passes time window validation
									if (g_uc_gsch_flag & GSCH_FLAG_TIME_WINDOW_VALID){
										//Command ADCS to update TLE
										#if (ADCS_CMD_ENABLE == 1)
										#if (DEBUGGING_FEATURE == 1)
										g_us_gsch_error_code = adcsSetTLE(0);
										#else
										g_us_gsch_error_code = adcsSetTLE();
										#endif
										#else
										g_us_gsch_error_code = EC_ADCS_CMD_DISABLED;
										#endif
	
										#if (SENDING_ENABLE == 1)
										#if (ISIS_CMD_ENABLE == 1)
										//Send ACK MSG
										sendAckMsg();
										#endif
										#endif
									}																		
									break;

								//To update ADCS CGAIN
								case GDSN_UPDATE_CGAIN:
									//Time window validation check
									timeWindowCheck();

									//Only proccess the command if it passes time window validation
									if (g_uc_gsch_flag & GSCH_FLAG_TIME_WINDOW_VALID){
										//Command ADCS to update CGAIN
										#if (ADCS_CMD_ENABLE == 1)
										#if (DEBUGGING_FEATURE == 1)
										g_us_gsch_error_code = adcsSetCGain(0);
										#else
										g_us_gsch_error_code = adcsSetCGain();
										#endif
										#else
										g_us_gsch_error_code = EC_ADCS_CMD_DISABLED;
										#endif
										
										#if (SENDING_ENABLE == 1)
										#if (ISIS_CMD_ENABLE == 1)
										//Send ACK MSG
										sendAckMsg();
										#endif
										#endif
									}
									break;

								#if (SATELLITE_SCHEDULE_HANDLER_ENABLE == 1)
								//To schedule sun tracking
								case GDSN_SCHEDULE_SUN_TRACKING:
									//Time window validation check
									timeWindowCheck();

									//Only proccess the command if it passes time window validation
									if (g_uc_gsch_flag & GSCH_FLAG_TIME_WINDOW_VALID){
										//Only process the command if there are at least 4 empty schedule slots
										if (str_obdh_hk.uc_scheduler_block_size <= (SCHEDULER_BLOCK_SIZE-4)){
											//Schedule mission mode at requested time -20s with elapsed time as requested +30s
											g_us_gsch_error_code = scheduleMissionMode((uchar2ulong(g_uc_gsch_gs_arg[0],g_uc_gsch_gs_arg[1],g_uc_gsch_gs_arg[2],g_uc_gsch_gs_arg[3])-SECONDS_FROM_TAI_TO_KILL_SWITCH_DEACTIVATION-20),(unsigned short)((unsigned short)(g_uc_gsch_gs_arg[4] << 8)+g_uc_gsch_gs_arg[5]+30));

											//If there is no error code, it means that the scheduled time has not passed.
											//Proceeds by turning on ADCS at -20s, and uploading the scheduled sun tracking on schedule with the given elapsed time/10
											if (!g_us_gsch_error_code){
												//Schedule to turn ON ADCS first
												g_us_gsch_error_code = scheduleTurnOnADCS(uchar2ulong(g_uc_gsch_gs_arg[0],g_uc_gsch_gs_arg[1],g_uc_gsch_gs_arg[2],g_uc_gsch_gs_arg[3])-SECONDS_FROM_TAI_TO_KILL_SWITCH_DEACTIVATION-20);

												//Schedule sun tracking mission
												scheduleSunTracking();
											}
										}
										//Return error code: EC_NOT_ENOUGH_SCHEDULE_SLOT
										else g_us_gsch_error_code = EC_NOT_ENOUGH_SCHEDULE_SLOT;
										
										#if (SENDING_ENABLE == 1)
										//Save latest schedule and send acknowledgement message
										groundSaveScheduleAndAck();
										#endif
									}
									break;

								//To upload timed command (A)
								case GDSN_UPLOAD_TCMD_A:
									//Insert the scheduled command to the register
									for (l_us_i = 0; l_us_i < MAX_GSCH_CMD_ARG_LEN; l_us_i++)
										g_uc_gsch_sch_register[l_us_i] = g_uc_gsch_gs_arg[l_us_i];
									
									//Always successful
									g_us_gsch_error_code = EC_SUCCESSFUL;

									#if (SENDING_ENABLE == 1)
									#if (ISIS_CMD_ENABLE == 1)
									//Send ACK MSG
									sendAckMsg();
									#endif
									#endif
									break;

								//To upload timed command (B)
								case GDSN_UPLOAD_TCMD_B:
									//Insert the scheduled command to the register
									for (l_us_i = 0; l_us_i < (MAX_SCHEDULER_ARG+SCHEDULER_NON_ARG_SIZE+2)-MAX_GSCH_CMD_ARG_LEN; l_us_i++)
										g_uc_gsch_sch_register[l_us_i+MAX_GSCH_CMD_ARG_LEN] = g_uc_gsch_gs_arg[l_us_i];

									#if (CRC_ENABLE == 1)
									//Check the uploaded timed command's CRC16
									//Ian (2012-05-15): Magic constant 7 is changed to SCHEDULER_NON_ARG_SIZE
									g_us_gsch_error_code = checkCRC16Match (&g_uc_gsch_sch_register, MAX_SCHEDULER_ARG+SCHEDULER_NON_ARG_SIZE);
									#else
									//Always successful
									g_us_gsch_error_code = EC_SUCCESSFUL;
									#endif

									//If the CRC matches, attempt to upload the schedule
									if (!g_us_gsch_error_code){
										//Insert schedule to start the wanted task on the scheduled time
										str_scheduler.ul_scheduled_time_register 	= uchar2ulong(g_uc_gsch_sch_register[0],g_uc_gsch_sch_register[1],g_uc_gsch_sch_register[2],g_uc_gsch_sch_register[3]) - SECONDS_FROM_TAI_TO_KILL_SWITCH_DEACTIVATION;
										str_scheduler.us_cmd_register 				= (unsigned short)(g_uc_gsch_sch_register[4] << 8) + g_uc_gsch_sch_register[5];
										str_scheduler.uc_arg_length_register 		= g_uc_gsch_sch_register[6];
										//Ian (2012-05-15): Magic constant 7 is changed to SCHEDULER_NON_ARG_SIZE
										for (l_us_i = 0; l_us_i < MAX_SCHEDULER_ARG; l_us_i++)
											str_scheduler.uc_arg_register[l_us_i]	= g_uc_gsch_sch_register[l_us_i+SCHEDULER_NON_ARG_SIZE];	
										g_us_gsch_error_code = addNewSchedule();
									}

									#if (SENDING_ENABLE == 1)
									//Save latest schedule and send acknowledgement message
									groundSaveScheduleAndAck();
									#endif
									break;
								#endif

								//To turn OFF beacon
								case GDSN_OFF_TX:
									//Time window validation check
									timeWindowCheck();

									//Only proccess the command if it passes time window validation
									if (g_uc_gsch_flag & GSCH_FLAG_TIME_WINDOW_VALID){
										//Disable the beacon flag until unspecified amount of time
										str_obdh_hk.us_beacon_flag |= BEACON_FLAG_AUTO_BEACON_DISABLE;

										//Always successful
										g_us_gsch_error_code = EC_SUCCESSFUL;

										#if (SENDING_ENABLE == 1)
										//Save essential state and send acknowledgement
										groundSaveEssentialAndAck();
										#endif
									}
									break;

								//To turn ON beacon
								case GDSN_ON_TX:
									//Time window validation check
									timeWindowCheck();

									//Only proccess the command if it passes time window validation
									if (g_uc_gsch_flag & GSCH_FLAG_TIME_WINDOW_VALID){
										//Enable the beacon flag until unspecified amount of time
										str_obdh_hk.us_beacon_flag &= ~BEACON_FLAG_AUTO_BEACON_DISABLE;

										//Always successful
										g_us_gsch_error_code = EC_SUCCESSFUL;

										#if (SENDING_ENABLE == 1)
										//Save essential state and send acknowledgement
										groundSaveEssentialAndAck();
										#endif
									}
									break;

								#if (SENDING_ENABLE == 1)
								#if (ISIS_CMD_ENABLE == 1)
								//To get satellite orbit
								case GDSN_GET_SAT_REV:
									//Always successful
									g_us_gsch_error_code = EC_SUCCESSFUL;

									//Send current satellite revolution (orbit) no
									sendCommonData(SD_SAT_REV);
									break;
								#endif
								#endif

								//To set satellite orbit
								case GDSN_SET_SAT_REV:
									//Time window validation check
									timeWindowCheck();

									//Only proccess the command if it passes time window validation
									if (g_uc_gsch_flag & GSCH_FLAG_TIME_WINDOW_VALID){
										//Update revolution no based on GDSN inputs
										groundUpdateRevNo();

										//Always successful
										g_us_gsch_error_code = EC_SUCCESSFUL;
	
										#if (SENDING_ENABLE == 1)
										//Save essential state and send acknowledgement
										groundSaveEssentialAndAck();
										#endif
									}
									break;

								#if (SENDING_ENABLE == 1)
								#if (ISIS_CMD_ENABLE == 1)
								//To get a specified SD card block data
								case GDSN_GET_SD_BLOCK:
									//Set initial and end block pointer for the wanted data
									g_ul_obs_initial_bp = uchar2ulong(g_uc_gsch_gs_arg[0],g_uc_gsch_gs_arg[1],g_uc_gsch_gs_arg[2],g_uc_gsch_gs_arg[3]);
									g_ul_obs_end_bp = g_ul_obs_initial_bp;

									//Limit check
									if (g_ul_obs_initial_bp > SD_BP_MAX){
										//Set error code EC_SD_CARD_BLOCK_DOES_NOT_EXIST
										g_us_gsch_error_code = EC_SD_CARD_BLOCK_DOES_NOT_EXIST;

										//Send ACK MSG
										sendAckMsg();
									}
									//Successful request
									else{
										//Always successful
										g_us_gsch_error_code = EC_SUCCESSFUL;

										//Indicates taking data from SD card
										g_uc_ax25_header_package_property = AX25_HEADER_PROPERTY_STORED_DATA_BIT;
	
										//Set current package no to 1
										g_uc_ax25_header_no_of_this_package = 1;
	
										//Initialize subsystem and data type to load
										g_uc_gsch_subsystem_to_load = ALL_SUBSYSTEMS;
										g_uc_gsch_data_type_to_load = SD_REQUESTED_BLOCK;
	
										//Set the downlink rate as assigned (always one for this case)
										g_uc_gsch_sampling_step = 1;
	
										//Send stored data
										groundEnterSendStored();
									}
									break;
								#endif
								#endif

								//To schedule OBDH's mission mode
								case GDSN_SCHEDULE_MISSION_MODE:
									//Time window validation check
									timeWindowCheck();

									//Only proccess the command if it passes time window validation
									if (g_uc_gsch_flag & GSCH_FLAG_TIME_WINDOW_VALID){
										#if (SATELLITE_SCHEDULE_HANDLER_ENABLE == 1)
										//Schedule mission mode as requested
										g_us_gsch_error_code = scheduleMissionMode((uchar2ulong(g_uc_gsch_gs_arg[0],g_uc_gsch_gs_arg[1],g_uc_gsch_gs_arg[2],g_uc_gsch_gs_arg[3]) - SECONDS_FROM_TAI_TO_KILL_SWITCH_DEACTIVATION),(unsigned short)((unsigned short)(g_uc_gsch_gs_arg[4] << 8) + g_uc_gsch_gs_arg[5]));
										#else
										g_us_gsch_error_code = EC_SATELLITE_SCHEDULE_IS_DISABLED;
										#endif

										#if (SENDING_ENABLE == 1)
										//Save latest schedule and send acknowledgement message
										groundSaveScheduleAndAck();
										#endif
									}
									break;

								#if (SENDING_ENABLE == 1)
								#if (ISIS_CMD_ENABLE == 1)
								//To request lost frame package
								case GDSN_REQUEST_LOST_FRAME:
									//Check whether the input is not zero
									if (g_uc_gsch_gs_arg[0]){									
										//Always successful
										g_us_gsch_error_code = EC_SUCCESSFUL;
	
										//Set initial and end block pointer for the wanted data
										g_ul_obs_initial_bp = SD_BP_PACKAGE_RECOVERY_START + (g_uc_gsch_gs_arg[0]-1) * (1+NO_OF_COPIES_SD_RECOVERY_PACKAGE);
										g_ul_obs_end_bp = g_ul_obs_initial_bp;
	
										//Indicates taking data from SD card and is a special recovery case
										g_uc_ax25_header_package_property = AX25_HEADER_PROPERTY_STORED_DATA_BIT + AX25_HEADER_PROPERTY_PACKAGE_RECOVERY_DATA_BIT;
	
										//Set current package no to 1
										g_uc_ax25_header_no_of_this_package = 1;
	
										//Initialize subsystem and data type to load
										g_uc_gsch_subsystem_to_load = ALL_SUBSYSTEMS;
										g_uc_gsch_data_type_to_load = SD_RECOVERY_PACKAGE;
	
										//Set the downlink rate as assigned (always one for this case)
										g_uc_gsch_sampling_step = 1;
	
										//Send stored data
										groundEnterSendStored();
									}
									//If index = 0 -> Invalid lost frame index
									else{
										g_us_gsch_error_code = EC_INVALID_GDSN_LOST_FRAME_INDEX;
										sendAckMsg();										
									}
									break;

								//Get GDSN_GET_WOE data based on orbit dec22_yc
								case GDSN_GET_WOE:
									//Search proper block pointer based on the orbit requested
									g_us_obs_first_orbit_request 	= (unsigned short)(g_uc_gsch_gs_arg[0] << 8) + g_uc_gsch_gs_arg[1];
									g_us_obs_last_orbit_request 	= g_us_obs_first_orbit_request;
									#if (STORING_ENABLE == 1)
									g_us_gsch_error_code 			= searchBPByOrbitInfo(SD_EVENTLOG_DATA);
									#else
									g_us_gsch_error_code			= EC_STORING_FUNCTIONS_ARE_DISABLED;
									#endif

									//If there is initialization error, do not proceed
									//Else, proceeds to GSCH_STATE_SEND_STORED_DATA state
									if(!g_us_gsch_error_code){
										//Indicates taking data from SD card
										g_uc_ax25_header_package_property = AX25_HEADER_PROPERTY_STORED_DATA_BIT;

										//Set current package no to 1
										g_uc_ax25_header_no_of_this_package = 1;

										//Initialize subsystem and data type to load
										g_uc_gsch_subsystem_to_load = ALL_SUBSYSTEMS;
										g_uc_gsch_data_type_to_load = SD_EVENTLOG_DATA;
	
										//Set the downlink rate as assigned
										g_uc_gsch_sampling_step = g_uc_gsch_gs_arg[2];

										//Check the limits
										if (g_uc_gsch_sampling_step < WOD_SAMPLE_STEP_LOWER_LIMIT)//no change.. same sample step. 
											g_uc_gsch_sampling_step = WOD_SAMPLE_STEP_LOWER_LIMIT;
										else if (g_uc_gsch_sampling_step > WOD_SAMPLE_STEP_UPPER_LIMIT) 
											g_uc_gsch_sampling_step = WOD_SAMPLE_STEP_UPPER_LIMIT;

										//Send stored data
										groundEnterSendStored();
									}
									else sendAckMsg();
									break;
								#endif
								#endif

								//If GDSN command is not recognized, the satellite will still send ACK MSG will proper error codes
								//No, better don't send anything to save energy
								default:
									break;
							}
						}
					}		
				}
				//Else, if energy is insufficient, go to GSCH_STATE_IDLE state
				else groundTerminateGroundPass();
				
			}
			//Else, when the time has up, go back to idle state
			else groundTerminateGroundPass();
					
			break;

		//When GSCH is sending real time data
		case GSCH_STATE_SEND_REAL_TIME_DATA:
			//Sending time flag is always raised in this state
			g_uc_gsch_flag |= GSCH_FLAG_TOTAL_SENDING;

			//For all transition state case, it is important to get latest beacon status
			//Only proceeds when beacon is functional
			if ((str_obdh_hk.us_beacon_flag & BEACON_FLAG_FILTER_PRIMARY) == BEACON_FUNCTIONAL){
				//If it is functional, check its case
				l_us_beacon_status = str_obdh_hk.us_beacon_flag & BEACON_FLAG_FILTER_SECONDARY;

				//Switch the case for beacon status
				switch (l_us_beacon_status){
					//There are only four valid cases
					//First, beacon is free, initiates transmission here
					case BEACON_FREE:
						//Reset g_us_groundpass_time_register
						g_us_groundpass_time_register = TIME_RESET_GROUND_PASS_S;

						#if (SENDING_ENABLE == 1)
						//Start sending request
						groundStartSendRequest();
						#endif
						break;
	
					//Second, beacon is busy of other sending task, do nothing
					//Third, beacon is currently sending your request, do nothing too
					//If the sending has gone too long, however, terminates the requets
					//Go back to GSCH_STATE_GROUND_PASS state
					case BEACON_BUSY_1:
					case BEACON_BUSY_2:
					case BEACON_SENDING:
					case BEACON_WAITING:
						break;
	
					//Fourth, beacon sending is completed					
					case BEACON_SEND_COMPLETED:
						//Reset g_us_groundpass_time_register
						g_us_groundpass_time_register = TIME_RESET_GROUND_PASS_S;

						//If the sending package is not completed
						if (!(g_uc_strsnd_flag & SNDNG_FLAG_COMPLETED))
							#if (SENDING_ENABLE == 1)
							//Generate new sending request to the beaconHandler
							groundKeepSendRequest();						
							#else
							;
							#endif
						//Else if there is no remaining sending package
						else { 
							//Reset transitional state monitoring flag
							g_uc_gsch_flag &= ~GSCH_FLAG_MONITOR_TRANSITIONAL;

							//Reset sending completed flag
							g_uc_strsnd_flag &= ~SNDNG_FLAG_COMPLETED;

							#if (SENDING_ENABLE == 1)
							//Release beacon's claim
							groundReleaseClaimBeacon();
							#endif
	
							//Go back to the GSCH_STATE_GROUND_PASS state,
							str_obdh_hk.uc_gsch_state = GSCH_STATE_GROUND_PASS;
						}
						break;
	
					//Besides the above cases, something wrong with the beacon, terminates the sending
					default:
						#if (SENDING_ENABLE == 1)
						//Terminates send real time state
						groundTerminateSendRealTimeData();
						#endif
						break;
				}
			}
			#if (SENDING_ENABLE == 1)
			//If beacon is not found to be functional at this stage, return to GSCH_STATE_GROUND_PASS
			else
				//Terminates send real time state
				groundTerminateSendRealTimeData();

			//If at any point timeout for monitoring occurs
			if (g_us_gsch_monitoring_transitional_period > TIME_LIMIT_GSCH_TRANSITIONAL_STATE_S)
				//Terminates send real time state
				groundTerminateSendRealTimeData();
			#endif

			break;

		//When GSCH is sending stored data
		case GSCH_STATE_SEND_STORED_DATA:
			//Sending time flag is always raised in this state
			g_uc_gsch_flag |= GSCH_FLAG_TOTAL_SENDING;

			//For all transition state case, it is important to get latest beacon status
			//Only proceeds when beacon is functional
			if ((str_obdh_hk.us_beacon_flag & BEACON_FLAG_FILTER_PRIMARY) == BEACON_FUNCTIONAL){
				//If it is functional, check its case
				l_us_beacon_status = str_obdh_hk.us_beacon_flag & BEACON_FLAG_FILTER_SECONDARY;

				//Switch the case for beacon status
				switch (l_us_beacon_status){
					//There are only four valid cases
					//First, beacon is free, initiates transmission here
					case BEACON_FREE:
						//Reset g_us_groundpass_time_register
						g_us_groundpass_time_register = TIME_RESET_GROUND_PASS_S;

						#if (STORING_ENABLE == 1)
						//Attempt to load data
						g_us_sd_load_error_code = loadDataProtected(g_uc_gsch_subsystem_to_load,g_uc_gsch_data_type_to_load,g_ul_obs_initial_bp,g_ul_obs_end_bp);
						#else
						g_us_sd_load_error_code = EC_STORING_FUNCTIONS_ARE_DISABLED;
						#endif

						//If data is corrupted at this point, still sending, nothing to lose
						if (g_us_sd_load_error_code == EC_SD_ALL_STORED_DATA_IS_CORRUPTED) g_us_sd_load_error_code = EC_SUCCESSFUL;
						
						//If there is no error code, read the SD card block property it returns
						//and proceeds by sending the data
						if(!g_us_sd_load_error_code){
							g_uc_gsch_sd_block_property = g_uc_sd_data_buffer[2];
			
							//Reset g_uc_gsch_loading_retry_counter
							g_uc_gsch_loading_retry_counter = 0;

							#if (SENDING_ENABLE == 1)
							//Start sending request
							groundStartSendRequest();
							#endif
						}
						//Else, if there is error in first time loading, increase retry counter, try again next loop
						//Do not raise the send request flag yet, unless we are sure about our data to send
						else {
							g_uc_sd_load_data_timeout_counter = TIME_LIMIT_LOAD_DATA_S; 
							g_uc_gsch_loading_retry_counter++;
						}

						//If for any reason the number of loading trial has gone too many times
						//Terminates Groundpass
						if (g_uc_gsch_loading_retry_counter > GSCH_LOADING_FAIL_LIMIT){
							//Set SD load error code as SD card irresponsive
							g_us_sd_load_error_code = EC_SD_CARD_IRRESPONSIVE;

							#if (SENDING_ENABLE == 1)
							//Terminates send stored data state but gives ACK_MSG to MCC
							groundTerminateSendStoredDataWithAckMsg();
							#endif
						}
						break;
	
					//Second, beacon is busy of other sending task, do nothing
					//Third, beacon is currently sending your request, do nothing too
					//If the sending has gone too long, however, terminates the requets
					//Go back to GSCH_STATE_GROUND_PASS state
					case BEACON_BUSY_1:
					case BEACON_BUSY_2:
					case BEACON_SENDING:
					case BEACON_WAITING:
						break;
	
					//Fourth, beacon sending is completed					
					case BEACON_SEND_COMPLETED:
						//Reset g_us_groundpass_time_register
						g_us_groundpass_time_register = TIME_RESET_GROUND_PASS_S;

						//If the sending package is not completed
						if (!(g_uc_strsnd_flag & SNDNG_FLAG_COMPLETED))
							#if (SENDING_ENABLE == 1)
							//Generate new sending request to the beaconHandler
							groundKeepSendRequest();						
							#else
							;
							#endif

						//If sending is completed but loading is not completed
						else if ((g_uc_strsnd_flag & SNDNG_FLAG_COMPLETED) && !(g_uc_strsnd_flag & STRNG_FLAG_SD_LOAD_COMPLETED)){

							//Do the loading based on the previous g_uc_gsch_sd_block_property
							//And downloading rate the ground station specifies	in g_uc_gsch_sampling_step
			
							//If the g_uc_gsch_sd_block_property shows that previous data is on going, download once only
							if (g_uc_gsch_sd_block_property == 0x01){
								#if (STORING_ENABLE == 1)
								//Attempt to load data
								g_us_sd_load_error_code = loadDataProtected(g_uc_gsch_subsystem_to_load,g_uc_gsch_data_type_to_load,g_ul_obs_initial_bp,g_ul_obs_end_bp);

								//If data is corrupted at this point, still sending, nothing to lose
								if (g_us_sd_load_error_code == EC_SD_ALL_STORED_DATA_IS_CORRUPTED) g_us_sd_load_error_code = EC_SUCCESSFUL;						
								#else
								g_us_sd_load_error_code = EC_STORING_FUNCTIONS_ARE_DISABLED;
								#endif
							}
			
							//Else, if the g_uc_gsch_sd_block_property shows that previous data is the end
							//Load as many number of times as (g_uc_gsch_sampling_step-1) times we get g_uc_gsch_sd_block_property == 0x02
							//Then the next loading is the wanted data
							else if (g_uc_gsch_sd_block_property == 0x02){
								//Reset error code for loading
								g_us_sd_load_error_code = EC_SUCCESSFUL;
			
								//Initialize no of data loading as zero
								g_uc_gsch_no_of_loading = 0; 
			
								//This is to anticipate different downloading rate applies by the GDSN
								//While the number of data loading is less than (g_uc_gsch_sampling_step-1), repeat the steps
								//If the number of data loading is equal to or more than (g_uc_gsch_sampling_step-1)
								//Whenever there is an error code, terminates
								while ((g_uc_gsch_no_of_loading < (g_uc_gsch_sampling_step-1)) && (g_us_sd_load_error_code == EC_SUCCESSFUL)){
									//Attempt to load data, so long as the loading is not completed
									if (!(g_uc_strsnd_flag & STRNG_FLAG_SD_LOAD_COMPLETED)){
										#if (STORING_ENABLE == 1)
										g_us_sd_load_error_code = loadDataProtected(g_uc_gsch_subsystem_to_load,g_uc_gsch_data_type_to_load,g_ul_obs_initial_bp,g_ul_obs_end_bp);

										//If data is corrupted at this point, still proceeds, nothing to lose
										if (g_us_sd_load_error_code == EC_SD_ALL_STORED_DATA_IS_CORRUPTED) g_us_sd_load_error_code = EC_SUCCESSFUL;						
										#else
										g_us_sd_load_error_code = EC_STORING_FUNCTIONS_ARE_DISABLED;
										#endif
									}
									else g_uc_gsch_no_of_loading = g_uc_gsch_sampling_step; //This is impossible to happen, use this as indicator of pre-mature termination
									
									//If there is no error code
									if (!g_us_sd_load_error_code){
										//Check the block property of the SD card block loaded
										g_uc_gsch_sd_block_property = g_uc_sd_data_buffer[2];
			
										//If for any reason g_uc_gsch_sd_block_property is neither 0x01 nor 0x02
										if (g_uc_gsch_sd_block_property != 0x01 && g_uc_gsch_sd_block_property != 0x02)
											g_us_sd_load_error_code = EC_UNEXPECTED_BLOCK_PROPERTY;
			
										//If the block property shows that this block is end block
										//Ian (2012-05-15): By the format defined when we write to SD card, if the block property is not 0x02,
										//					then something is wrong with the data.
										if(g_uc_gsch_sd_block_property == 0x02)
											//Add the number of loading
											g_uc_gsch_no_of_loading++;
									}
	
									//If there is any error code during this reading, it would have been pretty disastrous
									//Hence, terminates the sending immediately, go to GSCH_STATE_GROUND_PASS again,
									//It is better than sending wrong, funny data. No retry is possible for this case
									if (g_us_sd_load_error_code){
										//Set error code as interrupted loading
										g_us_sd_load_error_code = EC_LOADING_INTERRUPTED_WITH_ERROR;

										#if (SENDING_ENABLE == 1)
										//Terminates send stored data state but gives ACK_MSG to MCC
										groundTerminateSendStoredDataWithAckMsg();
										#endif
									}
								}

								//If it reaches this point without error code, 
								//It means all data which is not supposed to be sent would have been skipped. 
								//Attempt to load data for the last time occurs here, given that the loading is not finished earlier
								if (!g_us_sd_load_error_code && !(g_uc_strsnd_flag & STRNG_FLAG_SD_LOAD_COMPLETED)){
									#if (STORING_ENABLE == 1)
									g_us_sd_load_error_code = loadDataProtected(g_uc_gsch_subsystem_to_load,g_uc_gsch_data_type_to_load,g_ul_obs_initial_bp,g_ul_obs_end_bp);
									//If data is corrupted at this point, still proceeds, nothing to lose
									if (g_us_sd_load_error_code == EC_SD_ALL_STORED_DATA_IS_CORRUPTED) g_us_sd_load_error_code = EC_SUCCESSFUL;						
									#else
									g_us_sd_load_error_code = EC_STORING_FUNCTIONS_ARE_DISABLED;
									#endif
								}
							}
							//If for any reason g_uc_gsch_sd_block_property is neither 0x01 nor 0x02
							else {
								//If there is any error code during this reading, it would have been pretty disastrous
								//Hence, terminates the sending immediately, go to GSCH_STATE_GROUND_PASS again,
								//It is better than sending wrong, funny data. No retry is possible for this case
								g_us_sd_load_error_code = EC_UNEXPECTED_BLOCK_PROPERTY;

								#if (SENDING_ENABLE == 1)
								//Terminates send stored data state but gives ACK_MSG to MCC
								groundTerminateSendStoredDataWithAckMsg();
								#endif
							}

							//If there is no error code so far, proceeds by sending the data
							if(!g_us_sd_load_error_code){	
								//The sending data is purposely put here so that if the loading data failed
								//Next turn it will come back to this state (not somewhere else)
								g_uc_strsnd_flag &= ~SNDNG_FLAG_COMPLETED;

								#if (SENDING_ENABLE == 1)
								//Generate new sending request to the beaconHandler
								groundKeepSendRequest();						
								#endif
							}

							//Else, if the error in loading is neither EC_UNEXPECTED_BLOCK_PROPERTY nor EC_LOADING_INTERRUPTED_WITH_ERROR, 
							//increase retry counter, try again next time
							else if ((g_us_sd_load_error_code != EC_UNEXPECTED_BLOCK_PROPERTY) && (g_us_sd_load_error_code != EC_LOADING_INTERRUPTED_WITH_ERROR)) 
								g_uc_gsch_loading_retry_counter++;
			
							#if (SENDING_ENABLE == 1)
							//Other error would be unhandled, terminates the sending immediately				
							else
								//Terminates send stored data state but gives ACK_MSG to MCC
								groundTerminateSendStoredDataWithAckMsg();
							#endif

							//If for any reason the number of loading trial has gone too many times
							//Terminates Groundpass
							if (g_uc_gsch_loading_retry_counter > GSCH_LOADING_FAIL_LIMIT){
								//Set SD load error code as SD card irresponsive
								g_us_sd_load_error_code = EC_SD_CARD_IRRESPONSIVE;
	
								#if (SENDING_ENABLE == 1)
								//Terminates send stored data state but gives ACK_MSG to MCC
								groundTerminateSendStoredDataWithAckMsg();
								#endif
							}
						} 

						//If both sending and loading are completed
						else if ((g_uc_strsnd_flag & SNDNG_FLAG_COMPLETED) && (g_uc_strsnd_flag & STRNG_FLAG_SD_LOAD_COMPLETED)){
							//Don't reset the sending and loading flags here
							//Because in the event of retry, it cannot come back to this state
							//But reset the sending and loading flags at the beginning of sending
							//Reset sending and loading flags
							//In ground pass, all flags will be reset, anyway
							g_uc_strsnd_flag &= ~SNDNG_FLAG_COMPLETED;
							g_uc_strsnd_flag &= ~STRNG_FLAG_SD_LOAD_COMPLETED;
			
							//Reset transitional state monitoring flag
							g_uc_gsch_flag &= ~GSCH_FLAG_MONITOR_TRANSITIONAL;

							//Reset g_uc_gsch_loading_retry_counter
							g_uc_gsch_loading_retry_counter = 0;

							//Reset recovery package block pointer
							str_bp.ul_package_recovery_write_p = SD_BP_PACKAGE_RECOVERY_START;

							#if (SENDING_ENABLE == 1)
							//Release beacon's claim
							groundReleaseClaimBeacon();
							#endif
	
							//Go back to GSCH_STATE_GROUND_PASS
							str_obdh_hk.uc_gsch_state = GSCH_STATE_GROUND_PASS;			
						}
						break;
	
					//Besides the above cases, something wrong with the beacon, terminates the sending
					default:
						#if (SENDING_ENABLE == 1)
						//Terminates send stored data state
						groundTerminateSendStoredData();
						#endif
						break;
				}
				//See if we should put something here: to check if we receive further command and terminates the current sending
				//Get AX.25 no of frame from COMM board twice
				for (l_us_i = 0; l_us_i < 2; l_us_i++)
					if (isisI2C(IMC_ADDR,IMC_I2C_GET_NOFR,0)) 
						break;

				if (l_us_i == 2 && g_uc_i2c_data_in[0]) //If it is successful & there is something in the COMM board, then terminates the current sending
					//Terminates send stored data state
					groundTerminateSendStoredData();
			}
			#if (SENDING_ENABLE == 1)
			//If beacon is not found to be functional at this stage, return to GSCH_STATE_GROUND_PASS
			else 
				//Terminates send stored data state
				groundTerminateSendStoredData();

			//If at any point timeout for monitoring occurs
			if (g_us_gsch_monitoring_transitional_period > TIME_LIMIT_GSCH_TRANSITIONAL_STATE_S)
				//Terminates send stored data state
				groundTerminateSendStoredData();
			#endif

			break;

		//When GSCH is dead
		case GSCH_STATE_DEAD:
			//This flag should be raised every second
			if (g_uc_gsch_flag & GSCH_FLAG_INITIAL_CHECK){
				//Reset first the checking flag
				g_uc_gsch_flag &= ~GSCH_FLAG_CHECK_RESET;
	
				//Increase monitoring period
				g_us_gsch_monitoring_period++;
	
				//If the monitoring period has exceeding TIME_LIMIT_GSCH_DEAD_STATE_S
				//Enter recovery state
				if (g_us_gsch_monitoring_period > TIME_LIMIT_GSCH_DEAD_STATE_S){
					resetGSCHCounters();
					str_obdh_hk.uc_gsch_state = GSCH_STATE_RECOVERY;
				}
			}			
			break;

		//When GSCH is recovering
		case GSCH_STATE_RECOVERY:
			//This flag should be raised every second
			if (g_uc_gsch_flag & GSCH_FLAG_INITIAL_CHECK){
				//Reset first the checking flag
				g_uc_gsch_flag &= ~GSCH_FLAG_CHECK_RESET;
		
				//Increase monitoring period
				g_us_gsch_monitoring_period++;
	
				//If the monitoring period has exceeding TIME_LIMIT_GSCH_RECOVERY_STATE_S
				//Enter dead state again
				if (g_us_gsch_monitoring_period > TIME_LIMIT_GSCH_RECOVERY_STATE_S){
					resetGSCHCounters();
					str_obdh_hk.uc_gsch_state = GSCH_STATE_DEAD;
				}				
				//Else 
				else{
					//Try to communicate with COMM board
					l_us_comm_error_code = isisI2C(ITC_ADDR,ITC_I2C_TRN_UPTM,0);
	
					//If the communication is successful, enters one of the following states:
					//1) If (g_uc_gsch_flag & GSCH_FLAG_GROUNDPASS_STARTED) flag is raised, go to groundpass mode
					if(!l_us_comm_error_code){
						//If there is currently groundpass going on
						if	((g_uc_gsch_flag & GSCH_FLAG_GROUNDPASS_STARTED))
							str_obdh_hk.uc_gsch_state = GSCH_STATE_GROUND_PASS;
						else
							str_obdh_hk.uc_gsch_state = GSCH_STATE_IDLE;
					}
				}
			}			
			break;

		//No other state is possible
		default:
			break;
	}

	#if (SENDING_ENABLE == 1)
	//Check if at any point of time the sending time has gone too long, terminates the transitional (sending) state
	if (g_us_gsch_total_sending_time >= TIME_LIMIT_GROUND_PASS_S && (g_uc_gsch_flag & GSCH_FLAG_TOTAL_SENDING))
		groundTerminateSendStoredData();
	#endif

	//If for any reason there is no communication occur, go to GSCH_STATE_DEAD
	if (g_uc_gsch_comm_retry_counter > GSCH_COMM_FAIL_LIMIT){
		//Reset all counters and flags
		g_uc_gsch_comm_retry_counter = 0;

		//Terminates transitional states
		groundTerminateTransitionalState();

		//Set GSCH to dead state
		str_obdh_hk.uc_gsch_state = GSCH_STATE_DEAD;			
	}

	#if (EXTRA_ENABLE == 1)
	//If, for whatever reason there is no communication with MCC until timeout
	if (g_ul_gsch_time_elapsed_since_last_communication_with_mcc >= TIME_LIMIT_NO_COMM_WITH_MCC_S) {
		//Mark reset cause as RESET_SOURCE_GSCH_WATCHDOG 
		str_obdh_data.uc_sat_rstsrc = RESET_SOURCE_GSCH_WATCHDOG;

		//Trigger reset satellite every 5 minutes (prevent from satellite's reset failure)
		g_ul_gsch_time_elapsed_since_last_communication_with_mcc -= 300;

		//Trigger satellite's reset
		callReset(OBDH_CMD_RESET_SAT);	
	}
	#endif

	#endif
}
#endif

#if (SATELLITE_SCHEDULE_HANDLER_ENABLE == 1)
//------------------------------------------------------------------------------------
// To handle satellite scheduled command
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
void satelliteScheduleHandler (void)
{
	unsigned long l_ul_nearest_scheduled_time;
	unsigned short l_us_i;
	unsigned short l_us_error_code;

	//Check if there is something in the scheduler
	if (str_obdh_hk.uc_scheduler_block_size){
		//If the schedule is not arranged, arrange it first
		if(!(g_uc_evha_flag & EVENT_FLAG_SCHEDULE_IS_REARRANGED)){
			rearrangeSchedule();
			g_uc_evha_flag |= EVENT_FLAG_SCHEDULE_IS_REARRANGED;
		}

		//Check the schedule, so long as there is a scheduled command, attempt to
		//upload it to the command queue. So long as the upload is successful or 
		//there is an invalid command length but there is any scheduled command
		//left, keeps uploading till all the schduled 
		//command is uploaded. Not so if the error code is EC_COMMAND_ALREADY_EXIST
		//or EC_COMMAND_QUEUE_FULL, we will stop uploading the command
		do {
		
			//Check nearest scheduled time
			l_ul_nearest_scheduled_time = str_scheduler.ul_scheduled_time[str_scheduler.uc_reindexing[0]];

			//Initialize the error code as EC_SCHEDULE_STOP_UPLOADING_COMMAND
			//In the case where the schedule does not upload any command and the last upload
			//was successful, and the nearest schedule time reading is greater than zero,
			//This error code will act as a way of exit from this infinite loop
			l_us_error_code = EC_SCHEDULE_STOP_UPLOADING_COMMAND;
	
			#if (CQUEUE_ENABLE == 1)
			//If satellite time has exceeded or equal to the scheduled time and the 
			//nearest scheduled time is not zero, upload the schedule to the command queue
			//then, upon successful uploading, remove the scheduled item from the schedule
			if ((l_ul_nearest_scheduled_time > 0) && (str_obdh_data.ul_obc_time_s >= l_ul_nearest_scheduled_time)){
				//Upload the command here!!
				//Upload command header
				g_uc_command_entry[0] = 0xBB;
				g_uc_command_entry[1] = str_scheduler.us_cmd[str_scheduler.uc_reindexing[0]] >> 8;
				g_uc_command_entry[2] = str_scheduler.us_cmd[str_scheduler.uc_reindexing[0]];
				g_uc_command_entry[3] = str_scheduler.uc_arg_length[str_scheduler.uc_reindexing[0]];

				//Limit the scheduler length to MAX_CMD_ARG_LEN for making command
				if (str_scheduler.uc_arg_length[str_scheduler.uc_reindexing[0]] > MAX_CMD_ARG_LEN)
					str_scheduler.uc_arg_length[str_scheduler.uc_reindexing[0]] = MAX_CMD_ARG_LEN;

				//Upload command argument (if any)
				for (l_us_i = 0; l_us_i < str_scheduler.uc_arg_length[str_scheduler.uc_reindexing[0]]; l_us_i++)
					g_uc_command_entry[l_us_i+COMMAND_HEADER_SIZE] = str_scheduler.uc_arg[str_scheduler.uc_reindexing[0]][l_us_i];

				#if (CRC_ENABLE == 1)
				//Create CRC16 bytes for the command generated
				insertCRC16(g_uc_command_entry,str_scheduler.uc_arg_length[str_scheduler.uc_reindexing[0]]+COMMAND_HEADER_SIZE);
				#endif

				//Attempt to upload the scheduled item to the command queue
				l_us_error_code = addCommandToQueue();
				
				#if (EVENT_HANDLER_ENABLE == 1)
				//dec22_yc here add even logging for command added to queue tested			
				eventHandler(EV_SCHEDULER_TO_COMMAND+l_us_error_code,
					str_scheduler.us_cmd[str_scheduler.uc_reindexing[0]] >> 8,str_scheduler.us_cmd[str_scheduler.uc_reindexing[0]],
						str_scheduler.uc_arg[str_scheduler.uc_reindexing[0]][0],
						str_scheduler.uc_arg[str_scheduler.uc_reindexing[0]][1],
						str_scheduler.uc_arg[str_scheduler.uc_reindexing[0]][2],
						str_scheduler.uc_arg[str_scheduler.uc_reindexing[0]][3]);
				#endif

				//Remove top scheduled command, if adding command is successful
				//Or if the error code for adding command is due to unacceptable command length
				//
				//If the adding command to the queue is not successful, the top most
				//Schedule will still be there. It will try to be uploaded again
				//In the next second
				//
				//There are only four possible error codes for uploading a command to a queue
				// 1) EC_SUCCESSFUL
				// 2) EC_COMMAND_ALREADY_EXIST
				// 3) EC_INVALID_COMMAND_LENGTH
				// 4) EC_COMMAND_QUEUE_FULL
				//
				//Error codes (2) and (4) allow re-attempt
				if (!l_us_error_code || l_us_error_code == EC_INVALID_COMMAND_LENGTH) 
					removeSchedule(1,1);
			}
			#else
			//If satellite time has exceeded or equal to the scheduled time and the 
			//nearest scheduled time is not zero, upload the schedule to the command queue
			//then, upon successful uploading, remove the scheduled item from the schedule
			if ((l_ul_nearest_scheduled_time > 0) && (str_obdh_data.ul_obc_time_s >= l_ul_nearest_scheduled_time)){
				//Limit the scheduler length to MAX_CMD_ARG_LEN for making command
				if (str_scheduler.uc_arg_length[str_scheduler.uc_reindexing[0]] > MAX_CMD_ARG_LEN)
					l_us_error_code = EC_INVALID_COMMAND_LENGTH;
				else{
					//Upload the command here!!
					//Upload command header
					g_uc_call_task_buffer[0] = 0xBB;
					g_uc_call_task_buffer[1] = str_scheduler.us_cmd[str_scheduler.uc_reindexing[0]] >> 8;
					g_uc_call_task_buffer[2] = str_scheduler.us_cmd[str_scheduler.uc_reindexing[0]];
					g_uc_call_task_buffer[3] = 0x00;
					g_uc_call_task_buffer[4] = str_scheduler.uc_arg_length[str_scheduler.uc_reindexing[0]];
	
					//Upload command argument (if any)
					for (l_us_i = 0; l_us_i < str_scheduler.uc_arg_length[str_scheduler.uc_reindexing[0]]; l_us_i++)
						g_uc_call_task_buffer[l_us_i+COMMAND_HEADER_SIZE+1] = str_scheduler.uc_arg[str_scheduler.uc_reindexing[0]][l_us_i];
	
					#if (CRC_ENABLE == 1)
					//Create CRC16 bytes for the command generated
					insertCRC16(g_uc_call_task_buffer,str_scheduler.uc_arg_length[str_scheduler.uc_reindexing[0]]+COMMAND_HEADER_SIZE+1);
					#endif
	
					//Mark as successful
					l_us_error_code = EC_SUCCESSFUL;
				}

				//Always removing the top schedule
				removeSchedule(1,1);

				//If there is no error
				if (!l_us_error_code) {
					//Proceeds to executing the command
					commandControlModule(g_uc_call_task_buffer,g_uc_call_task_buffer);

					//Clear call task buffer header
					g_uc_call_task_buffer[0] = 0x00;
				}
			}
			#endif
		//Keep uploading the command if:
		// 1) Error code while updating the command is either successful or invalid command length, and
		// 2) There is still something in the scheduler
		// 3) The nearest scheduled time (that is obtained from the schedule) is greater than zero
		} while ((!l_us_error_code || l_us_error_code == EC_INVALID_COMMAND_LENGTH) && str_obdh_hk.uc_scheduler_block_size && l_ul_nearest_scheduled_time > 0);
	}
}
#endif

#if (SATELLITE_MODE_HANDLER_ENABLE == 1)
#if (BODY_RATE_HANDLER_ENABLE == 1)
#if (ADCS_CMD_ENABLE == 1)
#if (PWRS_CMD_ENABLE == 1)
//------------------------------------------------------------------------------------
// To handle body rate control of the satellite
//------------------------------------------------------------------------------------
// This handler assumes that ADS is ON and PWRS is functional
// If any of the two is not satisfied, this handler should be disabled elsewhere
// in the program.
//------------------------------------------------------------------------------------
void bodyRateHandler (void)
{
	unsigned short l_us_error_code = EC_INIT;

	//Only check if it is time to check
	if ((str_obdh_hk.us_brh_flag & BRH_CHECK_FILTER) == BRH_CHECK_CONDITION){

		//Reset check flag
		str_obdh_hk.us_brh_flag &= ~BRH_FLAG_CHECK;

		//If there is an error at this state, try to recover first
		if (str_obdh_hk.us_brh_flag & BRH_FLAG_ERROR){
			//Attempt to communicate with PWRS
			#if (DEBUGGING_FEATURE == 1)
			l_us_error_code = pwrsGetData((unsigned char)DATA_PWRS_US_FIRMWARE_VER, 0);
			#else
			l_us_error_code = pwrsGetData((unsigned char)DATA_PWRS_US_FIRMWARE_VER);
			#endif

			//If there is no error, then attempt to recover is successful, reset the error flag
			//And the error counter
			if(!l_us_error_code) {
				str_obdh_hk.us_brh_flag &= ~BRH_FLAG_ERROR;
				g_uc_brh_retry_counter = 0;
				g_uc_brh_timeout_retry_counter = 0;
			}
		}

		//If there is no error at this point of time, we can start considering the current state of ACS
		if (!(str_obdh_hk.us_brh_flag & BRH_FLAG_ERROR)){
			//Check if timeout flag is not set
			if (!(str_obdh_hk.us_brh_flag & BRH_FLAG_DET_TO)){
				//Check whether what we desire state has been achieved, if not, just skip this check
				if ((str_obdh_hk.us_brh_flag & BRH_FLAG_DET_WT) && (((str_obdh_hk.us_brh_flag & BRH_FLAG_DET_ON) && (str_obdh_hk.us_brh_flag & BRH_FLAG_DET_RS)) || (!(str_obdh_hk.us_brh_flag & BRH_FLAG_DET_ON) && !(str_obdh_hk.us_brh_flag & BRH_FLAG_DET_RS)))){
					//Drop waiting & timeout flags
					str_obdh_hk.us_brh_flag &= ~BRH_FLAG_DET_WT;
					str_obdh_hk.us_brh_flag &= ~BRH_FLAG_DET_TO;

					//Reset waiting time and timeout retry counter
					g_uc_brh_wait_det_time_s = 0;
					g_uc_brh_timeout_retry_counter = 0;
				}

				//If currently we are not waiting for something to happen (Detumbling mode has achieved its desired state)
				if (!(str_obdh_hk.us_brh_flag & BRH_FLAG_DET_WT)){
					//If timeout occurs and Detumbling mode is ON (always undesired), turn OFF Detumbling mode
					//Ian (2012-05-15): it is noted that this code segment is almost identical with other code below
					if ((str_obdh_hk.us_brh_flag & BRH_FLAG_BR_ST_TO) && (str_obdh_hk.us_brh_flag & BRH_FLAG_DET_ON)){
						//Set Detumbling mode desired output to OFF (for repetition purpose, in case it failed)
						str_obdh_hk.us_brh_flag &= ~BRH_FLAG_DET_RS;

						//Set waiting flag
						str_obdh_hk.us_brh_flag |= BRH_FLAG_DET_WT;
						
						//Clear stabilizing flag (stabilizing time is over)
						str_obdh_hk.us_brh_flag &= ~BRH_FLAG_STABILIZING;

						//Attempt to set Detumbling mode to OFF immediately.
						#if (DEBUGGING_FEATURE == 1)
						l_us_error_code = adcsSetMode(ADCS_MODE_IDLE,0,0);
						#else
						l_us_error_code = adcsSetMode(ADCS_MODE_IDLE,0);
						#endif

						//If this fail, increase the retry counter, and if it is successful, reset it.
						if (l_us_error_code) g_uc_brh_retry_counter++;
						else g_uc_brh_retry_counter = 0;						
					}

					//If timeout does not occur and Detumbling mode is ON
					//Monitor its current state, handle if there is any undesired state
					else if (!(str_obdh_hk.us_brh_flag & BRH_FLAG_BR_ST_TO) && (str_obdh_hk.us_brh_flag & BRH_FLAG_DET_ON)){
						//Check stabilizing flag: if stabilizing flag is set, then this is during stabilization
						if (str_obdh_hk.us_brh_flag & BRH_FLAG_STABILIZING){
							//If this is stabilizing state, Detumbling mode should be ON so long as BR_LOW is not set
							//However, if BR_LOW is set, Detumbling mode should be turned OFF, and stabilizing time is over
							if (str_obdh_hk.us_brh_flag & BRH_FLAG_BR_LOW){
								//Set Detumbling mode desired output to OFF (for repetition purpose, in case it failed)
								str_obdh_hk.us_brh_flag &= ~BRH_FLAG_DET_RS;
		
								//Set waiting flag
								str_obdh_hk.us_brh_flag |= BRH_FLAG_DET_WT;
								
								//Clear stabilizing flag (stabilizing time is over, man)
								str_obdh_hk.us_brh_flag &= ~BRH_FLAG_STABILIZING;

								//Attempt to set Detumbling mode to OFF immediately.
								#if (DEBUGGING_FEATURE == 1)
								l_us_error_code = adcsSetMode(ADCS_MODE_IDLE,0,0);
								#else
								l_us_error_code = adcsSetMode(ADCS_MODE_IDLE,0);
								#endif
		
								//If this fail, increase the retry counter, and if it is successful, reset it.
								if (l_us_error_code) g_uc_brh_retry_counter++;
								else g_uc_brh_retry_counter = 0;						
							}
						}

						//Else, this is not stabilizing state
						else{							
							//This is very funny, because if Detumbling mode is ON, we should be in stabilizing state. 
							//Unknown state, very dangerous to let Detumbling mode ON, turn it OFF!
							//Set Detumbling mode desired output to OFF (for repetition purpose, in case 1st time it failed)
							str_obdh_hk.us_brh_flag &= ~BRH_FLAG_DET_RS;
	
							//Set waiting flag
							str_obdh_hk.us_brh_flag |= BRH_FLAG_DET_WT;
							
							//Attempt to set Detumbling mode to OFF immediately.
							#if (DEBUGGING_FEATURE == 1)
							l_us_error_code = adcsSetMode(ADCS_MODE_IDLE,0,0);
							#else
							l_us_error_code = adcsSetMode(ADCS_MODE_IDLE,0);
							#endif
	
							//If this fail, increase the retry counter, and if it is successful, reset it.
							if (l_us_error_code) g_uc_brh_retry_counter++;
							else g_uc_brh_retry_counter = 0;						
						}
					}

					//If timeout does not occur and Detumbling mode is OFF
					//Monitor its current state, handle if there is any undesired state
					else if (!(str_obdh_hk.us_brh_flag & BRH_FLAG_BR_ST_TO) && !(str_obdh_hk.us_brh_flag & BRH_FLAG_DET_ON)){
						//Check stabilizing flag: if stabilizing flag is set, then this is during stabilization
						if (str_obdh_hk.us_brh_flag & BRH_FLAG_STABILIZING){
							//This is undesired, if for whatsoever reason Detumbling mode is OFF though it is desired to be ON
							//Set Detumbling mode desired output to ON (for repetition purpose, in case 1st time it failed)
							str_obdh_hk.us_brh_flag |= BRH_FLAG_DET_RS;
	
							//Set waiting flag
							str_obdh_hk.us_brh_flag |= BRH_FLAG_DET_WT;

							//Set error code as error init
							l_us_error_code = EC_INIT;
							
							//Attempt to set Detumbling mode to ON immediately if ADS has been ON for at least TIME_DELAY_ACS_TO_ON_AFTER_ADS_ON_S second(s).
							if (g_ul_ads_uptime_s >= TIME_DELAY_ACS_TO_ON_AFTER_ADS_ON_S){
								#if (DEBUGGING_FEATURE == 1)
								l_us_error_code = adcsSetMode(g_uc_desired_adcs_mode,str_obdh_par.ul_det_uptime_limit_s/10,0);
								#else
								l_us_error_code = adcsSetMode(g_uc_desired_adcs_mode,str_obdh_par.ul_det_uptime_limit_s/10);
								#endif
							}
	
							//If this fail, increase the retry counter, and if it is successful, reset it.
							if (l_us_error_code && l_us_error_code != EC_INIT) g_uc_brh_retry_counter++;
							else g_uc_brh_retry_counter = 0;						
						}

						//Else, this is not stabilizing state
						else{
							//If this is not stabilizing state, then we can see what is going on
							//If BR_HI is set, try to stabilize the satellite (turn ON detumbling mode). Else, leave it alone
							if (str_obdh_hk.us_brh_flag & BRH_FLAG_BR_HI){
								//Set detumbling mode desired output to ON (for repetition purpose, in case 1st time it failed)
								str_obdh_hk.us_brh_flag |= BRH_FLAG_DET_RS;
		
								//Set waiting flag
								str_obdh_hk.us_brh_flag |= BRH_FLAG_DET_WT;
								
								//Set stabilizing flag
								str_obdh_hk.us_brh_flag |= BRH_FLAG_STABILIZING;
								
								//Set error code as error init
								l_us_error_code = EC_INIT;
								
								//Attempt to set detumbling mode to ON immediately
								if (g_ul_ads_uptime_s >= TIME_DELAY_ACS_TO_ON_AFTER_ADS_ON_S){
									#if (DEBUGGING_FEATURE == 1)
									l_us_error_code = adcsSetMode(g_uc_desired_adcs_mode,str_obdh_par.ul_det_uptime_limit_s/10,0);
									#else
									l_us_error_code = adcsSetMode(g_uc_desired_adcs_mode,str_obdh_par.ul_det_uptime_limit_s/10);
									#endif
								}
		
								//If this fail, increase the retry counter, and if it is successful, reset it.
								if (l_us_error_code && l_us_error_code != EC_INIT) g_uc_brh_retry_counter++;
								else g_uc_brh_retry_counter = 0;						
							}
						}
					}

					//If timeout occurs and detumbling mode is OFF, it is always a desired state, do nothing
				}
			}
			//But if we are waiting detumbling mode to be in the desired state and there is 
			//time out flag, clear the timeout flag and the timer, but increase the timeout retry counter
			else if ((str_obdh_hk.us_brh_flag & BRH_FLAG_DET_WT) && (str_obdh_hk.us_brh_flag & BRH_FLAG_DET_TO)){
				//Reset waiting time
				g_uc_brh_wait_det_time_s = 0;

				//Reset timeout flag
				str_obdh_hk.us_brh_flag &= ~BRH_FLAG_DET_TO;

				//Increase timeout retry counter
				g_uc_brh_timeout_retry_counter++;

				//Set error code as error init
				l_us_error_code = EC_INIT;

				//Get what is the reference state of detumbling mode again
				if (str_obdh_hk.us_brh_flag & BRH_FLAG_DET_RS){
					//Only turn ON detumbling mode if ADS has been ON for at least 10s
					if (g_ul_ads_uptime_s >= TIME_DELAY_ACS_TO_ON_AFTER_ADS_ON_S){
						//Attempt to set detumbling mode to the desired state again immediately
						#if (DEBUGGING_FEATURE == 1)
						l_us_error_code = adcsSetMode(g_uc_desired_adcs_mode,str_obdh_par.ul_det_uptime_limit_s/10,0);
						#else
						l_us_error_code = adcsSetMode(g_uc_desired_adcs_mode,str_obdh_par.ul_det_uptime_limit_s/10);
						#endif
					}
				}
				else {
					//Attempt to set detumbling mode to the desired state again immediately
					#if (DEBUGGING_FEATURE == 1)
					l_us_error_code = adcsSetMode(ADCS_MODE_IDLE,0,0);
					#else
					l_us_error_code = adcsSetMode(ADCS_MODE_IDLE,0);
					#endif
				}

				//If this fail, increase the retry counter, and if it is successful, reset it.
				if (l_us_error_code && l_us_error_code != EC_INIT) g_uc_brh_retry_counter++;
				else g_uc_brh_retry_counter = 0;
			}
			//If for any reason an unexpected state occur, recover it
			else if (!(str_obdh_hk.us_brh_flag & BRH_FLAG_DET_WT) && (str_obdh_hk.us_brh_flag & BRH_FLAG_DET_TO)){
				//Reset waiting time
				g_uc_brh_wait_det_time_s = 0;

				//Reset timeout flag
				str_obdh_hk.us_brh_flag &= ~BRH_FLAG_DET_TO;

				//Reset timeout retry counter
				g_uc_brh_timeout_retry_counter = 0;
			}
				
		}

		//If at any point of time, reset counter goes beyond the limit
		if (g_uc_brh_retry_counter > BRH_RETRY_FAIL_LIMIT || g_uc_brh_timeout_retry_counter > BRH_TIMEOUT_RETRY_FAIL_LIMIT){
			//Reset retry counter
			g_uc_brh_retry_counter = 0;
			g_uc_brh_timeout_retry_counter = 0;

			//Reset waiting time
			g_uc_brh_wait_det_time_s = 0;

			//Clear unrelevant (transitional) flags
			str_obdh_hk.us_brh_flag &= ~BRH_FLAG_DET_WT;
			str_obdh_hk.us_brh_flag &= ~BRH_FLAG_DET_TO;
			str_obdh_hk.us_brh_flag &= ~BRH_FLAG_STABILIZING;

			//Set error flag
			str_obdh_hk.us_brh_flag |= BRH_FLAG_ERROR;
		}

	}

	//Readings always going on whether BRH is enabled or not, so long as this function is called
	//At any point of time, if the satellite is found to be having low body rate, considers it as stable
	if (str_obdh_hk.us_brh_flag & BRH_FLAG_BR_LOW)								
		//Stabilizing is completed successfully, set the successful flag
		str_obdh_hk.us_brh_flag |= BRH_FLAG_ST_COMPLETED;

	//At any point of time, if the satellite is found to be having high body rate, considers it as unstable
	if (str_obdh_hk.us_brh_flag & BRH_FLAG_BR_HI)								
		//Stabilizing is completed successfully, set the successful flag
		//Ian (2012-05-15): The case for BRH_FLAG_BR_HI and BRH_FLAG_BR_LOW is not made as an either-OR to prevent oscillatory behavior.
		str_obdh_hk.us_brh_flag &= ~BRH_FLAG_ST_COMPLETED;

}
#endif
#endif
#endif

#if (IDLE_MODE_HANDLER_ENABLE == 1)
#if (PWRS_CMD_ENABLE == 1)
//------------------------------------------------------------------------------------
// To control satellite state during idle mode
//------------------------------------------------------------------------------------
// Just like bodyRateHandler, this handler assumes that PWRS board is okay
// if PWRS board is not okay, this handler will not be working at all.
//
// In addition, idle mode handler only works straightforwardly when bodyRateHandler
// is disabled.
//------------------------------------------------------------------------------------
void idleModeHandler (void)
{	
	unsigned short l_us_error_code = EC_INIT;
	unsigned char l_uc_desired_state;

	//In the idle mode, so long as bodyRateHandler is not enabled the ADS
	//and ACS channels must be turned OFF
	if (!(str_obdh_hk.us_brh_flag & BRH_FLAG_ENABLE)){

		//Only check if it is time to check
		if ((str_obdh_hk.us_imh_flag & IMH_CHECK_FILTER) == IMH_CHECK_CONDITION){
			//Reset check flag
			str_obdh_hk.us_imh_flag &= ~IMH_FLAG_CHECK;
	
			//If there is an error at this state, try to recover first
			if (str_obdh_hk.us_imh_flag & IMH_FLAG_ERROR){
				//Attempt to communicate with PWRS
				#if (DEBUGGING_FEATURE == 1)
				l_us_error_code = pwrsGetData((unsigned char)DATA_PWRS_US_FIRMWARE_VER, 0);
				#else
				l_us_error_code = pwrsGetData((unsigned char)DATA_PWRS_US_FIRMWARE_VER);
				#endif
	
				//If there is no error, then attempt to recover is successful, reset the error flag
				//And the error counter
				if(!l_us_error_code) {
					str_obdh_hk.us_imh_flag &= ~IMH_FLAG_ERROR;
					g_uc_imh_retry_counter = 0;
					g_uc_imh_acs_timeout_retry_counter = 0;
					g_uc_imh_ads_timeout_retry_counter = 0;
				}
			}
	
			//If there is no error at this point of time, we can start considering the current state of ADS
			if (!(str_obdh_hk.us_imh_flag & IMH_FLAG_ERROR)){
				//ADS case
				//Check if timeout flag is not set
				if (!(str_obdh_hk.us_imh_flag & IMH_FLAG_ADS_TO)){
					//Check whether what we desire state has been achieved, if not, just skip this check
					if ((str_obdh_hk.us_imh_flag & IMH_FLAG_ADS_WT) && (((str_obdh_hk.us_imh_flag & IMH_FLAG_ADS_ON) && (str_obdh_hk.us_imh_flag & IMH_FLAG_ADS_RS)) || (!(str_obdh_hk.us_imh_flag & IMH_FLAG_ADS_ON) && !(str_obdh_hk.us_imh_flag & IMH_FLAG_ADS_RS)))){
						//Drop waiting & timeout flags
						str_obdh_hk.us_imh_flag &= ~IMH_FLAG_ADS_WT;
						str_obdh_hk.us_imh_flag &= ~IMH_FLAG_ADS_TO;
	
						//Reset waiting time and timeout retry counter
						g_uc_imh_wait_ads_time_s = 0;
						g_uc_imh_ads_timeout_retry_counter = 0;
					}

					//If currently we are not waiting for something to happen (ADS has achieved its desired state)
					if (!(str_obdh_hk.us_imh_flag & IMH_FLAG_ADS_WT)){
						//If ADS is ON, turns it off
						if (str_obdh_hk.us_imh_flag & IMH_FLAG_ADS_ON){
							//Set ADS desired output to OFF (for repetition purpose, in case it failed)
							str_obdh_hk.us_imh_flag &= ~IMH_FLAG_ADS_RS;
	
							//Set waiting flag
							str_obdh_hk.us_imh_flag |= IMH_FLAG_ADS_WT;
							
							//Attempt to set ADS to OFF in 50ms
							#if (DEBUGGING_FEATURE == 1)
							l_us_error_code = pwrsSetSingleOutput(ADS_CHANNEL,CHANNEL_OFF,TIME_LIMIT_SET_RESET_CHANNEL_MS,0);
							#else
							l_us_error_code = pwrsSetSingleOutput(ADS_CHANNEL,CHANNEL_OFF,TIME_LIMIT_SET_RESET_CHANNEL_MS);
							#endif
	
							//If this fail, increase the retry counter, and if it is successful, reset it.
							if (l_us_error_code) g_uc_imh_retry_counter++;
							else g_uc_imh_retry_counter = 0;						
						}
						//Else, it is desired
					}
				}
	
				//But if we are waiting ADS to be in the desired state and there is 
				//time out flag, clear the timeout flag and the timer, but increase the timeout retry counter
				else if ((str_obdh_hk.us_imh_flag & IMH_FLAG_ADS_WT) && (str_obdh_hk.us_imh_flag & IMH_FLAG_ADS_TO)){
					//Reset waiting time
					g_uc_imh_wait_ads_time_s = 0;
	
					//Reset timeout flag
					str_obdh_hk.us_imh_flag &= ~IMH_FLAG_ADS_TO;
	
					//Increase timeout retry counter
					g_uc_imh_ads_timeout_retry_counter++;
	
					//Get what is the reference state of ADS again
					if (str_obdh_hk.us_imh_flag & IMH_FLAG_ADS_RS)
						l_uc_desired_state = CHANNEL_ON;
					else l_uc_desired_state = CHANNEL_OFF;
	
					//Attempt to set ADS to the desired state again in 50ms
					#if (DEBUGGING_FEATURE == 1)
					l_us_error_code = pwrsSetSingleOutput(ADS_CHANNEL,l_uc_desired_state,TIME_LIMIT_SET_RESET_CHANNEL_MS,0);
					#else
					l_us_error_code = pwrsSetSingleOutput(ADS_CHANNEL,l_uc_desired_state,TIME_LIMIT_SET_RESET_CHANNEL_MS);
					#endif
	
					//If this fail, increase the retry counter, and if it is successful, reset it.
					if (l_us_error_code) g_uc_imh_retry_counter++;
					else g_uc_imh_retry_counter = 0;
				}
				//If for any reason an unexpected state occur, recover it
				else if (!(str_obdh_hk.us_imh_flag & IMH_FLAG_ADS_WT) && (str_obdh_hk.us_imh_flag & IMH_FLAG_ADS_TO)){
					//Reset waiting time
					g_uc_imh_wait_ads_time_s = 0;
	
					//Reset timeout flag
					str_obdh_hk.us_imh_flag &= ~IMH_FLAG_ADS_TO;
	
					//Reset timeout retry counter
					g_uc_imh_acs_timeout_retry_counter = 0;
				}
			}
	
			//If at any point of time, reset counter goes beyond the limit
			if (g_uc_imh_retry_counter > IMH_RETRY_FAIL_LIMIT || g_uc_imh_acs_timeout_retry_counter > IMH_ACS_TIMEOUT_RETRY_FAIL_LIMIT || g_uc_imh_ads_timeout_retry_counter > IMH_ADS_TIMEOUT_RETRY_FAIL_LIMIT){
				//Reset retry counter
				g_uc_imh_retry_counter = 0;
				g_uc_imh_acs_timeout_retry_counter = 0;
				g_uc_imh_ads_timeout_retry_counter = 0;
	
				//Reset waiting time
				g_uc_imh_wait_acs_time_s = 0;
				g_uc_imh_wait_ads_time_s = 0;
	
				//Clear unrelevant (transitional) flags
				str_obdh_hk.us_imh_flag &= ~IMH_FLAG_ADS_WT;
				str_obdh_hk.us_imh_flag &= ~IMH_FLAG_ADS_TO;
	
				//Set error flag
				str_obdh_hk.us_imh_flag |= IMH_FLAG_ERROR;
			}
		}
	}
}
#endif
#endif
#endif

#if (SATELLITE_MODE_HANDLER_ENABLE == 1)
#if (SAFE_MODE_HANDLER_ENABLE == 1)
#if (ADCS_CMD_ENABLE == 1)
#if (PWRS_CMD_ENABLE == 1)
//------------------------------------------------------------------------------------
// To handle state during Safe-Mode (M4 or M8)
//------------------------------------------------------------------------------------
void safeModeHandler (void)
{
	unsigned short l_us_error_code = EC_SUCCESSFUL;

	//Check case 1, compare SoC, only done once
	if (g_ul_smh_time_register >= str_obdh_par.us_rev_time_s && !(str_obdh_hk.uc_smh_flag & 0x01)){
		//Record current SoC
		g_uc_soc_after_one_orbit_in_safe_mode = str_obdh_data.uc_batt_soc;

		//Record comparison as has been conducted
		str_obdh_hk.uc_smh_flag |= 0x01;

		//Trigger comparison reading (once)
		str_obdh_hk.uc_smh_flag |= 0x02;
	}

	//If comparison has been conducted, get the comparison result once
	if (str_obdh_hk.uc_smh_flag & 0x02){
		//Clear comparison reading flag
		str_obdh_hk.uc_smh_flag &= ~0x02;
		
		//If the result shows negative result
		if (g_uc_soc_after_one_orbit_in_safe_mode < g_uc_soc_reading_when_entering_safe_mode)
			//Gives negative comparison result
			str_obdh_hk.uc_smh_flag |= 0x04;
		else 
			//Gives positive comparison result
			str_obdh_hk.uc_smh_flag |= 0x08;
	}

	#if (SATELLITE_SCHEDULE_HANDLER_ENABLE == 1)
	//If the comparison result is negative
	if (str_obdh_hk.uc_smh_flag & 0x04){
		#if (EVENT_HANDLER_ENABLE == 1)
		#if (STORING_ENABLE == 1)
		//Save SMH event 20120224
		eventHandlerSafeholdMode();
		#endif
		#endif

		//Schedule to turn ON ADCS in the next 10 sec
		scheduleTurnOnADCS(str_obdh_data.ul_obc_time_s+10);

		#if (BODY_RATE_HANDLER_ENABLE == 1)
		#if (ADCS_CMD_ENABLE == 1)
		#if (PWRS_CMD_ENABLE == 1)
		//Re-enabling BRH
		reenableBRH();
		#endif
		#endif
		#endif

        #if (IDLE_MODE_HANDLER_ENABLE == 1)
        #if (PWRS_CMD_ENABLE == 1)
		//Disable IMH
		str_obdh_hk.us_imh_flag &= ~IMH_FLAG_ENABLE;
		#endif
		#endif

		//Update negative comparison as being handled
		str_obdh_hk.uc_smh_flag &= ~0x04;

		//Waiting for timeout
		str_obdh_hk.uc_smh_flag |= 0x10;
	}
	//If the comparison result is positive, however, do nothing, just continue waiting
	#endif

	//If the negative comparison is being handled, see if ADCS has been enabled for 20 mins
	if (str_obdh_hk.uc_smh_flag & 0x10){
		#if (BODY_RATE_HANDLER_ENABLE == 1)
		#if (ADCS_CMD_ENABLE == 1)
		#if (PWRS_CMD_ENABLE == 1)
		//Do body rate control for this time being
		bodyRateHandler();
		#endif
		#endif
		#endif

		//If ADCS has been turned ON for 20 mins (+20 sec buffer), 
		if (g_ul_smh_time_register >= (str_obdh_par.us_rev_time_s+1200+20)){
			#if (EVENT_HANDLER_ENABLE == 1)
			#if (STORING_ENABLE == 1)
			//Save SMH event 20120224
			eventHandlerSafeholdMode();
			#endif
			#endif

			#if (BODY_RATE_HANDLER_ENABLE == 1)
			#if (ADCS_CMD_ENABLE == 1)
			#if (PWRS_CMD_ENABLE == 1)
			//Disable bodyRateHandler
			str_obdh_hk.us_brh_flag &= ~BRH_FLAG_ENABLE;
			#endif
			#endif
			#endif

	        #if (IDLE_MODE_HANDLER_ENABLE == 1)
	        #if (PWRS_CMD_ENABLE == 1)
			//Enable IMH again
			str_obdh_hk.us_imh_flag |= IMH_FLAG_ENABLE;
			#endif
			#endif

			//Update negative comparison as has been passing first fail case (time limit)
			str_obdh_hk.uc_smh_flag &= ~0x10;
			str_obdh_hk.uc_smh_flag |= 0x20;
		}
	}

	//If at any point of time, SoC has dropped far too low, turn OFF COMM
	if (str_obdh_data.uc_batt_soc < str_obdh_par.uc_soc_threshold[4] && !(str_obdh_hk.uc_smh_flag & 0xC0) && g_ul_smh_time_register >= TIME_LIMIT_SAT_ON_AFTER_LAST_EFFORT_RESET){
		//Attempt to set COMM to OFF in 50ms (no error code is returned, keep retrying)
		#if (DEBUGGING_FEATURE == 1)
		l_us_error_code = pwrsSetSingleOutput(COMM_CHANNEL,CHANNEL_OFF,TIME_LIMIT_SET_RESET_CHANNEL_MS,0);
		#else
		l_us_error_code = pwrsSetSingleOutput(COMM_CHANNEL,CHANNEL_OFF,TIME_LIMIT_SET_RESET_CHANNEL_MS);
		#endif

		//If there is no error code, proceeds
		if (!l_us_error_code){
			//Update safe mode flag as has been issuing command to turn OFF COMM
			str_obdh_hk.uc_smh_flag |= 0x40;
	
			//Safe current safe-mode time
			g_ul_smh_saved_time = str_obdh_data.ul_obc_time_s;
		}
		#if (EVENT_HANDLER_ENABLE == 1)
		#if (STORING_ENABLE == 1)
		//Save SMH event 20120224
		eventHandlerSafeholdMode();
		#endif
		#endif

	}
	

	//If command to turn OFF COMM has been issue, 
	if (str_obdh_hk.uc_smh_flag & 0x40){
		//Check if 15 sec has passed 
		if (str_obdh_data.ul_obc_time_s >= g_ul_smh_saved_time+15){
			//If COMM has been turned OFF within this 15 s, update the status as COMM turned OFF
			if (!g_us_latest_ch_stat_update_ec && !(str_pwrs_data.uc_channel_status & COMM_CHANNEL_BIT)){
				str_obdh_hk.uc_smh_flag &= ~0x40;
				str_obdh_hk.uc_smh_flag |= 0x80;
			}
			//Else, retry to turn OFF COMM again
			else str_obdh_hk.uc_smh_flag &= ~0x40;
		}
	}

	#if (EXTRA_ENABLE == 1)
	//If at any point of time, SoC is dropped to less than 10% and satellite reset has not been initialized
	//And it has been at least 10 minutes in the safemode
	//Prepare to reset the satellite...
	if ((str_obdh_data.uc_batt_soc < str_obdh_par.uc_soc_threshold[6]) && (!(g_uc_evha_flag & EVENT_FLAG_RESET_SAT_INITIALIZED)) && g_ul_smh_time_register >= TIME_LIMIT_SAT_ON_AFTER_LAST_EFFORT_RESET){
		//Need to do something here...
		//Mark reset cause as RESET_SOURCE_SAFEHOLD 
		str_obdh_data.uc_sat_rstsrc = RESET_SOURCE_SAFEHOLD;

		//Trigger satellite's reset
		callReset(OBDH_CMD_RESET_SAT);
	}
	#endif

}
#endif
#endif
#endif
#endif

#if (SATELLITE_MODE_HANDLER_ENABLE == 1)
//------------------------------------------------------------------------------------
// Normal procedure to enter M3 (unique procedure is excluded)
//------------------------------------------------------------------------------------
void enterM3 (void)
{
	//Reset idle time register
	g_ul_idle_time_register = 0;

	#if (SATELLITE_SCHEDULE_HANDLER_ENABLE == 1)
	//Schedule to turn ON ADS 2 seconds later
	scheduleSingleChannel(ADS_CHANNEL,CHANNEL_ON,str_obdh_data.ul_obc_time_s+2);
	#endif

	#if (IDLE_MODE_HANDLER_ENABLE == 1)
	#if (PWRS_CMD_ENABLE == 1)
	//Disable idleModeHandler
	str_obdh_hk.us_imh_flag &= ~IMH_FLAG_ENABLE;
	#endif
	#endif

	#if (BODY_RATE_HANDLER_ENABLE == 1)
	#if (ADCS_CMD_ENABLE == 1)
	#if (PWRS_CMD_ENABLE == 1)
	//Re-enabling BRH
	reenableBRH();
	#endif
	#endif
	#endif

	//Terminate whatever mission we have
	g_us_mission_time_register = 0;

	#if (EVENT_HANDLER_ENABLE == 1)
	#if (STORING_ENABLE == 1)
    //yc added 20120224
	eventHandlerUpdateStateMode (SATELLITE_MODE_M3);
	#endif
	#endif
	//Set ADCS desired state as ADCS_MODE_DETUMBLE
	g_uc_desired_adcs_mode = ADCS_MODE_DETUMBLE;

	//Change satellite mode into M3
	str_obdh_hk.uc_sat_mode = SATELLITE_MODE_M3;
}

//------------------------------------------------------------------------------------
// Normal procedure to enter M4 (unique procedure is excluded)
//------------------------------------------------------------------------------------
void enterM4 (void)
{
	#if (BODY_RATE_HANDLER_ENABLE == 1)
	#if (ADCS_CMD_ENABLE == 1)
	#if (PWRS_CMD_ENABLE == 1)
	//Disable bodyRateHandler
	str_obdh_hk.us_brh_flag &= ~BRH_FLAG_ENABLE;
	#endif
	#endif
	#endif

	#if (IDLE_MODE_HANDLER_ENABLE == 1)
	#if (PWRS_CMD_ENABLE == 1)
	//Enable idleModeHandler
	str_obdh_hk.us_imh_flag |= IMH_FLAG_ENABLE;
	#endif
	#endif

	//Record SoC
	g_uc_soc_reading_when_entering_safe_mode = str_obdh_data.uc_batt_soc;

	#if (SAFE_MODE_HANDLER_ENABLE == 1)
	#if (ADCS_CMD_ENABLE == 1)
	#if (PWRS_CMD_ENABLE == 1)
	//Reset safe mode flag
	str_obdh_hk.uc_smh_flag = 0;
	#endif
	#endif
	#endif

	//Terminate whatever mission we have
	g_us_mission_time_register = 0;

	#if (EVENT_HANDLER_ENABLE == 1)
	#if (STORING_ENABLE == 1)
    //yc added 20120224
	eventHandlerUpdateStateMode (SATELLITE_MODE_M4);
	#endif
	#endif

	//Set ADCS desired state as ADCS_MODE_RATE_CTRL
	g_uc_desired_adcs_mode = ADCS_MODE_RATE_CTRL;

	//Change satellite mode into M4
	str_obdh_hk.uc_sat_mode = SATELLITE_MODE_M4;
}

//------------------------------------------------------------------------------------
// Normal procedure to enter M5 (unique procedure is excluded)
//------------------------------------------------------------------------------------
void enterM5 (void)
{
	//Reset idle time register
	g_ul_idle_time_register = 0;

	#if (BODY_RATE_HANDLER_ENABLE == 1)
	#if (ADCS_CMD_ENABLE == 1)
	#if (PWRS_CMD_ENABLE == 1)
	//Disable bodyRateHandler again
	str_obdh_hk.us_brh_flag &= ~BRH_FLAG_ENABLE;
	#endif
	#endif
	#endif

	#if (IDLE_MODE_HANDLER_ENABLE == 1)
	#if (PWRS_CMD_ENABLE == 1)
	//Enable idleModeHandler
	str_obdh_hk.us_imh_flag |= IMH_FLAG_ENABLE;
	#endif
	#endif

	//Terminate whatever mission we have
	g_us_mission_time_register = 0;

	#if (EVENT_HANDLER_ENABLE == 1)
	#if (STORING_ENABLE == 1)
    //yc added 20120224
	eventHandlerUpdateStateMode (SATELLITE_MODE_M5);
	#endif
	#endif

	//Set ADCS desired state as ADCS_MODE_DETUMBLE
	g_uc_desired_adcs_mode = ADCS_MODE_DETUMBLE;

	//Change satellite mode into M5
	str_obdh_hk.uc_sat_mode = SATELLITE_MODE_M5;
}

//------------------------------------------------------------------------------------
// Normal procedure to enter M8 (unique procedure is excluded)
//------------------------------------------------------------------------------------
void enterM8 (void)
{
	#if (BODY_RATE_HANDLER_ENABLE == 1)
	#if (ADCS_CMD_ENABLE == 1)
	#if (PWRS_CMD_ENABLE == 1)
	//Disable bodyRateHandler
	str_obdh_hk.us_brh_flag &= ~BRH_FLAG_ENABLE;
	#endif
	#endif
	#endif

	#if (IDLE_MODE_HANDLER_ENABLE == 1)
	#if (PWRS_CMD_ENABLE == 1)
	//Enable idleModeHandler
	str_obdh_hk.us_imh_flag |= IMH_FLAG_ENABLE;
	#endif
	#endif

	//Record SoC
	g_uc_soc_reading_when_entering_safe_mode = str_obdh_data.uc_batt_soc;

	#if (SAFE_MODE_HANDLER_ENABLE == 1)
	#if (ADCS_CMD_ENABLE == 1)
	#if (PWRS_CMD_ENABLE == 1)
	//Reset safe mode flag
	str_obdh_hk.uc_smh_flag = 0;
	#endif
	#endif
	#endif

	//Terminate whatever mission we have
	g_us_mission_time_register = 0;

	#if (EVENT_HANDLER_ENABLE == 1)
	#if (STORING_ENABLE == 1)
    //yc added 20120224
    eventHandlerUpdateStateMode (SATELLITE_MODE_M8);
	#endif
	#endif

	//Set ADCS desired state as ADCS_MODE_RATE_CTRL
	g_uc_desired_adcs_mode = ADCS_MODE_RATE_CTRL;

	//Change satellite mode into M8
	str_obdh_hk.uc_sat_mode = SATELLITE_MODE_M8;
}
#endif

#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)
//------------------------------------------------------------------------------------
// Normal procedure to enter M6 (unique procedure is excluded)
//------------------------------------------------------------------------------------
void enterGroundPass (void)
{
	//Reset transitional state monitoring period
	g_us_gsch_monitoring_transitional_period = 0;

	//Start groundpass state
	g_uc_gsch_flag |= GSCH_FLAG_GROUNDPASS_STARTED;

	//If the satellite mode is currently on M5, M7, or M8 change it to M6, record the previous mode
	if (str_obdh_hk.uc_sat_mode == SATELLITE_MODE_M5 || str_obdh_hk.uc_sat_mode == SATELLITE_MODE_M7 || str_obdh_hk.uc_sat_mode == SATELLITE_MODE_M8){
		str_obdh_data.uc_previous_mode = str_obdh_hk.uc_sat_mode;

		#if (EVENT_HANDLER_ENABLE == 1)
		#if (STORING_ENABLE == 1)
	    //yc added 20120224
		eventHandlerUpdateStateMode (SATELLITE_MODE_M6);
		#endif
		#endif
		 
		str_obdh_hk.uc_sat_mode = SATELLITE_MODE_M6;
	}

	//Set GSCH state to groundpass
	str_obdh_hk.uc_gsch_state = GSCH_STATE_GROUND_PASS;
	
}
#endif

#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)
//------------------------------------------------------------------------------------
// Reset GSCH counters
//------------------------------------------------------------------------------------
void resetGSCHCounters (void)
{
	//Reset some GSCH common counters
	g_uc_gsch_comm_retry_counter = 0;
	g_uc_gsch_loading_retry_counter = 0;
	g_us_gsch_monitoring_period = 0;
}
#endif

#if (SATELLITE_MODE_HANDLER_ENABLE == 1)
#if (BODY_RATE_HANDLER_ENABLE == 1)
#if (ADCS_CMD_ENABLE == 1)
#if (PWRS_CMD_ENABLE == 1)
//------------------------------------------------------------------------------------
// To re-enable BRH
//------------------------------------------------------------------------------------
void reenableBRH (void)
{
	//Enable bodyRateHandler flag
	str_obdh_hk.us_brh_flag |= BRH_FLAG_ENABLE;

	//Reset BRH end flags
	resetBRHEndFlags();

	//Reset Body Rate values
	resetBodyRateValue();
}

//------------------------------------------------------------------------------------
// Reset BRH end flags
//------------------------------------------------------------------------------------
void resetBRHEndFlags (void)
{
	//Reset its stabilizing, timeout, and complete flag
	str_obdh_hk.us_brh_flag &= ~BRH_FLAG_STABILIZING;
	str_obdh_hk.us_brh_flag &= ~BRH_FLAG_BR_ST_TO;
	str_obdh_hk.us_brh_flag &= ~BRH_FLAG_ST_COMPLETED;
}
#endif
#endif
#endif
#endif
