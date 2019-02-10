#ifndef luxp_cqueue_h
#define luxp_cqueue_h

#include <C8051F120.H>
#include <luxp_consvar.h>
#include <luxp_struct.h>
#include <luxp_check.h>

#if (CQUEUE_ENABLE == 1)
//These function will most likely be useless for final executable
//------------------------------------------------------------------------------------
// Command queue functions library declarations
//------------------------------------------------------------------------------------
unsigned short addCommandToQueue (void);
void exeCommandInQueue (void);
void commandQueueHandler (void);
#endif

#endif
