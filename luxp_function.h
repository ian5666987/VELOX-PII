#ifndef luxp_function_h
#define luxp_function_h

#include <C8051F120.H>
#include <luxp_ISIS.h>
#include <luxp_add.h>
#include <luxp_consvar.h>
#include <luxp_RTC.h>
#include <luxp_storing.h>
#include <luxp_time.h>
#include <luxp_task.h>
#include <luxp_struct.h>
#include <luxp_schedule.h>
#include <luxp_check.h>				   
#include <luxp_ADCS.h>
#include <luxp_SD.h>
#include <luxp_PWRS.h>
#include <luxp_ground.h>
#include <luxp_send.h>
#include <luxp_debugging.h>
#include <luxp_cqueue.h>
#include <luxp_extra.h>
#include <luxp_antenna.h>
#include <luxp_main.h>

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------
unsigned short functionModule (unsigned char *l_uc_call_function_buffer_pointer, unsigned char *l_uc_response_function_buffer_pointer);
void callReset (unsigned char l_uc_reset_type);

#endif

