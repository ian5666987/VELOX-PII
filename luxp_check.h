#ifndef luxp_check_h
#define luxp_check_h

#include <C8051F120.H>
#include <luxp_consvar.h>
#include <luxp_add.h>

#if (CRC_ENABLE == 1)
//Check functions
unsigned short 	checkCRC16Match 	(unsigned char *l_uc_msg_p, unsigned short l_us_msg_length);
unsigned short 	createChecksum 		(unsigned char *l_uc_msg_pointer, unsigned short l_us_msg_length, unsigned char l_uc_checksum_type);

//For adding Checksum bytes
void addChecksumBytes16 (unsigned char *l_uc_msg_pointer, unsigned short l_us_msg_length, unsigned short l_us_checksum_bytes);
#endif

#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)
#if (ISIS_CMD_ENABLE == 1)
//Time window checking
void timeWindowCheck (void);

//Check if in the log data, there are ADCS data
unsigned char checkADCSDataInLogDataType (void);
#endif
#endif

//Event handler
#if (EVENT_HANDLER_ENABLE == 1)
#if (STORING_ENABLE == 1)
void eventHandlerUpdateStateMode (unsigned char l_uc_sat_mode_next); ////yc added 20120224
#if (SATELLITE_MODE_HANDLER_ENABLE == 1)
#if (SAFE_MODE_HANDLER_ENABLE == 1)
#if (ADCS_CMD_ENABLE == 1)
#if (PWRS_CMD_ENABLE == 1)
void eventHandlerSafeholdMode (void); //yc added 20120224
#endif
#endif
#endif
#endif
void eventHandler (unsigned short l_us_error_code, unsigned char l_uc_event_arg_0,unsigned char l_uc_event_arg_1,
						unsigned char l_uc_event_arg_2,unsigned char l_uc_event_arg_3,unsigned char l_uc_event_arg_4,unsigned char l_uc_event_arg_5);
#endif			
#endif
#endif