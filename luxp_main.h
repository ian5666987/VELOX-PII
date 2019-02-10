#ifndef luxp_main_h
#define luxp_main_h

#include <C8051F120.H>
#include <luxp_init.h>
#include <luxp_function.h>
#include <luxp_extra.h>

//-----------------------------------------------------------------------------
// MAIN Routine Functions
//-----------------------------------------------------------------------------
void satelliteControlModule (void);
	#if (SATELLITE_MODE_HANDLER_ENABLE == 1)
	void satelliteModeHandler (void);
		#if (BODY_RATE_HANDLER_ENABLE == 1)
		#if (ADCS_CMD_ENABLE == 1)
		#if (PWRS_CMD_ENABLE == 1)
		void bodyRateHandler (void);
			void reenableBRH (void);
			void resetBRHEndFlags (void);
		#endif
		#endif
		#endif

		#if (IDLE_MODE_HANDLER_ENABLE == 1)
		#if (PWRS_CMD_ENABLE == 1)
		void idleModeHandler (void);
		#endif
		#endif

		void enterM3 (void);
		void enterM4 (void);
		void enterM5 (void);
		void enterM8 (void);

		#if (SAFE_MODE_HANDLER_ENABLE == 1)
		#if (ADCS_CMD_ENABLE == 1)
		#if (PWRS_CMD_ENABLE == 1)
		void safeModeHandler (void);
		#endif
		#endif
		#endif
	#endif

		#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)
		void enterGroundPass (void);
		#endif
	void peripheralErrorHandler(void);
	void systemUpdateAndMonitor (void);

	#if (SENDING_ENABLE == 1)
	#if (ISIS_CMD_ENABLE == 1)
	#if (BEACON_HANDLER_ENABLE == 1)
	void beaconHandler(void);
	#endif
	#endif
	#endif

	#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)
	void groundStationCommandHandler (void);
		void resetGSCHCounters (void);
	#endif

	#if (SATELLITE_SCHEDULE_HANDLER_ENABLE == 1)
	void satelliteScheduleHandler (void);
	#endif

void commandControlModule (unsigned char *l_uc_call_task_buffer_pointer, unsigned char *l_uc_response_task_buffer_pointer);

#endif

