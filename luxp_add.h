#ifndef luxp_add_h
#define luxp_add_h

#include <C8051F120.H>
#include <luxp_consvar.h>
#include <luxp_time.h>
#include <luxp_RTC.h>
#include <luxp_struct.h>
#include <luxp_check.h>

//Bytes manipulation functions
unsigned long 	ushort2ulong	(unsigned short l_us_high_short, unsigned short l_us_low_short);
unsigned long 	uchar2ulong		(unsigned char 	l_uc_hh_byte, unsigned char l_uc_hl_byte, unsigned char l_uc_lh_byte, unsigned char l_uc_ll_byte);	
unsigned char 	float2ullchar	(float l_f_float);
unsigned char 	float2ulhchar	(float l_f_float);
unsigned char 	float2uhlchar	(float l_f_float);
unsigned char 	float2uhhchar	(float l_f_float);
unsigned char 	twoDec2oneHex	(unsigned char 	l_uc_two_dec_no);
#if (RTC_CMD_ENABLE == 1)
unsigned char 	oneHex2twoDec	(unsigned char 	l_uc_one_hex_no);
#endif
float 			uchar2float 	(unsigned char 	l_uc_hh_byte, unsigned char l_uc_hl_byte, unsigned char l_uc_lh_byte, unsigned char l_uc_ll_byte);	

//Clear functions
void clearUARTBuffer(void);
void clearCallTaskBuffer(void);

//Reset functions
void resetI2CCommunication(void);

#if (TASK_ENABLE == 1)
void resetTask(unsigned char l_uc_task);
#endif

#if (SATELLITE_MODE_HANDLER_ENABLE == 1)
#if (BODY_RATE_HANDLER_ENABLE == 1)
#if (ADCS_CMD_ENABLE == 1)
#if (PWRS_CMD_ENABLE == 1)
void resetBodyRateValue (void);
#endif
#endif
#endif
#endif

//Insert functions
unsigned short insertFloatToArray (float l_f_to_insert, unsigned char *l_uc_array_p, unsigned short l_us_starting_counter);
unsigned short insertLongToArray (unsigned long l_ul_to_insert, unsigned char *l_uc_array_p, unsigned short l_us_starting_counter);
unsigned short insertShortToArray (unsigned short l_us_to_insert, unsigned char *l_uc_array_p, unsigned short l_us_starting_counter);
float insertArrayToFloatGC (unsigned char *l_uc_array_p);
unsigned long insertArrayToLongGC (unsigned char *l_uc_array_p);
unsigned short insertArrayToShortGC (unsigned char *l_uc_array_p);
#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)
#if (ISIS_CMD_ENABLE == 1)
void insertGDSNCommandToGSCHVar (void);
#endif
#endif
void insertCRC16 (unsigned char *l_uc_msg_pointer, unsigned short l_us_msg_length);
#if (CQUEUE_ENABLE == 1)
void insertGSCHCommandToQueue (void);
#endif

//Fill functions
unsigned short fillData (unsigned char *l_uc_array_p, unsigned short l_us_data_counter, unsigned short l_us_fill_type, unsigned short l_us_fill_argument);

#endif
