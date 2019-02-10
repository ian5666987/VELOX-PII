#ifndef luxp_ground_h
#define luxp_ground_h

#include <luxp_consvar.h>
#include <luxp_time.h>
#include <luxp_RTC.h>
#include <luxp_RW.h>
#include <luxp_add.h>
#include <luxp_check.h>

#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)
#if (ISIS_CMD_ENABLE == 1)
void groundUpdateTime (unsigned long l_ul_tai_time);
void groundUpdateRevNo (void);
void groundCommandListener (void);
void groundPassRequestKeyGenerator (void);
#endif
#endif
#if (SENDING_ENABLE == 1)
#if (ISIS_CMD_ENABLE == 1)
#if (BEACON_HANDLER_ENABLE == 1)
void groundInitCWBeacon (unsigned short l_us_cw_beacon_mode);
void groundInitAX25Beacon (void);
#endif
#endif
#endif
#if (GROUND_STATION_COMMAND_HANDLER_ENABLE == 1)
#if (ISIS_CMD_ENABLE == 1)
#if (SENDING_ENABLE == 1)
void groundSaveScheduleAndAck (void);
void groundSaveEssentialAndAck (void);
#endif
#endif
void groundTerminateTransitionalState (void);
void groundTerminateGroundPass (void);
#if (ISIS_CMD_ENABLE == 1)
#if (SENDING_ENABLE == 1)
void groundTerminateSendRealTimeData (void);
void groundTerminateSendStoredData (void);
void groundTerminateSendStoredDataWithAckMsg (void);
void groundStartClaimBeacon (void);
void groundEnterSendRealTime (void);
void groundEnterSendStored (void);
void groundStartSendRequest (void);
void groundKeepSendRequest (void);
void groundReleaseClaimBeacon (void);
#endif
#endif
#if (SATELLITE_MODE_HANDLER_ENABLE == 1)
#if (OBDH_CMD_ENABLE == 1)
void groundEnterMissionMode (unsigned short l_us_elapsed_time_s);
#endif
#endif
#endif

#endif