#ifndef luxp_extra_h
#define luxp_extra_h

#include <luxp_consvar.h>
#include <luxp_isis.h>
#include <luxp_time.h>

//Extra Functions
#if (BEACON_HANDLER_ENABLE == 1)
#if (ISIS_CMD_ENABLE == 1)
#if (SENDING_ENABLE == 1)
unsigned short overlyLongTransmissionRecovery (void);
#endif
#endif
#endif

#if (EXTRA_ENABLE == 1)
#if (UPDATE_HANDLER_ENABLE == 1)
unsigned short convertA2D (unsigned char l_uc_ad_ch);
#endif
void resetSatellite (void);
#if (UPDATE_HANDLER_ENABLE == 1)
void readMCUTemperature (void);
#endif

#if (BODY_RATE_HANDLER_ENABLE == 1)
#if (ADCS_CMD_ENABLE == 1)
#if (PWRS_CMD_ENABLE == 1)
void updateBodyRateAverage (void);
#endif
#endif
#endif

void swReset (void);
void hwReset (void);
#endif
//void switchAntennaAddress (void);

#endif