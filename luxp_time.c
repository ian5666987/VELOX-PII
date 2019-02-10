//------------------------------------------------------------------------------------
// Luxp_time.c
//------------------------------------------------------------------------------------
// Copyright (C) 2013 Nanyang Technological University.
//
// AUTH: IAN
// DATE: 03 APR 2013
//
// The program Luxp_time.c will be downloaded to the Pumpkin Motherboard with 
// C8051F120 uC as part of the VELOX-P Pico-Satellite Flight Software. 
//
// This file contains all basic time functions of the Flight Software.
//
//------------------------------------------------------------------------------------
// Overall functional test results
//------------------------------------------------------------------------------------
// Fully functional with warning(s)
//
//------------------------------------------------------------------------------------
// Overall documentation status
//------------------------------------------------------------------------------------
// Averagely documented
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// List of functions				| Test result					| Documentation
//------------------------------------------------------------------------------------
// waitMs							| Tested, fully functional (w)	| Average
// waitS							| Tested, fully functional		| Mininum
// countNoOfDays					| Tested, fully functional		| Average
// getHMSInfo						| Tested, fully functional		| Average
// checkEndDayOfTheMonth			| Tested, fully functional		| Average
// updateSatDate					| Tested, fully functional		| Mininum
// getSatTimeFromRTC				| Tested, fully functional		| Average
// convertSatTimeFromRTCToSec		| Tested, fully functional		| Minimum
// getSatTimeFromRTCInSec			| Tested, fully functional		| Minimum
// recordSatelliteTime				| Tested, fully functional		| Mininum
// fastRecordTimerValue				| Tested, fully functional		| Average
//------------------------------------------------------------------------------------

#include <luxp_time.h>

//------------------------------------------------------------------------------------
// Wait function using Timer0 in microsecond
//------------------------------------------------------------------------------------
// Function's test result: fully functional, with an indefinite while loop
//------------------------------------------------------------------------------------
void waitUs (unsigned char l_uc_us)
{
	char SFRPAGE_SAVE = SFRPAGE;        //Save Current SFR page
	unsigned long l_ul_val;
	
	l_ul_val = 0x00010000-((SYSCLK/1000000)*l_uc_us);

	SFRPAGE = TIMER01_PAGE;

	TCON 	&= ~0x30;			//Stop Timer0; Clear TF0
	TMOD 	&= ~0x0f;			//16-bit free run mode
	TMOD 	|=  0x01;
	
	CKCON 	|= 0x08;			//Timer0 counts SYSCLKs
								//Not overlapping with Timer1
								//which uses pre-scaled SYSCLK

	//!!----Warning----!!
	//an indefinite while loop	
	//With PLL
	g_uc_spit0ad0_flag |= TIMER0_FLAG_WAIT;
	TR0 = 0;                					//Stop Timer0
	TH0 = l_ul_val >> 8;						//Overflow in 1us
	TL0 = l_ul_val;
	TF0 = 0;									//Clear overflow indicator
	TR0 = 1; 									//Start Timer0
	while (!TF0 && !(g_uc_spit0ad0_flag & TIMER0_FLAG_WAIT_TIMEOUT));	//Wait for overflow
	g_uc_spit0ad0_flag &= ~TIMER0_FLAG_WAIT_TIMEOUT;
	g_uc_spit0ad0_flag &= ~TIMER0_FLAG_WAIT;
	
	TR0 	= 0;						//Stop Timer0
	
	SFRPAGE = SFRPAGE_SAVE;				//Restore SFR page detector
}

//------------------------------------------------------------------------------------
// Wait function using Timer0 in tenth milisecond
//------------------------------------------------------------------------------------
// Function's test result: fully functional, with an indefinite while loop
//------------------------------------------------------------------------------------
void waitTenthMs (unsigned char l_uc_a_tenth_ms)
{
	char SFRPAGE_SAVE = SFRPAGE;        //Save Current SFR page
	unsigned long l_ul_val;

	//The value 100 is a constant for the other codes processing, obtain from experiment
	l_ul_val = 0x00010000-(SYSCLK/10000-100);
	
	SFRPAGE = TIMER01_PAGE;

	TCON 	&= ~0x30;					//Stop Timer0; Clear TF0
	TMOD 	&= ~0x0f;					//16-bit free run mode
	TMOD 	|=  0x01;
	
	CKCON 	|= 0x08;					//Timer0 counts SYSCLKs
										//Not overlapping with Timer1
										//which uses pre-scaled SYSCLK

	//!!----Warning----!!
	//an indefinite while loop	
	//With PLL
	while (l_uc_a_tenth_ms) {
		g_uc_spit0ad0_flag |= TIMER0_FLAG_WAIT;
		TR0 = 0;                        			//Stop Timer0
		TH0 = l_ul_val >> 8;						//Overflow in 0.1ms
		TL0 = l_ul_val;
		TF0 = 0;									//Clear overflow indicator
		TR0 = 1; 									//Start Timer0
		while (!TF0 && !(g_uc_spit0ad0_flag & TIMER0_FLAG_WAIT_TIMEOUT));	//Wait for overflow
		g_uc_spit0ad0_flag &= ~TIMER0_FLAG_WAIT_TIMEOUT;
		g_uc_spit0ad0_flag &= ~TIMER0_FLAG_WAIT;
		g_uc_timer0_stuck_counter = 0;
		l_uc_a_tenth_ms--;							//Update ms counter
	}
	
	TR0 	= 0;						//Stop Timer0
	
	SFRPAGE = SFRPAGE_SAVE;				//Restore SFR page detector
}

//------------------------------------------------------------------------------------
// Wait function using Timer0 in milisecond
//------------------------------------------------------------------------------------
// Function's test result: fully functional, with an indefinite while loop
//------------------------------------------------------------------------------------
void waitMs (unsigned char l_uc_milisecond)
{
	while (l_uc_milisecond){
		waitTenthMs(10);
		l_uc_milisecond--;
	}
}

//------------------------------------------------------------------------------------
// Wait function using Timer0 in second
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
void waitS (unsigned char l_uc_s)
{
	while(l_uc_s){
		waitMs(250);
		waitMs(250);
		waitMs(250);
		waitMs(249);
		l_uc_s--;
	}
}

#if (TIME_ENABLE == 1)
#if (RTC_CMD_ENABLE == 1)
#if (UPDATE_HANDLER_ENABLE == 1)
//------------------------------------------------------------------------------------
// Calculate number of days between two dates
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
unsigned short countNoOfDays (void)
{
	unsigned short 	l_us_no_of_days 			= 0x0000;
	unsigned char 	l_uc_temp_year 				= 0x00;
	unsigned char 	l_uc_temp_month 			= 0x00;
	unsigned char 	l_uc_last_day_to_count 		= 0x00;
	unsigned char 	l_uc_initial_day_to_count 	= 0x00;
	unsigned char 	l_uc_last_month_to_count 	= 0x00;
	unsigned char 	l_uc_initial_month_to_count = 0x00;
	
	//Initialize l_uc_temp_year value to be the same as satellite initial year
	//Redundant assignment is removed
	//l_uc_temp_year = str_sat_date.uc_init_year;			
		
	//so long as the satellite's current year is greater or equal to l_uc_temp_year variable value
	for (l_uc_temp_year = str_sat_date.uc_init_year; l_uc_temp_year <= str_sat_date.uc_current_year; l_uc_temp_year++) 
	{
		//Calculate initial month to count
		if (l_uc_temp_year == str_sat_date.uc_init_year)				//If we are currently counting the days passed for the very first year
			l_uc_initial_month_to_count = str_sat_date.uc_init_month;	//The initial month taken for counting is the same as satellite's initial month
		else																//Otherwise
			l_uc_initial_month_to_count = 1;								//The initial month taken for counting is assigned as one

		//Calculate last month to count
		if (l_uc_temp_year == str_sat_date.uc_current_year)			//If we are currently counting the days passed for the very last year
			l_uc_last_month_to_count = str_sat_date.uc_current_month;	//The last month taken for counting is the same as satellite's current month
		else																//Otherwise
			l_uc_last_month_to_count = 0x0C;								//The last month taken is assigned as twelve

		l_uc_temp_month = l_uc_initial_month_to_count;						//Assign initial month taken for calculation to the l_uc_temp_month variable

		//so long as the satellite's last month to count is greater than l_uc_temp_month variable value
		for (l_uc_temp_month = l_uc_initial_month_to_count; l_uc_temp_month <= l_uc_last_month_to_count; l_uc_temp_month++)
		{ 				
			//Calculate initial day to count
			if ((l_uc_temp_month == str_sat_date.uc_init_month) && (l_uc_temp_year == str_sat_date.uc_init_year))	//If we are currently counting the days passed for the very first month
				l_uc_initial_day_to_count = str_sat_date.uc_init_day_of_month;	//The initial day taken for counting is the same as satellite's initial day
			else 																		//Otherwise
				l_uc_initial_day_to_count = 0x01;										//The initial day taken for counting is assigned as one
			
			//Calculate last day to count
			if ((l_uc_temp_month == str_sat_date.uc_current_month) && (l_uc_temp_year == str_sat_date.uc_current_year))	//If we are currently counting the days passed for the very last month
				l_uc_last_day_to_count = str_sat_date.uc_current_day_of_month;	//The last day taken for counting is the same as satellite's current day				
			else																		//Otherwise
				l_uc_last_day_to_count = checkEndDayOfTheMonth(l_uc_temp_month,l_uc_temp_year);	//Check the number of the days for that particular month
			
			l_us_no_of_days += (l_uc_last_day_to_count-l_uc_initial_day_to_count+1);	//Increase the number of the days counted
		}
	}

	l_us_no_of_days--;

	return l_us_no_of_days;
}

//------------------------------------------------------------------------------------
// Get Hour, Minute, and Seconds info
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
// It is noted that the way to get Hour, minute, second info from this function is
// not so much useful near the final implementation, since it is only used once.
unsigned long getHMSInfo (void)
{
	char 			l_c_no_of_second 			= 0x00;
	char 			l_c_no_of_minute 			= 0x00;
	char 			l_c_no_of_hour 				= 0x00;
	char 			l_c_initial_no_of_second 	= 0x00;
	char 			l_c_current_no_of_second 	= 0x00;
	char 			l_c_initial_no_of_minute 	= 0x00;
	char 			l_c_current_no_of_minute 	= 0x00;
	char 			l_c_initial_no_of_hour 		= 0x00;
	char 			l_c_current_no_of_hour 		= 0x00;
	char 			l_c_minus_a_minute 			= 0x00;
	char 			l_c_minus_an_hour 			= 0x00;
	char 			l_c_minus_a_day 			= 0x00;
	unsigned long 	l_ul_temp_hms_info 			= 0x00000000;
	unsigned long 	l_ul_hms_info 				= 0x00000000;

	//Assign second info
	l_c_initial_no_of_second = str_sat_date.uc_init_s;
	l_c_current_no_of_second = str_sat_date.uc_current_s;

	if (l_c_current_no_of_second >= l_c_initial_no_of_second)								//If currentNoOfSeconds is greater or equal to initialNoOfSeconds
		l_c_no_of_second 	= 		l_c_current_no_of_second - l_c_initial_no_of_second;	//Assign noOfSeconds value as currentNoOfSeconds - initialNoOfSeconds
	else {																					//Otherwise
		l_c_no_of_second 	= 60 + 	l_c_current_no_of_second - l_c_initial_no_of_second;	//Adds 60 to the calculation
		l_c_minus_a_minute 	= 0x01;															//Indicates tha minus a minute operation would be needed later
	}

	l_ul_hms_info 	= l_c_no_of_second;														//Assign l_c_no_of_second value to the last two bytes of l_ul_hms_info
	l_ul_hms_info 	&= 0x000000FF;															//Ensures that only the last two bytes of the l_ul_hms_info is now occupied by seconds info

	//Assign minute info
	l_c_initial_no_of_minute = str_sat_date.uc_init_min;
	l_c_current_no_of_minute = str_sat_date.uc_current_min - l_c_minus_a_minute;

	if (l_c_current_no_of_minute >= l_c_initial_no_of_minute)								//If l_c_current_no_of_minute is greater or equal to l_c_initial_no_of_minute
		l_c_no_of_minute 	= 		l_c_current_no_of_minute - l_c_initial_no_of_minute;	//Assign l_c_no_of_minute value as l_c_current_no_of_minute - l_c_initial_no_of_minute
	else {																					//Otherwise
		l_c_no_of_minute 	= 60 + 	l_c_current_no_of_minute - l_c_initial_no_of_minute;	//Adds 60 to the calculation
		l_c_minus_an_hour 	= 1;															//Indicates tha minus an hour operation would be needed later
	}

	l_ul_temp_hms_info 	= 	l_c_no_of_minute;					//Assign l_c_no_of_minute value to the last two bytes of l_ul_temp_hms_info
	l_ul_temp_hms_info 	= 	l_ul_temp_hms_info << 8;			//Shift the value by 8 bits to the left
	l_ul_temp_hms_info 	&= 	0x0000FF00;							//clear all info but minutes info
	l_ul_hms_info 		= 	l_ul_hms_info | l_ul_temp_hms_info;	//Insert minutes info to l_ul_hms_info variable
	l_ul_temp_hms_info 	&= 	0x00000000;							//clear l_ul_temp_hms_info variable

	//Assign hour info
	l_c_initial_no_of_hour = str_sat_date.uc_init_hour;
	l_c_current_no_of_hour = str_sat_date.uc_current_hour - l_c_minus_an_hour;

	if (l_c_current_no_of_hour >= l_c_initial_no_of_hour)							//If l_c_current_no_of_hour is greater or equal to l_c_initial_no_of_hour
		l_c_no_of_hour 	= 		l_c_current_no_of_hour - l_c_initial_no_of_hour;	//Assign l_c_no_of_hour value as l_c_current_no_of_hour - l_c_initial_no_of_hour
	else {																			//Otherwise
		l_c_no_of_hour 	= 24 + 	l_c_current_no_of_hour - l_c_initial_no_of_hour;	//Adds 24 to the calculation
		l_c_minus_a_day = 1;														//Indicates tha minus a day operation would be needed later
	}

	l_ul_temp_hms_info 	= 	l_c_no_of_hour;						//Assign l_c_no_of_hour value to the last two bytes of l_ul_temp_hms_info
	l_ul_temp_hms_info 	= 	l_ul_temp_hms_info << 16;			//Shift the value by 16 bits to the left
	l_ul_temp_hms_info 	&= 	0x00FF0000;							//clear all info but hours info
	l_ul_hms_info 		= 	l_ul_hms_info | l_ul_temp_hms_info;	//Insert hours info to l_ul_hms_info variable

	if (l_c_minus_a_day)	l_ul_hms_info |= 0x01000000;		//If needed, insert minus one day information in the l_ul_hms_info value

	return l_ul_hms_info;
}
#endif

//------------------------------------------------------------------------------------
// To assign last day of the month
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
// In OBDH software, there is only one checkEndDayOfTheMonth(). The adcs/real_time.h
// which contains this function is used for ADCS software.
unsigned char checkEndDayOfTheMonth (unsigned char l_uc_month_to_check, unsigned char l_uc_year_to_check)
{
	unsigned char 	l_uc_end_day_of_the_month;
	bit 			l_b_leap_year;

	//If the last two bits of the year to check has value or the year to check is 0x00 (year 2000), it is not a leap year
	if ((l_uc_year_to_check & 0x03)	|| (l_uc_year_to_check == 0x00)) 
		l_b_leap_year = 0;
	//If the last two bits of the year to check has no value, it is a leap year	
	else l_b_leap_year = 1;

	//For month: Feb
	if (l_uc_month_to_check == 0x02)
	{
		if (!l_b_leap_year)						//If it is not a leap year
			l_uc_end_day_of_the_month = 0x1C; 	//(28 days)
		else 									//If it is a leap year
			l_uc_end_day_of_the_month = 0x1D; 	//(29 days)
	}
	//For months: Apr, Jun, Sep, Nov
	else if (l_uc_month_to_check == 0x04 || l_uc_month_to_check == 0x06 || l_uc_month_to_check == 0x09 || l_uc_month_to_check == 0x0B)
		l_uc_end_day_of_the_month = 0x1E; 		//(30 days)
	//For other months: Jan, Mar, May, Jul, Aug, Oct, Dec
	else
		l_uc_end_day_of_the_month = 0x1F; 		//(31 days)

	return l_uc_end_day_of_the_month;
}

#if (UPDATE_HANDLER_ENABLE == 1)
//------------------------------------------------------------------------------------
// To update and store current satellite date
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
void updateSatDate (void)
{
	str_sat_date.us_current_ms 				= (unsigned short)(twoDec2oneHex(str_rtc_data.uc_ths) * 10);
	str_sat_date.uc_current_s 				= twoDec2oneHex(str_rtc_data.uc_s);
	str_sat_date.uc_current_min 			= twoDec2oneHex(str_rtc_data.uc_min);
	str_sat_date.uc_current_hour 			= twoDec2oneHex(str_rtc_data.uc_cenh);
	str_sat_date.uc_current_day_of_week 	= twoDec2oneHex(str_rtc_data.uc_day);
	str_sat_date.uc_current_day_of_month	= twoDec2oneHex(str_rtc_data.uc_date);
	str_sat_date.uc_current_month 			= twoDec2oneHex(str_rtc_data.uc_month);
	str_sat_date.uc_current_year			= twoDec2oneHex(str_rtc_data.uc_year);
}
#endif

#if (UPDATE_HANDLER_ENABLE == 1)
//------------------------------------------------------------------------------------
// To retrieve satellite's time
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
unsigned short getSatTimeFromRTC (void)
{
	unsigned short l_us_error_code;
	unsigned short l_us_rtc_error_code;
	unsigned short l_us_no_of_days;
	unsigned long l_ul_hms_info;

	#if (DEBUGGING_FEATURE == 1)
	l_us_rtc_error_code = rtcGetTime(0);			//Get latest date from RTC
	#else
	l_us_rtc_error_code = rtcGetTime();			//Get latest date from RTC
	#endif

	if (!l_us_rtc_error_code)
		updateSatDate();		//Update current satellite days
	else return EC_RTC_GET_TIME_FAIL;

	l_us_no_of_days 	= countNoOfDays();	//Get no of day elapsed since first initialized
	l_ul_hms_info 		= getHMSInfo();		//Get hour, minute, and seconds info

	if (l_ul_hms_info & 0x01000000)	l_us_no_of_days--;	//Adjust no of day elapsed if HMS info contain 
														//minus one day information

	//Set satellite's current time
	str_sat_time.us_ms 		= str_sat_date.us_current_ms;
	str_sat_time.uc_s 		= l_ul_hms_info;
	str_sat_time.uc_min 	= l_ul_hms_info >> 8; 
	str_sat_time.uc_hour 	= l_ul_hms_info >> 16;
	str_sat_time.us_day 	= l_us_no_of_days;

	//Set function's error code
	l_us_error_code = EC_SUCCESSFUL;

	//Limit check, update function's l_us_error_code if there is any time limit violated
	//The superfluous check are removed
	if (str_sat_time.us_ms > 990)
		l_us_error_code = EC_SATELLITE_TIME_MILISECOND_ERROR;
	if (str_sat_time.uc_s > 59)
		l_us_error_code = EC_SATELLITE_TIME_SECOND_ERROR;
	if (str_sat_time.uc_min > 59)
		l_us_error_code = EC_SATELLITE_TIME_MINUTE_ERROR;
	if (str_sat_time.uc_hour > 23)
		l_us_error_code = EC_SATELLITE_TIME_HOUR_ERROR;
	if (str_sat_time.us_day > 7300)
		l_us_error_code = EC_SATELLITE_TIME_DAY_ERROR;

	return l_us_error_code;
}

//------------------------------------------------------------------------------------
// To convert retrieved satellite's time to no of seconds
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
unsigned long convertSatTimeFromRTCToSec (void)
{
	unsigned long l_ul_satellite_second = 0x00000000;
	// The proposed lines:
	//
	// l_ul_satellite_second=str_sat_time.us_day;
	// l_ul_satellite_second*=24;
	// l_ul_satellite_second+=str_sat_time.uc_hour;
	// l_ul_satellite_second*=60;
	// l_ul_satellite_second+=str_sat_time.uc_min;
	// l_ul_satellite_second*=60;
	// l_ul_satellite_second+=str_sat_time.uc_s;
	//
	// are not used because it was found to be using more code size than the
	// current line. Since the uC used is 16-bit, long and float data type operations
	// would introduce more assembly code instructions than short or char.
	l_ul_satellite_second = (unsigned long)(((unsigned long)(((unsigned long)(str_sat_time.us_day*24) + (unsigned long)(str_sat_time.uc_hour))*60) + (unsigned long)(str_sat_time.uc_min))*60) + (unsigned long)(str_sat_time.uc_s);
	return l_ul_satellite_second;
}

//------------------------------------------------------------------------------------
// To get satellite's time in no of seconds
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
unsigned short getSatTimeFromRTCInSec (void)
{
	unsigned short l_us_error_code = EC_INIT;

	l_us_error_code = getSatTimeFromRTC();
	if (!l_us_error_code)
		str_obdh_data.ul_rtc_ref_time_s = convertSatTimeFromRTCToSec();

	return l_us_error_code;
}
#endif
#endif

//------------------------------------------------------------------------------------
// To get satellite's time and orbit for HK purpose
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
void getHKTimeAndOrbit (void)
{
	//Get satellite time
	str_obdh_data.ul_latest_hk_sampling_time_s = str_obdh_data.ul_obc_time_s;

	//Get satellite orbit
	str_obdh_data.us_latest_hk_sampling_orbit = str_obdh_data.us_current_rev_no;
}

#if (RTC_CMD_ENABLE == 1)
//------------------------------------------------------------------------------------
// To convert second info to date update info
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
void sec2DateUpdateFromRef (unsigned long l_ul_input_s)
{
    unsigned char l_uc_s_remainder;
    unsigned char l_uc_min_remainder;
    unsigned char l_uc_hour_remainder;
    unsigned short l_us_no_of_days;
    unsigned long l_ul_temp_s;

    unsigned char l_uc_this_second;    
    unsigned char l_uc_this_min;    
    unsigned char l_uc_this_hour;    
    unsigned char l_uc_this_year;
    unsigned char l_uc_this_month;
    unsigned char l_uc_this_day_of_month;
    unsigned char l_uc_end_day_of_this_month;
    
    //Find the excessive seconds
    l_uc_s_remainder = l_ul_input_s % 60;

    //Find the excessive minutes
    l_ul_temp_s = l_ul_input_s/60;
    l_uc_min_remainder = l_ul_temp_s % 60;

    //Find the excessive hours
    l_ul_temp_s = l_ul_input_s/3600;
    l_uc_hour_remainder = l_ul_temp_s % 24;

    //Find the excessive days
    l_us_no_of_days = l_ul_input_s/86400;
    
    //Handle carriers
    l_uc_this_second = l_uc_s_remainder + str_sat_date.uc_init_s;
    l_uc_this_min = l_uc_min_remainder + str_sat_date.uc_init_min;
    l_uc_this_hour = l_uc_hour_remainder + str_sat_date.uc_init_hour;
    
    if (l_uc_this_second >= 60){
        l_uc_this_second -= 60;
        l_uc_this_min++;
    }
    
    if (l_uc_this_min >=60){
        l_uc_this_min -=60;
        l_uc_this_hour++;
    }
    
    if (l_uc_this_hour >= 24){
        l_uc_this_hour -=24;
        l_us_no_of_days++;
    }
    
    //Find at first, this day of month, this month and this year is the same as
    //reference value
    l_uc_this_year = str_sat_date.uc_init_year;
    l_uc_this_month = str_sat_date.uc_init_month;
    l_uc_this_day_of_month = str_sat_date.uc_init_day_of_month;
    
    //While there is an uncounted day, keep this loop
    while (l_us_no_of_days){
        //Get end of the month value for this month and this year
        l_uc_end_day_of_this_month = checkEndDayOfTheMonth(l_uc_this_month,l_uc_this_year);
        
        //If end day of the month - this day of the month + 1
        if (l_us_no_of_days >= (l_uc_end_day_of_this_month-l_uc_this_day_of_month+1)){
            l_us_no_of_days -= (l_uc_end_day_of_this_month-l_uc_this_day_of_month+1);
            l_uc_this_month++;
            if (l_uc_this_month > 12){
                l_uc_this_month = 1;
                l_uc_this_year++;
            }
            l_uc_this_day_of_month = 1;
        }
        else{
            l_uc_this_day_of_month += l_us_no_of_days;
            l_us_no_of_days = 0;
        }
    }

	str_date_update.us_ms = 0;
	str_date_update.uc_s = l_uc_this_second;
	str_date_update.uc_min = l_uc_this_min;
	str_date_update.uc_hour = l_uc_this_hour;
	str_date_update.uc_day = l_uc_this_day_of_month;
	str_date_update.uc_month = l_uc_this_month;
	str_date_update.uc_year = l_uc_this_year;
}
#endif
#endif

#if (DEBUGGING_FEATURE == 1)
//------------------------------------------------------------------------------------
// To measure uC's timer value at one moment, for time measurement (faster mode)
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
void fastRecordTimerValue (unsigned char l_uc_record_block)
{
	char SFRPAGE_SAVE = SFRPAGE;        //Save Current SFR page
	unsigned char l_uc_temp_timer_value;

	SFRPAGE = TMR4_PAGE;

	//To record timer value
	//Ian (2012-05-15): the overflow occurs when TMR4 (TMR4H and TMR4L) value change from
	//					0xffff to 0x0000
	g_uc_timer_value_low[l_uc_record_block] 			= TMR4L;
	g_uc_timer_value_high[l_uc_record_block] 			= TMR4H;
	g_us_timer_flag_counter_value[l_uc_record_block] 	= g_us_timer_flag_counter;

	//To correct error in recording extreme high timer value
	l_uc_temp_timer_value									= TMR4L;	
	if (l_uc_temp_timer_value < g_uc_timer_value_low[l_uc_record_block]){
		g_uc_timer_value_low[l_uc_record_block] 			= l_uc_temp_timer_value;
		g_uc_timer_value_high[l_uc_record_block]			= TMR4H;
		g_us_timer_flag_counter_value[l_uc_record_block] 	= g_us_timer_flag_counter;
	}

	SFRPAGE = SFRPAGE_SAVE;             //Restore SFR page detector
}

//------------------------------------------------------------------------------------
// Common sequence to start timing
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
void startTiming (void)
{
	g_uc_combn_flag |= STMR_FLAG_COUNT_START;
	fastRecordTimerValue(0);
}

//------------------------------------------------------------------------------------
// Common sequence to end timing
//------------------------------------------------------------------------------------
// Function's test result: fully functional
//------------------------------------------------------------------------------------
void endTiming (void)
{
	fastRecordTimerValue(1);
	g_uc_combn_flag &= ~STMR_FLAG_COUNT_START;
}

#endif
