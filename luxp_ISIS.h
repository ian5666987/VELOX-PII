#ifndef luxp_ISIS_h
#define luxp_ISIS_h

#include <C8051F120.H>
#include <luxp_consvar.h>
#include <luxp_RW.h>
#include <luxp_add.h>
#include <luxp_time.h>
#include <luxp_storing.h>

#if (ISIS_CMD_ENABLE == 1)
//ISIS functions
#if (DEBUGGING_FEATURE == 1)
unsigned short isisGetHousekeeping(unsigned char l_uc_record_time_par);
#else
unsigned short isisGetHousekeeping(void);
#endif
unsigned short isisI2C(unsigned char l_uc_isis_board_addr, unsigned char l_uc_msg_out, unsigned char l_uc_func_par);
unsigned short isisI2CPar(unsigned char l_uc_msg_out, unsigned short l_us_msg_par, unsigned char l_uc_func_par);
unsigned short isisI2CMsg(unsigned char l_uc_msg_out, unsigned short l_us_msg_par, unsigned short l_us_msg_length, unsigned char l_uc_func_par);
#endif

#endif

