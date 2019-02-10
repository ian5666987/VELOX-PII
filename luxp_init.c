//------------------------------------------------------------------------------------
// Luxp_init.c
//------------------------------------------------------------------------------------
// Copyright (C) 2013 Nanyang Technological University.
//
// AUTH: IAN
// DATE: 03 APR 2013
//
// The program Luxp_init.c will be downloaded to the Pumpkin Motherboard with 
// C8051F120 uC as part of the VELOX-P Pico-Satellite Flight Software. 
//
// This file contains all functions related to initialization of the Flight Software
//
//------------------------------------------------------------------------------------
// Overall functional test status
//------------------------------------------------------------------------------------
// Completely tested, all fully functional
//
//------------------------------------------------------------------------------------
// Overall documentation status
//------------------------------------------------------------------------------------
// Averagely documented
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// List of functions				| Test status					| Documentation
//------------------------------------------------------------------------------------
// initSystem						| Tested, fully functional		| Average	
// disableWdt						| Tested, fully functional		| Average	
// initOscillator					| Tested, fully functional		| Average	
// initPort							| Tested, fully functional		| Average	
// initSPI							| Untested						| Minimum
// initUart0						| Tested, fully functional		| Average	
// initUart1						| Tested, fully functional		| Average	
// initSMBus						| Tested, fully functional		| Average	
// initTimer1						| Tested, fully functional		| Average	
// initTimer2						| Tested, fully functional		| Average	
// initTimer3						| Tested, fully functional		| Average	
// initTimer4						| Tested, fully functional		| Average	
// initSatelliteDate				| Tested, fully functional		| Minimum	
// initSatelliteTime				| Tested, fully functional		| Minimum
// initMissionSchedule				| Tested, fully functional		| Minimum	
// initCRC							| Tested, fully functional		| Minimum	
//------------------------------------------------------------------------------------

#include <luxp_init.h>

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F12x
//-----------------------------------------------------------------------------
sfr16 RCAP2	= 0xCA;		//Timer2 reload value
sfr16 TMR2	= 0xCC;		//Timer2 counter
sfr16 RCAP3	= 0xCA;		//Timer3 reload value
sfr16 TMR3	= 0xCC;		//Timer3 counter
sfr16 RCAP4	= 0xCA;		//Timer4 reload value
sfr16 TMR4	= 0xCC;		//Timer4 counter

//-----------------------------------------------------------------------------
// sbit for 'F12x
//-----------------------------------------------------------------------------
sbit MOSI 	= P0^4;
sbit MISO 	= P0^3;
sbit SCK 	= P0^2;
sbit CS 	= P0^5;
sbit ON_SD 	= P4^1;
sbit ON_MHX = P4^2;
sbit OE_MHX = P4^3;
sbit OE_USB = P4^4;

#define ON_USB 0

//------------------------------------------------------------------------------------
// Initialization Function
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Program Initialization: initSystem()
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
void initSystem (void)
{
	disableWdt();			//Disable Watch Dog Timer
	initOscillator();		//Oscillator Initialization
	initPort();				//Port Initialization
	initEMIF();				//EMIF Initialization
	initSPI();				//SPI Initialization
	initUart0();			//UART0 Initialization
	initUart1();			//UART1 Initialization
	initSMBus();			//SMBus Initialization
	#if (DEBUGGING_FEATURE == 1)
	initTimer1();			//Timer1 Initialization, used for UART1 timer and baudrate
	initTimer2();			//Timer2 Initialization, used for UART0 baudrate
	#endif
	initTimer3();			//Timer3 Initialization, used for I2C timeout
	initTimer4();			//Timer4 Initialization, used for software timeouts
	initA2DConverter();		//Analog-to-Digital Converter Initialization
	EA = 1;					//Enable global interrupt	

	waitS(3);				//Do nothing for 3 second(s), who knows PWRS initiates the reset.

	#if (TIME_ENABLE == 1)
	initSatelliteDate();	//Satellite Date Initialization
	initSatelliteTime();	//Satellite Time Initialization
	#endif

	initSatelliteState();	//Satellite State Initialization

	#if (CRC_ENABLE == 1)
	initCRC();				//CRC Table Initialization
	#endif

	initBlockPointer();		//Memory Block Pointers Initialization
	initOBDHParameters();	//OBDH Parameters Initialization
	
	#if (TASK_ENABLE == 1)
	initTaskFlags();		//Task Flags' Initialization
	initTaskScript();		//Task Script's Initialization
	#endif

	initScheduler();		//Scheduler Initialization
	
	#if (CQUEUE_ENABLE == 1)
	initOBDHCQueue();			//OBDH Command Queue Initialization
	#endif

	SFRPAGE = LEGACY_PAGE;
	SI = 0;					//Put down SMBus flag
	#if (STORING_ENABLE == 1)
	initSD();				//SD card initialization
	#endif

}

//------------------------------------------------------------------------------------
// Disable Watchdog Timer
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
void disableWdt (void)
{
	char SFRPAGE_SAVE = SFRPAGE;

	SFRPAGE = CONFIG_PAGE;
	
	WDTCN = 0xDE;
	WDTCN = 0xAD;

	SFRPAGE = SFRPAGE_SAVE;
}

//------------------------------------------------------------------------------------
// Oscillator Initialization
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
void initOscillator (void)
{
	#if (SYSCLK_USED == LEVEL_3_SYSCLK)
	unsigned short l_us_counter;
	#elif (SYSCLK_USED == LEVEL_2_SYSCLK)
	unsigned short l_us_counter;
	#endif

	char SFRPAGE_SAVE = SFRPAGE;

	//Sit in this SFRPAGE, to get (or set) latest reset source
	SFRPAGE = LEGACY_PAGE;

	//Reset Source Register (RSTSRC)
	//Bit7: Reserved
	
	//Bit6: CNVRSEF: Convert Start 0 Reset Source Enable and Flag
	//Write
	//0: CNVSTR0 is not a reset source
	//1: CNVSTR0 is a reset source (active low)
	//Read
	//0: Source of prior reset was not CNVSTR0
	//1: Source of prior reset was CNVSTR0
	
	//Bit5: C0RSEF: Comparator0 Reset Enable and Flag.
	//Write
	//0: Comparator0 is not a reset source
	//1: Comparator0 is a reset source (active low)
	//Read
	//0: Source of last reset was not Comparator0
	//1: Source of last reset was Comparator0.
	
	//Bit4: SWRSF: Software Reset Force and Flag
	//Write
	//0: No effect
	//1: Forces an internal reset. RST pin is not effected.
	//Read
	//0: Source of last reset was not a write to the SWRSF bit.
	//1: Source of last reset was a write to the SWRSF bit.
	
	//Bit3: WDTRSF: Watchdog Timer Reset Flag.
	//0: Source of last reset was not WDT timeout
	//1: Source of last reset was WDT timeout
	
	//Bit2: MCDRSF: Missing Clock Detector Flag
	//Write
	//0: Missing Clock Detector disabled
	//1: Missing Clock Detector enabled; triggers a reset if a missing clock condition is detected
	//Read
	//0: Source of last reset was not a Missing Clock Detector timeout
	//1: Source of last reset was a Missing Clock Detector timeout
	
	//Bit1: PORSF: Power-On Reset Flag
	//Write
	//If the VDD monitor circuitry is enabled (by tying the MONEN pin to a logic high state), this bit can be written to
	//select or de-select the VDD monitor as a reset source
	//0: De-select the VDD monitor as a reset source
	//1: Select the VDD monitor as a reset source
	//Important:
	//At power-on, the VDD monitor is enabled/disabled using the external VDD monitor enable pin (MONEN).
	//The PORSF bit does not disable or enable the VDD monitor circuit. It simply selects the VDD monitor as a reset source
	//Read
	//This bit is set whenever a power-on reset occurs. This may be due to a true power-on reset or a VDD monitor reset.
	//In either case, data memory should be considered indeterminate following the reset.
	//0: Source of last reset was not a power-on or VDD monitor reset
	//1: Source of last reset was a power-on or VDD monitor reset
	//Note: When this flag is read as '1', all other reset flags are indeterminate
	
	//Bit0: PINRSF: HW Pin Reset Flag
	//Write
	//0: No effect
	//1: Forces a Power-On Reset. RST is driven low
	//Read
	//0: Source of prior reset was not RST pin
	//1: Source of prior reset was RST pin
	//Missing Clock Detector enabled; triggers a reset if a missing clock condition is detected
	//Select the VDD monitor as a reset source

	//Set reset source to VDD monitor reset (write, bit1 = 1), not CNVSTR0 (bit6), not Comparator0 (bit5)
	//Triggers a reset if a missing clock condition is detected (write, bit2 = 1)
	RSTSRC |= 0x06;

	//Store last reset source
	str_obdh_hk.uc_mcu_rstsrc = RSTSRC;

	#if (SYSCLK_USED == LEVEL_3_SYSCLK)
	//Initialize external clock first
	//Select the external crystal oscillator
	g_uc_evha_flag &= ~EVENT_FLAG_EXTOSC_FAIL;

	//Initialize oscillator parameters as external
	OSC_FREQUENCY 	= EXT_OSC_FREQUENCY;
	SYSCLK 			= EXT_SYSCLK;
	PLL_MUL 		= EXT_PLL_MUL;
	PLL_DIV 		= EXT_PLL_DIV;
	PLL_DIV_CLK 	= EXT_PLL_DIV_CLK;
	PLL_OUT_CLK 	= EXT_PLL_OUT_CLK;
	FLSCL_FLRT		= EXT_FLSCL_FLRT;
	PLLFLT_ICO		= EXT_PLLFLT_ICO;
	PLLFLT_LOOP		= EXT_PLLFLT_LOOP;

	//Set SFRPAGE to config page
	SFRPAGE = CONFIG_PAGE;

	//External Oscillator Control Register (OSCXCN)
	//Bit 7: XTLVLD: Crystal Oscillator Valid Flag
	//(Valid only when XOSCMD = 11x.)
	//0: Crystal Oscillator is unused or not yet stable
	//1: Crystal Oscillator is running and stable
	//Bits [6..4]: XOSCMD[2..0]: External Oscillator Mode Bits
	//00x: External Oscillator circuit off
	//010: External CMOS Clock Mode (External CMOS Clock input on XTAL1 pin)
	//011: External CMOS Clock Mode with divide by 2 stage (External CMOS Clock input on XTAL1 pin)
	//10x: RC/C Oscillator Mode with divide by 2 stage
	//110: Crystal Oscillator Mode
	//111: Crystal Oscillator Mode with divide by 2 stage
	//Bit3: RESERVED. Read = 0, Write = don't care
	//Bits [2..0]: XFCN[2..0]: External Oscillator Frequency Control Bits
	//000-111: see table below:
	//XFCN	Crystal (XOSCMD = 11x)	RC (XOSCMD = 10x)    	C (XOSCMD = 10x)
	//000 	f = 32 kHz					f = 25 kHz					K Factor = 0.87
	//001 	32 kHz < f = 84 kHz     25 kHz < f = 50 kHz		K Factor = 2.6
	//010 	84 kHz < f = 225 kHz    50 kHz < f = 100 kHz    K Factor = 7.7
	//011 	225 kHz < f = 590 kHz   100 kHz < f = 200 kHz   K Factor = 22
	//100 	590 kHz < f = 1.5 MHz   200 kHz < f = 400 kHz   K Factor = 65
	//101 	1.5 MHz < f = 4 MHz     400 kHz < f = 800 kHz   K Factor = 180
	//110 	4 MHz < f = 10 MHz      800 kHz < f = 1.6 MHz   K Factor = 664
	//111 	10 MHz < f = 30 MHz 

	OSCXCN = 0x60;
	OSCXCN |= XFCN;
	
	//Wait at least 1ms for initialization
	for (l_us_counter = 0; l_us_counter < 30000; l_us_counter++);

	//Reset waiting counter
	l_us_counter = 0;
	
	//Loop until XTLVLD = 1 -> Crystal Oscillator is running and stable
	while (((OSCXCN & 0x80) == 0) && l_us_counter < 30000) l_us_counter++;
	
	0x80 

	//Check if the waiting time is NOT passed (external clock is working)
	if (l_us_counter < 30000){
		//System Clock Selection Register (CLKSEL)
		//Bits 7–6: Reserved.
		//Bits 5–4: CLKDIV1–0: Output SYSCLK Divide Factor.
		//These bits can be used to pre-divide SYSCLK before it is output to a port pin through the crossbar.
		//00: Output will be SYSCLK.
		//01: Output will be SYSCLK/2.
		//10: Output will be SYSCLK/4.
		//11: Output will be SYSCLK/8.
		//See Section “18. Port Input/Output” on page 235 for more details about routing this output to a port pin.
		//Bits 3–2: Reserved.
		//Bits 1–0: CLKSL1–0: System Clock Source Select Bits.
		//00: SYSCLK derived from the Internal Oscillator, and scaled as per the IFCN bits in OSCICN.
		//01: SYSCLK derived from the External Oscillator circuit.
		//10: SYSCLK derived from the PLL.
		//11: Reserved.
	
		//SYSCLK is derived from External oscillator circuit
		CLKSEL = 0x01;
	
		//PLL reference clock is External Oscillator
	  	PLL0CN	= 0x04;
	
		//Cache Control Register (CCH0CN)
	
		//Bit 7: CHWREN: Cache Write Enable
		//This bit enables the processor to write to the cache memory.
		//0: Cache contents are not allowed to change, except during Flash writes/erasures or cache locks.
		//1: Writes to cache memory are allowed.
		//Bit 6: CHRDEN: Cache Read Enable.
		//This bit enables the processor to read instructions from the cache memory.
		//0: All instruction data comes from Flash memory or the prefetch engine.
		//1: Instruction data is obtained from cache (when available).
		//Bit 5: CHPFEN: Cache Prefetch Enable.
		//This bit enables the prefetch engine.
		//0: Prefetch engine is disabled.
		//1: Prefetch engine is enabled.
		//Bit 4: CHFLSH: Cache Flush.
		//When written to a ‘1’, this bit clears the cache contents. This bit always reads ‘0’.
		//Bit 3: CHRETI: Cache RETI Destination Enable.
		//This bit enables the destination of a RETI address to be cached.
		//0: Destinations of RETI instructions will not be cached.
		//1: RETI destinations will be cached.
		//Bit 2: CHISR: Cache ISR Enable.
		//This bit allows instructions which are part of an Interrupt Service Rountine (ISR) to be cached.
		//0: Instructions in ISRs will not be loaded into cache memory.
		//1: Instructions in ISRs can be cached.
		//Bit 1: CHMOVC: Cache MOVC Enable.
		//This bit allows data requested by a MOVC instruction to be loaded into the cache memory.
		//0: Data requested by MOVC instructions will not be cached.
		//1: Data requested by MOVC instructions will be loaded into cache memory.
		//Bit 0: CHBLKW: Block Write Enable.
		//This bit allows block writes to Flash memory from software.
		//0: Each byte of a software Flash write is written individually.
		//1: Flash bytes are written in groups of four (for code space writes) or two (for scratchpad writes).
	
	  	CCH0CN	&= ~0x20;
	
		//Set FLASH read time according to SYSCLK
	  	SFRPAGE	= LEGACY_PAGE;
	  	FLSCL	= FLSCL_FLRT;
	
	  	SFRPAGE	= CONFIG_PAGE;
	
	  	CCH0CN	|= 0x20;
	
		//PLL Control Register (PLL0CN)
		//Bits [7..5]:	UNUSED: Read = 000b; Write = don’t care.
		//Bit 4:	PLLCK: PLL Lock Flag.
		//0: PLL Frequency is not locked.
		//1: PLL Frequency is locked.
		//Bit 3: RESERVED. Must write to ‘0’.
		//Bit 2: PLLSRC: PLL Reference Clock Source Select Bit
		//0: PLL Reference Clock Source is Internal Oscillator
		//1: PLL Reference Clock Source is External Oscillator
		//Bit 1: PLLEN: PLL Enable Bit
		//0: PLL is held in reset
		//1: PLL is enabled. PLLPWR must be ‘1’
		//Bit 0: PLLPWR: PLL Power Enable
		//0: PLL bias generator is de-activated. No static power is consumed
		//1: PLL bias generator is active. Must be set for PLL to operate
	
		PLL0CN	|= 0x07;
	}
	//Check if the waiting time is passed (external clock is NOT working)
	else{
		//Reset external oscillator control register
		OSCXCN = 0x00;
	
		//Set oscillator used as internal oscillator
		g_uc_evha_flag |= EVENT_FLAG_EXTOSC_FAIL;

		//Initialize oscillator parameters as internal
		OSC_FREQUENCY 	= INT_OSC_FREQUENCY;
		SYSCLK 			= INT_SYSCLK;
		PLL_MUL 		= INT_PLL_MUL;
		PLL_DIV 		= INT_PLL_DIV;
		PLL_DIV_CLK 	= INT_PLL_DIV_CLK;
		PLL_OUT_CLK 	= INT_PLL_OUT_CLK;
		FLSCL_FLRT		= INT_FLSCL_FLRT;
		PLLFLT_ICO		= INT_PLLFLT_ICO;
		PLLFLT_LOOP		= INT_PLLFLT_LOOP;

		//Set SFRPAGE to config page
		SFRPAGE = CONFIG_PAGE;

		//Internal Oscillator Control Register (OSCICN)
		//Bit 7: IOSCEN: Internal Oscillator Enable Bit.
		//0: Internal Oscillator Disabled.
		//1: Internal Oscillator Enabled.
		//Bit 6: IFRDY: Internal Oscillator Frequency Ready Flag.
		//0: Internal Oscillator not running at programmed frequency.
		//1: Internal Oscillator running at programmed frequency.
		//Bits 5–2: Reserved.
		//Bits 1–0: IFCN1-0: Internal Oscillator Frequency Control Bits.
		//00: Internal Oscillator is divided by 8.
		//01: Internal Oscillator is divided by 4.
		//10: Internal Oscillator is divided by 2.
		//11: Internal Oscillator is divided by 1.
		OSCICN = 0x83;						// To divide internal oscillator by 1

		//PLL Control Register Register (PLL0CN)
		//Bits 7–5: UNUSED: Read = 000b; Write = don’t care.
		//Bit 4: PLLCK: PLL Lock Flag.
		//0: PLL Frequency is not locked.
		//1: PLL Frequency is locked.
		//Bit 3: RESERVED. Must write to ‘0’.
		//Bit 2: PLLSRC: PLL Reference Clock Source Select Bit.
		//0: PLL Reference Clock Source is Internal Oscillator.
		//1: PLL Reference Clock Source is External Oscillator.
		//Bit 1: PLLEN: PLL Enable Bit.
		//0: PLL is held in reset.
		//1: PLL is enabled. PLLPWR must be ‘1’.
		//Bit 0: PLLPWR: PLL Power Enable.
		//0: PLL bias generator is de-activated. No static power is consumed.
		//1: PLL bias generator is active. Must be set for PLL to operate.

		//With PLL (remove this part if not using PLL)
		PLL0CN	= 0x00; 					// Set internal oscillator as PLL source (bit2 = 0)
	
		SFRPAGE = LEGACY_PAGE;

		//Set internal oscillator as PLL source
		FLSCL	= FLSCL_FLRT;				// Set FLASH read time according to SYSCLK. Default 0x30 -> 00110000 flash read time for 75MHz <= SYSCLK <= 100MHz

		SFRPAGE = CONFIG_PAGE;
	
		PLL0CN	|= 0x01;					// Enable Power to PLL. Bit0 = 1 -> PLL bias generator is active. Must be set for PLL to operate
	}
	
	PLL0DIV = PLL_DIV;                  // Set PLL divider value using macro (default value = 1)
	PLL0MUL = PLL_MUL;                  // Set PLL multiplier value using macro (default value = 4)
	PLL0FLT = PLLFLT_ICO|PLLFLT_LOOP;   // Set the PLL filter loop and ICO bits
										// Default PLLFLT_ICO = 0x00 (for 72,500,000 <= PLL_OUT_CLK (98,000,000) <= 100,000,000)
										// Default PLLFLT_LOOP = 0x01 (for 19,500,000 <= PLL_DIV_CLK (24,500,000) <= 30,000,000)
										// Default PLL0FLT = 0x01 (00 00 0001) -> Bit5-4 = 00 (PLL output clock 65-100MHz), Bit3-0 = 0001 (Divided PLL Reference Clock = 19-30MHz)
	for (l_us_counter = 0x04FF; l_us_counter != 0; l_us_counter--);			// Wait at least 5us
	PLL0CN  |= 0x02;                    // Enable the PLL -> bit1 = 1 (PLL is enabled)
	while(!(PLL0CN & 0x10));            // Wait until PLL frequency is locked (Bit4 = 0, PLL frequency is not locked, only if Bit4 = 1 -> PLL0CN & 0x10 = true, while() = false)
	CLKSEL  = 0x02;                     // Select PLL as SYSCLK source -> bit1-0 = 10, SYSCLK derived from PLL, normally 00, SYSCLK is derived from internal oscillator)
	//With PLL part ends
	
	#elif (SYSCLK_USED == LEVEL_2_SYSCLK)
	//Initialize external clock first
	//Select the external crystal oscillator
	g_uc_evha_flag &= ~EVENT_FLAG_EXTOSC_FAIL;

	//Initialize oscillator parameters as external
	OSC_FREQUENCY 	= EXT_OSC_FREQUENCY;
	SYSCLK 			= EXT_SYSCLK;

	//Set SFRPAGE to config page
	SFRPAGE = CONFIG_PAGE;

	//External Oscillator Control Register (OSCXCN)
	//Bit 7: XTLVLD: Crystal Oscillator Valid Flag
	//(Valid only when XOSCMD = 11x.)
	//0: Crystal Oscillator is unused or not yet stable
	//1: Crystal Oscillator is running and stable
	//Bits [6..4]: XOSCMD[2..0]: External Oscillator Mode Bits
	//00x: External Oscillator circuit off
	//010: External CMOS Clock Mode (External CMOS Clock input on XTAL1 pin)
	//011: External CMOS Clock Mode with divide by 2 stage (External CMOS Clock input on XTAL1 pin)
	//10x: RC/C Oscillator Mode with divide by 2 stage
	//110: Crystal Oscillator Mode
	//111: Crystal Oscillator Mode with divide by 2 stage
	//Bit3: RESERVED. Read = 0, Write = don't care
	//Bits [2..0]: XFCN[2..0]: External Oscillator Frequency Control Bits
	//000-111: see table below:
	//XFCN	Crystal (XOSCMD = 11x)	RC (XOSCMD = 10x)    	C (XOSCMD = 10x)
	//000 	f = 32 kHz					f = 25 kHz					K Factor = 0.87
	//001 	32 kHz < f = 84 kHz     25 kHz < f = 50 kHz		K Factor = 2.6
	//010 	84 kHz < f = 225 kHz    50 kHz < f = 100 kHz    K Factor = 7.7
	//011 	225 kHz < f = 590 kHz   100 kHz < f = 200 kHz   K Factor = 22
	//100 	590 kHz < f = 1.5 MHz   200 kHz < f = 400 kHz   K Factor = 65
	//101 	1.5 MHz < f = 4 MHz     400 kHz < f = 800 kHz   K Factor = 180
	//110 	4 MHz < f = 10 MHz      800 kHz < f = 1.6 MHz   K Factor = 664
	//111 	10 MHz < f = 30 MHz 

	OSCXCN = 0x60;
	OSCXCN |= XFCN;
	
	//Wait at least 1ms for initialization
	for (l_us_counter = 0; l_us_counter < 30000; l_us_counter++);

	//Reset waiting counter
	l_us_counter = 0;
	
	//Loop until XTLVLD = 1 -> Crystal Oscillator is running and stable
	while (((OSCXCN & 0x80) == 0) && l_us_counter < 30000) l_us_counter++;

	//Check if the waiting time is NOT passed (external clock is working)
	if (l_us_counter < 30000){
		//System Clock Selection Register (CLKSEL)
		//Bits 7–6: Reserved.
		//Bits 5–4: CLKDIV1–0: Output SYSCLK Divide Factor.
		//These bits can be used to pre-divide SYSCLK before it is output to a port pin through the crossbar.
		//00: Output will be SYSCLK.
		//01: Output will be SYSCLK/2.
		//10: Output will be SYSCLK/4.
		//11: Output will be SYSCLK/8.
		//See Section “18. Port Input/Output” on page 235 for more details about routing this output to a port pin.
		//Bits 3–2: Reserved.
		//Bits 1–0: CLKSL1–0: System Clock Source Select Bits.
		//00: SYSCLK derived from the Internal Oscillator, and scaled as per the IFCN bits in OSCICN.
		//01: SYSCLK derived from the External Oscillator circuit.
		//10: SYSCLK derived from the PLL.
		//11: Reserved.
	
		//SYSCLK is derived from External oscillator circuit
		CLKSEL = 0x01;
	}
	//Check if the waiting time is passed (external clock is NOT working)
	else{
		//Reset external oscillator control register
		OSCXCN = 0x00;
	
		//Set oscillator used as internal oscillator
		g_uc_evha_flag |= EVENT_FLAG_EXTOSC_FAIL;

		//Initialize oscillator parameters as internal
		OSC_FREQUENCY 	= INT_OSC_FREQUENCY;
		SYSCLK 			= INT_SYSCLK;

		//Internal Oscillator Control Register (OSCICN)
		//Bit 7: IOSCEN: Internal Oscillator Enable Bit.
		//0: Internal Oscillator Disabled.
		//1: Internal Oscillator Enabled.
		//Bit 6: IFRDY: Internal Oscillator Frequency Ready Flag.
		//0: Internal Oscillator not running at programmed frequency.
		//1: Internal Oscillator running at programmed frequency.
		//Bits 5–2: Reserved.
		//Bits 1–0: IFCN1-0: Internal Oscillator Frequency Control Bits.
		//00: Internal Oscillator is divided by 8.
		//01: Internal Oscillator is divided by 4.
		//10: Internal Oscillator is divided by 2.
		//11: Internal Oscillator is divided by 1.
		OSCICN = 0x83;						// To divide internal oscillator by 1
		CLKSEL = 0x00;		//To use internal oscillator as reference for system clock	
	}
		
	#elif (SYSCLK_USED == LEVEL_1_SYSCLK)
	//Set oscillator used as internal oscillator
	g_uc_evha_flag |= EVENT_FLAG_EXTOSC_FAIL;

	//Initialize oscillator parameters as internal
	OSC_FREQUENCY 	= INT_OSC_FREQUENCY;
	SYSCLK 			= INT_SYSCLK;

	//Internal Oscillator Control Register (OSCICN)
	//Bit 7: IOSCEN: Internal Oscillator Enable Bit.
	//0: Internal Oscillator Disabled.
	//1: Internal Oscillator Enabled.
	//Bit 6: IFRDY: Internal Oscillator Frequency Ready Flag.
	//0: Internal Oscillator not running at programmed frequency.
	//1: Internal Oscillator running at programmed frequency.
	//Bits 5–2: Reserved.
	//Bits 1–0: IFCN1-0: Internal Oscillator Frequency Control Bits.
	//00: Internal Oscillator is divided by 8.
	//01: Internal Oscillator is divided by 4.
	//10: Internal Oscillator is divided by 2.
	//11: Internal Oscillator is divided by 1.
	OSCICN = 0x80;		//To divide internal oscillator by 8
	CLKSEL = 0x00;		//To use internal oscillator as reference for system clock	
	#endif

	SFRPAGE = SFRPAGE_SAVE;
}

//------------------------------------------------------------------------------------
// Port Initialization
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
//Ian (2012-05-15): the ports which are not initialized here are "input" ports by default
void initPort (void)
{
	char SFRPAGE_SAVE = SFRPAGE;

	SFRPAGE = CONFIG_PAGE;

	XBR0 |= 0x07;		//To enable UART0 (XBR0^2), SPI (XBR0^1), and SM Bus (XBR0^0)
	XBR2 |= 0x44;		//To enable crossbar and weak pull-ups

	P0MDOUT &= ~0xC0;	//P0^6-7 pins open-drain output (that SDA and SCL lines 
						//may draw current from outsource power supply)
					
						//To make P0^2 to P0^5 (SPI pins) push-pull pins too
	P0MDOUT |= 0x3D;	//Make UART0 TX(P0.0 push-pull) (that it may send data
						//through UART cable by its own power supply)
	P0 = 0xFF;			//By default, all pins will be 'high'

	P1MDOUT = 0x00;		//All P1 pins open-drain output (that UART1 
						//may draw current from outsource power supply)
	P1MDOUT |= 0x01;	//Make UART1 TX(P1.0 push-pull) (that it may send data
						//through UART cable by its own power supply)
	P1 = 0xFF;			//By default, all pins will be 'high'

	P4MDOUT |= 0x12;  //To make P4^1 & P4^4 pins (-ON_SD pin & -OE_USB) as push-pull

	P4 = 0xFF; //make sure that all pins in port 4 are enabled as high first.

	OE_USB	= ON_USB;		//Enable USB port
	ON_SD 	= SD_ASSERT; 	//Turn on SD Card
	NSSMD0 	= SD_DEASSERT;	//Disable chip select for now

	SFRPAGE = SFRPAGE_SAVE;	
}

//------------------------------------------------------------------------------------
// initEMIF()
//------------------------------------------------------------------------------------
void initEMIF (void)
{
	char SFRPAGE_SAVE = SFRPAGE;

	SFRPAGE = EMI0_PAGE;


	SFRPAGE = SFRPAGE_SAVE;	
}

//------------------------------------------------------------------------------------
// initSPI()
//------------------------------------------------------------------------------------
void initSPI (void)
{
	char SFRPAGE_SAVE = SFRPAGE;

	SFRPAGE = SPI0_PAGE;

	//With PLL
	SPI0CKR = (SYSCLK /(SPICLK*2)) - 1;

	SPI0CFG |= 0x40;
	NSSMD0 	= 1;
	NSSMD1 	= 1;
	SPIEN 	= 1;
	//EIE1 	= 0x01; 		//Enable SPI0 Interrupt

	SFRPAGE = SFRPAGE_SAVE;	
}

//------------------------------------------------------------------------------------
// UART0 Initialization
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
void initUart0 (void)
{
	char SFRPAGE_SAVE = SFRPAGE;

	SFRPAGE = UART0_PAGE;

	SCON0 = 0x50;		//SCON: mode 1, 8-bit UART, enable RX and TX
	SSTA0 = 0x05;		//Following Timer2 for UART Baud Rate source for
						//Transfer and Receive, Timer2 overflows generating
						//TX and RX Baud Rate
	ES0 = 1;			//Enable UART0 interrupt
	IP |= 0x10;			//UART0 interrupt is set to high priority

	SFRPAGE = SFRPAGE_SAVE;
}

//------------------------------------------------------------------------------------
// UART1 Initialization
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
void initUart1 (void)
{
	char SFRPAGE_SAVE = SFRPAGE;

	SFRPAGE = UART1_PAGE;

	SCON1 = 0x50;		//SCON: mode 1, 8-bit UART, enable RX and TX
	EIE2 |= 0x40;		//Enable UART1 interrupt, TI1 is set to '1' when transmitting
						//RI1 is set to '1' when receiving
	EIP2 |= 0x40;		//UART1 interrupt is set to high priority
	
	SFRPAGE = SFRPAGE_SAVE;
}

//------------------------------------------------------------------------------------
// SMBus Initialization
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
void initSMBus (void)
{
	char SFRPAGE_SAVE = SFRPAGE;		
	
	SFRPAGE = SMB0_PAGE;
	
	SMB0CN = 0x07;	//bit2 = 1, Assert Acknowledge low (AA bit = 1b) during ACK cycle (not NACK during ACK cycle)
					//bit1 = 1, enable SMBus free timeout detect (as specified by SMB0CR value)
					//bit0 = 1, enable SCL low timeout detect (as specified by timer3 overflow)

	//With PLL	
	SMB0CR = 257 - (SYSCLK / (8 * SMB_FREQUENCY));	//using SYSCLK 98MHz and SMB freq 400KHz, it is about 226
													//Thus, SMBus free timeout period is about 12.35 us
													//TLow = 1.22 us, THigh = 1.93 us
	
	SMB0ADR = OBDH_ADDR;	//set own address.
	SMB0CN |= 0x40;			//bit6 = 1, enable SMBus;
	EIE1 |= 0x02;			//enable the SMBus interrupt 
	
	SFRPAGE = SFRPAGE_SAVE;             
}

#if (DEBUGGING_FEATURE == 1)
//------------------------------------------------------------------------------------
// Timer1 Initialization, used for UART1
// The ISR software for this timer is not used, nevertheless this timer needs
// to be set to control UART speed, less the uC cannot communicate with PC.
// If debugging feature is disabled, however, this function will be removed
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
void initTimer1 (void)
{
	char SFRPAGE_SAVE = SFRPAGE;

	SFRPAGE = TIMER01_PAGE;

	TMOD &= ~0x30;	//Clear bit5-4
	TMOD |= 0x20;	//Timer1 mode is mode 2: 8-bit auto reload
	CKCON &= ~0x13;	//Clear bit4 and bit1-0

	//With PLL
	CKCON |= 0x01;	//bit4 = 0 -> timer1 is using pre clock defined by prescale bits (bit1-0)
					//bit1-0 = 01 -> Clock source is pre-scaled (SYSCLK/4 = 24.5MHz (internal)/24.8832Mhz (external)) clock input	
	TH1	= 256-(SYSCLK/4/2/BAUDRATE);	//Reload value = 0x96 (150), no of count ups = 256-150 = 106 - internal oscillator
										//Reload value = 0x94 (148), no of count ups = 256-148 = 108 - external oscillator
										//UART1 baudrate = 98,000,000/4/106/2 = 115566bps (115200) - internal oscillator
										//UART1 baudrate = 99,532,800/4/108/2 = 115200bps - external oscillator

	TL1 = TH1;		//Initialize TL1 value as TH1
	
	TR1 = 1;		//Start timer1

	SFRPAGE = SFRPAGE_SAVE;		
}

//------------------------------------------------------------------------------------
// Timer2 Initialization, used for UART0
// The ISR software for this timer is not used, nevertheless this timer needs
// to be set to control UART speed, less the uC cannot communicate with PC.
// If debugging feature is disabled, however, this function will be removed
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
void initTimer2 (void)
{
	char SFRPAGE_SAVE = SFRPAGE;

	SFRPAGE = TMR2_PAGE;

	TMR2CN = 0x00;	//Timer in 16-bit auto-reload up timer mode
	TMR2CF = 0x08;	//SYSCLK is time base; no output; up count only

	//With PLL
	RCAP2 = 0xFFFF-(SYSCLK/BAUDRATE/16)+1;	//The value is set up for UART0 
											//SYSCLK = 98MHz, BAUDRATE = 115200 (113425)
											//SYSCLK = 99.5328MHz, BAUDRATE = 115200

	TMR2 = RCAP2;
	
	TR2 = 1;		//Start timer2

	SFRPAGE = SFRPAGE_SAVE;		
}
#endif

//------------------------------------------------------------------------------------
// Timer3 Initialization, used for SMBus line (SCL low) timeout
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
void initTimer3 (void)
{
	char SFRPAGE_SAVE = SFRPAGE;

	SFRPAGE = TMR3_PAGE;

	TMR3CN = 0x00;	//bit3 = 0, timer3 external enable off (ignore external disturbance)
					//bit2 (TR3) = 0, timer3 is disabled (later needs to be enabled)
					//bit1 = 0, timer3 is in timer mode - not counter mode 
					//bit0 = 0, timer3 in auto-reload mode

	TMR3CF = 0x00;	//bit4-3 = 00, timer3 uses SYSCLK/12
					//bit1 = 0, timer3 output not available
					//bit0 = 0, timer3 counts up
	
	#if (SYSCLK_USED == LEVEL_3_SYSCLK)
	//With PLL
	RCAP3 = -(SYSCLK/12000*7);	//to configure overflow after 7ms
								//this is used to set SCL low timeout period
	#elif (SYSCLK_USED == LEVEL_2_SYSCLK)
	//Without PLL
	RCAP3 = -(SYSCLK/12000*25);	//to configure overflow after 25ms
								//this is used to set SCL low timeout period
	#elif (SYSCLK_USED == LEVEL_1_SYSCLK)
	//Without PLL
	RCAP3 = -(SYSCLK/12000*25);	//to configure overflow after 25ms
								//this is used to set SCL low timeout period
	#endif

	TMR3 = RCAP3;				//timer3 initial value (-51041)

	EIE2 |= 0x01;	//Enable timer3 interrupt
	TR3 = 1;		//Start timer3

	SFRPAGE = SFRPAGE_SAVE;		
}

//------------------------------------------------------------------------------------
// Timer4 Initialization, acts as the primary timer for the system
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
void initTimer4 (void)
{
	char SFRPAGE_SAVE = SFRPAGE;

	SFRPAGE = TMR4_PAGE;

	TMR4CN = 0x00;	//bit3 = 0, timer4 external enable off (ignore external disturbance)
					//bit2 (TR4) = 0, timer4 is disabled (later needs to be enabled)
					//bit1 = 0, timer4 is in timer mode - not counter mode 
					//bit0 = 0, timer4 in auto-reload mode

	TMR4CF = 0x00;	//bit4-3 = 00, timer4 uses SYSCLK/12
					//bit1 = 0, timer4 output not available
					//bit0 = 0, timer4 counts up
	
	//With PLL
	RCAP4 = -(SYSCLK/12000*5);	//to configure overflow every 5ms
								//this is used for global software timer

	TMR4 = RCAP4;		//timer4 initial value

	EIE2 |= 0x04;		//Enable timer4 interrupt
	EIP2 |= 0x04;		//Set timer4 interrupt to high priority
	TR4 = 1;			//Start timer4

	SFRPAGE = SFRPAGE_SAVE;		
}

//-----------------------------------------------------------------------------
// initA2DConverter
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure ADC0 to use Timer3 overflows as conversion source, to
// generate an interrupt on conversion complete, and to use right-justified
// output mode. Enables ADC end of conversion interrupt. Leaves ADC disabled.
//
//-----------------------------------------------------------------------------
void initA2DConverter (void)
{
	//Save Current SFR page
	char SFRPAGE_SAVE = SFRPAGE;

	SFRPAGE = ADC0_PAGE;

	//ADC0 disabled; normal tracking mode; ADC0 conversions are initiated every write of '1' to AD0BUSY; ADC0 data is right-justified
	ADC0CN = 0x40;

	//Enable temp sensor, on-chip VREF, and VREF output buffer
	REF0CN = 0x07;

	//AIN inputs are single-ended (default)
	AMX0CF = 0x00;

	//Select internal temperature sensor as the ADC mux input
	AMX0SL = 0x0F;

	//ADC conversion clock = 2.5MHz
	ADC0CF = (unsigned char)(((SYSCLK / SAR_CLK / 2) - 1) << 3);

	//PGA gain = 0.5 (default = 1, ADC0CF = 0x00)
	ADC0CF |= 0x07;

	//Enable ADC
	AD0EN = 1;

	//Restore SFR page
	SFRPAGE = SFRPAGE_SAVE;
}

#if (TIME_ENABLE == 1)
//------------------------------------------------------------------------------------
// Satellite Date Initialization
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
//Ian (2012-05-15): the launch date is unknown until today.
//Ian (2013-08-13): the launch date & time is expected to be on 2013-10-14 07:25:48
void initSatelliteDate (void)
{
	str_sat_date.us_init_ms 				= twoDec2oneHex(SATELLITE_INIT_MS) * 10;
	str_sat_date.uc_init_s 					= twoDec2oneHex(SATELLITE_INIT_S);
	str_sat_date.uc_init_min 				= twoDec2oneHex(SATELLITE_INIT_MIN);
	str_sat_date.uc_init_hour 				= twoDec2oneHex(SATELLITE_INIT_HOUR);
	str_sat_date.uc_init_day_of_week 		= twoDec2oneHex(SATELLITE_INIT_DAY_OF_WEEK);
	str_sat_date.uc_init_day_of_month 		= twoDec2oneHex(SATELLITE_INIT_DAY_OF_MONTH);
	str_sat_date.uc_init_month 				= twoDec2oneHex(SATELLITE_INIT_MONTH);
	str_sat_date.uc_init_year 				= twoDec2oneHex(SATELLITE_INIT_YEAR);
	str_sat_date.us_current_ms 				= str_sat_date.us_init_ms;
	str_sat_date.uc_current_s 				= str_sat_date.uc_init_s;
	str_sat_date.uc_current_min 			= str_sat_date.uc_init_min;
	str_sat_date.uc_current_hour 			= str_sat_date.uc_init_hour;
	str_sat_date.uc_current_day_of_week 	= str_sat_date.uc_init_day_of_week;
	str_sat_date.uc_current_day_of_month 	= str_sat_date.uc_init_day_of_month;
	str_sat_date.uc_current_month 			= str_sat_date.uc_init_month;
	str_sat_date.uc_current_year 			= str_sat_date.uc_init_year;
}

//------------------------------------------------------------------------------------
// Satellite Time Initialization
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
void initSatelliteTime (void)
{
	str_sat_time.us_ms			= 0;
	str_sat_time.uc_s			= 0;
	str_sat_time.uc_min			= 0;
	str_sat_time.uc_hour		= 0;
	str_sat_time.us_day			= 0;	

	#if (RTC_CMD_ENABLE == 1)
	#if (DEBUGGING_FEATURE == 1)
	rtcDateInit(0);
	#else
	rtcDateInit();
	#endif
	#endif
}
#endif

//------------------------------------------------------------------------------------
// Satellite State Initialization
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
void initSatelliteState (void)
{
	unsigned char l_uc_i = 0;

	//Initialize OBDH housekeeping's initial value
	str_obdh_hk.uc_sat_mode							= SATELLITE_MODE_M2;
	str_obdh_hk.uc_gsch_state						= GSCH_STATE_IDLE; //First, initialize this to be in the early mode
	str_obdh_hk.ul_wod_sample_count					= 0; //No hk sets stored at first
	str_obdh_hk.uc_scheduler_block_size 			= 0; //At first, nothing is in the scheduler
	str_obdh_hk.us_beacon_flag						= 0x0000; //Uninitialized beacon flag
	str_obdh_hk.us_brh_flag 						= 0x0000; //Uninitialized BRH flag
	str_obdh_hk.us_imh_flag 						= 0x0000; //Uninitialized IMH flag
	str_obdh_hk.uc_smh_flag 						= 0x00; //Uninitialized SMH flag
	str_obdh_hk.us_cw_char_rate_par 				= CW_BEACON_RATE_DEFAULT; //Original = 6 dots/second
	str_obdh_hk.uc_ax25_bit_rate_par 				= BITRATE_1200; //1200 bits/second	
	str_obdh_hk.uc_mcu_rst_count					= 0; //Initialize reset count as 0
	str_obdh_hk.ul_data_byte_sent					= 0; //At first, zero byte has been sent by the satellite
	str_obdh_hk.uc_sat_rstsrc						= RESET_SOURCE_UNKNOWN;	//Initialized as unknown unless proven otherwise later

	//Initialize OBDH other data's initial value
	str_obdh_data.uc_sat_rstsrc						= RESET_SOURCE_UNKNOWN;	//Initialized as unknown unless proven otherwise later
	str_obdh_data.ul_obc_time_s						= 0; //Initialize OBC time as zero
	str_obdh_data.us_current_rev_no					= 0; //First, declare this as zero
	str_obdh_data.us_rev_offset_time				= 0; //Initialize this as zero
	str_obdh_data.ul_latest_hk_sampling_time_s 		= 0; //Initialize this as zero, this is for housekeeping data
	str_obdh_data.us_latest_hk_sampling_orbit 		= 0; //Initialize this as zero,	this is for housekeeping data
	str_obdh_data.uc_software_mode 					= SOFTWARE_REAL_MODE; //Starts the software in REAL mode, not LOOPHOLD
	str_obdh_data.uc_no_of_data_cat_sent			= 1;
	str_obdh_data.ul_log_data_start_time 			= 0x00000000;
	str_obdh_data.us_log_data_duration_s 			= 0x0000;
	str_obdh_data.us_crc16_poly						= 0x8005;
	str_obdh_data.us_auto_reference_rev_no			= 1;	//First, declare this as one
	str_obdh_data.uc_timer4_interrupt_counter 		= 0;
	str_obdh_data.uc_orbit_no_changed_counter 		= 0;

	for (l_uc_i = 0; l_uc_i < OBDH_DATA_INTERVAL_PERIOD_SIZE; l_uc_i++)
		str_obdh_data.us_interval_period[l_uc_i] 	= 0;

	//Initialize other data
	str_pwrs_hk.uc_batt_soc							= 100; 		//Initialize POWER as maximum
	str_obdh_data.uc_batt_soc						= 100; 		//Initialize POWER as maximum
	str_pwrs_data.uc_channel_status					= 0x12;		//Channel status is assumed to be 0x12 until proven otherwise
	str_antenna_hk.us_deployment_status				= INITIAL_ANTENNA_STATUS;	//Initialize antenna as not deployed.
	//Ian (2012-05-15): comment on luxp_extra.c
	for (l_uc_i = 0; l_uc_i < BR_ECI_SAMPLE_NO; l_uc_i++){
		g_s_br_eci[0][l_uc_i] = 1000;	//Initialize body rate X sample data points as 1000/20 = 50 deg/s
		g_s_br_eci[1][l_uc_i] = 1000;	//Initialize body rate Y sample data points as 1000/20 = 50 deg/s
		g_s_br_eci[2][l_uc_i] = 1000;	//Initialize body rate Z sample data points as 1000/20 = 50 deg/s
	}
	str_obdh_data.us_br_eci_avg[0] = 1000; //Initialize body rate X average as 1000/20 = 50 deg/s
	str_obdh_data.us_br_eci_avg[1] = 1000; //Initialize body rate Y average as 1000/20 = 50 deg/s
	str_obdh_data.us_br_eci_avg[2] = 1000; //Initialize body rate Z average as 1000/20 = 50 deg/s

	//Initialize body rate as high unless proven otherwise
	str_obdh_hk.us_brh_flag |= BRH_FLAG_BR_HI;
}

#if (CRC_ENABLE == 1)
//------------------------------------------------------------------------------------
// CRC Initialization
//------------------------------------------------------------------------------------
// Function's test result: fully functional	(tested in Dev C++)
//------------------------------------------------------------------------------------
void initCRC (void)
{
	unsigned char 	l_uc_counter;
	unsigned char 	l_uc_crc16_top_byte_register;
	unsigned char	l_uc_crc16_control_bit[8] = 0;
	unsigned char	l_uc_crc16_poly_top_byte = 0x80;
	unsigned short	l_us_temp_crc16_poly = 0x0000;
	unsigned short 	l_us_top_byte;

	//To produce control bytes from every possible top byte	
	//Ian (2012-05-15): the limit is changed to CRC_TABLE_SIZE
	for (l_us_top_byte = 0x00; l_us_top_byte < CRC_TABLE_SIZE; l_us_top_byte++)	
	{
		l_uc_counter 					= 0x08;
		l_uc_crc16_top_byte_register 	= l_us_top_byte;

		while (l_uc_counter){
			//Check top bit of the register
			if((l_uc_crc16_top_byte_register >> 7) & 0x01){
				l_uc_crc16_top_byte_register = ((l_uc_crc16_top_byte_register << 1) & 0xFE) ^ l_uc_crc16_poly_top_byte;
				l_uc_crc16_control_bit[8-l_uc_counter] = 0x01;
			}
			else{
				l_uc_crc16_top_byte_register = (l_uc_crc16_top_byte_register << 1);
				l_uc_crc16_control_bit[8-l_uc_counter] = 0x00;
			}
				
			l_uc_counter--;      
		}
    
		//To create CRC16 polynomial tables
		l_us_temp_crc16_poly = 0x0000;    

		for (l_uc_counter = 0x00; l_uc_counter < 0x08; l_uc_counter++){
			if (l_uc_crc16_control_bit[l_uc_counter]) //if the control bit is one          
				l_us_temp_crc16_poly ^= (str_obdh_data.us_crc16_poly << (7-l_uc_counter));
		}

		g_us_crc16_table[l_us_top_byte] = l_us_temp_crc16_poly;
		
		for (l_uc_counter = 0x00; l_uc_counter < 0x08; l_uc_counter++){
			l_uc_crc16_control_bit[l_uc_counter] = 0x00;		        
		}
	}
}	
#endif

//------------------------------------------------------------------------------------
// Memory Block Pointer Initialization
//------------------------------------------------------------------------------------
// Function's test result: fully functional	(tested in Dev C++)
//------------------------------------------------------------------------------------
void initBlockPointer (void)
{
	str_bp.ul_hk_write_p 				 = SD_BP_HK_START;
	str_bp.ul_datalog_write_p 			 = SD_BP_DATALOG_START;
	str_bp.ul_package_recovery_write_p 	 = SD_BP_PACKAGE_RECOVERY_START;
	str_bp.ul_eventlog_write_p 			 = SD_BP_EVENTLOG_START;//dec22_yc
	str_bp.uc_orbit_indexing_overwritten = 0x00;
	str_bp.uc_hk_overwritten 			 = 0x00;
}

//------------------------------------------------------------------------------------
// OBDH Paramaters' Initialization
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
void initOBDHParameters (void)
{
	unsigned char l_uc_i = 0;

	//Initialize OBDH SSCP values
	str_obdh_par.uc_manual_control_enabled			= 0; 	//At first, the satellite is in the auto-mode
	str_obdh_par.uc_no_of_interrupt_per_s			= 200;
	str_obdh_par.uc_gsch_time_window_10s			= THRESHOLD_GSCH_TIME_WINDOW_DEFAULT_S/10;
	str_obdh_par.uc_soc_threshold[0] 				= THRESHOLD_SOC_M3_TO_M4_DEFAULT;
	str_obdh_par.uc_soc_threshold[1] 				= THRESHOLD_SOC_M4_TO_M3_DEFAULT;
	str_obdh_par.uc_soc_threshold[2] 				= THRESHOLD_SOC_M7_TO_M8_DEFAULT;
	str_obdh_par.uc_soc_threshold[3] 				= THRESHOLD_SOC_LISTEN_MCC_DEFAULT;
	str_obdh_par.uc_soc_threshold[4] 				= THRESHOLD_SOC_TURN_OFF_COMM_DEFAULT;
	str_obdh_par.uc_soc_threshold[5] 				= 0;
	str_obdh_par.uc_soc_threshold[6] 				= THRESHOLD_SOC_RESET_SAT_DEFAULT;
	str_obdh_par.uc_soc_threshold[7] 				= 0;
	str_obdh_par.us_rev_time_s						= 5700;	//Initialize satellite revolution time as 5700s (95 minutes)

	//Initialize log data mission parameters
	for (l_uc_i = 0; l_uc_i < MAX_NO_OF_DATA_TYPE_LOGGED; l_uc_i++)
		str_obdh_par.us_log_data_id[l_uc_i] 		= 0x0000;

	str_obdh_par.us_br_threshold_low[0]				= THRESHOLD_BR_X_LOW_DEFAULT;
	str_obdh_par.us_br_threshold_low[1]				= THRESHOLD_BR_Y_LOW_DEFAULT;
	str_obdh_par.us_br_threshold_low[2]				= THRESHOLD_BR_Z_LOW_DEFAULT;
	str_obdh_par.us_br_threshold_high[0]			= THRESHOLD_BR_X_HIGH_DEFAULT;
	str_obdh_par.us_br_threshold_high[1]			= THRESHOLD_BR_Y_HIGH_DEFAULT;
	str_obdh_par.us_br_threshold_high[2]			= THRESHOLD_BR_Z_HIGH_DEFAULT;
	str_obdh_par.uc_beacon_interval[0]				= CW_BEACON_INTERVAL_M3;
	str_obdh_par.uc_beacon_interval[1]				= CW_BEACON_INTERVAL_M4_M5_M7;
	str_obdh_par.uc_beacon_interval[2]				= CW_BEACON_INTERVAL_M8;
	str_obdh_par.uc_beacon_interval[3]				= CW_BEACON_INTERVAL_COMM_ZONE;
	str_obdh_par.ul_det_uptime_limit_s 				= TIME_LIMIT_DET_UPTIME_INITIAL_S;
}

#if (TASK_ENABLE == 1)
//------------------------------------------------------------------------------------
// Task Flags' Initialization
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
void initTaskFlags (void)
{
	unsigned char l_uc_i;

	for (l_uc_i = 0; l_uc_i < SCRIPTLINE_TIMER_SIZE; l_uc_i++){
		str_scriptline_timer[l_uc_i].uc_scriptspace_owner 	= SP_SCRIPTSPACE_EMPTY;
		str_scriptline_timer[l_uc_i].uc_scriptline_owner 	= 0x00;
		str_scriptline_timer[l_uc_i].uc_started 			= 0x00;
		str_scriptline_timer[l_uc_i].uc_counter 			= 0x00;
		str_scriptline_timer[l_uc_i].ul_timer 				= 0x00000000;
	}

	for (l_uc_i = 0; l_uc_i < TASK_SIZE; l_uc_i++){
		//Simplification of the task reset (deleting the redundancy)
		resetTask(l_uc_i);

		//So long as it is within the semi-permanent script space,
		//assign dedicated script space for a task
		if (l_uc_i < SEMI_PERMANENT_SCRIPTSPACE_SIZE)
			str_task[l_uc_i].uc_scriptspace 					= l_uc_i;
		//Else, assign the last	scriptspace for that task
		else str_task[l_uc_i].uc_scriptspace 					= SCRIPTSPACE_SIZE-1;
	}

	for (l_uc_i = 0; l_uc_i < SCRIPTLINE_RETRY_COUNTER_SIZE; l_uc_i++){
		str_scriptline_retry_counter[l_uc_i].uc_scriptspace_owner 	= SP_SCRIPTSPACE_EMPTY;
		str_scriptline_retry_counter[l_uc_i].uc_scriptline_owner 	= 0x00;
		str_scriptline_retry_counter[l_uc_i].uc_counter 			= 0x00;
	}
}

//------------------------------------------------------------------------------------
// Task's script Initialization
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
void initTaskScript (void){
	unsigned char l_uc_i = 0;
	unsigned char l_uc_j = 0;
	unsigned char l_uc_k = 0;

	for (l_uc_i = 0; l_uc_i < SCRIPTSPACE_SIZE; l_uc_i++)
		for (l_uc_j = 0; l_uc_j < MAX_SCRIPTLINE_SIZE; l_uc_j++)
			for (l_uc_k = 0; l_uc_k < MAX_SCRIPT_LENGTH; l_uc_k++)	
				str_scriptspace[l_uc_i].uc_script[l_uc_j][l_uc_k] = 0;

	//Various task's scripts initialization
	initGetHKScript();
	initDeployAntennaScript();
	initLogSTDataScript();
	initTestTKScript();
}

//------------------------------------------------------------------------------------
// Get housekeeping script Initialization
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
void initGetHKScript (void)
{
	unsigned char l_uc_i;
	unsigned char l_uc_running_no = 0;

//------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	// Init get all housekeeping task's script
	//--------------------------------------------------------------------------------
	// Scriptline 0			: to change the subtask's flags accordingly
	// 	mode used			: normal running task (00000)
	//	intended change		: 0000 0000 0000 1110 (0x000E)
	//--------------------------------------------------------------------------------
	enterScriptline(SP_SCRIPTSPACE_GET_HK,l_uc_running_no++,SCRIPT_CMD_CHANGE_TASK_FLAGS,0x00,0x00,0x0E);

	//--------------------------------------------------------------------------------
	// Scriptline 1			: to get HK Data
	// Properties			: timed, non-last, non-critical: 100x xxxx (0x80)
	// Interval used		: no. 0			(xxx0 0000) - 0x00
	// Completed end doing	: nothing 		(0000) - 0
	// Error end doing		: nothing		(0000) - 0
	// Channel dependency	: nothing
	// CD fail end doing	: nothing		(0000) - 0
	// Argument				: nothing		(0000) - 0
	//--------------------------------------------------------------------------------
	str_obdh_data.us_interval_period[0] = 100; //The interval is the value (100) x 100 ms = 10s
	enterScriptline(SP_SCRIPTSPACE_GET_HK,l_uc_running_no++,SCRIPT_CMD_GET_HK			,0x80,0x00,0x00);

	//--------------------------------------------------------------------------------
	// Scriptline 2			: to store HK Data
	// Properties			: timed, non-last, non-critical: 100x xxxx (0x80)
	// Interval used		: no. 1			(xxx0 0001) - 0x01
	// Completed end doing	: nothing 		(0000) - 0
	// Error end doing		: nothing		(0000) - 0
	// Channel dependency	: nothing
	// CD fail end doing	: nothing		(0000) - 0
	// Argument				: nothing		(0000) - 0
	//--------------------------------------------------------------------------------
	str_obdh_data.us_interval_period[1] = 600; //The interval is the value (600) x 100 ms = 60s
	enterScriptline(SP_SCRIPTSPACE_GET_HK,l_uc_running_no++,SCRIPT_CMD_STORE_HK			,0x81,0x00,0x00);

	//--------------------------------------------------------------------------------
	// Scriptline 3			: to get SSCP Data
	// Properties			: timed, non-last, non-critical: 100x xxxx (0x80)
	// Interval used		: no. 3			(xxx0 0011) - 0x03
	// Completed end doing	: nothing 		(0000) - 0
	// Error end doing		: nothing		(0000) - 0
	// Channel dependency	: nothing
	// CD fail end doing	: nothing		(0000) - 0
	// Argument				: nothing		(0000) - 0
	//--------------------------------------------------------------------------------
	str_obdh_data.us_interval_period[3] = 100; //The interval is the value (100) x 100 ms = 10s
	enterScriptline(SP_SCRIPTSPACE_GET_HK,l_uc_running_no++,SCRIPT_CMD_GET_SSCP			,0x83,0x00,0x00);

	//Pad the rests of the script to zero
	for (l_uc_i = l_uc_running_no; l_uc_i < MAX_SCRIPTLINE_SIZE; l_uc_i++) //Put zero for the rests
		enterScriptline(SP_SCRIPTSPACE_GET_HK,l_uc_i,0x00,0x00,0x00,0x00);
//------------------------------------------------------------------------------------
}

//------------------------------------------------------------------------------------
// Deploy antenna script initialization
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
void initDeployAntennaScript (void)
{
	unsigned char l_uc_i;
	unsigned char l_uc_running_no = 0;

//------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	// Set DPLY channel to ON
	//--------------------------------------------------------------------------------
	// Scriptline 0			: to set a PWRS' channel state
	// Properties			: non-timed, non-last, critical: 001x xxxx (0x20)
	// Interval used		: NA			(xxx0 0000) - 0x00
	// Completed end doing	: progress (1) 	(1001) - 9
	// Error end doing		: nothing		(0000) - 0
	// Channel dependency	: nothing
	// Argument				: turn ON,		(1000) - 8
	//						  channel 3     (0011) - 3
	//--------------------------------------------------------------------------------
	enterScriptline(SP_SCRIPTSPACE_DEPLOY_ANTENNA,l_uc_running_no++,SCRIPT_CMD_SET_SINGLE_PWRS_CHANNEL,0x20,0x90,0x83);

	//--------------------------------------------------------------------------------
	// Check DPLY channel's status, whether it has been turned ON
	//--------------------------------------------------------------------------------
	// Scriptline 1			: to check a PWRS' channel status
	// Properties			: timed, non-last, non-critical: 100x xxxx (0x80)
	// Interval used		: 7 			(xxx0 0111) - 0x07
	// Completed end doing	: progress (1) 	(1001) - 9
	// Error end doing		: regress (1)	(0001) - 1
	// Channel dependency	: nothing
	// Argument				: turn ON		(1000) - 8
	//						  channel 3 bit	(1000) - 8
	//--------------------------------------------------------------------------------
	str_obdh_data.us_interval_period[7] = 40; //The interval is the value (40) x 100 ms = 4s
	enterScriptline(SP_SCRIPTSPACE_DEPLOY_ANTENNA,l_uc_running_no++,SCRIPT_CMD_CHECK_SINGLE_PWRS_CHANNEL,0x87,0x91,0x88);

	//--------------------------------------------------------------------------------
	// Arm and starts antenna deployment mechanism
	//--------------------------------------------------------------------------------
	// Scriptline 2			: to arm and starts antenna deployment mechanism
	// Properties			: non-timed, non-last, non-critical: 001x xxxx (0x00)
	// Interval used		: NA			(xxx0 0000) - 0x00
	// Completed end doing	: progress (1) 	(1001) - 9
	// Error end doing		: progress (2)  (1010) - A
	// Channel dependency	: nothing
	// Argument				: nothing
	//--------------------------------------------------------------------------------
	enterScriptline(SP_SCRIPTSPACE_DEPLOY_ANTENNA,l_uc_running_no++,SCRIPT_CMD_ANTENNA_AUTO_DPLY,0x00,0x9A,0x00);

	//--------------------------------------------------------------------------------
	// To check antenna deployment status
	//--------------------------------------------------------------------------------
	// Scriptline 3			: to check antenna deployment status
	// Properties			: timed, non-last, non-critical: 100x xxxx (0x80)
	// Interval used		: 8 			(xxx0 1000) - 0x08
	// Completed end doing	: progress (1) 	(1001) - 9
	// Error end doing		: progress (1) 	(1001) - 9
	// Channel dependency	: nothing
	// Argument				: nothing
	//--------------------------------------------------------------------------------
	str_obdh_data.us_interval_period[8] = 50; //The interval is the value (50) x 100 ms = 5s
	enterScriptline(SP_SCRIPTSPACE_DEPLOY_ANTENNA,l_uc_running_no++,SCRIPT_CMD_CHECK_ANTENNA_DPLY_STAT,0x88,0x99,0x00);

	//--------------------------------------------------------------------------------
	// Set DPLY channel to OFF
	//--------------------------------------------------------------------------------
	// Scriptline 4			: to set a PWRS' channel state
	// Properties			: non-timed, non-last, critical: 001x xxxx (0x20)
	// Interval used		: NA			(xxx0 0000) - 0x00
	// Completed end doing	: progress (1) 	(1001) - 9
	// Error end doing		: nothing		(0000) - 0
	// Channel dependency	: nothing
	// Argument				: turn OFF,		(0000) - 0
	//						  channel 3     (0011) - 3
	//--------------------------------------------------------------------------------
	enterScriptline(SP_SCRIPTSPACE_DEPLOY_ANTENNA,l_uc_running_no++,SCRIPT_CMD_SET_SINGLE_PWRS_CHANNEL,0x20,0x90,0x03);

	//--------------------------------------------------------------------------------
	// Check DPLY channel's status, whether it has been turned OFF
	//--------------------------------------------------------------------------------
	// Scriptline 5			: to check a PWRS' channel status
	// Properties			: timed, last, non-critical: 110x xxxx (0xC0)
	// Interval used		: 9 			(xxx0 1001) - 0x09
	// Completed end doing	: nothing 		(0000) - 0
	// Error end doing		: regress (1)	(0001) - 1
	// Channel dependency	: nothing
	// Argument				: turn OFF		(0000) - 0
	//						  channel 3 bit	(1000) - 8
	//--------------------------------------------------------------------------------
	str_obdh_data.us_interval_period[9] = 40; //The interval is the value (40) x 100 ms = 4s
	enterScriptline(SP_SCRIPTSPACE_DEPLOY_ANTENNA,l_uc_running_no++,SCRIPT_CMD_CHECK_SINGLE_PWRS_CHANNEL,0xC9,0x01,0x08);

	//Pad the rests of the script to zero
	for (l_uc_i = l_uc_running_no; l_uc_i < MAX_SCRIPTLINE_SIZE; l_uc_i++) //Put zero for the rests
		enterScriptline(SP_SCRIPTSPACE_DEPLOY_ANTENNA,l_uc_i,0x00,0x00,0x00,0x00);
//------------------------------------------------------------------------------------
}

//------------------------------------------------------------------------------------
// Log special telemetry data script Initialization
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
void initLogSTDataScript (void)
{
	unsigned char l_uc_i;
	unsigned char l_uc_running_no = 0;

//------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	// Init log data task's script
	//--------------------------------------------------------------------------------
	// Scriptline 0			: to initialize logging data task
	// Properties			: non-timed, non-last, non-critical: 000x xxxx (0x00)
	// Interval used		: NA			(xxx0 0000) - 0x00
	// Completed end doing	: progress (1) 	(1001) - 9
	// Error end doing		: nothing		(0000) - 0
	// Channel dependency	: nothing
	// CD fail end doing	: nothing		(0000) - 0
	// Argument				: nothing		(0000) - 0
	//--------------------------------------------------------------------------------
	enterScriptline(SP_SCRIPTSPACE_LOG_ST_DATA,l_uc_running_no++,SCRIPT_CMD_INIT_LOG_DATA,0x00,0x90,0x00);

	//--------------------------------------------------------------------------------
	// Log data task's script
	//--------------------------------------------------------------------------------
	// Scriptline 1			: to log data
	// Properties			: timed, non-last, non-critical: 100x xxxx (0x80)
	// Interval used		: 2				(xxx0 0010) - 0x02
	// Completed end doing	: progress (1) 	(1001) - 9
	// Error end doing		: nothing		(0000) - 0
	// Channel dependency	: nothing
	// CD fail end doing	: nothing		(0000) - 0
	// Argument				: nothing		(0000) - 0
	//--------------------------------------------------------------------------------
	str_obdh_data.us_interval_period[2] = 10; //The interval is the value (10) x 100 ms = 1s
	enterScriptline(SP_SCRIPTSPACE_LOG_ST_DATA,l_uc_running_no++,SCRIPT_CMD_LOG_DATA,0x82,0x90,0x00);

	//--------------------------------------------------------------------------------
	// To terminate log data task
	//--------------------------------------------------------------------------------
	// Scriptline 2			: to terminate log data task
	// Properties			: non-timed, last, non-critical: 010x xxxx (0x40)
	// Interval used		: NA			(xxx0 0000) - 0x00
	// Completed end doing	: nothing 		(0000) - 0
	// Error end doing		: nothing		(0000) - 0
	// Channel dependency	: nothing
	// CD fail end doing	: nothing		(0000) - 0
	// Argument				: nothing		(0000) - 0
	//--------------------------------------------------------------------------------
	enterScriptline(SP_SCRIPTSPACE_LOG_ST_DATA,l_uc_running_no++,SCRIPT_CMD_TERMINATE_LOG_DATA,0x40,0x00,0x00);

	//Pad the rests of the script to zero
	for (l_uc_i = l_uc_running_no; l_uc_i < MAX_SCRIPTLINE_SIZE; l_uc_i++) //Put zero for the rests
		enterScriptline(SP_SCRIPTSPACE_LOG_ST_DATA,l_uc_i,0x00,0x00,0x00,0x00);
//------------------------------------------------------------------------------------
}

//------------------------------------------------------------------------------------
// Test thermal knife script Initialization
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
void initTestTKScript (void)
{
	unsigned char l_uc_i;
	unsigned char l_uc_running_no = 0;

//------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	// Set DPLY channel to ON
	//--------------------------------------------------------------------------------
	// Scriptline 0			: to set a PWRS' channel state
	// Properties			: non-timed, non-last, critical: 001x xxxx (0x20)
	// Interval used		: NA			(xxx0 0000) - 0x00
	// Completed end doing	: progress (1) 	(1001) - 9
	// Error end doing		: nothing		(0000) - 0
	// Channel dependency	: nothing
	// Argument				: turn ON,		(1000) - 8
	//						  channel 3     (0011) - 3
	//--------------------------------------------------------------------------------
	enterScriptline(SP_SCRIPTSPACE_TEST_TK,l_uc_running_no++,SCRIPT_CMD_SET_SINGLE_PWRS_CHANNEL,0x20,0x90,0x83);

	//--------------------------------------------------------------------------------
	// Check DPLY channel's status, whether it has been turned ON
	//--------------------------------------------------------------------------------
	// Scriptline 1			: to check a PWRS' channel status
	// Properties			: timed, non-last, non-critical: 100x xxxx (0x80)
	// Interval used		: 7 			(xxx0 0111) - 0x07
	// Completed end doing	: progress (1) 	(1001) - 9
	// Error end doing		: regress (1)	(0001) - 1
	// Channel dependency	: nothing
	// Argument				: turn ON		(1000) - 8
	//						  channel 3 bit	(1000) - 8
	//--------------------------------------------------------------------------------
	str_obdh_data.us_interval_period[7] = 40; //The interval is the value (40) x 100 ms = 4s
	enterScriptline(SP_SCRIPTSPACE_TEST_TK,l_uc_running_no++,SCRIPT_CMD_CHECK_SINGLE_PWRS_CHANNEL,0x87,0x91,0x88);

	//--------------------------------------------------------------------------------
	// Set thermal knife's to ON
	//--------------------------------------------------------------------------------
	// Scriptline 2			: to set thermal knife's state
	// Properties			: non-timed, non-last, critical: 001x xxxx (0x20)
	// Interval used		: NA			(xxx0 0000) - 0x00
	// Completed end doing	: progress 1	(1001) - 9
	// Error end doing		: nothing		(0000) - 0
	// Channel dependency	: nothing
	// CD fail end doing	: nothing		(0000) - 0
	// Argument				: turn ON		(0001) - 1
	//--------------------------------------------------------------------------------
	enterScriptline(SP_SCRIPTSPACE_TEST_TK,l_uc_running_no++,SCRIPT_CMD_SET_TK_CHANNEL,0x20,0x90,0x01);

	//--------------------------------------------------------------------------------
	// Check thermal knife's enable flag, whether its channel has been turned ON
	//--------------------------------------------------------------------------------
	// Scriptline 3			: to check thermal knife's enable flag
	// Properties			: timed, non-last, non-critical: 100x xxxx (0x80)
	// Interval used		: 4 			(xxx0 0100) - 0x04
	// Completed end doing	: progress (1) 	(1001) - 9
	// Error end doing		: regress (1)	(0001) - 1
	// Channel dependency	: nothing
	// CD fail end doing	: nothing		(0000) - 0
	// Argument				: turn ON		(0001) - 1
	//--------------------------------------------------------------------------------
	str_obdh_data.us_interval_period[4] = 10; //The interval is the value (10) x 100 ms = 1s
	enterScriptline(SP_SCRIPTSPACE_TEST_TK,l_uc_running_no++,SCRIPT_CMD_CHECK_TK_CHANNEL,0x84,0x91,0x01);

	//--------------------------------------------------------------------------------
	// Check thermal knife's cutting flag, whether it has been cut
	//--------------------------------------------------------------------------------
	// Scriptline 4			: to check thermal knife's cutting flag
	// Properties			: timed, non-last, non-critical: 100x xxxx (0x80)
	// Interval used		: 5 			(xxx0 0101) - 0x05
	// Completed end doing	: progress (1) 	(1001) - 9
	// Error end doing		: progress (1)	(1001) - 9
	// Channel dependency	: nothing
	// CD fail end doing	: nothing		(0000) - 0
	// Argument				: Cut			(0002) - 2
	//--------------------------------------------------------------------------------
	str_obdh_data.us_interval_period[5] = 10; //The interval is the value (10) x 100 ms = 1s
	enterScriptline(SP_SCRIPTSPACE_TEST_TK,l_uc_running_no++,SCRIPT_CMD_CHECK_TK_CUT,0x85,0x99,0x02);

	//--------------------------------------------------------------------------------
	// Set thermal knife's to OFF
	//--------------------------------------------------------------------------------
	// Scriptline 5			: to set thermal knife's state
	// Properties			: non-timed, non-last, critical: 001x xxxx (0x20)
	// Interval used		: NA			(xxx0 0000) - 0x00
	// Completed end doing	: progress (1) 	(1001) - 9
	// Error end doing		: nothing		(0000) - 0
	// Channel dependency	: nothing
	// CD fail end doing	: nothing		(0000) - 0
	// Argument				: turn OFF		(0000) - 0
	//--------------------------------------------------------------------------------
	enterScriptline(SP_SCRIPTSPACE_TEST_TK,l_uc_running_no++,SCRIPT_CMD_SET_TK_CHANNEL,0x20,0x90,0x00);

	//--------------------------------------------------------------------------------
	// Set DPLY channel to OFF
	//--------------------------------------------------------------------------------
	// Scriptline 6			: to set a PWRS' channel state
	// Properties			: non-timed, non-last, critical: 001x xxxx (0x20)
	// Interval used		: NA			(xxx0 0000) - 0x00
	// Completed end doing	: progress (1) 	(1001) - 9
	// Error end doing		: nothing		(0000) - 0
	// Channel dependency	: nothing
	// Argument				: turn OFF,		(0000) - 0
	//						  channel 3     (0011) - 3
	//--------------------------------------------------------------------------------
	enterScriptline(SP_SCRIPTSPACE_TEST_TK,l_uc_running_no++,SCRIPT_CMD_SET_SINGLE_PWRS_CHANNEL,0x20,0x90,0x03);

	//--------------------------------------------------------------------------------
	// Check DPLY channel's status, whether it has been turned OFF
	//--------------------------------------------------------------------------------
	// Scriptline 7			: to check a PWRS' channel status
	// Properties			: timed, last, non-critical: 110x xxxx (0xC0)
	// Interval used		: 9 			(xxx0 1001) - 0x09
	// Completed end doing	: nothing 		(0000) - 0
	// Error end doing		: regress (1)	(0001) - 1
	// Channel dependency	: nothing
	// Argument				: turn OFF		(0000) - 0
	//						  channel 3 bit	(1000) - 8
	//--------------------------------------------------------------------------------
	str_obdh_data.us_interval_period[9] = 40; //The interval is the value (40) x 100 ms = 4s
	enterScriptline(SP_SCRIPTSPACE_TEST_TK,l_uc_running_no++,SCRIPT_CMD_CHECK_SINGLE_PWRS_CHANNEL,0xC9,0x01,0x08);

	//Pad the rests of the script to zero
	for (l_uc_i = l_uc_running_no; l_uc_i < MAX_SCRIPTLINE_SIZE; l_uc_i++) //Put zero for the rests
		enterScriptline(SP_SCRIPTSPACE_TEST_TK,l_uc_i,0x00,0x00,0x00,0x00);
//------------------------------------------------------------------------------------
}
#endif

//------------------------------------------------------------------------------------
// Scheduler Initialization
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
void initScheduler (void)
{
	unsigned char l_uc_i = 0;
	unsigned char l_uc_j = 0;

	//Delete whatever data in the schedule
	for (l_uc_i = 0; l_uc_i < SCHEDULER_BLOCK_SIZE; l_uc_i++){
		str_scheduler.uc_reindexing[l_uc_i]	 		= 0; //Is made consistent with scheduler's function
		str_scheduler.ul_scheduled_time[l_uc_i] 	= 0;
		str_scheduler.us_cmd[l_uc_i] 			 	= 0;
		str_scheduler.uc_arg_length[l_uc_i] 	 	= 0;
		for (l_uc_j = 0; l_uc_j < MAX_SCHEDULER_ARG; l_uc_j++)
			str_scheduler.uc_arg[l_uc_i][l_uc_j] 	= 0;
	}
	
	//Create initial schedules
	#if (SATELLITE_SCHEDULE_HANDLER_ENABLE == 1)
	#if (TEST_SCHEDULER_ENABLE == 1)
	scheduleSingleChannel (ADS_CHANNEL, CHANNEL_ON,  20);
	scheduleSingleChannel (ADS_CHANNEL, CHANNEL_OFF, 22);
	scheduleSingleChannel (ADS_CHANNEL, CHANNEL_ON,  24);
	scheduleSingleChannel (ADS_CHANNEL, CHANNEL_OFF, 26);
	scheduleSingleChannel (ADS_CHANNEL, CHANNEL_ON,  28);
	scheduleSingleChannel (ADS_CHANNEL, CHANNEL_OFF, 30);
	scheduleSingleChannel (ADS_CHANNEL, CHANNEL_ON,  32);
	scheduleSingleChannel (ADS_CHANNEL, CHANNEL_OFF, 34);
	scheduleSingleChannel (ADS_CHANNEL, CHANNEL_ON,  36);
	scheduleSingleChannel (ADS_CHANNEL, CHANNEL_OFF, 38);
	scheduleSingleChannel (ADS_CHANNEL, CHANNEL_ON,  40);
	scheduleSingleChannel (ADS_CHANNEL, CHANNEL_OFF, 42);
	scheduleSingleChannel (ADS_CHANNEL, CHANNEL_ON,  44);
	scheduleSingleChannel (ADS_CHANNEL, CHANNEL_OFF, 46);
	scheduleSingleChannel (ADS_CHANNEL, CHANNEL_ON,  48);
	scheduleSingleChannel (ADS_CHANNEL, CHANNEL_OFF, 50);
	#endif
	#endif
}

#if (CQUEUE_ENABLE == 1)
//------------------------------------------------------------------------------------
// OBDH Command Queue Initialization
//------------------------------------------------------------------------------------
// Function's test result: untested
//------------------------------------------------------------------------------------
void initOBDHCQueue (void)
{
	unsigned short l_us_i;
	unsigned short l_us_j;
	
	//Put all command queue element as zero (no command)
	for (l_us_i = 0; l_us_i < COMMAND_QUEUE_SIZE; l_us_i++)
		for (l_us_j = 0; l_us_j < COMMAND_LENGTH; l_us_j++)
			g_uc_command_queue[l_us_i][l_us_j] = 0;

	for (l_us_j = 0; l_us_j < COMMAND_LENGTH; l_us_j++)
		g_uc_command_entry[l_us_j] = 0;
}
#endif