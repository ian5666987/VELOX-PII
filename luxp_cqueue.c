//------------------------------------------------------------------------------------
// Luxp_cqueue.c
//------------------------------------------------------------------------------------
// Copyright (C) 2013 Nanyang Technological University.
//
// AUTH: IAN
// DATE: 03 APR 2013
//
// The program Luxp_cqueue.c will be downloaded to the Pumpkin Motherboard with 
// C8051F120 uC as part of the VELOX-P Pico-Satellite Flight Software. 
//
//------------------------------------------------------------------------------------

#include <luxp_cqueue.h>

#if (CQUEUE_ENABLE == 1)
//These function will most likely be useless for final executable
//Cqueue is used as an alternative for scheduler.
//------------------------------------------------------------------------------------
// Command queue functions library
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Add command to queue
//------------------------------------------------------------------------------------
// This function will tell whether command entry is successful or not.
//
// Adding a command to the command queue will be considered successful if:
//	1) The command queue is not full and the command length is valid
//	2) It does not have identical command bytes, identical command lengths
//		and identical CRC16 bytes with any existing command in the queue
//
// Upon successful command addition: 
//	1) The command inside the command entry register will be put in the command queue
//		as pointed by command queue size
//	2) Entry command register will be erased
//	3) Command queue size will be increased by 1
//	4) EC_SUCCESSFUL error code will be returned
//
// Upon failed command addition:
//	1) Command entry register will be erased
//	2) EC_COMMAND_QUEUE_FULL or EC_COMMAND_ALREADY_EXIST
//		or EC_INVALID_COMMAND_LENGTH error code will be returned
//------------------------------------------------------------------------------------
unsigned short addCommandToQueue (void)
{
	unsigned short l_us_error_code = EC_INIT;
	unsigned char l_uc_i;
	unsigned char l_uc_j;
	unsigned char l_uc_command_entry_length;
	unsigned char l_uc_existing_command_length;
	unsigned short l_us_command_entry_crc16;
	unsigned short l_us_existing_command_crc16;
	unsigned char l_uc_new_command_is_received = 0;
	unsigned char l_uc_identical_cmd;
	unsigned char l_uc_identical_len;
	unsigned char l_uc_identical_crc16;
	unsigned char l_uc_valid_cmd_len;

	//Count the command length as argument length plus COMMAND_HEADER_SIZE (1 byte header, 2 bytes command, 1 bytes argument length,
	//2 bytes crc16 will be started exactly in the command length index)
	l_uc_command_entry_length = g_uc_command_entry[COMMAND_HEADER_SIZE-1] + COMMAND_HEADER_SIZE;

	//Check for valid cmd len (maximum is 14)
	if (l_uc_command_entry_length <= (COMMAND_LENGTH-2))
		l_uc_valid_cmd_len = 1;
	else l_uc_valid_cmd_len = 0;			

	//If there is a space in the queue & command length is valid & this is not the first time uploading a command
	if(g_uc_command_queue_size < COMMAND_QUEUE_SIZE && l_uc_valid_cmd_len && g_uc_command_queue_size != 0){

		//Initialize command entry crc16 bytes
		l_us_command_entry_crc16 = (unsigned short)(g_uc_command_entry[l_uc_command_entry_length] << 8) + g_uc_command_entry[l_uc_command_entry_length+1];

		//Check if the command in the entry is already existing
		for (l_uc_i = 0; l_uc_i < g_uc_command_queue_size; l_uc_i++){
			//Check for identical cmd 
			if((g_uc_command_entry[1] == g_uc_command_queue[l_uc_i][1]) && (g_uc_command_entry[2] == g_uc_command_queue[l_uc_i][2]))
				l_uc_identical_cmd = 1;
			else l_uc_identical_cmd = 0;

			//Check for identical cmd length
			if (g_uc_command_entry[COMMAND_HEADER_SIZE-1] == g_uc_command_queue[l_uc_i][COMMAND_HEADER_SIZE-1])
				l_uc_identical_len = 1;
			else l_uc_identical_len = 0;

			//If the command length is identical, check crc16 bytes
			if(l_uc_identical_len){
				//Assign existing command length and crc16 bytes
				l_uc_existing_command_length = g_uc_command_queue[l_uc_i][COMMAND_HEADER_SIZE-1] + COMMAND_HEADER_SIZE;
				l_us_existing_command_crc16 = (unsigned short)(g_uc_command_queue[l_uc_i][l_uc_command_entry_length] << 8) + g_uc_command_queue[l_uc_i][l_uc_command_entry_length+1];;

				if (l_us_existing_command_crc16 == l_us_command_entry_crc16)
					l_uc_identical_crc16 = 1;
				else l_uc_identical_crc16 = 0;
			}
			else l_uc_identical_crc16 = 0;

			//If a command has identical cmd, length, and crc16 bytes, reject it, shows the error
			//code as the command already exists
			if(l_uc_identical_cmd && l_uc_identical_len && l_uc_identical_crc16){
				l_us_error_code = EC_COMMAND_ALREADY_EXIST;
				break;
			}

			//If this is the last loop, if the program can go inside this loop,
			//it means the command does not exist
			if (l_uc_i == g_uc_command_queue_size-1){
				//	1) The command inside the command register will be put in the command queue
				//		as pointed by command queue size
				//	2) Entry command register will be erased
				//	3) EC_SUCCESSFUL error code will be returned
				for (l_uc_j = 0; l_uc_j < COMMAND_LENGTH; l_uc_j++){
					g_uc_command_queue[g_uc_command_queue_size][l_uc_j] = g_uc_command_entry[l_uc_j];
					g_uc_command_entry[l_uc_j] = 0;
				}
				l_us_error_code = EC_SUCCESSFUL;
			}
		}

		//If it can go here without error code, it means the command is successfully added
		if (!l_us_error_code) g_uc_command_queue_size++;
	}

	//Else if this is the first time command is added, write the command to the queue directly
	else if (!g_uc_command_queue_size){
		//	1) The command inside the command register will be put in the command queue
		//		as pointed by command queue size
		//	2) Entry command register will be erased
		//	3) EC_SUCCESSFUL error code will be returned
		for (l_uc_j = 0; l_uc_j < COMMAND_LENGTH; l_uc_j++){
			g_uc_command_queue[g_uc_command_queue_size][l_uc_j] = g_uc_command_entry[l_uc_j];
			g_uc_command_entry[l_uc_j] = 0;
		}
		g_uc_command_queue_size++;
		l_us_error_code = EC_SUCCESSFUL;
	}

	//Else, if there is an invalid command length, returns EC_INVALID_COMMAND_LENGTH
	else if (!l_uc_valid_cmd_len)
		l_us_error_code = EC_INVALID_COMMAND_LENGTH;
	
	//If there is no more queue space, returns EC_COMMAND_QUEUE_FULL
	else 
		l_us_error_code = EC_COMMAND_QUEUE_FULL;	

	return l_us_error_code;
}

//------------------------------------------------------------------------------------
// Execute command in queue
//------------------------------------------------------------------------------------
// This function will:
//	1) Take the command as pointed by the command queue pointer to be executed
//	2) Clear the moved command
//	3) Increase command queue pointer by 1
//------------------------------------------------------------------------------------
void exeCommandInQueue (void)
{
	unsigned char l_uc_i;

	//Move the command pointed by command queue pointer to the call task buffer
	for (l_uc_i = 0; l_uc_i < (g_uc_command_queue[g_uc_command_queue_pointer][COMMAND_HEADER_SIZE-1] + COMMAND_HEADER_SIZE); l_uc_i++){
		if (l_uc_i < COMMAND_HEADER_SIZE-1)
			g_uc_call_task_buffer[l_uc_i] = g_uc_command_queue[g_uc_command_queue_pointer][l_uc_i];
		else if (l_uc_i == COMMAND_HEADER_SIZE-1){
			g_uc_call_task_buffer[l_uc_i] = 0;
			g_uc_call_task_buffer[l_uc_i+1] = g_uc_command_queue[g_uc_command_queue_pointer][l_uc_i];
		}
		else g_uc_call_task_buffer[l_uc_i+1] = g_uc_command_queue[g_uc_command_queue_pointer][l_uc_i];
	}

	#if (CRC_ENABLE == 1)
	//Create CRC16 bytes for the call task buffer
	insertCRC16(g_uc_call_task_buffer, l_uc_i+1);
	#endif

	//Clear the moved command
	for (l_uc_i = 0; l_uc_i < COMMAND_LENGTH; l_uc_i++)
		g_uc_command_queue[g_uc_command_queue_pointer][l_uc_i] = 0;

	//Increase command queue pointer
	g_uc_command_queue_pointer++;
}

//------------------------------------------------------------------------------------
// Command queue handler
//------------------------------------------------------------------------------------
// commandQueueHandler function will handle command to be run in this manner:
//	1) It will first check if the command queue pointer is greater or equal to 
//		command queue size
//	2) If it is, reset both value
//	3) If it is not, execute command pointed in the buffer
//------------------------------------------------------------------------------------
void commandQueueHandler (void)
{
	//Check if command queue pointer is greater than or equal to the command queue size.
	if (g_uc_command_queue_pointer >= g_uc_command_queue_size){
		//If it is, reset both to zero
		g_uc_command_queue_pointer = 0;
		g_uc_command_queue_size = 0;
	}
	//If it is not
	else
		//Execute the pointed command
		exeCommandInQueue();
}
#endif
