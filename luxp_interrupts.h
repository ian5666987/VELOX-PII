#ifndef luxp_interrupts_h
#define luxp_interrupts_h

#include <C8051F120.H>
#include <luxp_consvar.h>
#include <luxp_storing.h>
#include <luxp_add.h>
#include <luxp_struct.h>
#include <luxp_RTC.h>
#include <luxp_check.h>

//ISR functions
//Unused ISR functions are commented for now
//void ISRTimer1 (void);	//Timer1 ISR function
//void ISRTimer2 (void);	//Timer2 ISR function
void ISRTimer3 (void);	//Timer3 ISR function
void ISRTimer4 (void);	//Timer4 ISR function
void ISRSMBus (void);	//SMBus ISR function
void ISRUART0 (void);	//UART0 ISR function
void ISRUART1 (void);	//UART1 ISR function
void ISRSPI (void);		//SPI ISR function

#endif