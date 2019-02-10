#ifndef luxp_send_h
#define luxp_send_h

#include <C8051F120.H>
#include <luxp_consvar.h>
#include <luxp_time.h>
#include <luxp_add.h>
#include <luxp_ISIS.h>
#include <luxp_extra.h>

//Send Functions
void sendToUART0 (unsigned char *l_uc_msg_p, unsigned short l_us_starting_index, unsigned short l_us_no_of_bytes);
void sendToUART1 (unsigned char *l_uc_msg_p, unsigned short l_us_starting_index, unsigned short l_us_no_of_bytes);
unsigned short sendOptions (unsigned short l_us_option_bits, unsigned char *l_uc_response_output_buffer_p);

#if (SENDING_ENABLE == 1)
#if (ISIS_CMD_ENABLE == 1)
//Send Downlink Functions
unsigned short initSendData (void);
#if (BEACON_HANDLER_ENABLE == 1)
unsigned short sendData (void);
	unsigned short sendDataInAX25Frame (void);
#endif

#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)
//Sending function by GSCH
void sendAckMsg (void);
void sendCommonData (unsigned char l_uc_common_data);
void sendKeyRequest (void);
#endif
#endif
#endif

#endif
