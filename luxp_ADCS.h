#ifndef luxp_ADCS_h
#define luxp_ADCS_h

#include <C8051F120.H>
#include <luxp_consvar.h>
#include <luxp_RW.h>
#include <luxp_add.h>
#include <luxp_time.h>
#include <luxp_storing.h>

#if (ADCS_CMD_ENABLE == 1)
//ADCS I2C functions without input parameters
unsigned short adcsI2C (unsigned char l_uc_msg_out, unsigned char l_uc_func_par);

//ADCS I2C functions with input parameters
#if (DEBUGGING_FEATURE == 1)
unsigned short adcsGetData (unsigned char l_uc_data_id, unsigned char l_uc_record_time_par);
unsigned short adcsSetMode (unsigned char l_uc_acs_mode, unsigned short l_us_lapse_time_10s, unsigned char l_uc_record_time_par);
unsigned short adcsSetSSThreshold (short l_s_adcs_ss_threshold, unsigned char l_uc_record_time_par);
unsigned short adcsSetCGain (unsigned char l_uc_record_time_par);
unsigned short adcsSetTLE (unsigned char l_uc_record_time_par);
unsigned short adcsSetTime (unsigned long l_ul_time_from_tai, unsigned char l_uc_record_time_par);
unsigned short adcsSetVectorAlignToSun (unsigned char l_uc_record_time_par);
unsigned short adcsToggleIMU (char l_uc_imu, unsigned char l_uc_record_time_par);
unsigned short adcsSetBodyRate (char l_uc_adcs_ctrl_br_x, char l_uc_adcs_ctrl_br_y, char l_uc_adcs_ctrl_br_z, unsigned char l_uc_record_time_par);
unsigned short adcsSetDGain (short l_s_adcs_dgain, unsigned char l_uc_record_time_par);
#else
unsigned short adcsGetData (unsigned char l_uc_data_id);
unsigned short adcsSetMode (unsigned char l_uc_acs_mode, unsigned short l_us_lapse_time_10s);
unsigned short adcsSetSSThreshold (short l_s_adcs_ss_threshold);
unsigned short adcsSetCGain (void);
unsigned short adcsSetTLE (void);
unsigned short adcsSetTime (unsigned long l_ul_time_from_tai);
unsigned short adcsSetVectorAlignToSun (void);
unsigned short adcsToggleIMU (char l_uc_imu);
unsigned short adcsSetBodyRate (char l_uc_adcs_ctrl_br_x, char l_uc_adcs_ctrl_br_y, char l_uc_adcs_ctrl_br_z);
unsigned short adcsSetDGain (short l_s_adcs_dgain);
#endif
#endif

#endif
