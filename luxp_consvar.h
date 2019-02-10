#ifndef luxp_consvar_h
#define luxp_consvar_h

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

//To use longer groundpass time limit (default = 0, alternative = 1)
#define LONGER_GROUND_PASS		0

//UART used for debugging (default = 1, alternative = 0)
#define UART_LINE				1

//To enable debugging feature (released version = 0, debugging version = 1)
#define DEBUGGING_FEATURE		1

//To choose the method to generate key (simple method = 0, complex method = 1, default = 1)
#define SIMPLE_METHOD			0
#define COMPLEX_METHOD			1
#define KEY_GENERATION_METHOD	COMPLEX_METHOD

//To choose to do initialization (normal = 0, faster = 1, default = 0) 
#define INIT_TIME				0

//To choose initialize detumbling mode maximum uptime (normal = 0, faster = 1, default = 0) 
#define DET_UPTIME_MODE			0

//To choose to enable/disable save & load state feature (released version = 1, debugging version = 0)
#define SAVE_STATE_ENABLE		1
#define LOAD_STATE_ENABLE		1

//To choose to enable/disable event handler (released version = 1, debugging version = 0)
#define EVENT_HANDLER_ENABLE	1

//To choose to enable/disable various subsystems' commands (released version = 1, dummy version = 0)
#define PWRS_CMD_ENABLE			1
#define ADCS_CMD_ENABLE			1
#define ISIS_CMD_ENABLE			1
#define RTC_CMD_ENABLE			1
#define OBDH_CMD_ENABLE			1
#define OBDH_HDN_CMD_ENABLE		1
#define ANTENNA_CMD_ENABLE		1

//To choose to enable/disable various handlers (released version = 1, dummy version = 0)
#define SATELLITE_MODE_HANDLER_ENABLE			1
#define PERIPHERAL_ERROR_HANDLER_ENABLE			1
#define UPDATE_HANDLER_ENABLE					1
#define BEACON_HANDLER_ENABLE					1
#define GROUND_STATION_COMMAND_HANDLER_ENABLE	1
#define SATELLITE_SCHEDULE_HANDLER_ENABLE		1
#define BODY_RATE_HANDLER_ENABLE				1
#define IDLE_MODE_HANDLER_ENABLE				1
#define SAFE_MODE_HANDLER_ENABLE				1

//To choose to enable/disable storing and sending functions (released version = 1, dummy version = 0)
#define STORING_ENABLE			1
#define SENDING_ENABLE			1

//To choose to enable/disable various features (released version = 1, dummy version = 0)
//The dummy versions might sometimes be created for testing with other subsystems, that is
//to disable OBDH's brain that it will not interfere with other's subsystems testing.
//In the released version, all these constants must be set to 1
#define EXTRA_ENABLE			1
#define TASK_ENABLE				1
#define FULL_ADD_ENABLE			1
#define TIME_ENABLE				1
#define CRC_ENABLE				1
#define WDT_ENABLE				1

//To choose to enable/disable command queue feature (prevent multipe commands and CRC16 double-checking) (default = 0, alternative = 1)
#define CQUEUE_ENABLE			0

//To choose to enable/disable testing features (released version = 0, dummy version = 1)
#define TEST_SCHEDULER_ENABLE	0

//To choose system's clock speed used (released version = 2, alternatives = 1, 3)
#define LEVEL_3_SYSCLK	0x03
#define LEVEL_2_SYSCLK	0x02
#define LEVEL_1_SYSCLK	0x01
#define SYSCLK_USED		LEVEL_2_SYSCLK

#if (SYSCLK_USED == LEVEL_3_SYSCLK)

	//PLL constants
	#define INT_PLL_MUL				(4)	//Sets PLL multiplier N for internal oscillator
	#define INT_PLL_DIV				(1)	//Sets PLL divider M for internal oscillator
	#define EXT_PLL_MUL				(9)	//Sets PLL multiplier N for external oscillator
	#define EXT_PLL_DIV				(2)	//Sets PLL divider M for external oscillator

	//System Frequency
	#define INT_OSC_FREQUENCY		(24500000L)						//Internal oscillator frequency
	#define INT_PLL_DIV_CLK			(INT_OSC_FREQUENCY/INT_PLL_DIV)	//PLL divided clock input frequency for internal oscillator
	#define INT_PLL_OUT_CLK			(INT_PLL_DIV_CLK*INT_PLL_MUL)	//PLL output frequency for internal oscillator
	#define INT_SYSCLK				INT_PLL_OUT_CLK					//System clock derived from PLL (about 96MHz~100MHz, typical 98MHz) for internal oscillator
	#define EXT_OSC_FREQUENCY		(22118400L)						//External oscillator frequency
	#define EXT_PLL_DIV_CLK			(EXT_OSC_FREQUENCY/EXT_PLL_DIV)	//PLL divided clock input frequency for external oscillator
	#define EXT_PLL_OUT_CLK			(EXT_PLL_DIV_CLK*EXT_PLL_MUL)	//PLL output frequency for external oscillator
	#define EXT_SYSCLK				EXT_PLL_OUT_CLK					//System clock derived from PLL (about 98MHz~100MHz, typical 99MHz) for external oscillator

	//SPI clock rate
	#define SPICLK 					12000000L			//The maximum frequency is 12.25Mhz for current system clock. Speed is 1.53MB/s
	                                       
	//Transmission Frequencies
	#define SMB_FREQUENCY			400000L			//SMB desired frequency
	#define BAUDRATE				115200L			//Baud rate 
	
	//Other clock rate
	#define SAR_CLK      			2500000L		// Desired SAR clock speed

#elif (SYSCLK_USED == LEVEL_2_SYSCLK)

	//System Frequency
	#define INT_OSC_FREQUENCY		(24500000L)				//Internal oscillator frequency
	#define INT_SYSCLK				INT_OSC_FREQUENCY		
	#define EXT_OSC_FREQUENCY		(22118400L)				//External oscillator frequency
	#define EXT_SYSCLK				EXT_OSC_FREQUENCY		

	//SPI clock rate
	#define SPICLK 					3000000L		//The maximum frequency is about 3 Mhz for current system clock.
	                                       
	//Transmission Frequencies
	#define SMB_FREQUENCY			400000L			//SMB desired frequency
	#define BAUDRATE				115200L			//Baud rate 
	
	//Other clock rate
	#define SAR_CLK      			1250000L		// Desired SAR clock speed

#elif (SYSCLK_USED == LEVEL_1_SYSCLK)

	//System Frequency
	#define INT_OSC_FREQUENCY		(24500000L)				//Internal oscillator frequency
	#define INT_SYSCLK				(INT_OSC_FREQUENCY/8)

	//SPI clock rate
	#define SPICLK 					375000L		//The maximum frequency is about 375 Khz for current system clock.
	                                       
	//Transmission Frequencies
	#define SMB_FREQUENCY			125000L			//SMB desired frequency
	#define BAUDRATE				28800L			//Baud rate 
	
	//Other clock rate
	#define SAR_CLK      			156250L		// Desired SAR clock speed

#endif

//R/W bit
#define WRITE					0x00			//WRITE direction bit
#define READ					0x01			//READ direction bit

//Addresses
#define GOM_ADDR				0x04			//Power subsystem address for P-Sat (GOMSpace Address) -> 0000 0100
#define IMC_ADDR				0xA0			//COMM IMC subsystem address -> 1010 000|0 (0x50) or (0xA0)
#define ITC_ADDR				0xA2			//COMM ITC subsystem address -> 1010 001|0 (0x51) or (0xA2)
#define OBDH_ADDR				0x0A			//OBDH address -> 0000 1010
#define ADCS_ADDR				0x08			//ADCS address -> 0000 1000
#define PWRS_ADDR				0x0C			//PWRS address -> 0000 1100
#define PAYL_ADDR				0x0E			//PAYL address -> 0000 1110
#define RTC_ADDR				0xD0			//RTC M41781S address -> 1101 0000
#define GDSN_ADDR				0x03
#define TASK_ADDR				0x01
#define ANTENNA_1_ADDR			0x62			//ISIS Antenna microcontroller 1 address -> 0110 001|0 (0x31 or 0x62)
#define ANTENNA_2_ADDR			0x64			//ISIS Antenna microcontroller 2 address -> 0110 010|0 (0x32 or 0x64)

//Subsystem codes
#define OBDH_SUBSYSTEM			OBDH_ADDR
#define ADCS_SUBSYSTEM			ADCS_ADDR
#define PWRS_SUBSYSTEM			PWRS_ADDR
#define COMM_SUBSYSTEM			IMC_ADDR
#define PAYL_SUBSYSTEM			PAYL_ADDR
#define SD_CARD_BUFFER_CODE		0xB8
#define ALL_SUBSYSTEMS			0xEE

//SMB Interface Status
#define SMB_START				0x08			//(MT & MR) START transmitted
#define SMB_RP_START			0x10			//(MT & MR) repeated START
#define SMB_MTADDACK			0x18			//(MT) Slave address + W transmitted; ACK received
#define SMB_MTADDNACK			0x20			//(MT) Slave address + W transmitted; NACK received
#define SMB_MTDBACK				0x28			//(MT) data byte transmitted; ACK rec'vd
#define SMB_MTDBNACK			0x30			//(MT) data byte transmitted; NACK rec'vd
#define SMB_MTARBLOST			0x38			//(MT) arbitration lost
#define SMB_MRADDACK			0x40			//(MR) Slave address + R transmitted; ACK received
#define SMB_MRADDNACK			0x48			//(MR) Slave address + R transmitted; NACK received
#define SMB_MRDBACK				0x50			//(MR) data byte rec'vd; ACK transmitted
#define SMB_MRDBNACK			0x58			//(MR) data byte rec'vd; NACK transmitted
#define SMB_SROADACK			0x60			//(SR) SMB's own slave address + W rec'vd; ACK transmitted
#define SMB_SRGADACK			0x70			//(SR) general call address rec'vd; ACK transmitted
#define SMB_SRODBACK			0x80			//(SR) data byte received under own slave address; ACK returned
#define SMB_SRODBNACK			0x88			//(SR) data byte received under own slave address; NACK returned
#define SMB_SRGDBACK			0x90			//(SR) data byte received under general call address; ACK returned
#define SMB_SRGDBNACK			0x98			//(SR) data byte received under general call address; NACK returned
#define SMB_SRSTOP				0xA0			//(SR) STOP or repeated START received
#define SMB_STARBLOST			0xB0			//(ST) arbitration lost

//Define Message Type
#define EMPTY_MSG						0x00	//Indicate that there is no message sent

//Define ISIS IMC Message Type
#define IMC_I2C_RESET					0xAA 	//ISIS IMC reset
#define IMC_I2C_GET_NOFR				0x21 	//ISIS IMC get no of Frame in receive buffer
#define IMC_I2C_GET_FR					0x22 	//ISIS IMC get frame from receive buffer
#define IMC_I2C_RMV_FR					0x24 	//ISIS IMC remove frame from receive buffer
#define IMC_I2C_REC_DOFF				0x10 	//ISIS IMC get Doppler Offset
#define IMC_I2C_REC_SSTR				0x11 	//ISIS IMC measure receiver signal strength
#define IMC_I2C_TRN_RPOW				0x12 	//ISIS IMC measure transmitter reflected power
#define IMC_I2C_TRN_FPOW				0x13 	//ISIS IMC measure transmitter forward power
#define IMC_I2C_TRN_ICON				0x14 	//ISIS IMC measure transmitter current consumption
#define IMC_I2C_REC_ICON				0x15 	//ISIS IMC measure receiver current consumption
#define IMC_I2C_POW_AMPT				0x16 	//ISIS IMC measure power amplifier temperature
#define IMC_I2C_POW_BUSV				0x17 	//ISIS IMC measure power bus voltage
#define IMC_I2C_POW_BUSV_ICON			0x1F 	//ISIS IMC measure power bus voltage and current consumptions
#define IMC_I2C_REC_UPTM				0x40 	//ISIS IMC report receiver uptime

//Define ISIS ITC Message Type
#define ITC_I2C_RESET					0xAA 	//ISIS ITC Reset
#define ITC_I2C_SND_AX_FR_DFLT			0x10 	//ISIS ITC send AX.25 frame with default callsigns
#define ITC_I2C_SND_AX_FR_OVRD			0x11 	//ISIS ITC send AX.25 frame with override callsigns
#define ITC_I2C_SND_CW_MSG				0x12 	//ISIS ITC send CW message
#define ITC_I2C_SET_AX_BCN_DFLT			0x14 	//ISIS ITC set AX.25 beacon with default callsigns
#define ITC_I2C_SET_AX_BCN_OVRD			0x15 	//ISIS ITC set AX.25 beacon with override callsigns
#define ITC_I2C_SET_CW_BCN				0x16 	//ISIS ITC set CW beacon
#define ITC_I2C_CLR_BCN					0x1F 	//ISIS ITC clear beacon
#define ITC_I2C_SET_DFLT_AX_TO			0x22 	//ISIS ITC set default AX.25 to callsign
#define ITC_I2C_SET_DFLT_AX_FROM		0x23 	//ISIS ITC set default AX.25 from callsign
#define ITC_I2C_SET_TRN_IDLE			0x24 	//ISIS ITC set transmitter idle state
#define ITC_I2C_SET_TRN_OUTM			0x25 	//ISIS ITC set transmitter output mode
#define ITC_I2C_SET_AX_TRN_BRT			0x28 	//ISIS ITC set AX.25 transmission bitrate
#define ITC_I2C_SET_CW_CHAR_RT			0x29 	//ISIS ITC set CW character transmission rate
#define ITC_I2C_TRN_UPTM				0x40 	//ISIS ITC report transmitter uptime
#define ITC_I2C_TRN_STATE				0x41 	//ISIS ITC report transmitter state

//Number of transmitted bytes to ISIS IMC
#define IMC_I2C_WR						0x01	//There is always only one byte command from OBDH to IMC

//Number of response bytes received from ISIS IMC
#define IMC_I2C_GET_NOFR_RD				0x01	//Number of response bytes for GET_NOFR
#define IMC_I2C_GET_FR_RD				0x11	//Number of response bytes for GET_FR (17)
#define IMC_I2C_REC_DOFF_RD				0x02	//Number of response bytes for REC_DOFF
#define IMC_I2C_REC_SSTR_RD				0x02	//Number of response bytes for REC_SSTR
#define IMC_I2C_TRN_RPOW_RD				0x02	//Number of response bytes for TRN_RPOW
#define IMC_I2C_TRN_FPOW_RD				0x02	//Number of response bytes for TRN_FPOW
#define IMC_I2C_TRN_ICON_RD				0x02	//Number of response bytes for TRN_ICON
#define IMC_I2C_REC_ICON_RD				0x02	//Number of response bytes for REC_ICON
#define IMC_I2C_POW_AMPT_RD				0x02	//Number of response bytes for POW_AMPT
#define IMC_I2C_POW_BUSV_RD				0x02	//Number of response bytes for POW_BUSV
#define IMC_I2C_POW_BUSV_ICON_RD		0x06	//Number of response bytes for POW_BUSV_ICON
#define IMC_I2C_REC_UPTM_RD				0x03	//Number of response bytes for REP_REC_UPTIME

//Number of bytes transmitted to ISIS ITC
#define ITC_I2C_WR						0x01	//Number of transmitted bytes for one byte transfer
#define ITC_I2C_SET_DFLT_AX_TO_WR		0x08	//Number of transmitted bytes for SET_DFLT_AX_TO
#define ITC_I2C_SET_DFLT_AX_FROM_WR		0x08	//Number of transmitted bytes for SET_DFLT_AX_FROM
#define ITC_I2C_SET_TRN_IDLE_WR			0x02	//Number of transmitted bytes for SET_TRN_IDLE
#define ITC_I2C_SET_TRN_OUTM_WR			0x02	//Number of transmitted bytes for SET_TRN_OUTM
#define ITC_I2C_SET_AX_TRN_BRT_WR		0x02	//Number of transmitted bytes for SET_AX_TRN_BRT											   
#define ITC_I2C_SET_CW_CHAR_RT_WR		0x03	//Number of transmitted bytes for SET_CW_CHAR_RT

//Number of bytes transmitted to ISIS ITC
#define ITC_I2C_TRN_UPTM_RD				0x03	//Number of response bytes for RPT_TRN_UPTIME
#define ITC_I2C_TRN_STATE_RD			0x01	//Number of response bytes for RPT_TRN_STATE

//Define ADCS Message Type
#define ADCS_I2C_GET_HK					0x01
#define ADCS_I2C_GET_SSCP				0x02
#define ADCS_I2C_GET_DATA				0x03
#define ADCS_I2C_SET_MODE				0x04
#define ADCS_I2C_SET_SS_THRESHOLD		0x05
#define ADCS_I2C_SET_CGAIN				0x06
#define ADCS_I2C_SET_TLE				0x07
#define ADCS_I2C_SET_TIME				0x08
#define ADCS_I2C_SET_VECTOR_ALIGN2SUN	0x09
#define ADCS_I2C_SET_STANDBY			0x0A
#define ADCS_I2C_SET_SELFTEST			0x0B
#define ADCS_I2C_SET_TOGGLE_IMU			0x0C
#define ADCS_I2C_SET_GPS_ON				0x0D
#define ADCS_I2C_SET_GPS_OFF			0x0E
#define ADCS_I2C_RESET_SW				0x0F
#define ADCS_I2C_RESET_HW				0x10
#define ADCS_I2C_SET_BODY_RATE			0x11
#define ADCS_I2C_SET_DGAIN				0x12

//Number of bytes transmitted to/received from ADCS (for testing)
#define ADCS_I2C_GET_HK_RD					0x3B //59
#define ADCS_I2C_GET_SSCP_RD 				0x73 //115
#define ADCS_I2C_GET_DATA_WR				0x02			
#define ADCS_I2C_GET_DATA_RD				0x04
#define ADCS_I2C_SET_MODE_WR				0x04
#define ADCS_I2C_SET_SS_THRESHOLD_WR		0x03
#define ADCS_I2C_SET_CGAIN_WR				0x49 //73
#define ADCS_I2C_SET_TLE_WR					0x1B //27
#define ADCS_I2C_SET_TIME_WR				0x05				
#define ADCS_I2C_SET_VECTOR_ALIGN2SUN_WR	0x07
#define ADCS_I2C_SET_STANDBY_RD				0x01
#define ADCS_I2C_SET_SELFTEST_RD			0x01
#define ADCS_I2C_SET_TOGGLE_IMU_WR			0x02
#define ADCS_I2C_SET_BODY_RATE_WR			0x04
#define ADCS_I2C_SET_DGAIN_WR				0x03

//Define PWRS Message Type
#define PWRS_I2C_GET_HK					0x01
#define PWRS_I2C_GET_SSCP				0x02
#define PWRS_I2C_GET_DATA				0x03
#define PWRS_I2C_SET_HT					0x04
#define PWRS_I2C_SET_OUTPUT				0x05
#define PWRS_I2C_SET_SINGLE_OUTPUT		0x06
#define PWRS_I2C_SET_MPPT_MODE			0x07
#define PWRS_I2C_SET_PV_VOLT			0x08
#define PWRS_I2C_SET_TK_OUTPUT			0x09
#define PWRS_I2C_RESET_PWRS				0x0A
#define PWRS_I2C_RESET_OBDH				0x0B
#define PWRS_I2C_SET_SOC_LIMIT			0x0C
#define PWRS_I2C_SET_WDT_LIMIT			0x0D
#define PWRS_I2C_EEPROM_INIT			0x0E
#define PWRS_I2C_SET_PWRS_SDC			0x0F
//0x0CFx will be used by ADCS to communicate with power

//Number of bytes transmitted to/received from PWRS
#define PWRS_I2C_GET_HK_RD				0x38 //56
#define PWRS_I2C_GET_SSCP_RD			0x04
#define PWRS_I2C_GET_DATA_WR			0x02
#define PWRS_I2C_GET_DATA_RD			0x04
#define PWRS_I2C_SET_HT_WR				0x02
#define PWRS_I2C_SET_OUTPUT_WR			0x02
#define PWRS_I2C_SET_SINGLE_OUTPUT_WR	0x05
#define PWRS_I2C_SET_MPPT_MODE_WR		0x02
#define PWRS_I2C_SET_PV_VOLT_WR			0x07
#define PWRS_I2C_SET_TK_OUTPUT_WR		0x05
#define PWRS_I2C_RESET_PWRS_WR			0x03
#define PWRS_I2C_RESET_OBDH_WR			0x03
#define PWRS_I2C_SET_SOC_LIMIT_WR		0x03
#define PWRS_I2C_SET_WDT_LIMIT_WR		0x05
#define PWRS_I2C_EEPROM_INIT_WR			0x01
#define PWRS_I2C_SET_PWRS_SDC_WR		0x02
//Define GOMSpace Message Type

//Number of bytes transmitted to/received from GOMSpace

//Define RTC Message Type
#define RTC_I2C_GET_TIME				0xB1
#define RTC_I2C_DATE_INIT				0xB2
#define RTC_I2C_UPDATE_DATE				0xB3

//Number of bytes transmitted to/received from RTC
#define RTC_I2C_WR						0x15	//Number of bytes transmitted to RTC (21)
#define RTC_I2C_RD						0x14	//Number of bytes received from RTC (20)

//Define antenna message type
#define ANTENNA_I2C_RESET				0xAA	//To reset microcontroller (1010 1010)
#define ANTENNA_I2C_ARM					0xAD	//To arm the antenna system (1010 1101)
#define ANTENNA_I2C_DISARM				0xAC	//To disarm the antenna system (1010 1100)
#define ANTENNA_I2C_DPLY_1_DFLT			0xA1	//To activate deployment system for antenna 1 (1010 0001)
#define ANTENNA_I2C_DPLY_2_DFLT			0xA2	//To activate deployment system for antenna 2 (1010 0010)
#define ANTENNA_I2C_DPLY_3_DFLT			0xA3	//To activate deployment system for antenna 3 (1010 0011)
#define ANTENNA_I2C_DPLY_4_DFLT			0xA4	//To activate deployment system for antenna 4 (1010 0100)
#define ANTENNA_I2C_AUTO_DPLY			0xA5	//To activate sequential automatic deploymeny system (1010 0101)
#define ANTENNA_I2C_DPLY_1_OVRD			0xBA	//To activate deployment system for antenna 1 with overriding time value (10111010)
#define ANTENNA_I2C_DPLY_2_OVRD			0xBB	//To activate deployment system for antenna 2 with overriding time value (10111011)
#define ANTENNA_I2C_DPLY_3_OVRD			0xBC	//To activate deployment system for antenna 3 with overriding time value (10111100)
#define ANTENNA_I2C_DPLY_4_OVRD			0xBD	//To activate deployment system for antenna 4 with overriding time value (10111101)
#define ANTENNA_I2C_CANCEL_DPLY			0xA9	//To cancel deployment activation mechanism (1010 1001)
#define ANTENNA_I2C_GET_TEMP			0xC0	//To measure antenna system temperature (1100 0000)
#define ANTENNA_I2C_GET_DPLY_STAT		0xC3	//To get deployment status (1100 0011)
#define ANTENNA_I2C_GET_DPLY_1_COUNT	0xB0	//To get deployment count for antenna 1 (1011 0000)
#define ANTENNA_I2C_GET_DPLY_2_COUNT	0xB1	//To get deployment count for antenna 2 (1011 0001)
#define ANTENNA_I2C_GET_DPLY_3_COUNT	0xB2	//To get deployment count for antenna 3 (1011 0010)
#define ANTENNA_I2C_GET_DPLY_4_COUNT	0xB3	//To get deployment count for antenna 4 (1011 0011)
#define ANTENNA_I2C_GET_DPLY_1_TIME		0xB4	//To get deployment time for antenna 1 (1011 0100)
#define ANTENNA_I2C_GET_DPLY_2_TIME		0xB5	//To get deployment time for antenna 2 (1011 0101)
#define ANTENNA_I2C_GET_DPLY_3_TIME		0xB6	//To get deployment time for antenna 3 (1011 0110)
#define ANTENNA_I2C_GET_DPLY_4_TIME		0xB7	//To get deployment time for antenna 4 (1011 0111)

//Number of bytes transmitted to/received from antenna deployment board
#define ANTENNA_I2C_WR					0x01
#define ANTENNA_I2C_PAR_WR				0x02
#define ANTENNA_I2C_ONE_BYTE_RD			0x01
#define ANTENNA_I2C_TWO_BYTES_RD		0x02

//Define other constants
#define INITIAL_ANTENNA_STATUS			0x8888 	//four flags indicating the antenna is not deployed

//Define OBDH Command byte (can be called by GDSN)
#define	OBDH_CMD_REMOVE_SCHEDULE				0x01	//Used to remove unnecessary schedule
#define	OBDH_CMD_RESET_I2C_COMMUNICATION		0x02	//Used to reset I2C communication line
#define	OBDH_CMD_RESET_TASK						0x03	//Used to reset certain task
#define	OBDH_CMD_COMMAND_TASK					0x04	//Used to command certain task
#define	OBDH_CMD_INIT_SD						0x05	//Used to re-initialize SD card
#define	OBDH_CMD_SD_ERASE_SINGLE_BLOCK			0x06	//Used to erase single SD card block
#define OBDH_CMD_GET_HK							0x07	//Used to get HK data directly
#define OBDH_CMD_STORE_HK						0x08	//Used to store HK data directly
#define OBDH_CMD_ENTER_LEOP_IDLE				0x09	//Used to enter M3 state
#define OBDH_CMD_ENTER_LEOP_PWRS_SAFE_HOLD		0x0A	//Used to enter M4 state
#define OBDH_CMD_ENTER_NOP_IDLE					0x0B	//Used to enter M5 state
#define OBDH_CMD_ENTER_GROUNDPASS				0x0C	//Used to enter groundpass directly
#define OBDH_CMD_ENTER_MISSION_MODE				0x0D	//Used to enter mission mode
#define OBDH_CMD_ENTER_NOP_PWRS_SAFE_HOLD		0x0E	//Used to enter M8 state
#define OBDH_CMD_SET_BRH						0x0F	//Used to enable/disable bodyRateHandler
#define OBDH_CMD_SET_IMH						0x10	//Used to enable/disable idleModeHandler
#define OBDH_CMD_TURN_ON_ADCS					0x11	//Used to turn ON and OFF ADCS with the interval of 10 seconds
#define OBDH_CMD_TERMINATE_GROUNDPASS			0x12	//Used to terminate groundpass directly
#define OBDH_CMD_SW_RESET						0x13	//Used to do software reset on OBDH
#define OBDH_CMD_HW_RESET						0x14	//Used to do hardware reset on OBDH
#define OBDH_CMD_RESET_SAT						0x15	//Used to command OBDH to reset the satellite

//Define OBDH Command byte (cannot be called by GDSN - hidden commands)
#define	OBDH_HDN_CMD_WAIT_MS					0x80	//Used to hold the software (ms) (DF)
#define	OBDH_HDN_CMD_WAIT_S						0x81	//Used to hold the software (s) (DF)
#define OBDH_HDN_CMD_PING						0x82	
#define	OBDH_HDN_CMD_SEARCH_BP_BY_ORBIT_INFO	0x83	//Used to search block pointer based on orbit info (DF)
#define	OBDH_HDN_CMD_CHANGE_DATA				0x84
#define	OBDH_HDN_CMD_ENTER_SCRIPTLINE			0x85
#define	OBDH_HDN_CMD_SD_READ_SINGLE_BLOCK		0x86	//Used to read single SD card block (DF)
#define	OBDH_HDN_CMD_SD_WRITE_SINGLE_BLOCK		0x87	//Used to write single SD card block (DF)
#define	OBDH_HDN_CMD_SAVE_DATA_PROTECTED		0x88	//Used to save data (DF)
#define	OBDH_HDN_CMD_LOAD_DATA_PROTECTED		0x89	//Used to load data (DF)
#define OBDH_HDN_CMD_READ_DATA					0x8A	//Used to read certain data
#define OBDH_HDN_CMD_WRITE_INIT_STATE			0x8B	//Used to write the very first initial state to the original SD block (DF)
#define OBDH_HDN_CMD_COPY_INIT_STATE_TO_ESS		0x8C	//Used to copy SD card block from original initial state block to essential block
#define OBDH_HDN_CMD_CREATE_CRC16				0x8D	//Used to create CRC16 of a message (DF)
#define OBDH_HDN_CMD_SAVE_STATE					0x8E	
#define OBDH_HDN_CMD_LOAD_STATE					0x8F	
#define OBDH_HDN_CMD_GENESIS					0x90

//Ground station command (real)
#define GDSN_REQUEST_GDPASS			0x01
#define GDSN_SEND_GDPASS_CODE		0x02
#define GDSN_GET_ALL_HK				0x03
#define GDSN_GET_ALL_SSCP			0x04
#define GDSN_GET_DATA_SET			0x05
#define GDSN_GET_DATA				0x06
#define GDSN_SET_SSCP				0x07
#define GDSN_GET_WOD				0x08
#define GDSN_LOG_DATA				0x09
#define GDSN_GET_DATA_SPEC			0x0A
#define GDSN_GET_SAT_SCHEDULE		0x0B
#define GDSN_GET_SAT_TIME			0x0C
#define GDSN_SET_SAT_TIME			0x0D
#define GDSN_RESET_SAT				0x0E
#define GDSN_START_NOP				0x0F
#define GDSN_UPDATE_TLE				0x10
#define GDSN_UPDATE_CGAIN			0x11
#define GDSN_SCHEDULE_SUN_TRACKING	0x12
#define GDSN_UPLOAD_TCMD_A			0x13
#define GDSN_UPLOAD_TCMD_B			0x14
#define GDSN_OFF_TX					0x16
#define GDSN_ON_TX					0x17
#define GDSN_GET_SAT_REV			0x18
#define GDSN_SET_SAT_REV			0x19
#define GDSN_GET_SD_BLOCK			0x1A
#define GDSN_SCHEDULE_MISSION_MODE	0x1B
#define GDSN_REQUEST_LOST_FRAME		0x1C
#define GDSN_GET_WOE				0x1D //temp dec22_yc for reading saved events 

//Define ISIS ITC Idle State
#define IDLE_OFF 					0x00 //ISIS ITC transmitter will be turned off when idle (default)

//Define ISIS ITC Output Mode
#define NOMINAL_TELEMETRY			0x01 //ISIS ITC transmit the signal generated by the transceiver itself (AX.25 and CW generating functions of the transceiver) (default)

//Define ISIS ITC AX.25 Transmission Bitrate inputs
#define BITRATE_1200				0x01 //ISIS ITC will transmit the signal in 1200 bits per second (default)

//Define GOMSpace Solar Cell Power Tracking Mode

//Reset type
#define RESET_SOURCE_UNKNOWN		0x01
#define RESET_SOURCE_GSCH_WATCHDOG	0x02
#define RESET_SOURCE_MCC_COMMAND	0x04
#define RESET_SOURCE_SAFEHOLD		0x08
#define RESET_SOURCE_OBDH_COMMAND	0x10
#define RESET_SOURCE_OBDH_SW_RESET	0x20
#define RESET_SOURCE_OBDH_HW_RESET	0x40

//Define Power Output Switch Channel
#define ADS_CHANNEL					0x00
#define ACS_CHANNEL					0x01
#define PAYL_CHANNEL				0x02
#define COMM_CHANNEL				0x04
#define TK_TEST_CHANNEL				0x00

//Define Power Output Switch Channel
#define ADS_CHANNEL_BIT				0x01
#define ACS_CHANNEL_BIT				0x02
#define PAYL_CHANNEL_BIT			0x04
#define COMM_CHANNEL_BIT			0x10

//Thermal knife's register byte
#define TEST_TK_ENABLE_FLAG			0x01
#define TEST_TK_CUT_FLAG			0x02

//Antenna deployment status register bits

//Power channel status
#define CHANNEL_ON 					0x01
#define CHANNEL_OFF					0x00

//CW beacon default length, rate, and interval
#define CW_BEACON_LENGTH_M3 			19 	//"VELOXP" + "[][][][][][][][][][][][]"
#define CW_BEACON_LENGTH_M4_M5_M7 		19 	//"VELOXP" + "[][][][][][][][][][][][]"
#define CW_BEACON_LENGTH_M8 			10 	//"VELOXP" + "[SoC]"
#define CW_BEACON_LENGTH_COMM_ZONE 		19 	//"VELOXP" + "[][][][][][][][][][][][]"
#define CW_BEACON_RATE_DEFAULT			6	//6x2 = 12 dot(s)/s
#define CW_BEACON_INTERVAL_M3			60 	//60 seconds
#define CW_BEACON_INTERVAL_M4_M5_M7		120 //120 seconds
#define CW_BEACON_INTERVAL_M8			120 //120 seconds
#define CW_BEACON_INTERVAL_COMM_ZONE	50  //50 seconds
#define AX25_BEACON_INTERVAL_DEFAULT	10 	//10 seconds

//Constants for beacon flag
#define BEACON_FLAG_INIT				0x0001
#define BEACON_FLAG_ENABLE				0x0002
#define BEACON_FLAG_CHECK				0x0004
#define BEACON_FLAG_TIMEOUT				0x0008
#define BEACON_FLAG_MONITOR				0x0010
#define BEACON_FLAG_TYPE				0x0020
#define BEACON_FLAG_CW_MODE				0x00C0
#define BEACON_FLAG_CW_MODE_M3			0x0000
#define BEACON_FLAG_CW_MODE_M4_M5_M7	0x0040
#define BEACON_FLAG_CW_MODE_M8			0x0080
#define BEACON_FLAG_CW_MODE_COMM_ZONE	0x00C0
#define BEACON_FLAG_ERROR				0x0100
#define BEACON_FLAG_GSCH_CLAIM			0x0200
#define BEACON_FLAG_GSCH_SEND_REQUEST	0x0400
#define BEACON_FLAG_GSCH_SEND_ACK		0x0800
#define BEACON_FLAG_GSCH_SEND_COMPLETED	0x1000
#define BEACON_FLAG_GSCH_TYPE			0x2000
#define BEACON_FLAG_GSCH				0x3E00
#define BEACON_FLAG_RECOVERY			0x4000
#define BEACON_FLAG_AUTO_BEACON_DISABLE	0x8000

//Beacon flags set
#define BEACON_FLAG_INITIALIZED_CALC			(BEACON_FLAG_INIT+BEACON_FLAG_ENABLE+BEACON_FLAG_TYPE+BEACON_FLAG_CW_MODE_M3)
#define BEACON_FLAG_ERROR_SET_FILTER_CALC		(~BEACON_FLAG_AUTO_BEACON_DISABLE)	
#define BEACON_FLAG_CHECKING_FILTER_CALC		(BEACON_FLAG_ENABLE+BEACON_FLAG_CHECK+BEACON_FLAG_ERROR)
#define BEACON_FLAG_CHECKING_CONDITION_CALC		(BEACON_FLAG_CHECKING_FILTER-BEACON_FLAG_ERROR)
#define BEACON_FLAG_REQUEST_FILTER_CALC	 		(BEACON_FLAG_MONITOR+BEACON_FLAG_GSCH_CLAIM+BEACON_FLAG_GSCH_SEND_REQUEST)
#define BEACON_FLAG_REQUEST_CONDITION_CALC		(BEACON_FLAG_REQUEST_FILTER-BEACON_FLAG_MONITOR)
#define BEACON_FLAG_AUTO_BEACON_FILTER_CALC		(BEACON_FLAG_MONITOR+BEACON_FLAG_GSCH_CLAIM+BEACON_FLAG_TIMEOUT+BEACON_FLAG_AUTO_BEACON_DISABLE)
#define BEACON_FLAG_AUTO_BEACON_CONDITION_CALC	(BEACON_FLAG_AUTO_BEACON_FILTER-BEACON_FLAG_MONITOR-BEACON_FLAG_GSCH_CLAIM-BEACON_FLAG_AUTO_BEACON_DISABLE)
#define BEACON_FLAG_FILTER_PRIMARY_CALC			(BEACON_FLAG_INIT+BEACON_FLAG_ENABLE+BEACON_FLAG_ERROR+BEACON_FLAG_GSCH_CLAIM)
#define BEACON_FLAG_FILTER_SECONDARY_CALC		(BEACON_FLAG_MONITOR+BEACON_FLAG_GSCH_SEND_REQUEST+BEACON_FLAG_GSCH_SEND_ACK+BEACON_FLAG_GSCH_SEND_COMPLETED)
#define BEACON_FLAG_INITIALIZED					BEACON_FLAG_INITIALIZED_CALC		
#define BEACON_FLAG_ERROR_SET_FILTER			BEACON_FLAG_ERROR_SET_FILTER_CALC
#define BEACON_FLAG_CHECKING_FILTER				BEACON_FLAG_CHECKING_FILTER_CALC
#define BEACON_FLAG_CHECKING_CONDITION			BEACON_FLAG_CHECKING_CONDITION_CALC
#define BEACON_FLAG_REQUEST_FILTER				BEACON_FLAG_REQUEST_FILTER_CALC
#define BEACON_FLAG_REQUEST_CONDITION			BEACON_FLAG_REQUEST_CONDITION_CALC
#define BEACON_FLAG_AUTO_BEACON_FILTER			BEACON_FLAG_AUTO_BEACON_FILTER_CALC
#define BEACON_FLAG_AUTO_BEACON_CONDITION		BEACON_FLAG_AUTO_BEACON_CONDITION_CALC
#define	BEACON_FLAG_FILTER_PRIMARY				BEACON_FLAG_FILTER_PRIMARY_CALC
#define BEACON_FLAG_FILTER_SECONDARY			BEACON_FLAG_FILTER_SECONDARY_CALC

//Beacon check cases
#define BEACON_FUNCTIONAL_CALC					(BEACON_FLAG_FILTER_PRIMARY-BEACON_FLAG_ERROR)
#define BEACON_SEND_COMPLETED_CALC				(BEACON_FLAG_FILTER_SECONDARY-BEACON_FLAG_MONITOR-BEACON_FLAG_GSCH_SEND_ACK-BEACON_FLAG_GSCH_SEND_REQUEST)
#define BEACON_BUSY_1_CALC						(BEACON_FLAG_FILTER_SECONDARY-BEACON_FLAG_GSCH_SEND_COMPLETED-BEACON_FLAG_GSCH_SEND_ACK-BEACON_FLAG_GSCH_SEND_REQUEST)
#define BEACON_BUSY_2_CALC						(BEACON_FLAG_FILTER_SECONDARY-BEACON_FLAG_GSCH_SEND_ACK-BEACON_FLAG_GSCH_SEND_REQUEST)
#define BEACON_SENDING_CALC						(BEACON_FLAG_FILTER_SECONDARY-BEACON_FLAG_GSCH_SEND_COMPLETED-BEACON_FLAG_GSCH_SEND_REQUEST)
#define BEACON_WAITING_CALC						(BEACON_FLAG_FILTER_SECONDARY-BEACON_FLAG_MONITOR-BEACON_FLAG_GSCH_SEND_COMPLETED-BEACON_FLAG_GSCH_SEND_ACK)
#define BEACON_FREE_CALC						(BEACON_FLAG_FILTER_SECONDARY-BEACON_FLAG_MONITOR-BEACON_FLAG_GSCH_SEND_COMPLETED-BEACON_FLAG_GSCH_SEND_ACK-BEACON_FLAG_GSCH_SEND_REQUEST)
#define BEACON_FUNCTIONAL						BEACON_FUNCTIONAL_CALC
#define BEACON_SEND_COMPLETED					BEACON_SEND_COMPLETED_CALC
#define BEACON_BUSY_1							BEACON_BUSY_1_CALC
#define BEACON_BUSY_2							BEACON_BUSY_2_CALC
#define BEACON_SENDING							BEACON_SENDING_CALC
#define BEACON_WAITING							BEACON_WAITING_CALC
#define BEACON_FREE								BEACON_FREE_CALC				

//GSCH state
#define GSCH_STATE_IDLE						0x01
#define GSCH_STATE_GENERATING_KEY			0x02
#define GSCH_STATE_SEND_GENERATED_KEY		0x03
#define GSCH_STATE_WAITING_FOR_KEY_REPLY	0x04
#define GSCH_STATE_GROUND_PASS				0x05
#define GSCH_STATE_SEND_REAL_TIME_DATA		0x06
#define GSCH_STATE_SEND_STORED_DATA			0x07
#define GSCH_STATE_DEAD						0x08
#define GSCH_STATE_RECOVERY					0x09

//Acknowledgment character
#define GSCH_ACK_CHAR						0xAA

//Body rate handler flags
#define BRH_FLAG_ENABLE						0x0001
#define BRH_FLAG_READY						0x0002
#define BRH_FLAG_CHECK						0x0004
#define BRH_FLAG_DET_ON						0x0008
#define BRH_FLAG_DET_WT						0x0010
#define BRH_FLAG_DET_TO						0x0020
#define BRH_FLAG_DET_RS						0x0040
#define BRH_FLAG_BR_HI						0x0080
#define BRH_FLAG_BR_LOW						0x0100
#define BRH_FLAG_BR_ST_TO					0x0200
#define BRH_FLAG_ERROR						0x0400
#define BRH_FLAG_STABILIZING				0x0800
#define BRH_FLAG_ST_COMPLETED				0x1000

//Body rate handler filters and conditions
#define BRH_CHECK_FILTER_CALC				(BRH_FLAG_ENABLE+BRH_FLAG_READY+BRH_FLAG_CHECK)
#define BRH_CHECK_CONDITION_CALC			BRH_CHECK_FILTER_CALC
#define BRH_CHECK_FILTER					BRH_CHECK_FILTER_CALC
#define BRH_CHECK_CONDITION					BRH_CHECK_CONDITION_CALC

//Idle mode handler flags
#define IMH_FLAG_ENABLE						0x0001
#define IMH_FLAG_READY						0x0002
#define IMH_FLAG_CHECK						0x0004
#define IMH_FLAG_ADS_ON						0x0008
#define IMH_FLAG_ADS_WT						0x0010
#define IMH_FLAG_ADS_TO						0x0020
#define IMH_FLAG_ADS_RS						0x0040
#define IMH_FLAG_ERROR						0x0080

//Idle mode handler filters and conditions
#define IMH_CHECK_FILTER_CALC				(IMH_FLAG_ENABLE+IMH_FLAG_READY+IMH_FLAG_CHECK)
#define IMH_CHECK_CONDITION_CALC			IMH_CHECK_FILTER_CALC
#define IMH_CHECK_FILTER					IMH_CHECK_FILTER_CALC
#define IMH_CHECK_CONDITION					IMH_CHECK_CONDITION_CALC

//Satellite mode constant
#define SATELLITE_MODE_M2			0x02
#define SATELLITE_MODE_M3			0x03
#define SATELLITE_MODE_M4			0x04
#define SATELLITE_MODE_M5			0x05
#define SATELLITE_MODE_M6			0x06
#define SATELLITE_MODE_M7			0x07
#define SATELLITE_MODE_M8			0x08

//AX25 Property Header Flags
#define AX25_HEADER_PROPERTY_ON_GOING_TRANSMISSION_BIT			0x01
#define AX25_HEADER_PROPERTY_END_OF_TRANSMISSION_BIT			0x02
#define AX25_HEADER_PROPERTY_START_OF_STORED_DATA_BIT			0x04
#define AX25_HEADER_PROPERTY_END_OF_STORED_DATA_BIT				0x08
#define AX25_HEADER_PROPERTY_ON_GOING_STORED_DATA_BIT			0x10
#define AX25_HEADER_PROPERTY_PACKAGE_RECOVERY_DATA_BIT			0x20
#define AX25_HEADER_PROPERTY_STORED_DATA_BIT					0x80
#define AX25_HEADER_PROPERTY_SENDING_FILTER						(AX25_HEADER_PROPERTY_ON_GOING_TRANSMISSION_BIT+AX25_HEADER_PROPERTY_END_OF_TRANSMISSION_BIT+AX25_HEADER_PROPERTY_START_OF_STORED_DATA_BIT+AX25_HEADER_PROPERTY_END_OF_STORED_DATA_BIT)
#define AX25_HEADER_PROPERTY_START_END_STORED_FILTER			(AX25_HEADER_PROPERTY_START_OF_STORED_DATA_BIT+AX25_HEADER_PROPERTY_END_OF_STORED_DATA_BIT)
#define AX25_HEADER_PROPERTY_ON_GOING_END_TRANSMISSION_FILTER	(AX25_HEADER_PROPERTY_ON_GOING_TRANSMISSION_BIT+AX25_HEADER_PROPERTY_END_OF_TRANSMISSION_BIT)

//I2C Flags
#define	I2C_FLAG_RW									0x0001	//Software flag to indicate direction of the current transfer
#define	I2C_FLAG_DATA_RECEIVED						0x0002	//Set to '1' by the SMBus ISR when a data set has been fully received.
#define	I2C_FLAG_TRANSMISSION_START					0x0004	//Set to '1' when SMB is sending data to other subsystems
#define	I2C_FLAG_TRANSMISSION_TIMEOUT				0x0008	//Set to '1' by timer4 when it has been waiting too long to send SMB data
#define	I2C_FLAG_BUSY_START							0x0010	//Set to '1' when SMB busy is detected when a data transfer is initiated
#define	I2C_FLAG_BUSY_TIMEOUT						0x0020	//Set to '1' by timer4 when it has been waiting too long for SM Bus line to be free
#define	I2C_FLAG_RECEIVING_START					0x0040	//Set to '1' when SMB is waiting data from other subsystems
#define	I2C_FLAG_RECEIVING_TIMEOUT					0x0080	//Set to '1' by timer4 when it has been waiting too long to receive SMB data
#define I2C_FLAG_BUSY								0x0100	//Software flag to indicate SMBus is busy when SMB_Write of SMB_Read is performed
#define	I2C_FLAG_BUS_RECOVERY_ATTEMPT				0x0200	//Set to '1' if detecting failure in I2C lines for doing recovery attempt purpose
#define	I2C_FLAG_BUS_SCL_HELD_HIGH_RECOVERY_ATTEMPT	0x0400	//Set to '1' when starting recovery attempt for SCL line held high
#define	I2C_FLAG_BUS_SDA_HELD_LOW_RECOVERY_TIMEOUT	0x0800	//Set to '1' when timeout for recovering SDA line reaches its time limit
#define	I2C_FLAG_BUS_SCL_HELD_HIGH_RECOVERY_TIMEOUT	0x1000 	//Set to '1' when timeout for recovering SCL line reaches its time limit
#define	I2C_FLAG_OBDH_IN_SLAVE_MODE					0x2000	//Set to '1' whenever OBDH's I2C needs to be operating in slave mode
#define I2C_FLAG_STATUS_NON_FUNCTIONAL				0x4000	//Set to '1' whenever I2C bus is found to be non-functional

//SPI, timer0, AD0 flags
#define SPI_FLAG_BUSY_START						0x01	//Set to '1' whenever continual checking for SPI busy status is started
#define SPI_FLAG_BUSY_TIMEOUT					0x02	//Set to '1' when SPI is in busy status for too long
#define SPI_FLAG_TRANSMISSION_START				0x04	//Set to '1' whenever continual checking for SPI transmission status is started
#define SPI_FLAG_TRANSMISSION_TIMEOUT			0x08	//Set to '1' when SPI transmission has gone far too long
#define TIMER0_FLAG_WAIT						0x10	//Set to '1' when timer0 is waiting
#define TIMER0_FLAG_WAIT_TIMEOUT				0x20	//Set to '1' when timer0 is waiting for too long
#define AD0INT_FLAG_START						0x40	//Set to '1' when AD0 is waiting
#define AD0INT_FLAG_TIMEOUT						0x80	//Set to '1' when AD0 is waiting for too long

//Cmd, user input, software timer, and UART flags
#define CMD_FLAG_INVALID						0x01
#define CMD_FLAG_RESPONSE						0x02
#define USIP_FLAG_START							0x04
#define USIP_FLAG_READY							0x08
#define STMR_FLAG_COUNT_START					0x10
#define UART_FLAG_RX_BUSY						0x20	//Set to '1' if UART interface keep receiving bytes, this is also to initiate predicted receiving session
#define UART_FLAG_RX_BUSY_TIMEOUT				0x40	//Set to '1' if UART interface does not receive data after certain period

//Storing & sending flags
#define STRNG_FLAG_SD_LOAD_COMPLETED			0x01
#define STRNG_FLAG_SD_LOAD_OVERWRITTEN			0x02
#define STRNG_FLAG_SAVE_DATA_HOLD				0x04
#define SNDNG_FLAG_ON_GOING						0x08
#define SNDNG_FLAG_COMPLETED					0x10

//GCSH flags
#define GSCH_FLAG_GROUNDPASS_STARTED			0x01
#define GSCH_FLAG_TIME_WINDOW_VALID				0x02
#define GSCH_FLAG_MONITOR_TRANSITIONAL			0x04
#define GSCH_FLAG_TOTAL_SENDING					0x08
#define GSCH_FLAG_INITIAL_CHECK					0x10
#define GSCH_FLAG_VALID_COMMAND					0x20
#define GSCH_FLAG_CHECK_RESET					0x30

//Event & hold activation flags
#define EVENT_FLAG_SCHEDULE_IS_REARRANGED		0x01
#define EVENT_FLAG_SAT_INFO_TO_UPDATE			0x02
#define EVENT_FLAG_EXTOSC_FAIL					0x04
#define EVENT_FLAG_SCPTLN_FIRST_TIME_GET_TIMER	0x08
#define HACVTN_FLAG_SCM							0x10
#define HACVTN_FLAG_TM							0x20
#define HACVTN_FLAG_TS							0x40
#define EVENT_FLAG_RESET_SAT_INITIALIZED		0x80

//ADCS mode lists
#define ADCS_MODE_SUN_TRACK					0x02
#define ADCS_MODE_DETUMBLE					0x03
#define ADCS_MODE_RATE_CTRL					0x04
#define ADCS_MODE_IDLE						0x05

//PWRS mode lists
#define PWRS_BATT_CTRL_ON 					0x01
#define PWRS_BATT_CTRL_OFF					0x02

//Size, period and rate constants
#define UART_BUFFER_SIZE					600
#define CALL_TASK_BUFFER_SIZE				600
#define I2C_BUFFER_SIZE						256
#define OBDH_FIRMWARE_VERSION_LENGTH		18
#define	OBDH_DATA_INTERVAL_PERIOD_SIZE		16
#define TASK_SIZE							10
#define SCRIPTSPACE_SIZE					10
#define SEMI_PERMANENT_SCRIPTSPACE_SIZE		5
#define MAX_SCRIPTLINE_SIZE					8
#define MAX_SCRIPT_LENGTH					4
#define SCRIPTLINE_TIMER_SIZE				16
#define SCRIPTLINE_RETRY_COUNTER_SIZE		32
#define AX25_CALLSIGN_SIZE					14
#define AX25_BUFFER_SIZE					235
#define AX25_HEADER_SIZE					5
#define MAX_AX25_CONTENT_SIZE				(AX25_BUFFER_SIZE-AX25_HEADER_SIZE-2)
#define ITN_CHECK							2
#define ACK_MSG_SIZE						5
#define CW_BUFFER_SIZE						19
#define RECORD_BLOCK_SIZE					2
#define CRC_TABLE_SIZE						256
#define SD_CARD_BUFFER_SIZE					512
#define SD_CARD_HEADER_SIZE					32
#define MAX_SD_CARD_BLOCK_CONTENT_SIZE		(SD_CARD_BUFFER_SIZE-SD_CARD_HEADER_SIZE-2)
#define SCHEDULER_BLOCK_SIZE				70
#define COMMAND_HEADER_SIZE					4
#define SCHEDULER_NON_ARG_SIZE				7
#define MAX_SCHEDULER_ARG					10
#define MAX_CMD_ARG_LEN						10
#define MAX_GSCH_CMD_ARG_LEN				10
#define MAX_NO_OF_DATA_TYPE_TO_BE_SAVED		16
#define MAX_NO_OF_DATA_TYPE_TO_BE_SENT		16
#define MAX_DATA_CAT_TO_BE_SENT_FREELY		MAX_GSCH_CMD_ARG_LEN
#define COMMAND_QUEUE_SIZE					8
#define COMMAND_LENGTH						16
#define ISIS_TRN_ICON_LIMIT_WHEN_OFF		10
#define GSCH_COMM_FAIL_LIMIT				15
#define GSCH_LOADING_FAIL_LIMIT				5
#define BEACON_COMM_FAIL_LIMIT				10
#define NO_OF_BEACON_CHECK_PER_S			5	//Check beacon 5 times every second
#define WOD_SAMPLE_STEP_LOWER_LIMIT			1
#define WOD_SAMPLE_STEP_UPPER_LIMIT			20
#define MAX_NO_OF_DATA_TYPE_LOGGED			8
#define LOG_DATA_INIT_SET_SIZE				22
#define NO_OF_LOG_DATA_SET_PER_BLOCK		5
#define NO_OF_ADCS_CGAIN					18
#define LOG_DATA_SET_SIZE					35
#define BR_ECI_SAMPLE_NO					12
#define BRH_RETRY_FAIL_LIMIT				5
#define BRH_TIMEOUT_RETRY_FAIL_LIMIT		3
#define IMH_RETRY_FAIL_LIMIT				8
#define IMH_ADS_TIMEOUT_RETRY_FAIL_LIMIT	3
#define IMH_ACS_TIMEOUT_RETRY_FAIL_LIMIT	3
#define NO_OF_SOC_THRESHOLD_PAR				8
#define NO_OF_BEACON_INTERVAL				4
#define NO_OF_SECOND_PER_DAY				86400
#define SOFTWARE_TIMER_RATE_MS				100
#define SCRIPTLINE_MAX_NO_OF_RETRY			5
#define NO_OF_ANTENNA						4
#define NO_OF_SCHEDULE_PER_DATA_CATEGORY	10
#define INITIAL_BR_VALUE					1000
#define PWRS_V_CELL_MAX						4250 //Value given by PWRS' subsystem
#define PWRS_V_CELL_MIN						3000 //Value given by PWRS' subsystem
//dec22_yc
#define EVENTLOG_DATA_SET_SIZE				12
#define NO_OF_EVENTLOG_DATA_SET_PER_BLOCK	18
#define EVENTLOG_DATA_BUFFER_SIZE 			(EVENTLOG_DATA_SET_SIZE*NO_OF_EVENTLOG_DATA_SET_PER_BLOCK)//216

#if ((LOG_DATA_INIT_SET_SIZE+NO_OF_LOG_DATA_SET_PER_BLOCK*LOG_DATA_SET_SIZE) <= MAX_SD_CARD_BLOCK_CONTENT_SIZE)
#define LOG_DATA_BUFFER_SIZE 				(LOG_DATA_INIT_SET_SIZE+NO_OF_LOG_DATA_SET_PER_BLOCK*LOG_DATA_SET_SIZE)
#else 
#error "error: LOG_DATA_BUFFER_SIZE too big!"
#endif

//Thresholds
#define THRESHOLD_SOC_M3_TO_M4_DEFAULT			40 	//40 percent
#define THRESHOLD_SOC_M4_TO_M3_DEFAULT			100 //100 percent
#define THRESHOLD_SOC_M5_TO_M8_DEFAULT			60 	//60 percent
#define THRESHOLD_SOC_M7_TO_M8_DEFAULT			40 	//40 percent
#define THRESHOLD_SOC_M8_TO_M5_DEFAULT			100 //100 percent
#define THRESHOLD_SOC_TURN_OFF_COMM_DEFAULT		20 	//20 percent
#define THRESHOLD_SOC_RESET_SAT_DEFAULT			10 	//10 percent
#define THRESHOLD_SOC_LISTEN_MCC_DEFAULT		0 	//0 percent (use up everything for now)
#define THRESHOLD_BR_X_LOW_DEFAULT				40	//2 deg/s x 20 = 40
#define THRESHOLD_BR_Y_LOW_DEFAULT				40	//2 deg/s x 20 = 40
#define THRESHOLD_BR_Z_LOW_DEFAULT				40	//2 deg/s x 20 = 40
#define THRESHOLD_BR_X_HIGH_DEFAULT				160	//8 deg/s x 20 = 160
#define THRESHOLD_BR_Y_HIGH_DEFAULT				160	//8 deg/s x 20 = 160
#define THRESHOLD_BR_Z_HIGH_DEFAULT				160	//8 deg/s x 20 = 160
#define THRESHOLD_GSCH_TIME_WINDOW_DEFAULT_S	120	//Considers valid command within 120 second(s) (default)

//Dating constants
#define SECONDS_FROM_TAI_TO_1_JAN_2011					1672531234L		
#define SECONDS_FROM_TAI_TO_KILL_SWITCH_DEACTIVATION	1760426782L	//14-Oct-2013 07:25:48 + 34 Leap seconds (not 35)

//Period constants
#define ADCS_INFO_UPDATE_PERIOD			60
#define PWRS_INFO_UPDATE_PERIOD			60
#define COMM_INFO_UPDATE_PERIOD			60
#define PWRS_CH_STAT_UPDATE_PERIOD		4
#define GET_RTC_TIME_UPDATE_PERIOD		180 //Get time from RTC every 3 minutes
#define ANTENNA_STATUS_CHECK_PERIOD		240 //Check antenna deployment status every 4 minutes
#define OBDH_SAVE_STATE_PERIOD			300
#define BRH_CHECK_PERIOD				3	//Check BRH every 3 seconds
#define IMH_CHECK_PERIOD				3	//Check IMH every 3 seconds

//Timeout constants
#define TIME_LIMIT_UART_BUSY_OFL					4	//4 times timer 4 overflowing (about 15~20ms), //Max 255 times (about 1.2s)
#define TIME_LIMIT_TIMER0_STUCK_OFL					2	//2 times timer 4 overflowing (about 5-10ms)
#define TIME_LIMIT_A2D_CONVERSION					2	//2 times timer 4 overflowing (about 5-10ms)
#define TIME_LIMIT_I2C_TRANSMISSION_OFL				40	//40 times timer 4 overflowing (about 200ms)
#define TIME_LIMIT_I2C_BUSY_OFL						20	//20 times timer 4 overflowing (about 100ms)
#define TIME_LIMIT_I2C_RECEIVING_OFL				40	//40 times timer 4 overflowing (about 200ms)
#define TIME_LIMIT_NON_GROUND_PASS_COMMAND_S		180	//3 minutes without groundpass after the first command will terminate the groundpass
#if (LONGER_GROUND_PASS == 1)
#define TIME_LIMIT_GROUND_PASS_S					54000 //900 minutes (15 hours) is the limit for GSCH in groundpass mode, receiving no MCC command
#define TIME_RESET_GROUND_PASS_S					720
#else
#define TIME_LIMIT_GROUND_PASS_S					900 //15 minutes is the limit for GSCH in groundpass mode, receiving no MCC command. But as soon as the first command is received, only 3 minutes are allowed
#define TIME_RESET_GROUND_PASS_S					(TIME_LIMIT_GROUND_PASS_S-TIME_LIMIT_NON_GROUND_PASS_COMMAND_S)
#endif
#define TIME_LIMIT_GSCH_DEAD_STATE_S				10	//10 seconds to retry to enter recovery state
#define TIME_LIMIT_GSCH_RECOVERY_STATE_S			3	//3 seconds to retry to enter dead state back
#define TIME_LIMIT_SAVE_DATA_HOLD_S					30 	//30 seconds maximum to hold saving data
#define TIME_LIMIT_LOAD_DATA_S						30 	//30 seconds
#define TIME_LIMIT_SCL_FORCED_LOW_MS				35	//35 x 1 ms = 35ms (must be held at least 25ms)
#define TIME_LIMIT_SCL_FORCED_HIGH_TENTH_MS			5	//5 x 1/10 ms = 0.5ms
#define TIME_LIMIT_I2C_BUS_RECOVERY_OFL				15	//15 times timer 4 overflowing (70~75 ms)
#define TIME_LIMIT_I2C_BUS_SCL_HIGH_RECOVERY_OFL	2	//2 times timer 4 overflowing (5~10 ms)
#define TIME_LIMIT_SPI_BUS_BUSY_OFL					2	//2 times timer 4 overflowing (5~10 ms)
#define TIME_LIMIT_SPI_BUS_TRANSMISSION_OFL			2	//2 times timer 4 overflowing (5~10 ms)
#define TIME_LIMIT_GSCH_TRANSITIONAL_STATE_S		120	//Maximum 120s in the transitional state
#define TIME_LIMIT_BEACON_ERROR_S					2	//Retry to re-initialize beacon after two seconds of error
#define TIME_LIMIT_LOG_DATA_S						1200	//Maximum data log for 1200 s
#define TIME_LIMIT_WAITING_DET_S					15	//Time limit to wait ADCS detumbling mode to be turned to desired state by BRH or IMH
#define TIME_LIMIT_WAITING_ADS_S					15	//Time limit to wait ACS to be turned to desired state by IMH
#if (INIT_TIME == 0)
#define TIME_LIMIT_DEPLOY_ANTENNA_START_S			2700	//45 minutes, then deploy antenna
#define TIME_LIMIT_TO_STAY_IN_M2					3600	//Put one hour limit for now
#define TIME_LIMIT_ENABLE_BRH_IN_M2					900		//The first 15 min(s), no BRH is enabled 
#elif (INIT_TIME == 1) 
#define TIME_LIMIT_DEPLOY_ANTENNA_START_S			120		//2 mins, then deploy antenna
#define TIME_LIMIT_TO_STAY_IN_M2					180		//Put 3 mins limit for now
#define TIME_LIMIT_ENABLE_BRH_IN_M2					60		//The first 1 min(s), no BRH is enabled 
#else
#error "error: invalid INIT_TIME value!"
#endif
#if (DET_UPTIME_MODE == 0)
#define TIME_LIMIT_DET_UPTIME_INITIAL_S				12000	//Gives ACS two orbits uptime limit
#elif (DET_UPTIME_MODE == 1)
#define TIME_LIMIT_DET_UPTIME_INITIAL_S				60		//Gives ACS 1 minute uptime limit
#else
#error "error: invalid DET_UPTIME_MODE value!"
#endif
#define TIME_LIMIT_NO_COMM_WITH_MCC_S				216000	//Maximum 2.5 days of no communication with MCC
#define TIME_LIMIT_SAT_ON_AFTER_LAST_EFFORT_RESET	600 //Let the satellite to turn ON for at least 10 mins
#define TIME_LIMIT_TK_WAITING_S						3	//3 second(s) maximum waiting time for thermal knife being to the desired state
#define TIME_LIMIT_TK_CUTTING_S						30	//30 second(s) maximum waiting time for thermal knife being cut
#define TIME_LIMIT_DPLY_WAIITING_S					10	//10 second(s) maximum waiting time for deployment channel being in the desired state
#define TIME_LIMIT_ANTENNA_WAIITING_S				30	//30 second(s) safety limit for antenna deployment (as suggested)
#define TIME_LIMIT_BODY_RATE_UNCHECKED_S			86400 //Re-check body rate every day
#define TIME_LIMIT_SET_RESET_CHANNEL_MS				500	//500ms to set/reset the channel

//Time delay constants
#define TIME_DELAY_UART_ONE_CHAR_TRANSMISSION_US	100	//Experimentally, under 98Mhz, 25 us is the delay limit for successful UART transmission, this figure is 4 times the limit
#define TIME_DELAY_SPI_ONE_CHAR_TRANSMISSION_US		50	//Subjected to experimental result!!
#define TIME_DELAY_ISIS_I2C_CMD_WAITING_TENTH_MS	10
#define TIME_DELAY_ADCS_I2C_CMD_WAITING_TENTH_MS	20
#define TIME_DELAY_PWRS_I2C_CMD_WAITING_TENTH_MS	20
#define TIME_DELAY_ANTENNA_I2C_CMD_WAITING_TENTH_MS	10
#define TIME_DELAY_ISIS_I2C_CMD_DELAY_TENTH_MS		20
#define TIME_DELAY_ADCS_I2C_CMD_DELAY_TENTH_MS		1
#define TIME_DELAY_PWRS_I2C_CMD_DELAY_TENTH_MS		1
#define TIME_DELAY_ANTENNA_I2C_CMD_DELAY_TENTH_MS	20
#define TIME_DELAY_ACS_TO_ON_AFTER_ADS_ON_S			10
												
//Satellite Initial Date
#define SATELLITE_INIT_MS				0
#define SATELLITE_INIT_S				0x48
#define SATELLITE_INIT_MIN				0x25		
#define SATELLITE_INIT_HOUR				0x07	//07:25:48
#define SATELLITE_INIT_DAY_OF_WEEK		0x02	//Monday
#define SATELLITE_INIT_DAY_OF_MONTH		0x14	//14th
#define SATELLITE_INIT_MONTH			0x10	//Oct
#define SATELLITE_INIT_YEAR				0x13	//2013

//Error Codes
#define EC_SUCCESSFUL										0x0000
#define EC_INIT												0x0001
#define EC_INVALID_INPUT									0x0002
#define EC_I2C_WRITE_INCOMPLETE_TRANSMISSION				0x0003
#define EC_I2C_READ_INCOMPLETE_TRANSMISSION					0x0004
#define EC_I2C_BUS_BUSY										0x0006
#define EC_I2C_ISR_SLAVE_ADDR_NACK							0x0007
#define EC_I2C_ISR_COMMUNICATION_FAIL						0x0008
#define EC_SATELLITE_TIME_MILISECOND_ERROR					0x0009
#define EC_SATELLITE_TIME_SECOND_ERROR						0x000A
#define EC_SATELLITE_TIME_MINUTE_ERROR						0x000B
#define EC_SATELLITE_TIME_HOUR_ERROR						0x000C
#define EC_SATELLITE_TIME_DAY_ERROR							0x000D
#define EC_ADCS_NOT_READY									0x000E
#define EC_CRC_BYTES_DO_NOT_MATCH							0x000F
#define EC_I2C_ISR_ADCS_SLAVE_ADDR_NACK						0x00A0
#define EC_I2C_ISR_PWRS_SLAVE_ADDR_NACK						0x00A1
#define EC_I2C_ISR_IMC_SLAVE_ADDR_NACK						0x00A2
#define EC_I2C_ISR_ITC_SLAVE_ADDR_NACK						0x00A3
#define EC_I2C_ISR_PAYL_SLAVE_ADDR_NACK						0x00A4
#define EC_I2C_ISR_RTC_SLAVE_ADDR_NACK						0x00A5
#define EC_I2C_ISR_GOM_PWRS_SLAVE_ADDR_NACK					0x00A6
#define EC_GET_COMM_HK_FAIL									0x00A7
#define EC_RTC_GET_TIME_FAIL								0x00A8
#define EC_I2C_ISR_ANTENNA_1_SLAVE_ADDR_NACK				0x00A9
#define EC_I2C_ISR_ANTENNA_2_SLAVE_ADDR_NACK				0x00AA
#define EC_SD_CARD_IRRESPONSIVE								0x0011
#define EC_SD_ALL_STORED_DATA_IS_CORRUPTED 					0x0012
#define EC_SD_CARD_INITIALIZATION_ERROR						0x0014
#define EC_SD_CARD_ERASE_BLOCK_ERROR						0x0015
#define EC_SD_CARD_WRITE_BLOCK_ERROR						0x0016
#define EC_SD_CARD_READ_BLOCK_ERROR							0x0017
#define EC_SD_CARD_BLOCK_DOES_NOT_EXIST						0x0018
#define EC_SD_CARD_BLOCK_UNACCESSIBLE_TO_BE_ERASED			0x0019
#define EC_COMMAND_ALREADY_EXIST							0x0021
#define EC_COMMAND_QUEUE_FULL								0x0022
#define EC_INVALID_COMMAND_LENGTH							0x0023
#define EC_INVALID_SENDING_DATA_REQUEST						0x0031
#define EC_SENDING_PROCESS_IS_ON_GOING						0x0033
#define EC_INVALID_SAVING_DATA_REQUEST						0x0041
#define EC_SAVING_DATA_IS_DISABLED							0x0042
#define EC_INVALID_LOADING_DATA_REQUEST						0x0043
#define EC_INVALID_READING_DATA_REQUEST						0x0044
#define EC_UNEXPECTED_BLOCK_PROPERTY						0x0045
#define EC_LOADING_INTERRUPTED_WITH_ERROR					0x0046
#define EC_INVALID_SEARCHING_INPUT							0x0047
#define EC_RECORD_FOR_REQUESTED_ORBIT_DOES_NOT_EXIST		0x0048
#define EC_NO_DATA_STORED_IN_THIS_BLOCK						0x0049
#define EC_COMMAND_NOT_FOUND								0x0051
#define EC_IMC_COMMAND_NOT_FOUND							0x0052
#define EC_ITC_COMMAND_NOT_FOUND							0x0053
#define EC_ADCS_COMMAND_NOT_FOUND							0x0054
#define EC_PWRS_COMMAND_NOT_FOUND							0x0055
#define EC_RTC_COMMAND_NOT_FOUND							0x0056
#define EC_OBDH_COMMAND_NOT_FOUND							0x0057
#define EC_ANTENNA_COMMAND_NOT_FOUND						0x0058
#define EC_SCHEDULE_STOP_UPLOADING_COMMAND					0x0061
#define EC_SCHEDULE_FULL									0x0062
#define EC_SCHEDULE_ARGUMENT_TOO_LONG						0x0063
#define EC_SCHEDULED_TIME_HAS_ALREADY_PASSED				0x0064
#define EC_NOT_ENOUGH_SCHEDULE_SLOT							0x0065
#define EC_INVALID_GDSN_GET_DATA_REQUEST					0x0071
#define EC_INVALID_GDSN_SET_SSCP_REQUEST					0x0072
#define EC_INVALID_GDSN_SET_ADCS_SSCP_REQUEST				0x0074
#define EC_INVALID_GDSN_SET_PWRS_SSCP_REQUEST				0x0075
#define EC_INVALID_GDSN_COMMAND								0x0076
#define EC_INVALID_GDSN_COMMAND_IDLE_STATE					0x0077
#define EC_INVALID_GDSN_COMMAND_ARG_LENGTH					0x0078
#define EC_INVALID_GDSN_COMMAND_CRC_BYTES_DO_NOT_MATCH		0x0079
#define EC_INVALID_GDSN_COMMAND_WAITING_FOR_KEY_REPLY_STATE	0x007A
#define EC_INVALID_GDSN_COMMAND_GROUND_PASS_STATE			0x007B
#define EC_INVALID_GDSN_COMMAND_ON_LEOP						0x007C
#define EC_INVALID_GDSN_LOST_FRAME_INDEX					0x007D
#define EC_INVALID_GDSN_RESET_SAT_ALREADY_INITIALIZED		0x007E
#define EC_ALL_SCRIPTLINE_TIMER_IS_USED						0x0081
#define EC_FAIL_TO_READ_SCRIPTLINE_TIMER					0x0082
#define EC_INVALID_SCRIPTLINE_TIMER_NO						0x0083
#define EC_INVALID_TASK_NO									0x0084
#define EC_ALL_RETRY_COUNTER_IS_USED						0x0085
#define EC_FAIL_TO_READ_SCRIPTLINE_RETRY_COUNTER			0x0086
#define EC_INVALID_SCRIPTLINE_RETRY_COUNTER_NO				0x0087
#define EC_INVALID_SCRIPTSPACE_NO							0x0088
#define EC_SUBSYSTEM_TO_LOG_DATA_UNDEFINED					0x0091
#define EC_NO_DATA_LOGGED_YET								0x0092
#define EC_BEACON_FLAG_ERROR								0x00B1	//dec22_yc
#define EC_ADCS_CMD_DISABLED								0x00C0
#define EC_PWRS_CMD_DISABLED								0x00C1
#define EC_ISIS_CMD_DISABLED								0x00C2
#define EC_RTC_CMD_DISABLED									0x00C5
#define EC_STORING_FUNCTIONS_ARE_DISABLED					0x00C8
#define EC_COMM_IS_OFF										0x00C9
#define EC_SATELLITE_SCHEDULE_IS_DISABLED					0x00D1

//EVent sources dec_yc... will need to reorganize here somtime in future
//changed dec22_yc
#define EV_E_FUNCTION_MODULE								0x0100// {TAI,TAI,TAI,TAI,EV,EC,l_uc_command_header_byte,l_uc_command_tail_byte,l_us_arg[0], l_us_arg[1]}
#define EV_SCHEDULER_TO_COMMAND								0x0200// {TAI,TAI,TAI,TAI,EV,EC,l_uc_command_header_byte,l_uc_command_tail_byte,l_us_arg[0], l_us_arg[1]}
#define EV_TASK_MODULE										0x0300// {TAI,TAI,TAI,TAI,EV,l_uc_function_code,l_us_arg[]}
#define EV_SET_TASK_FLAG									0x0400// {TAI,TAI,TAI,TAI,EV,EC,l_uc_task_chosen,l_uc_task_cmd,str_task[l_uc_task_chosen].uc_status,str_task[l_uc_task_chosen].uc_script_error_flags,
																	//str_task[l_uc_task_chosen].uc_script_running_flags,str_task[l_uc_task_chosen].uc_script_completed_flags}

//yc added 20120224
#define EV_ADD_SCHEDULE										0x0500// {TAI,TAI,TAI,TAI,EV,EC,l_uc_command_header_byte,l_uc_command_tail_byte,l_us_arg[0], l_us_arg[1]}
#define EV_SATELLITE_MODE_HANDLER							0x1000// {TAI,TAI,TAI,TAI,EV,EC,str_obdh_data.uc_sat_mode(old),str_obdh_data.uc_sat_mode,str_obdh_hk.us_brh_flag, str_obdh_hk.us_imh_flag}
#define EV_SMH_HANDLER										0x2300// {TAI,TAI,TAI,TAI,EV,EC,str_obdh_hk.uc_smh_flag,str_obdh_data.uc_batt_soc,0,0,0,0}

//yc deleted 20120224 #define EV_SATELLITE_CONTROL_MODULE	0x1000
#define EV_PERIPHERAL_HANDLER								0x1100// {TAI,TAI,TAI,TAI,EV,str_obdh_hk.uc_i2c_bus_status,g_b_i2c_bus_recovery_attempt,0,0,0,0,0}
#define EV_RTC_TIME_UPDATE_HANDLER							0x1200// {TAI,TAI,TAI,TAI,EV,EC,str_obdh_hk.ul_rtc_ref_time_s*4,0,0}
#define EV_E_ADCS_INFO_UPDATE_HANDLER						0x1300// {TAI,TAI,TAI,TAI,EV,EC,pad0}//changed dec21_yc
#define EV_E_COMM_INFO_UPDATE_HANDLER						0x1500// {TAI,TAI,TAI,TAI,EV,EC,pad0}//changed dec21_yc	
#define EV_ORBIT_UPDATE_HANDLER								0x1600// {TAI,TAI,TAI,TAI,EV,EV,str_sat_state.us_auto_reference_rev_no*2,str_obdh_par.us_current_rev_no*2}

//dec14_yc changes summary here...
#define EV_BEACON_HANDLER									0x1700// {TAI,TAI,TAI,TAI,EV,EC,g_us_beacon_flag(2),g_us_beacon_monitoring_time*2,g_uc_beacon_retry_counter,(char)overlyLongTransmissionRecovery()} //track status before error.
																	  // {TAI,TAI,TAI,TAI,EV,EC,g_us_beacon_flag(2),g_us_beacon_monitoring_time*2,g_uc_beacon_retry_counter,g_uc_ax25_header_package_property} //track sendData() at every beacon send AX25 frame.
																	  // {TAI,TAI,TAI,TAI,EV,EC,g_us_beacon_flag(2),g_us_beacon_monitoring_time*2,g_uc_beacon_retry_counter,ITC_I2C_SND_CW_MSG} //track CW_Msg at send.
#define EV_GSCH_MONITORING_TRANSITIONAL_FLAG_HANDLER		0x1800// {TAI,TAI,TAI,TAI,EV,EC,str_sat_state.uc_gsch_state(prior),g_uc_gsch_gs_cmd_tail,l_us_data_id=g_uc_gsch_gs_arg[0,1](2bytes),g_us_gsch_gs_cmd_crc(2bytes)}
																	   // track valid ground station cmd and processing status.Called at everytime g_uc_gsch_monitoring flag transitions to 0xFF.
#define EV_EVENT_HANDLER									0x1900// {TAI,TAI,TAI,TAI,EV,g_uc_eventlog_status_flag,pad0}
//dec_28_yc
#define EV_SAVE_STATE										0x1B00// {TAI,TAI,TAI,TAI,EV,EC,0s}
#define EV_LOAD_STATE										0x1C00// {TAI,TAI,TAI,TAI,EV,EC,0s}
#define EV_INIT_COMPLETED									0x1D00// {TAI,TAI,TAI,TAI,EV,EC,g_uc_evha_flag,0s}
#define EV_MCC_TO_COMMAND									0x1E00// {TAI,TAI,TAI,TAI,EV,EC,l_uc_command_header_byte,l_uc_command_tail_byte,l_uc_arg[0], l_uc_arg[1],l_uc_arg[2], l_uc_arg[3]}
#define EV_SCRIPTLINE_END_FLAG								0x1F00//{TAI,TAI,TAI,TAI,EV,EC,l_uc_task,l_uc_end_type,
			//str_task[l_uc_task_chosen].uc_status,str_task[l_uc_task_chosen].uc_script_error_flags,
			//	str_task[l_uc_task_chosen].uc_script_running_flags,str_task[l_uc_task_chosen].uc_script_completed_flags}

//g_uc_eventlog_status_flag
#define EVENTLOG_STATUS_MISSING_ENTRY	0x01
#define EVENTLOG_STATUS_REQUIRE_SAVING	0x02
#define EVENTLOG_STATUS_FULL			0x80

//No of trials
#define I2C_RESTART_TRIAL_LIMIT			0x0A	//10 times retry

//Software Modes
#define SOFTWARE_LOOPHOLD_MODE			0x0A
#define SOFTWARE_REAL_MODE				0xA0

//Satellite's subsystems' states

//Misc Message Length

//TASK command
#define TASK_CMD_TERMINATE		0x00
#define TASK_CMD_RUN			0x01
#define	TASK_CMD_STOP			0x02
#define	TASK_CMD_RESTART		0x04
#define	TASK_CMD_DO_NOTHING		0x08

//TASK status flags
#define TASK_INITIALIZED_FLAG	0x01
#define TASK_RUNNING_FLAG		0x02
#define TASK_ERROR_FLAG			0x04
#define TASK_COMPLETED_FLAG		0x08
#define TASK_STATUS_READ_FLAG	0x10
#define TASK_SUMMARY_READ_FLAG	0x20
#define TASK_WARNING_FLAG		0x80
#define TASK_REC_FLAGS			0x0E
#define TASK_STAT_RUNNING_FLTR	0x0F
#define TASK_STAT_RUNNING		0x03

//Read tasks format bytes
#define READ_TASK_NO_READING	0xF0
#define READ_TASK_FORMAT_1		0xF1
#define READ_TASK_FORMAT_2		0xF2

//SEMI-PERMANENT SCRIPTSPACE (SP_SCRIPTSPACE) inidicators
#define SP_SCRIPTSPACE_GET_HK			0x00
#define SP_SCRIPTSPACE_DEPLOY_ANTENNA	0x01
#define SP_SCRIPTSPACE_LOG_ST_DATA		0x02
#define SP_SCRIPTSPACE_TEST_TK			0x03
#define SP_SCRIPTSPACE_EMPTY			0xFF

//Task's inidicator
#define TASK_GET_HK						0x00
#define TASK_DEPLOY_ANTENNA				0x01
#define TASK_LOG_ST_DATA				0x02
#define TASK_TEST_TK					0x03

//Scriptline end handler inputs
#define SCRIPTLINE_END_NON_CRITICAL_ERROR	0x01
#define SCRIPTLINE_END_CRITICAL_ERROR		0x02
#define SCRIPTLINE_END_SUCCESSFUL			0x03
#define SCRIPTLINE_END_TIMED_FLAG			0x80
#define SCRIPTLINE_END_LAST_FLAG			0x40
#define SCRIPTLINE_END_PROGRESS_FLAG		0x20
#define SCRIPTLINE_END_ONE_SHIFT_FLAG		0x04
#define SCRIPTLINE_END_TWO_SHIFT_FLAG		0x08
#define SCRIPTLINE_END_THREE_SHIFT_FLAG		0x10

//Script codes
#define SCRIPT_CMD_CHANGE_TASK_FLAGS			0x01
#define SCRIPT_CMD_GET_HK						0x02
#define SCRIPT_CMD_STORE_HK						0x03
#define SCRIPT_CMD_INIT_LOG_DATA				0x04
#define SCRIPT_CMD_LOG_DATA						0x05
#define SCRIPT_CMD_TERMINATE_LOG_DATA			0x06
#define SCRIPT_CMD_GET_SSCP						0x07
#define SCRIPT_CMD_SET_TK_CHANNEL				0x08
#define SCRIPT_CMD_CHECK_TK_CHANNEL				0x09
#define SCRIPT_CMD_CHECK_TK_CUT					0x0A
#define SCRIPT_CMD_SET_SINGLE_PWRS_CHANNEL		0x0B
#define SCRIPT_CMD_CHECK_SINGLE_PWRS_CHANNEL	0x0C
#define SCRIPT_CMD_ANTENNA_AUTO_DPLY			0x0D
#define SCRIPT_CMD_CHECK_ANTENNA_DPLY_STAT		0x0E

//Checksum constants
//Logic:
//	Bit1-0: 00 - Normal checksum
//			01 - CRC using table 
//			10 - CRC using simple method 
//			11 - reserved
//	Bit4-2: reserved 
//	Bit5: 	1 - 8-bit
//	Bit6:	1 - 16-bit
//	Bit7:	reserved
#define	CHECKSUM_CRC7_SIMPLE	0x02
#define CHECKSUM_8BIT			0x20
#define CHECKSUM_16BIT			0x40
#define CHECKSUM_CRC16_TABLE	0x41
#define CHECKSUM_CRC_FILTER		0x03
#define CHECKSUM_DEFAULT		CHECKSUM_16BIT			

//Constants for single data coding
#define DATA_OBDH_PAR_UC_MANUAL_CONTROL_ENABLED					0x0A01
#define DATA_OBDH_PAR_UC_NO_OF_INTERRUPT_PER_S					0x0A02
#define DATA_OBDH_PAR_UC_GSCH_TIME_WINDOW_10S					0x0A03
#define DATA_OBDH_PAR_UC_SOC_THRESHOLD_1						0x0A04
#define DATA_OBDH_PAR_UC_SOC_THRESHOLD_2						0x0A05
#define DATA_OBDH_PAR_UC_SOC_THRESHOLD_3						0x0A06
#define DATA_OBDH_PAR_UC_SOC_THRESHOLD_4						0x0A07
#define DATA_OBDH_PAR_UC_SOC_THRESHOLD_5						0x0A08
#define DATA_OBDH_PAR_UC_SOC_THRESHOLD_6						0x0A09
#define DATA_OBDH_PAR_UC_SOC_THRESHOLD_7						0x0A0A
#define DATA_OBDH_PAR_UC_SOC_THRESHOLD_8						0x0A0B
#define DATA_OBDH_PAR_US_REV_TIME_S								0x0A0C
#define DATA_OBDH_PAR_US_LOG_DATA_ID_1							0x0A0D
#define DATA_OBDH_PAR_US_LOG_DATA_ID_2							0x0A0E
#define DATA_OBDH_PAR_US_LOG_DATA_ID_3							0x0A0F
#define DATA_OBDH_PAR_US_LOG_DATA_ID_4							0x0A10
#define DATA_OBDH_PAR_US_LOG_DATA_ID_5							0x0A11
#define DATA_OBDH_PAR_US_LOG_DATA_ID_6							0x0A12
#define DATA_OBDH_PAR_US_LOG_DATA_ID_7							0x0A13
#define DATA_OBDH_PAR_US_LOG_DATA_ID_8							0x0A14
#define DATA_OBDH_PAR_US_BR_X_THRESHOLD_LOW						0x0A15
#define DATA_OBDH_PAR_US_BR_Y_THRESHOLD_LOW						0x0A16							
#define DATA_OBDH_PAR_US_BR_Z_THRESHOLD_LOW						0x0A17							
#define DATA_OBDH_PAR_US_BR_X_THRESHOLD_HIGH					0x0A18							
#define DATA_OBDH_PAR_US_BR_Y_THRESHOLD_HIGH					0x0A19							
#define DATA_OBDH_PAR_US_BR_Z_THRESHOLD_HIGH					0x0A1A							
#define DATA_OBDH_PAR_UC_BEACON_INTERVAL_M3						0x0A1B							
#define DATA_OBDH_PAR_UC_BEACON_INTERVAL_M4_M5_M7				0x0A1C							
#define DATA_OBDH_PAR_UC_BEACON_INTERVAL_M8						0x0A1D							
#define DATA_OBDH_PAR_UC_BEACON_INTERVAL_COMM_ZONE				0x0A1E							
#define DATA_OBDH_PAR_UL_DET_UPTIME_LIMIT						0x0A1F							
#define DATA_OBDH_G_US_ERROR_CODE								0x0A8A
#define DATA_OBDH_G_F_RUN_TIME									0x0A8B
#define DATA_OBDH_G_US_CHECKSUM_REGISTER						0x0A8C
#define DATA_OBDH_US_INTERVAL_PERIOD_1							0x0A90
#define DATA_OBDH_US_INTERVAL_PERIOD_2							0x0A91
#define DATA_OBDH_US_INTERVAL_PERIOD_3							0x0A92
#define DATA_OBDH_US_INTERVAL_PERIOD_4							0x0A93
#define DATA_OBDH_US_INTERVAL_PERIOD_5							0x0A94
#define DATA_OBDH_US_INTERVAL_PERIOD_6							0x0A95
#define DATA_OBDH_US_INTERVAL_PERIOD_7							0x0A96
#define DATA_OBDH_US_INTERVAL_PERIOD_8							0x0A97
#define DATA_OBDH_US_INTERVAL_PERIOD_9							0x0A98
#define DATA_OBDH_US_INTERVAL_PERIOD_10							0x0A99
#define DATA_OBDH_US_INTERVAL_PERIOD_11							0x0A9A
#define DATA_OBDH_US_INTERVAL_PERIOD_12							0x0A9B
#define DATA_OBDH_US_INTERVAL_PERIOD_13							0x0A9C
#define DATA_OBDH_US_INTERVAL_PERIOD_14							0x0A9D
#define DATA_OBDH_US_INTERVAL_PERIOD_15							0x0A9E
#define DATA_OBDH_US_INTERVAL_PERIOD_16							0x0A9F
#define DATA_OBDH_UC_SCM_HOLD_ACTIVATION						0x0AA3
#define DATA_OBDH_UC_TM_HOLD_ACTIVATION							0x0AA4
#define DATA_OBDH_UC_AUTO_BEACON_ACTIVATION						0x0AA6
#define DATA_OBDH_G_UC_FROM_TO_CALLSIGN_1						0x0AA7
#define DATA_OBDH_G_UC_FROM_TO_CALLSIGN_2						0x0AA8
#define DATA_OBDH_G_UC_FROM_TO_CALLSIGN_3						0x0AA9
#define DATA_OBDH_G_UC_FROM_TO_CALLSIGN_4						0x0AAA
#define DATA_OBDH_G_UC_FROM_TO_CALLSIGN_5						0x0AAB
#define DATA_OBDH_G_UC_FROM_TO_CALLSIGN_6						0x0AAC
#define DATA_OBDH_G_UC_FROM_TO_CALLSIGN_7						0x0AAD
#define DATA_OBDH_G_UC_FROM_TO_CALLSIGN_8						0x0AAE
#define DATA_OBDH_G_UC_FROM_TO_CALLSIGN_9						0x0AAF
#define DATA_OBDH_G_UC_FROM_TO_CALLSIGN_10						0x0AB0
#define DATA_OBDH_G_UC_FROM_TO_CALLSIGN_11						0x0AB1
#define DATA_OBDH_G_UC_FROM_TO_CALLSIGN_12						0x0AB2
#define DATA_OBDH_G_UC_FROM_TO_CALLSIGN_13						0x0AB3
#define DATA_OBDH_G_UC_FROM_TO_CALLSIGN_14						0x0AB4
#define DATA_OBDH_G_UL_OBC_TIME									0x0AB5 //Ian, 19-JUL-2012: to support satellite time reset

//Subsystems data
#define DATA_PWRS_HK_UC_MODE										0x0C2B
#define DATA_PWRS_PAR_UC_MPPT_MODE									0x0C0F
#define DATA_PWRS_PAR_UC_BATT_HEATER								0x0C16
#define DATA_PWRS_PAR_UC_BATT_CHARGE_UPP_LIMIT						0x0C33
#define DATA_PWRS_PAR_UC_BATT_CHARGE_LOW_LIMIT						0x0C34
#define DATA_PWRS_UC_CHANNEL_STATUS									0x0C32
#define DATA_PWRS_US_PV_FV_1										0x0C0C
#define DATA_PWRS_US_PV_FV_2										0x0C0D
#define DATA_PWRS_US_PV_FV_3										0x0C0E
#define DATA_PWRS_C_THERMAL_KNIFE									0x0C29
#define DATA_PWRS_US_FIRMWARE_VER									0x0C2A
#define DATA_ADCS_PAR_S_SS_THRESHOLD								0x080D
#define DATA_ADCS_PAR_F_CGAIN_1										0x0837
#define DATA_ADCS_PAR_F_CGAIN_2										0x0838
#define DATA_ADCS_PAR_F_CGAIN_3										0x0839
#define DATA_ADCS_PAR_F_CGAIN_4										0x083A
#define DATA_ADCS_PAR_F_CGAIN_5										0x083B
#define DATA_ADCS_PAR_F_CGAIN_6										0x083C
#define DATA_ADCS_PAR_F_CGAIN_7										0x083D
#define DATA_ADCS_PAR_F_CGAIN_8										0x083E
#define DATA_ADCS_PAR_F_CGAIN_9										0x083F
#define DATA_ADCS_PAR_F_CGAIN_10									0x0840
#define DATA_ADCS_PAR_F_CGAIN_11									0x0841
#define DATA_ADCS_PAR_F_CGAIN_12									0x0842
#define DATA_ADCS_PAR_F_CGAIN_13									0x0843
#define DATA_ADCS_PAR_F_CGAIN_14									0x0844
#define DATA_ADCS_PAR_F_CGAIN_15									0x0845
#define DATA_ADCS_PAR_F_CGAIN_16									0x0846
#define DATA_ADCS_PAR_F_CGAIN_17									0x0847
#define DATA_ADCS_PAR_F_CGAIN_18									0x0848
#define DATA_ADCS_PAR_F_TLE_EPOCH									0x084A
#define DATA_ADCS_PAR_F_TLE_INC										0x084B
#define DATA_ADCS_PAR_F_TLE_RAAN									0x084C
#define DATA_ADCS_PAR_F_TLE_ARG_PER									0x084D
#define DATA_ADCS_PAR_F_TLE_ECC										0x084E
#define DATA_ADCS_PAR_F_TLE_MA										0x084F
#define DATA_ADCS_PAR_US_TLE_MM										0x0850
#define DATA_ADCS_PAR_UL_TIME										0x0851
#define DATA_ADCS_PAR_S_VECTOR_2SUN_X								0x0854
#define DATA_ADCS_PAR_S_VECTOR_2SUN_Y								0x0855
#define DATA_ADCS_PAR_S_VECTOR_2SUN_Z								0x0856
#define DATA_ADCS_PAR_C_CTRL_BR_X									0x085A
#define DATA_ADCS_PAR_C_CTRL_BR_Y									0x085B
#define DATA_ADCS_PAR_C_CTRL_BR_Z									0x085C
#define DATA_ADCS_PAR_S_DGAIN										0x085D

//Constants for single data category
#define DATA_CAT_OBDH_FIRMWARE_VERSION								0x0001
#define DATA_CAT_COMPLETE_BEACON_DATA_VAR							0x0002
#define DATA_CAT_COMPLETE_BRH_DATA_VAR								0x0003
#define DATA_CAT_COMPLETE_IMH_DATA_VAR								0x0004
#define DATA_CAT_COMPLETE_SMH_DATA_VAR								0x0005
#define DATA_CAT_COMPLETE_TASK_STRUCTURE							0x0006
#define DATA_CAT_GSCH_VAR											0x0007
#define DATA_CAT_LOG_DATA_VAR										0x0008
#define DATA_CAT_COMM_DATA											0x0009
#define DATA_CAT_STORING_VAR										0x000A
#define DATA_CAT_SENDING_VAR										0x000B
#define DATA_CAT_OBS_VAR											0x000C
#define DATA_CAT_SAT_TIME											0x000D
#define DATA_CAT_SAT_DATE_CURRENT									0x000E
#define DATA_CAT_SAT_DATE_INIT										0x000F
#define DATA_CAT_SAT_STATE											0x0010
#define DATA_CAT_OBDH_BP											0x0011
#define DATA_CAT_SCRIPTSPACE_1										0x0012
#define DATA_CAT_SCRIPTSPACE_2										0x0013
#define DATA_CAT_SCRIPTSPACE_3										0x0014
#define DATA_CAT_SCRIPTSPACE_4										0x0015
#define DATA_CAT_SCRIPTSPACE_5										0x0016
#define DATA_CAT_SCRIPTSPACE_6										0x0017
#define DATA_CAT_SCRIPTSPACE_7										0x0018
#define DATA_CAT_SCRIPTSPACE_8										0x0019
#define DATA_CAT_SCRIPTSPACE_9										0x001A
#define DATA_CAT_SCRIPTSPACE_10										0x001B
#define DATA_CAT_COMM_HK											0x0080
#define DATA_CAT_ADCS_HK											0x0081
#define DATA_CAT_PWRS_HK											0x0082
#define DATA_CAT_OBDH_HK											0x0083
#define DATA_CAT_FROM_TO_CALLSIGN									0x0084
#define DATA_CAT_PWRS_SSCP											0x0085
#define DATA_CAT_SINGLE_TASK_STRUCTURE								0x0086 	//Index 1
#define DATA_CAT_SINGLE_SCRIPTSPACE									0x0087 	//Index 1
#define DATA_CAT_SCRIPTLINE_TIMER									0x0088 	//Index 1
#define DATA_CAT_SCRIPTLINE_RETRY_COUNTER							0x0089 	//Index 1
#define DATA_CAT_BUF_SD_DATA_IN										0x008A
#define DATA_CAT_BUF_SD_DATA_OUT									0x008B
#define DATA_CAT_SCHEDULE											0x008C 	//Index 1 (block 1/ block 2 / block 3/ block 4/ block 5/ block 6)
#define DATA_CAT_BUF_I2C_DATA_IN_VARY								0x008D
#define DATA_CAT_BUF_I2C_DATA_OUT_VARY								0x008E
#define DATA_CAT_SCHEDULE_PAR										0x008F
#define DATA_CAT_SCHEDULE_1_TO_10									0x0090
#define DATA_CAT_SCHEDULE_11_TO_20									0x0091
#define DATA_CAT_SCHEDULE_21_TO_30									0x0092
#define DATA_CAT_SCHEDULE_31_TO_40									0x0093
#define DATA_CAT_SCHEDULE_41_TO_50									0x0094
#define DATA_CAT_SCHEDULE_51_TO_60									0x0095
#define DATA_CAT_SCHEDULE_61_TO_70									0x0096
#define DATA_CAT_OBDH_PAR_TAI_UL_OBC_TIME_S							0x0097
#define DATA_CAT_LATEST_HK_SAMPLING_TIME_AND_ORBIT_TAI_FORMAT		0x0098
#define DATA_CAT_ADCS_SSCP											0x0099
#define DATA_CAT_LATEST_TIME_AND_ORBIT_TAI_FORMAT					0x009A
#define DATA_CAT_OBDH_MP_VAR_G_UC									0x009B
#define DATA_CAT_OBDH_MP_VAR_G_US									0x009C
#define DATA_CAT_OBDH_MP_VAR_G_S									0x009D
#define DATA_CAT_OBDH_MP_VAR_G_UL									0x009E
#define DATA_CAT_OBDH_MP_VAR_G_F									0x009F
#define DATA_CAT_SAT_DATE_UPDATE									0x00A0
#define DATA_CAT_PWRS_DATA											0x00A1
#define DATA_CAT_SD_HEADER											0x00A2
#define DATA_CAT_AX25_HEADER										0x00A3
#define DATA_CAT_HOLD_ACTIVATION									0x00A4
#define DATA_CAT_COMPLETE_AX25_FRAME_BUFFER							0x00A5
#define DATA_CAT_LAST_HK_EC											0x00A6
#define DATA_CAT_OBDH_SSCP											0x00A7
#define DATA_CAT_SINGLE_SUBSYSTEM_DATA								0x00A8
#define DATA_CAT_ACK_MSG											0x00A9
#define DATA_CAT_SCRIPTLINE_ITEM_ERROR_CODES						0x00AA
#define DATA_CAT_GP_REQUEST_KEY										0x00AB
#define DATA_CAT_LATEST_HK_SAMPLING_TIME_TAI_FORMAT					0x00AC
#define DATA_CAT_SCRIPTSPACE_PAR									0x00AD
#define DATA_CAT_LOG_DATA_INIT_SET									0x00AE
#define DATA_CAT_LOG_DATA_SET										0x00AF
#define DATA_CAT_LOG_DATA_BUFFER									0x00B0
#define DATA_CAT_FILLED_SCHEDULE									0x00B1
#define DATA_CAT_SD_HEADER_INFO_SENT								0x00B2
#define DATA_CAT_SAT_REV_NO_IN_FLOAT								0x00B3
#define DATA_CAT_FILLED_AX25_FRAME									0x00B4
#define DATA_CAT_OBDH_UL_OBC_TIME_S									0x00B5
#define DATA_CAT_SINGLE_TASK_UC_STATUS								0x00B6
#define DATA_CAT_OBDH_SELECTED_HK									0x00B7
#define DATA_CAT_OBDH_SELECTED_DATA									0x00B8
#define DATA_CAT_EVENTLOG_DATA_BUFFER								0x00B9	//dec_yc event data storing
#define DATA_CAT_FUNCTION_MODULE_ERROR_CODE							0x00BA
#define DATA_CAT_LAST_SSCP_EC										0x00BB

//SD card header constants
//Owner bytes

//Data type bytes
#define SD_ESSENTIAL				0x01
#define SD_HK						0x02
#define SD_IMAGE					0x04
#define SD_FLASH_PROGRAM			0x05
#define SD_BP						0x06
#define SD_RECOVERY_PACKAGE			0x07
#define SD_EVENTLOG_DATA			0x08	//dec22_yc eventlog storing
#define SD_SCHEDULE					0x09
#define SD_SCRIPT					0x0A
#define SD_LOG_DATA					0x0B
#define SD_COMPLETE_TASK_STRUCTURE	0x0C
#define SD_FREE_CATEGORY			0x0D
#define SD_SSCP						0x0E
#define SD_SAT_TIME					0x0F
#define SD_SINGLE_SUBSYSTEM_DATA	0x10
#define SD_GP_REQUEST_KEY			0x11
#define SD_REAL_TIME				0x12
#define SD_REQUESTED_BLOCK			0x13
#define SD_SAT_REV					0x14
#define SD_RCVRY_MSG				0x15
#define SD_NULL						0x66

//Pointer constants
#define SD_BP_OBDH_ESS_START				0x00000000	
#define SD_BP_OBDH_ESS_END					0x0000003F	//Buffer for later development
#define SD_BP_OBDH_SCHEDULE_START			0x00000040
#define SD_BP_OBDH_SCHEDULE_END				0x0000007F	//Buffer for later development
#define SD_BP_OBDH_SCRIPT_START				0x00000080
#define SD_BP_OBDH_SCRIPT_END				0x000000BF	//Buffer for later development
#define SD_BP_OBDH_ORIGINAL_ESS_START		0x000000C0
#define SD_BP_OBDH_ORIGINAL_ESS_END			0x000000FF	//Buffer for later development
#define SD_BP_OBDH_ORIGINAL_SCHEDULE_START	0x00000100
#define SD_BP_OBDH_ORIGINAL_SCHEDULE_END	0x0000013F	//Buffer for later development
#define SD_BP_OBDH_ORIGINAL_SCRIPT_START	0x00000140
#define SD_BP_OBDH_ORIGINAL_SCRIPT_END		0x0000017F	//Buffer for later development
#define SD_BP_HK_START						0x00010000	
#define SD_BP_HK_LIMIT						0x000CFF00	
#define SD_BP_HK_END						0x000CFFFF	
#define SD_BP_DATALOG_START					0x00100000	
#define SD_BP_DATALOG_LIMIT					0x0012F000	
#define SD_BP_DATALOG_END					0x0012FFFF	
#define SD_BP_OBDH_ORBIT_START				0x00130000	
#define SD_BP_OBDH_ORBIT_LIMIT				0x0013FF00	
#define SD_BP_OBDH_ORBIT_END				0x0013FFFF
#define SD_BP_PACKAGE_RECOVERY_START		0x00140000	
#define SD_BP_PACKAGE_RECOVERY_LIMIT		0x0014FF00
#define SD_BP_PACKAGE_RECOVERY_END			0x0014FFFF
#define SD_BP_EVENTLOG_START				0x00150000	//dec22_yc event log storing same size as HK
#define SD_BP_EVENTLOG_LIMIT				0x0020FF00	//dec22_yc event log storing
#define SD_BP_EVENTLOG_END					0x0020FFFF	//dec22_yc event log storing
#define SD_BP_MAX							0x003BAFFF

//No of copies for various data sets
#define NO_OF_COPIES_SD_ESSENTIAL				4
#define NO_OF_COPIES_SD_HK						2
#define NO_OF_COPIES_SD_BP						2
#define NO_OF_COPIES_SD_SCHEDULE				2
#define NO_OF_COPIES_SD_SCRIPT					2
#define NO_OF_COPIES_SD_LOG_DATA				1
#define NO_OF_COPIES_SD_EVENTLOG_DATA			2//dec22_yc eventlog storing
#define NO_OF_COPIES_SD_RECOVERY_PACKAGE		1

//------------------------------------------------------------------------------------
// Command for SD Card
//------------------------------------------------------------------------------------
#define SD_GO_IDLE_STATE            0
#define SD_SEND_OP_COND             1
#define SD_SEND_STATUS              13
#define SD_READ_SINGLE_BLOCK        17
#define SD_WRITE_SINGLE_BLOCK       24

#define SD_DEASSERT					1
#define SD_ASSERT					0

//-----------------------------------------------------------------------------
// Macros used to calculate Loop Filter Bits
//
// Reference
// C8051F121x-13x.pdf 
// SFR Definition 14.8. PLL0FLT: PLL Filter
//
// Note that PLL Loop filter bits give a suitable frequency range for each 
// setting. In some cases more than one setting may be acceptable for a 
// particular frequency. This macro will select the best fit setting for
// a particular frequency. 
//
//-----------------------------------------------------------------------------											
#if (SYSCLK_USED == LEVEL_3_SYSCLK)
	#if (INT_PLL_DIV_CLK < 5000000L)
	   #error "error: PLL divided clock frequency Too Low!"
	#elif (INT_PLL_DIV_CLK < 8000000L)
	   #define INT_PLLFLT_LOOP 0x0F
	#elif (INT_PLL_DIV_CLK < 12500000L)
	   #define INT_PLLFLT_LOOP 0x07
	#elif (INT_PLL_DIV_CLK < 19500000L)
	   #define INT_PLLFLT_LOOP 0x03
	#elif (INT_PLL_DIV_CLK < 30000001L)
	   #define INT_PLLFLT_LOOP 0x01
	#else
	   #error "error: PLL divided clock frequency Too High!"
	#endif
	
	#if (EXT_PLL_DIV_CLK < 5000000L)
		#error "error: PLL divided clock frequency Too Low!"
	#elif (EXT_PLL_DIV_CLK < 8000000L)
	  	#define EXT_PLLFLT_LOOP 0x0F
	#elif (EXT_PLL_DIV_CLK < 12500000L)
	  	#define EXT_PLLFLT_LOOP 0x07
	#elif (EXT_PLL_DIV_CLK < 19500000L)
	  	#define EXT_PLLFLT_LOOP 0x03
	#elif (EXT_PLL_DIV_CLK < 30000001L)
	  	#define EXT_PLLFLT_LOOP 0x01
	#else
	  	#error "error: PLL divided clock frequency Too High!"
	#endif
#endif

//-----------------------------------------------------------------------------
// Set the appropriate XFCN bits for the crystal frequency
//
//   XFCN     Crystal (XOSCMD = 11x)
//   000      f <= 32 kHz
//   001      32	kHz < f <=	84		kHz
//   010      84	kHz < f <=	225 	kHz
//   011      225	kHz < f <=	590 	kHz
//   100      590 kHz < f <=	1.5 	MHz
//   101      1.5 MHz < f <=	4		MHz
//   110      4	MHz < f <=	10 	MHz
//   111      10	MHz < f <=	30 	MHz
//-----------------------------------------------------------------------------
#if (SYSCLK_USED == LEVEL_3_SYSCLK)
	#if (EXT_OSC_FREQUENCY <= 32000)
		#define XFCN 0
	#elif (EXT_OSC_FREQUENCY <= 84000L)
		#define XFCN 1
	#elif (EXT_OSC_FREQUENCY <= 225000L)
		#define XFCN 2
	#elif (EXT_OSC_FREQUENCY <= 590000L)
		#define XFCN 3
	#elif (EXT_OSC_FREQUENCY <= 1500000L)
		#define XFCN 4
	#elif (EXT_OSC_FREQUENCY <= 4000000L)
		#define XFCN 5
	#elif (EXT_OSC_FREQUENCY <= 10000000L)
		#define XFCN 6
	#elif (EXT_OSC_FREQUENCY <= 30000000L)
		#define XFCN 7
	#else
		#error "Defined Crystal Frequency outside allowable range!"
		#define XFCN 0
	#endif
#elif (SYSCLK_USED == LEVEL_2_SYSCLK)
	#if (EXT_OSC_FREQUENCY <= 32000)
		#define XFCN 0
	#elif (EXT_OSC_FREQUENCY <= 84000L)
		#define XFCN 1
	#elif (EXT_OSC_FREQUENCY <= 225000L)
		#define XFCN 2
	#elif (EXT_OSC_FREQUENCY <= 590000L)
		#define XFCN 3
	#elif (EXT_OSC_FREQUENCY <= 1500000L)
		#define XFCN 4
	#elif (EXT_OSC_FREQUENCY <= 4000000L)
		#define XFCN 5
	#elif (EXT_OSC_FREQUENCY <= 10000000L)
		#define XFCN 6
	#elif (EXT_OSC_FREQUENCY <= 30000000L)
		#define XFCN 7
	#else
		#error "Defined Crystal Frequency outside allowable range!"
		#define XFCN 0
	#endif
#endif

//-----------------------------------------------------------------------------
// Macros used to calculate ICO  Bits
//
// Reference
// C8051F121x-13x.pdf 
// SFR Definition 14.8. PLL0FLT: PLL Filter
//
// Note that PLL ICO bits give a suitable frequency range for each setting. 
// In some cases more than one setting may be acceptable for a particular 
// frequency. This macro will select the best fit setting for a particular
// frequency. 
//
//-----------------------------------------------------------------------------

#if (SYSCLK_USED == LEVEL_3_SYSCLK)
	#if (INT_PLL_OUT_CLK < 25000000L)
	   #error "error: PLL output frequency Too Low!"
	#elif (INT_PLL_OUT_CLK < 42500000L)
	   #define INT_PLLFLT_ICO 0x30
	#elif (INT_PLL_OUT_CLK < 52500000L)
	   #define INT_PLLFLT_ICO 0x20
	#elif (INT_PLL_OUT_CLK < 72500000L)
	   #define INT_PLLFLT_ICO 0x10
	#elif (INT_PLL_OUT_CLK < 100000001L)
	   #define INT_PLLFLT_ICO 0x00
	#else
	   #error "error: PLL output frequency Too High"
	#endif
	
	#if (EXT_PLL_OUT_CLK < 25000000L)
		#error "error: PLL output frequency Too Low!"
	#elif (EXT_PLL_OUT_CLK < 42500000L)
		#define EXT_PLLFLT_ICO 0x30
	#elif (EXT_PLL_OUT_CLK < 52500000L)
		#define EXT_PLLFLT_ICO 0x20
	#elif (EXT_PLL_OUT_CLK < 72500000L)
		#define EXT_PLLFLT_ICO 0x10
	#elif (EXT_PLL_OUT_CLK < 100000001L)
		#define EXT_PLLFLT_ICO 0x00
	#else
		#error "error: PLL output frequency Too High"
	#endif
#endif

//-----------------------------------------------------------------------------
// Macros used to calculate Flash Read Time bits
//
// Reference
// C8051F121x-13x.pdf 
// SFR Definition 15.2. FLSCL: Flash Memory Control
//-----------------------------------------------------------------------------

#if (SYSCLK_USED == LEVEL_3_SYSCLK)
	#if (INT_SYSCLK < 25000001L)
	   #define	INT_FLSCL_FLRT	0x00
	#elif (INT_SYSCLK < 50000001L)
	   #define	INT_FLSCL_FLRT	0x10
	#elif (INT_SYSCLK < 50000001L)
	   #define	INT_FLSCL_FLRT	0x20
	#elif (INT_SYSCLK < 100000001L)
	   #define	INT_FLSCL_FLRT 0x30
	#else
	   #error "error: SYSCLK Too High"
	#endif
	
	#if (EXT_SYSCLK < 25000001L)
		#define EXT_FLSCL_FLRT 0x00
	#elif (EXT_SYSCLK < 50000001L)
		#define EXT_FLSCL_FLRT 0x10
	#elif (EXT_SYSCLK < 50000001L)
		#define EXT_FLSCL_FLRT 0x20
	#elif (EXT_SYSCLK < 100000001L)
		#define EXT_FLSCL_FLRT 0x30
	#else
		#error "error: SYSCLK Too High"
	#endif
#endif

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F12x
//-----------------------------------------------------------------------------
sfr16 ADC0		= 0xBE;        //ADC0 data

//-----------------------------------------------------------------------------
// Oscillator variables (parameters)
//-----------------------------------------------------------------------------
extern unsigned long OSC_FREQUENCY;
extern unsigned long SYSCLK;

#if (SYSCLK_USED == LEVEL_3_SYSCLK)
extern unsigned char PLL_MUL;
extern unsigned char PLL_DIV;
extern unsigned long PLL_DIV_CLK;
extern unsigned long PLL_OUT_CLK;
extern unsigned char FLSCL_FLRT;
extern unsigned char PLLFLT_ICO;
extern unsigned char PLLFLT_LOOP;
#endif

#if (CQUEUE_ENABLE == 1)
//-----------------------------------------------------------------------------
// Command queue variables
//-----------------------------------------------------------------------------
extern unsigned char	g_uc_command_queue[COMMAND_QUEUE_SIZE][COMMAND_LENGTH];
extern unsigned char 	g_uc_command_queue_size;	//At first, there is nothing in the queue
extern unsigned char 	g_uc_command_queue_pointer;	//At first, the pointer points the first command in queue
extern unsigned char	g_uc_command_entry[COMMAND_LENGTH];
#endif

//-----------------------------------------------------------------------------
// Global variables for main loop
//-----------------------------------------------------------------------------
extern unsigned char	g_uc_call_task_buffer[CALL_TASK_BUFFER_SIZE];
extern unsigned short	g_us_function_module_error_code;
extern unsigned short 	g_us_res_length;
extern unsigned short 	g_us_error_code;
extern unsigned short	g_us_data_counter;	//This variable act as global data counter

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
extern unsigned char 	g_uc_i2c_data_out[I2C_BUFFER_SIZE];
extern unsigned char 	g_uc_i2c_data_in[I2C_BUFFER_SIZE];
extern unsigned char 	g_uc_i2c_transferred_byte_out;		
extern unsigned char 	g_uc_i2c_transferred_byte_in;		
extern unsigned char 	g_uc_i2c_target;
extern unsigned char 	g_uc_uart_buffer[UART_BUFFER_SIZE];
extern unsigned short 	g_us_i2c_isr_error_code;
extern unsigned char 	g_uc_i2c_msg_out;
extern unsigned short 	g_us_uart_buffer_filled;
extern unsigned short 	g_us_uart_input_first;
extern unsigned short 	g_us_uart_output_first;

//-----------------------------------------------------------------------------
// Timer variables
//-----------------------------------------------------------------------------
extern unsigned char 	g_uc_timer_value_low[RECORD_BLOCK_SIZE];
extern unsigned char 	g_uc_timer_value_high[RECORD_BLOCK_SIZE];
extern unsigned short 	g_us_timer_flag_counter_value[RECORD_BLOCK_SIZE];
extern unsigned short 	g_us_timer_flag_counter;
extern unsigned short 	g_us_timer_value[RECORD_BLOCK_SIZE];

//-----------------------------------------------------------------------------
// ISIS ITC Variables
//-----------------------------------------------------------------------------
extern unsigned char 	g_uc_itc_snd_ax_fr_dflt_wr;
extern unsigned char 	g_uc_itc_snd_ax_fr_ovrd_wr;
extern unsigned char 	g_uc_itc_snd_cw_msg_wr;
extern unsigned char 	g_uc_itc_set_ax_bcn_dflt_wr;
extern unsigned char 	g_uc_itc_set_ax_bcn_ovrd_wr;
extern unsigned char 	g_uc_itc_set_cw_bcn_wr;

//-----------------------------------------------------------------------------
// ISIS ITC Message Variables
//-----------------------------------------------------------------------------
extern unsigned char 	g_uc_cw_message[CW_BUFFER_SIZE];
extern unsigned char	g_uc_cw_message_length;
extern unsigned char 	g_uc_beacon_interval;
extern unsigned char 	g_uc_ax25_message[AX25_BUFFER_SIZE];
extern unsigned char 	g_uc_from_to_callsign[AX25_CALLSIGN_SIZE];

//-----------------------------------------------------------------------------
// Timeout counters
//-----------------------------------------------------------------------------
extern unsigned char 	g_uc_uart_busy_timeout_counter;
extern unsigned char 	g_uc_i2c_transmission_timeout_counter;
extern unsigned char 	g_uc_i2c_busy_timeout_counter;
extern unsigned char 	g_uc_i2c_receiving_timeout_counter;
extern unsigned char 	g_uc_i2c_bus_sda_held_low_recovery_timeout_counter;
extern unsigned char 	g_uc_i2c_bus_scl_held_high_recovery_timeout_counter;
extern unsigned char	g_uc_spi_busy_timeout_counter;
extern unsigned char	g_uc_spi_transmission_timeout_counter;
extern unsigned char 	g_uc_timer0_stuck_counter;

//-----------------------------------------------------------------------------
// SD Card variables
//-----------------------------------------------------------------------------
extern unsigned char 	g_uc_sd_data_buffer[SD_CARD_BUFFER_SIZE];

//-----------------------------------------------------------------------------
// Global variables for SD Card header
//-----------------------------------------------------------------------------
extern unsigned char	g_uc_sd_header_subsystem;
extern unsigned char	g_uc_sd_header_data_type;
extern unsigned short	g_us_sd_header_block_property;
extern unsigned short	g_us_sd_header_no_of_this_block;
extern unsigned long	g_ul_sd_header_sd_write_block;
extern unsigned short	g_us_sd_header_this_block_data_size;

//-----------------------------------------------------------------------------
// Global variables for AX.25 frame package buffer
//-----------------------------------------------------------------------------
extern unsigned char	g_uc_ax25_header_subsystem;
extern unsigned char	g_uc_ax25_header_data_type;
extern unsigned char	g_uc_ax25_header_package_property;
extern unsigned char	g_uc_ax25_header_no_of_this_package;
extern unsigned char	g_uc_ax25_header_this_package_size;

//-----------------------------------------------------------------------------
// Orbital-based search (OBS) variables
// Some unused global variables have been removed
//-----------------------------------------------------------------------------
extern unsigned long	g_ul_obs_initial_bp;
extern unsigned long	g_ul_obs_end_bp;
extern unsigned short	g_us_obs_first_orbit_request;
extern unsigned short	g_us_obs_last_orbit_request;

//-----------------------------------------------------------------------------
// Data storing variables
//-----------------------------------------------------------------------------
extern unsigned char 	g_uc_data_type_saved_buffer[MAX_NO_OF_DATA_TYPE_TO_BE_SAVED];
extern unsigned char 	g_uc_temporary_data_buffer[MAX_SD_CARD_BLOCK_CONTENT_SIZE];
extern unsigned char 	g_uc_no_of_data_saved;
extern unsigned long 	g_ul_initial_sd_card_bp;
extern unsigned long 	*g_ul_initial_sd_card_bp_address;
extern unsigned char 	g_uc_no_of_copies;
extern unsigned char 	*g_uc_no_of_overwritten_p;
extern unsigned char	g_uc_data_sets_property;
extern unsigned long 	g_ul_memory_address_lower_limit;
extern unsigned long 	g_ul_memory_address_upper_limit;
extern unsigned long 	g_ul_memory_address_end;	//New
extern unsigned long	g_ul_start_sd_card_load_bp;		 
extern unsigned long    g_ul_end_sd_card_load_bp;	//New
extern unsigned char   	g_uc_sd_load_data_timeout_counter;	//New
extern unsigned short	g_us_sd_load_error_code;	//New
extern unsigned char	g_uc_save_data_hold_timeout_counter;	//New

//-----------------------------------------------------------------------------
// Global variables for data sending
//-----------------------------------------------------------------------------
extern unsigned char	g_uc_data_type_sent_buffer[MAX_NO_OF_DATA_TYPE_TO_BE_SENT];
extern unsigned char 	g_uc_no_of_data_sent;
extern unsigned short	g_us_sd_card_meaningful_data_size;
extern unsigned char	g_uc_current_data_sent_counter;
extern unsigned short	g_us_beacon_send_error_code;
extern unsigned char 	g_uc_schedule_sent_counter;
extern unsigned short	g_us_expected_no_of_itn_checking_per_transmission;

//-----------------------------------------------------------------------------
// CRC variables
//-----------------------------------------------------------------------------
extern unsigned short 	g_us_crc16_table[CRC_TABLE_SIZE];
extern unsigned short 	g_us_checksum_register;

//-----------------------------------------------------------------------------
// A2D conversion software flags and counters
//-----------------------------------------------------------------------------
extern unsigned char 	g_uc_ad0int_counter;

//-----------------------------------------------------------------------------
// OBDH variables
//-----------------------------------------------------------------------------
extern unsigned char 	g_uc_obdh_firmware_version[OBDH_FIRMWARE_VERSION_LENGTH];

//------------------------------------------------------------------------------------
// Global variables for task
//------------------------------------------------------------------------------------
extern unsigned short	g_us_scriptline_timer_error_code;
extern unsigned short 	g_us_scriptline_retry_counter_error_code;

//------------------------------------------------------------------------------------
// Global variables for ground station command handler
//------------------------------------------------------------------------------------
extern unsigned short	g_us_groundpass_time_register;

//------------------------------------------------------------------------------------
// Global variables for essential error codes
//------------------------------------------------------------------------------------
extern unsigned short	g_us_latest_obdh_hk_ec;
extern unsigned short	g_us_latest_adcs_hk_ec;
extern unsigned short	g_us_latest_pwrs_hk_ec;
extern unsigned short	g_us_latest_comm_hk_ec;
extern unsigned short	g_us_latest_rtc_time_update_ec;
extern unsigned short	g_us_latest_ch_stat_update_ec;
extern unsigned short	g_us_latest_saving_hk_ec;
extern unsigned short	g_us_latest_gdsn_to_rtc_update_ec;
extern unsigned short	g_us_latest_obdh_sscp_ec;
extern unsigned short	g_us_latest_adcs_sscp_ec;
extern unsigned short	g_us_latest_pwrs_sscp_ec;

//------------------------------------------------------------------------------------
// Global multi-purpose registers
//------------------------------------------------------------------------------------
extern unsigned char 	g_uc;
extern unsigned short	g_us;
extern short			g_s;
extern unsigned long	g_ul;
extern float			g_f;

//------------------------------------------------------------------------------------
// Global variables for beacon handler
//------------------------------------------------------------------------------------
extern unsigned char	g_uc_beacon_time_since_last_timeout_sending;
extern unsigned char	g_uc_beacon_timeout_value;
extern unsigned char 	g_uc_beacon_retry_counter;
extern unsigned char 	g_uc_beacon_timeout_retry_counter;
extern unsigned short 	g_us_beacon_init_ax25_error_code;
extern unsigned short	g_us_beacon_monitoring_time;
extern unsigned char 	g_uc_beacon_error_counter;

//------------------------------------------------------------------------------------
// Global variables for GSCH
//------------------------------------------------------------------------------------
//Variable for GSCH error codes
extern unsigned short g_us_gsch_error_code;

//Variables for further GSCH monitoring
extern unsigned short g_us_gsch_command_validation_error_code;

//Retry counter in each state
extern unsigned char g_uc_gsch_comm_retry_counter;
extern unsigned char g_uc_gsch_loading_retry_counter;

//Counter for monitoring GSCH transitional period
extern unsigned short g_us_gsch_monitoring_transitional_period;

//Monitoring time for DEAD state and RECOVERY state
extern unsigned short g_us_gsch_monitoring_period;

//Variables to store generated key and answers
extern unsigned short g_us_gsch_key;
extern unsigned short g_us_gsch_answer_1;
extern unsigned short g_us_gsch_answer_2;

//Variables for GSCH command frame
extern unsigned char g_uc_gsch_gs_header;
extern unsigned char g_uc_gsch_gs_cmd_header;
extern unsigned char g_uc_gsch_gs_cmd_tail;
extern unsigned char g_uc_gsch_gs_arg_len;
extern unsigned char g_uc_gsch_gs_arg[MAX_GSCH_CMD_ARG_LEN];
extern unsigned short g_us_gsch_gs_cmd_crc;

//Variable to store subsystem and data type to load as commanded by GDSN
extern unsigned char g_uc_gsch_subsystem_to_load;
extern unsigned char g_uc_gsch_data_type_to_load;
extern unsigned char g_uc_gsch_sd_block_property;
extern unsigned char g_uc_gsch_no_of_loading;
extern unsigned char g_uc_gsch_sampling_step;

//Register for scheduled command given
extern unsigned char g_uc_gsch_sch_register[MAX_SCHEDULER_ARG+SCHEDULER_NON_ARG_SIZE+2];
extern unsigned short g_us_gsch_total_sending_time;

//Register for GSCH watchdog timer
extern unsigned long g_ul_gsch_time_elapsed_since_last_communication_with_mcc;

//-----------------------------------------------------------------------------
// Datalog
//-----------------------------------------------------------------------------
extern unsigned char g_uc_log_data_buffer[LOG_DATA_BUFFER_SIZE];
extern unsigned char g_uc_log_data_set[LOG_DATA_SET_SIZE];
extern unsigned short g_us_log_data_elapsed_time_s;
extern unsigned short g_us_log_data_buffer_index;
extern unsigned short g_us_log_data_block_size;

//-----------------------------------------------------------------------------
// Eventlog //dec22_yc
//-----------------------------------------------------------------------------
extern unsigned char g_uc_eventlog_data_buffer[EVENTLOG_DATA_BUFFER_SIZE];
extern unsigned short g_us_no_of_eventlog_this_orbit;
extern unsigned char g_uc_eventlog_status_flag;

//-----------------------------------------------------------------------------
// Idle / Mission time variables
//-----------------------------------------------------------------------------
extern unsigned short g_us_mission_time_register;
extern unsigned long g_ul_idle_time_register;

//-----------------------------------------------------------------------------
// Body rate control variables
//-----------------------------------------------------------------------------
extern unsigned char g_uc_desired_adcs_mode;
extern unsigned char g_uc_brh_retry_counter;
extern unsigned char g_uc_brh_timeout_retry_counter;
extern unsigned char g_uc_brh_wait_det_time_s;
extern unsigned long g_ul_ads_uptime_s;
extern unsigned long g_ul_det_uptime_s;
extern short g_s_br_eci[3][BR_ECI_SAMPLE_NO];

//-----------------------------------------------------------------------------
// Idle mode variables
//-----------------------------------------------------------------------------
extern unsigned char g_uc_imh_retry_counter;
extern unsigned char g_uc_imh_acs_timeout_retry_counter;
extern unsigned char g_uc_imh_ads_timeout_retry_counter;
extern unsigned char g_uc_imh_wait_acs_time_s;
extern unsigned char g_uc_imh_wait_ads_time_s;

//-----------------------------------------------------------------------------
// Safe mode variables
//-----------------------------------------------------------------------------
extern unsigned long g_ul_smh_time_register;
extern unsigned long g_ul_smh_saved_time;
extern unsigned char g_uc_soc_reading_when_entering_safe_mode;
extern unsigned char g_uc_soc_after_one_orbit_in_safe_mode;

//-----------------------------------------------------------------------------
// Flags collection
//-----------------------------------------------------------------------------
extern unsigned short g_us_i2c_flag;
extern unsigned char g_uc_spit0ad0_flag;
extern unsigned char g_uc_combn_flag;
extern unsigned char g_uc_strsnd_flag;
extern unsigned char g_uc_gsch_flag;
extern unsigned char g_uc_evha_flag;

//-----------------------------------------------------------------------------
// Update handler's timers
//-----------------------------------------------------------------------------
extern unsigned char g_uc_adcs_time_lapse_from_last_update_s;
extern unsigned char g_uc_pwrs_time_lapse_from_last_update_s;
extern unsigned char g_uc_rtc_time_lapse_from_last_update_s;
extern unsigned char g_uc_pwrs_time_lapse_from_last_ch_stat_update_s;
extern unsigned char g_uc_comm_time_lapse_from_last_update_s;
extern unsigned char g_uc_antenna_status_check_from_last_update_s;
extern unsigned short g_us_obdh_time_lapse_from_last_save_state_s;

//-----------------------------------------------------------------------------
// Thermal knife task's variables
//-----------------------------------------------------------------------------
extern unsigned char g_uc_tk_waiting_s;
extern unsigned char g_uc_tk_uptime_s;

//-----------------------------------------------------------------------------
// Antenna deployment task's variables
//-----------------------------------------------------------------------------
extern unsigned char g_uc_deployment_channel_waiting_s;
extern unsigned char g_uc_antenna_deployment_waiting_s;
extern unsigned char g_uc_antenna_addr;

//-----------------------------------------------------------------------------
// Special block pointers
//-----------------------------------------------------------------------------
extern unsigned long g_ul_essential_bp; //This is a very special block pointer, used to get satellite initialization data from the SD card, it is never been changed unless in a very special circumstance (Genesis)
extern unsigned long g_ul_schedule_bp; 	//This is a very special block pointer, used to get satellite initialization data from the SD card, it is never been changed unless in a very special circumstance (Genesis)
extern unsigned long g_ul_script_bp;	//This is a very special block pointer, used to get satellite initialization data from the SD card, it is never been changed unless in a very special circumstance (Genesis)

#endif
