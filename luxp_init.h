#ifndef luxp_init_h
#define luxp_init_h

#include <C8051F120.H>
#include <luxp_consvar.h>
#include <luxp_struct.h>
#include <luxp_RTC.h>
#include <luxp_add.h>
#include <luxp_SD.h>
#include <luxp_check.h>
#include <luxp_time.h>
#include <luxp_RW.h>
#include <luxp_schedule.h>
#include <luxp_task.h>

//Hardware Initialization
void initSystem (void);				//System Initialization
void disableWdt (void);				//Disable Watch Dog Timer
void initOscillator (void);			//Oscillator Initialization
void initPort (void);				//Port Initialization
void initEMIF (void);				//EMIF Initialization
void initSPI(void);					//SPI Initialization
void initUart0 (void);				//UART0 Initialization
void initUart1 (void);				//UART1 Initialization
void initSMBus (void);				//SMBus Initialization
#if (DEBUGGING_FEATURE == 1)
void initTimer1 (void);				//Timer1 Initialization
void initTimer2 (void);				//Timer2 Initialization
#endif
void initTimer3 (void);				//Timer3 Initialization
void initTimer4 (void);				//Timer4 Initialization
void initA2DConverter (void);		//Analog-to-Digital Converter Initialization

//Other Initializations
#if (TIME_ENABLE == 1)
void initSatelliteDate (void);		//Satellite Date Initialization
void initSatelliteTime (void);		//Satellite Time Initialization
#endif
void initSatelliteState (void);		//Satellite State Initialization
#if (CRC_ENABLE == 1)
void initCRC (void);				//CRC Initialization
#endif
void initBlockPointer (void);		//Memory block pointer(s) Initialization
void initOBDHParameters (void);		//OBDH Parameters Initialization
#if (TASK_ENABLE == 1)
void initTaskFlags (void);			//Task flags Initialization
void initTaskScript (void);			//Task Script's Initialization
	//Initialize various scripts
	void initGetHKScript (void);
	void initDeployAntennaScript (void);
	void initLogSTDataScript (void);
	void initTestTKScript (void);
#endif
void initScheduler (void);			//Scheduler's Initialization

#if (CQUEUE_ENABLE == 1)
void initOBDHCQueue (void);			//OBDH Command Queue Initialization
#endif


#endif

