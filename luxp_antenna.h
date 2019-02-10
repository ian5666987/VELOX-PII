#ifndef luxp_antenna_h
#define luxp_antenna_h

#include <C8051F120.H>
#include <luxp_consvar.h>
#include <luxp_RW.h>
#include <luxp_add.h>
#include <luxp_time.h>

#if (ANTENNA_CMD_ENABLE == 1)
//Antenna deployment functions
unsigned short antennaI2C(unsigned char l_uc_antenna_board_addr, unsigned char l_uc_msg_out, unsigned char l_uc_func_par);
#if (DEBUGGING_FEATURE == 1)
unsigned short antennaI2CPar(unsigned char l_uc_antenna_board_addr, unsigned char l_uc_msg_out, unsigned char l_uc_msg_par, unsigned char l_uc_record_time_par);
#else
unsigned short antennaI2CPar(unsigned char l_uc_antenna_board_addr, unsigned char l_uc_msg_out, unsigned char l_uc_msg_par);
#endif
#endif

#endif

