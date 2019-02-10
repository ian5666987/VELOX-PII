#ifndef luxp_schedule_h
#define luxp_schedule_h

#include <luxp_consvar.h>
#include <luxp_add.h>
#include <luxp_check.h>
#include <luxp_time.h>
#include <luxp_struct.h>

#if (SATELLITE_SCHEDULE_HANDLER_ENABLE == 1)
//Scheduling functions
void rearrangeSchedule (void);
void swapScheduleIndex (unsigned char l_uc_index_1, unsigned char l_uc_index_2);
void removeSchedule (unsigned char l_uc_removal_start_point, unsigned char l_uc_removal_end_point);
unsigned short addNewSchedule (void);

//Scheduled functions
unsigned short scheduleTurnOnADCS (unsigned long l_ul_scheduled_time);
unsigned short scheduleSingleChannel (unsigned char l_uc_channel, unsigned char l_uc_state, unsigned long l_ul_scheduled_time);
unsigned short scheduleMissionMode (unsigned long l_ul_start_time_tai, unsigned short l_us_elapsed_time);
unsigned short scheduleResetPWRSOBDH (unsigned long l_ul_scheduled_time, unsigned char l_uc_subsystem);
unsigned short scheduleOffAllSubsystems (unsigned long l_ul_scheduled_time);
void scheduleLogData (void);		//Scheduled time is taken from GSCH argument directly, no need to have an input parameter
void scheduleSunTracking (void);	//Scheduled time is taken from GSCH argument directly, no need to have an input parameter
#endif

#endif