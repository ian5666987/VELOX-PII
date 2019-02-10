#ifndef luxp_SD_h
#define luxp_SD_h

#include <C8051F120.H>
#include <luxp_consvar.h>
#include <luxp_time.h>
#include <string.h>
#include <stdio.h>

#if (STORING_ENABLE == 1)
unsigned short initSD (void);		//SD Card Initialization
unsigned short sdReadSingleBlock(unsigned long l_ul_sd_start_block);
unsigned short sdWriteSingleBlock(unsigned long block, void* buffer, unsigned short length);
#if (OBDH_CMD_ENABLE == 1)
unsigned short sdEraseSingleBlock(unsigned long l_ul_sd_start_block);
#endif
#endif

#endif
