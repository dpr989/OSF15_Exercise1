#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "command.h"

#define MAX_CMD_COUNT 50
#define MAX_CMD_LEN 25

 //TODO FUNCTION COMMENT
/* 
 * PURPOSE: Breaks command input string into pieces and stores the individual commands
 * 		    into an instance Commands_t in the array of strings contained within.		
 * INPUTS: 
 * 		   input : string from user of commands wanting to execute.
 * 		   cmd : list of command pointers
 * RETURN: True if input properly parsed and stored in instance of cmd
 **/	
bool parse_user_input (const char* input, Commands_t** cmd) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS

	char *string = strdup(input);
	
	*cmd = calloc(1,sizeof(Commands_t));
	(*cmd)->cmds = calloc(MAX_CMD_COUNT,sizeof(char*));

	unsigned int i = 0;
	char *token;
	token = strtok(string, " \n");
	for (; token != NULL && i < MAX_CMD_COUNT; ++i) {
		(*cmd)->cmds[i] = calloc(MAX_CMD_LEN,sizeof(char));
		if (!(*cmd)->cmds[i]) {
			perror("Allocation Error\n");
			return false;
		}	
		strncpy((*cmd)->cmds[i],token, strlen(token) + 1);
		(*cmd)->num_cmds++;
		token = strtok(NULL, " \n");
	}
	free(string);
	return true;
}

	//TODO FUNCTION COMMENT
	/* 
	 * PURPOSE: Unallocate memory dedicated to commands.
	 * INPUTS: 
	 * 		   cmd : array of command_t pointers
	 * RETURN: void
	 **/
void destroy_commands(Commands_t** cmd) {

	//TODO ERROR CHECK INCOMING PARAMETERS
	
	for (int i = 0; i < (*cmd)->num_cmds; ++i) {
		free((*cmd)->cmds[i]);
	}
	free((*cmd)->cmds);
	free((*cmd));
	*cmd = NULL;
}

