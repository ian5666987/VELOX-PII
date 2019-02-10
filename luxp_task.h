#ifndef luxp_task_h
#define luxp_task_h

#include <luxp_consvar.h>
#include <luxp_PWRS.h>
#include <luxp_ISIS.h>
#include <luxp_ADCS.h>
#include <luxp_ground.h>
#include <luxp_send.h>

#if (TASK_ENABLE == 1)
//Main functions for task module
void 			taskModule				(unsigned char l_uc_current_task, unsigned char *l_uc_response_task_buffer_p);
void 			setTaskFlags 			(unsigned char l_uc_task_chosen, unsigned char l_uc_task_cmd, unsigned char l_uc_task_arg);
unsigned char 	getReadingFormat 		(unsigned char l_uc_task_chosen);

//Main script reading functions
unsigned short 	readScriptline 						(unsigned char l_uc_task, unsigned char l_uc_scriptline_bit, unsigned short l_us_res_length);
void 			scriptlineEndHandler 				(unsigned char l_uc_task, unsigned char l_uc_scriptline_bit, unsigned char l_uc_end_type, unsigned char l_uc_retry_counter_no);
unsigned char	checkScriptlineBit					(unsigned char l_uc_scriptline_bit);

//Task flags timers
unsigned char 	scriptlineGetTimer 					(unsigned char l_uc_task, unsigned char l_uc_scriptline_bit);
unsigned char 	scriptlineReadTimerNo 				(unsigned char l_uc_task, unsigned char l_uc_scriptline_bit);
unsigned char 	scriptlineClaimTimer				(unsigned char l_uc_task, unsigned char l_uc_scriptline_bit);
void 			scriptlineStartTimer 				(unsigned char l_uc_task, unsigned char l_uc_timer_no);
void 			scriptlineStopTimer 				(unsigned char l_uc_task, unsigned char l_uc_timer_no);
void 			scriptlineReleaseTimer 				(unsigned char l_uc_task, unsigned char l_uc_timer_no);
void 			scriptspaceReleaseAllTimer 			(unsigned char l_uc_task);
void 			scriptspaceStartAllTimer 			(unsigned char l_uc_task);
void 			scriptspaceStopAllTimer 			(unsigned char l_uc_task);
void 			resetScriptlineTimer 				(unsigned char l_uc_timer_no);

//Helpful scriptline functions
void 			enterScriptline 					(unsigned char l_uc_scriptspace, unsigned char l_uc_scriptline, unsigned char l_uc_function_code, unsigned char l_uc_property, unsigned char l_uc_end_doing, unsigned char l_uc_argument);

//Retry counter functions
unsigned char 	scriptlineGetRetryCounter 			(unsigned char l_uc_task, unsigned char l_uc_scriptline_bit);
unsigned char 	scriptlineReadRetryCounter 			(unsigned char l_uc_task, unsigned char l_uc_scriptline_bit);
unsigned char 	scriptlineClaimRetryCounter 		(unsigned char l_uc_task, unsigned char l_uc_scriptline_bit);
void 			scriptlineReleaseRetryCounter 		(unsigned char l_uc_task, unsigned char l_uc_retry_counter_no);
void 			scriptspaceReleaseAllRetryCounter 	(unsigned char l_uc_task);
#endif

//Script functions
void getHK (void);
#if (STORING_ENABLE == 1)
void storeHK (void);
#endif
#if (TASK_ENABLE == 1)
void getSSCP (void);
void initLogData (void);
void logData (void);
void terminateLogData (void);
#endif

#endif