#ifndef luxp_struct_h
#define luxp_struct_h

#include <luxp_consvar.h>

//-----------------------------------------------------------------------------
// Structure for IMC parameters
//-----------------------------------------------------------------------------
struct IMCHKStruct {
	unsigned char 	uc_receiver_no_of_frame;
	unsigned short 	us_doppler_offset;
	unsigned short 	us_receiver_signal_strength;
	unsigned short 	us_transmitter_reflected_power;
	unsigned short 	us_transmitter_forward_power;
	unsigned short 	us_transmitter_current_consumption;
	unsigned short 	us_receiver_current_consumption;
	unsigned short 	us_power_amplifier_temperature;
	unsigned short 	us_power_bus_voltage;
	unsigned char 	uc_receiver_uptime_minutes;
	unsigned char 	uc_receiver_uptime_hours;
	unsigned char 	uc_receiver_uptime_days;
};

//-----------------------------------------------------------------------------
// Structure for ITC parameters
//-----------------------------------------------------------------------------
struct ITCHKStruct {
	unsigned char 	uc_transmitter_uptime_minutes;
	unsigned char 	uc_transmitter_uptime_hours;
	unsigned char 	uc_transmitter_uptime_days;
	unsigned char 	uc_transmitter_state;
};

//-----------------------------------------------------------------------------
// Structure for antenna housekeeping
//-----------------------------------------------------------------------------
struct AntennaHKStruct {
	unsigned short	us_deployment_status;
	unsigned short	us_temp;
	unsigned char	uc_actv_count[NO_OF_ANTENNA];
	unsigned char	uc_actv_time[NO_OF_ANTENNA];
};

//-----------------------------------------------------------------------------
// Structure for RTC parameters
//-----------------------------------------------------------------------------
struct RTCDataStruct {
	unsigned char 	uc_ths;
	unsigned char 	uc_s;
	unsigned char 	uc_min;
	unsigned char 	uc_cenh;
	unsigned char 	uc_day;
	unsigned char 	uc_date;
	unsigned char 	uc_month;
	unsigned char 	uc_year;
};

//-----------------------------------------------------------------------------
// Structure for PWRS housekeeping parameters
//-----------------------------------------------------------------------------
struct PWRSHKStruct {
	unsigned char uc_sp_temp[5];
	unsigned short us_pv_v[3];
	unsigned short us_pv_i[3];
	unsigned short us_batt_v[2];
	unsigned short us_batt_i;
	unsigned char uc_batt_soc;
	unsigned char uc_batt_temp[2];
	unsigned short us_sys_i;
	unsigned short us_pcc_v;
	unsigned short us_load_v[4];
	unsigned short us_load_i[4];
	unsigned char uc_latch_count[4];
	unsigned char uc_rst_count;
	unsigned char uc_rst_cause;
	unsigned char uc_mode;
	short s_mcu_temp;
	char c_mcu_rstsrc;
	//PWRS mode should be provided - Not provided, says PWRS subsystem
};

//-----------------------------------------------------------------------------
// Structure for PWRS version parameters
//-----------------------------------------------------------------------------
struct PWRSParStruct {
	unsigned char uc_mppt_mode;
	unsigned char uc_batt_heater;
	unsigned char uc_batt_charge_upp_limit;
	unsigned char uc_batt_charge_low_limit;
};

//-----------------------------------------------------------------------------
// Structure for PWRS data
//-----------------------------------------------------------------------------
struct PWRSDataStruct {
	unsigned char uc_channel_status;
	unsigned short us_pv_fv[3];
};

//-----------------------------------------------------------------------------
// Structure for ADCS housekeeping parameters
//-----------------------------------------------------------------------------
struct ADCSHKStruct {
	float f_sunv[3];	//x,y,z
	float f_qeb[4];		//1-4
	short s_br_eci[3];	//x,y,z
	float f_posv[3];	//x,y,z
	char c_sensor_enable;
	char c_sensor_credit;
	char c_ac_m[3];		//x,y,z
	char c_mt_i[3];		//x,y,z
	char c_actuator_enable;
	char c_mode;
	short s_mcu_temp;
	char c_mcu_rstsrc;
};

//-----------------------------------------------------------------------------
// Structure for ADCS data parameters
//-----------------------------------------------------------------------------
struct ADCSParStruct {
	short s_ss_threshold;
	float f_cgain[NO_OF_ADCS_CGAIN];
	float f_tle_epoch;
	float f_tle_inc;
	float f_tle_raan;
	float f_tle_arg_per;
	float f_tle_ecc;
	float f_tle_ma;
	unsigned short us_tle_mm;
	unsigned long ul_time;
	short s_vector_2sun[3];	//x,y,z
	char c_ctrl_br[3];		//x,y,z
	short s_dgain;
};

//-----------------------------------------------------------------------------
// Structure OBDH housekeeping data
//-----------------------------------------------------------------------------
struct OBDHHKStruct {
	unsigned char	uc_sat_mode;
	unsigned char	uc_gsch_state;	//New
	unsigned long	ul_wod_sample_count;	//New
	unsigned char 	uc_scheduler_block_size;
	unsigned short	us_beacon_flag; //New
	unsigned short	us_brh_flag; //New
	unsigned short	us_imh_flag; //New
	unsigned char	uc_smh_flag; //New
	unsigned short 	us_cw_char_rate_par; //New
	unsigned char 	uc_ax25_bit_rate_par; //New
	unsigned char	uc_mcu_rstsrc;
	unsigned char	uc_mcu_rst_count;
	short			s_mcu_temp;	//New
	unsigned long	ul_data_byte_sent;
	unsigned char	uc_sat_rstsrc; //Last sat reset source recorded (not changed until the next initilization)
};

//-----------------------------------------------------------------------------
// Structure for OBDH parameters
//-----------------------------------------------------------------------------
struct OBDHParStruct {
	unsigned char	uc_manual_control_enabled;
	unsigned char	uc_no_of_interrupt_per_s;
	unsigned char	uc_gsch_time_window_10s;
	unsigned char	uc_soc_threshold[NO_OF_SOC_THRESHOLD_PAR];	
	unsigned short	us_rev_time_s;
	unsigned short	us_log_data_id[MAX_NO_OF_DATA_TYPE_LOGGED];
	unsigned short	us_br_threshold_low[3];
	unsigned short	us_br_threshold_high[3];
	unsigned char	uc_beacon_interval[NO_OF_BEACON_INTERVAL];
	unsigned long 	ul_det_uptime_limit_s;
};

//-----------------------------------------------------------------------------
// Structure for OBDH data
//-----------------------------------------------------------------------------
struct OBDHDataStruct {
	unsigned char	uc_sat_rstsrc; //Anticipated reset source, after initialization, this is to be set as unanticipated less reset initialization is started	unsigned long	ul_obc_time_s; //New
	unsigned long	ul_obc_time_s;
	unsigned short	us_current_rev_no; //New
	unsigned short	us_rev_offset_time; //New data
	unsigned short	us_br_eci_avg[3];
	unsigned long	ul_log_data_start_time;
	unsigned short	us_log_data_duration_s;
	unsigned char	uc_no_of_data_cat_sent;
	unsigned short 	us_crc16_poly;
	unsigned char	uc_software_mode;
	unsigned short	us_interval_period[OBDH_DATA_INTERVAL_PERIOD_SIZE];
	unsigned char	uc_previous_mode;
	float			f_gs_reference_rev_no; //New
	unsigned long	ul_gs_ref_time_s; //New
	unsigned long	ul_rtc_ref_time_s; //New
	unsigned short	us_auto_reference_rev_no; //New
	unsigned short	us_gs_reference_rev_no; //New
	unsigned char	uc_orbit_no_changed_counter;	//New event flag
	unsigned char	uc_timer4_interrupt_counter;
	unsigned long	ul_latest_hk_sampling_time_s; //New
	unsigned short 	us_latest_hk_sampling_orbit; //New
	unsigned char	uc_batt_soc;
};

//-----------------------------------------------------------------------------
// Structure for block pointers
//-----------------------------------------------------------------------------
struct BPStruct {
	unsigned long ul_hk_write_p;
	unsigned long ul_datalog_write_p;
	unsigned long ul_package_recovery_write_p;
	unsigned long ul_eventlog_write_p;//dec22_yc
	unsigned char uc_orbit_indexing_overwritten;
	unsigned char uc_hk_overwritten;
};

//-----------------------------------------------------------------------------
// Structure for scheduler
//-----------------------------------------------------------------------------
// Mix of scheduler entry and scheduler in one structure is noted
//-----------------------------------------------------------------------------
struct SchedulerStruct {
	unsigned char uc_reindexing[SCHEDULER_BLOCK_SIZE];
	unsigned long ul_scheduled_time[SCHEDULER_BLOCK_SIZE];
	unsigned short us_cmd[SCHEDULER_BLOCK_SIZE];
	unsigned char uc_arg_length[SCHEDULER_BLOCK_SIZE];
	unsigned char uc_arg[SCHEDULER_BLOCK_SIZE][MAX_SCHEDULER_ARG];
	unsigned long ul_scheduled_time_register;
	unsigned short us_cmd_register;
	unsigned char uc_arg_length_register;
	unsigned char uc_arg_register[MAX_SCHEDULER_ARG]; //Scheduler's argument register
};

//-----------------------------------------------------------------------------
// Structure for task flag module indicators
//-----------------------------------------------------------------------------
// The task flag structure shows status of a task
// 1) uc_status, shows the task's status itself, contain of 8-bit flag
// 	  a) bit0 	-> Task initialization flag 
//				   '1' if the task is initiated, '0' if it is not initiated
//	  b) bit1 	-> Task running flag
//				   '1' if the task is running, '0' if the task is not running
//    c) bit2 	-> Task error flag
//				   '1' if the task contains one or more errors, '0' if it is error-free
//	  d) bit3 	-> Task completed flag
//				   '1' if the task has been completed, '0' if it is still running
//	  e) bit4 	-> Task status read flag
//				   '1' if a task status is being read, '0' if it is not being read
//	  f) bit5 	-> Task summary read flag
//				   '1' if entire task structure is being read, '0' if it is not being read
//	  g) bit6 	-> Task detail read flag
//				   '1' if task and its subtasks is being read, '0' if is is not being read
//	  h) bit7	-> Reserved task flag
//
//	  some conditions for task status are as follow:
//	  a) Unless a task is initialized it cannot be run and cannot contain error(s)
//	  b) If a task is running, it may contain errors, but cannot be completed
//	  c) If a task is completed, it cannot run, but its initialization flag is still set
//	  d) A completed task may contain error(s)
//	  e) Task read flags can be raised in any of task's condition
//	  f) All task read flags will be reset once task flags module is finished
//	
// 2) us_script_running_flags, shows what particular sub-task(s) is(are) currently running
// 3) us_script_error_flags, shows what particular sub-task(s) contain(s) error
// 4) us_script_completed_flags, shows what particular sub-task(s) has(have) completed its task without error
// 5) us_script_timer_owning_flags, shows what particular sub-task(s) own(s) timer
// 6) us_script_timer_running_flags, shows what particular sub-task(s) which own(s) timer which is running
//-----------------------------------------------------------------------------
struct TaskStruct {
	unsigned char uc_status;
	unsigned char uc_scriptspace;
	unsigned char uc_script_running_flags;
	unsigned char uc_script_error_flags;
	unsigned char uc_script_completed_flags;
	unsigned char uc_script_timer_owning_flags;
	unsigned char uc_script_timer_running_flags;
	unsigned char uc_script_retry_counter_owning_flags;
	#if (DEBUGGING_FEATURE == 1)
	unsigned char uc_status_held;	//To hold task's status whenever necessary and return task to its previous status as held
									//This is only used if DEBUGGING_FEATURE is set
	#endif
};

//-----------------------------------------------------------------------------
// Structure for scriptspace
//-----------------------------------------------------------------------------
struct ScriptspaceStruct {
	unsigned char uc_script[MAX_SCRIPTLINE_SIZE][MAX_SCRIPT_LENGTH];
};

//-----------------------------------------------------------------------------
// Structure for scriptline's timers
//-----------------------------------------------------------------------------
struct ScriptlineTimerStruct {
	unsigned char uc_scriptspace_owner;
	unsigned char uc_scriptline_owner;
	unsigned char uc_started;
	unsigned char uc_counter;
	unsigned long ul_timer;
};

//-----------------------------------------------------------------------------
// Structure for retry counter
//-----------------------------------------------------------------------------
struct ScriptlineRetryCounterStruct {
	unsigned char uc_scriptspace_owner;
	unsigned char uc_scriptline_owner;
	unsigned char uc_counter;
};

//-----------------------------------------------------------------------------
// Structure for satellite time parameters
//-----------------------------------------------------------------------------
struct SatTimeStruct {
	unsigned short 	us_ms;
	unsigned char 	uc_s;
	unsigned char 	uc_min;
	unsigned char 	uc_hour;
	unsigned short 	us_day;
};

//-----------------------------------------------------------------------------
// Structure for satellite date parameters
//-----------------------------------------------------------------------------
// The use of two structure element with similar names rather than an array 
// with dimension of two is noticed
//-----------------------------------------------------------------------------
struct SatDateStruct {
	unsigned short 	us_init_ms;
	unsigned char 	uc_init_s;
	unsigned char 	uc_init_min;
	unsigned char 	uc_init_hour;
	unsigned char 	uc_init_day_of_week;
	unsigned char 	uc_init_day_of_month;
	unsigned char 	uc_init_month;
	unsigned char 	uc_init_year;
	unsigned short 	us_current_ms;
	unsigned char 	uc_current_s;
	unsigned char 	uc_current_min;
	unsigned char 	uc_current_hour;
	unsigned char 	uc_current_day_of_week;
	unsigned char 	uc_current_day_of_month;
	unsigned char 	uc_current_month;
	unsigned char 	uc_current_year;
};

//-----------------------------------------------------------------------------
// Structure for satellite time update parameters
//-----------------------------------------------------------------------------
struct DateUpdateFromRefStruct {
	unsigned short 	us_ms;
	unsigned char 	uc_s;
	unsigned char 	uc_min;
	unsigned char 	uc_hour;
	unsigned char 	uc_day;
	unsigned char 	uc_month;
	unsigned char 	uc_year;
};

//-----------------------------------------------------------------------------
// Structure Variables
//-----------------------------------------------------------------------------
extern struct IMCHKStruct 					str_imc_hk;
extern struct ITCHKStruct 					str_itc_hk;
extern struct AntennaHKStruct				str_antenna_hk;
extern struct SatTimeStruct 				str_sat_time;
extern struct SatDateStruct 				str_sat_date;
extern struct DateUpdateFromRefStruct 		str_date_update;
extern struct RTCDataStruct 				str_rtc_data;
extern struct PWRSHKStruct					str_pwrs_hk;
extern struct PWRSParStruct					str_pwrs_par;
extern struct PWRSDataStruct				str_pwrs_data;
extern struct ADCSHKStruct					str_adcs_hk;
extern struct ADCSParStruct					str_adcs_par;
extern struct BPStruct						str_bp;
extern struct ScriptlineTimerStruct			str_scriptline_timer[SCRIPTLINE_TIMER_SIZE];
extern struct TaskStruct					str_task[TASK_SIZE];
extern struct OBDHHKStruct					str_obdh_hk;
extern struct OBDHParStruct					str_obdh_par;
extern struct OBDHDataStruct				str_obdh_data;
extern struct ScriptlineRetryCounterStruct	str_scriptline_retry_counter[SCRIPTLINE_RETRY_COUNTER_SIZE];
extern struct SchedulerStruct				str_scheduler;
extern struct ScriptspaceStruct				str_scriptspace[SCRIPTSPACE_SIZE];

#endif
