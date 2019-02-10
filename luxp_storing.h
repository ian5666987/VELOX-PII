#ifndef luxp_storing_h
#define luxp_storing_h

#include <C8051F120.H>
#include <luxp_add.h>
#include <luxp_main.h>

//Store functions
#if (ISIS_CMD_ENABLE == 1)
void storeISIS (void);
#endif

#if (RTC_CMD_ENABLE == 1)
void storeRTC (void);
#endif

#if (ANTENNA_CMD_ENABLE == 1)
void storeAntenna (void);
#endif

#if (ADCS_CMD_ENABLE == 1)
void storeADCS (unsigned char *l_uc_array_p);
#endif

#if (PWRS_CMD_ENABLE == 1)
void storePWRS (unsigned char *l_uc_array_p);
#endif

#if (STORING_ENABLE == 1)
//To store data in SD Card
unsigned short saveDataProtected (unsigned char l_uc_subsystem, unsigned char l_uc_data_type, unsigned char *l_uc_array_p);
	unsigned short initSDData (void);
	unsigned short saveData (unsigned char *l_uc_array_p);
		unsigned short storeInSDCard (unsigned char *l_uc_array_p, unsigned short l_us_buffer_size);
	void saveDataEndHandler (unsigned long l_ul_new_bp);

//Save the satellite latest state
void saveState (void);

//To load data with SD card buffer writing protection, CRC16 checking, and auto-writing-hold-timeout features
unsigned short loadDataProtected (unsigned char l_uc_subsystem, unsigned char l_uc_data_type,unsigned long l_ul_start_block_num,unsigned long l_ul_end_block_num);

//To load the satellite latest state
void loadState (void);
unsigned short loadSchedule (unsigned char *l_uc_array_p); //To load the schedule with better efficiency
unsigned short loadScriptspace (unsigned char *l_uc_array_p); //To load the scriptspace with better efficiency

//To update various OBDH variables
void updateOBDHEssential (unsigned char *l_uc_array_p);
void updateOBDHSSCP (unsigned char *l_uc_array_p);
void updateOBDHSelectedHK (unsigned char *l_uc_array_p);
void updateOBDHSelectedData (unsigned char *l_uc_array_p);
void updateOBDHBP (unsigned char *l_uc_array_p);

//To search wanted block pointer by orbit info
unsigned short searchBPByOrbitInfo (unsigned char l_uc_data_type);
#endif

#endif