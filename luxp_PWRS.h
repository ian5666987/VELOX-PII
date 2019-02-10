#ifndef luxp_PWRS_h
#define luxp_PWRS_h

#include <luxp_consvar.h>
#include <luxp_RW.h>
#include <luxp_time.h>
#include <luxp_storing.h>
#include <luxp_add.h>

#if (PWRS_CMD_ENABLE == 1)
//PWRS functions
unsigned short pwrsI2C (unsigned char l_uc_msg_out, unsigned char l_uc_func_par);

#if (DEBUGGING_FEATURE == 1)
unsigned short pwrsGetData (unsigned char l_uc_data_id, unsigned char l_uc_record_time_par);
unsigned short pwrsUpdateBatteryHeater (unsigned char l_uc_pwrs_battery_heater, unsigned char l_uc_record_time_par);
unsigned short pwrsSetOutput (unsigned char l_uc_pwrs_channel_enable, unsigned char l_uc_record_time_par);
unsigned short pwrsSetSingleOutput (unsigned char l_uc_pwrs_var_channel, unsigned char l_uc_value, unsigned short l_us_delay, unsigned char l_uc_record_time_par);
unsigned short pwrsSetMPPTMode (unsigned char l_uc_pwrs_mppt_mode, unsigned char l_uc_record_time_par);
unsigned short pwrsSetPVVolt (unsigned char l_uc_record_time_par);
unsigned short pwrsSetTKOutput (unsigned char l_uc_pwrs_var_channel, unsigned char l_uc_value, unsigned short l_us_delay, unsigned char l_uc_record_time_par);
unsigned short pwrsResetPWRS (unsigned short l_us_delay, unsigned char l_uc_record_time_par);
unsigned short pwrsResetOBDH (unsigned short l_us_delay, unsigned char l_uc_record_time_par);
unsigned short pwrsSetSOCLimit (unsigned char l_uc_pwrs_soc_on_limit, unsigned char l_uc_pwrs_soc_off_limit, unsigned char l_uc_record_time_par);
unsigned short pwrsSetWDTLimit (unsigned long l_ul_pwrs_wdt_limit, unsigned char l_uc_record_time_par);
unsigned short pwrsInitEeprom (unsigned char l_uc_record_time_par);
unsigned short pwrsSetSDC (unsigned char l_uc_pwrs_sdc, unsigned char l_uc_record_time_par);
#else
unsigned short pwrsGetData (unsigned char l_uc_data_id);
unsigned short pwrsUpdateBatteryHeater (unsigned char l_uc_pwrs_battery_heater);
unsigned short pwrsSetOutput (unsigned char l_uc_pwrs_channel_enable);
unsigned short pwrsSetSingleOutput (unsigned char l_uc_pwrs_var_channel, unsigned char l_uc_value, unsigned short l_us_delay);
unsigned short pwrsSetMPPTMode (unsigned char l_uc_pwrs_mppt_mode);
unsigned short pwrsSetPVVolt (void);
unsigned short pwrsSetTKOutput (unsigned char l_uc_pwrs_var_channel, unsigned char l_uc_value, unsigned short l_us_delay);
unsigned short pwrsResetPWRS (unsigned short l_us_delay);
unsigned short pwrsResetOBDH (unsigned short l_us_delay);
unsigned short pwrsSetSOCLimit (unsigned char l_uc_pwrs_soc_on_limit, unsigned char l_uc_pwrs_soc_off_limit);
unsigned short pwrsSetWDTLimit (unsigned long l_ul_pwrs_wdt_limit);
unsigned short pwrsInitEeprom (void);
unsigned short pwrsSetSDC (unsigned char l_uc_pwrs_sdc);
#endif
#endif

#endif