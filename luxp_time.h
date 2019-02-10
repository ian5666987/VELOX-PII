#ifndef luxp_time_h
#define luxp_time_h

#include <C8051F120.H>
#include <luxp_consvar.h>
#include <luxp_struct.h>
#include <luxp_RTC.h>
#include <luxp_add.h>

//Definite waiting function
void waitUs(unsigned char l_uc_us);	
void waitTenthMs(unsigned char l_uc_a_tenth_ms);	
void waitMs(unsigned char l_uc_milisecond);	
void waitS(unsigned char l_uc_second);	

#if (TIME_ENABLE == 1)
//Date-time conversion functions
#if (RTC_CMD_ENABLE == 1)
#if (UPDATE_HANDLER_ENABLE == 1)
unsigned short countNoOfDays (void);
unsigned long getHMSInfo(void);
#endif
unsigned char checkEndDayOfTheMonth (unsigned char l_uc_month_to_check, unsigned char l_uc_year_to_check);	

//Satellite date and time functions
#if (UPDATE_HANDLER_ENABLE == 1)
void updateSatDate (void);
#endif
#if (UPDATE_HANDLER_ENABLE == 1)
unsigned short getSatTimeFromRTC (void);
unsigned long convertSatTimeFromRTCToSec (void);
unsigned short getSatTimeFromRTCInSec (void);
#endif
#endif
void getHKTimeAndOrbit (void);
#if (RTC_CMD_ENABLE == 1)
void sec2DateUpdateFromRef (unsigned long l_ul_input_s);
#endif
#endif

#if (DEBUGGING_FEATURE == 1)
void fastRecordTimerValue (unsigned char l_uc_record_block);
void startTiming (void);
void endTiming (void);
#endif

#endif