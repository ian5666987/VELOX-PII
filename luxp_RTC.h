#ifndef luxp_RTC_h
#define luxp_RTC_h

#include <luxp_consvar.h>
#include <luxp_RW.h>
#include <luxp_add.h>
#include <luxp_storing.h>
#include <luxp_time.h>

#if (RTC_CMD_ENABLE == 1)
//RTC Functions
#if (DEBUGGING_FEATURE == 1)
unsigned short rtcGetTime (unsigned char l_uc_record_time_par);
unsigned short rtcUpdateDate (unsigned char l_uc_update_s, unsigned char l_uc_update_min, unsigned char l_uc_update_hour, unsigned char l_uc_update_day_of_week, unsigned char l_uc_update_day_of_month, unsigned char l_uc_update_month, unsigned char l_uc_update_year, unsigned char l_uc_record_time_par);
unsigned short rtcDateInit (unsigned char l_uc_record_time_par);
#else
unsigned short rtcGetTime (void);
unsigned short rtcUpdateDate (unsigned char l_uc_update_s, unsigned char l_uc_update_min, unsigned char l_uc_update_hour, unsigned char l_uc_update_day_of_week, unsigned char l_uc_update_day_of_month, unsigned char l_uc_update_month, unsigned char l_uc_update_year);
unsigned short rtcDateInit (void);
#endif
#endif

#endif