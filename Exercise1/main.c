#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#include<readline/readline.h>

#include "command.h"
#include "matrix.h"

void run_commands (Commands_t* cmd, Matrix_t** mats, unsigned int num_mats);
unsigned int find_matrix_given_name (Matrix_t** mats, unsigned int num_mats, 
			const char* target);

//TODO complete the defintion of this function. 
void destroy_remaining_heap_allocations(Matrix_t **mats, unsigned int num_mats);

/* 
 * PURPOSE: The main function that ties everything together and processes user input commands and calls
 *  		the appropriate functions based on said input.
 * 
 * INPUTS: 
 *	      argc : is the number of commands entered when starting the program matlab
 * 		  argv : list of commands used to start the program
 * 
 * RETURN: If no errors during execution output = 0 if errors exist some other error value will be returned.
 *
 **/
 
int main (int argc, char **argv) {
	
	srand(time(NULL));  		
	char *line = NULL; 
	Commands_t* cmd;
	Matrix_t *mats[10];

	memset(&mats,0, sizeof(Matrix_t*) * 10); //IMPORTANT C FUNCTION TO LEARN
	Matrix_t *temp = NULL;
	
	//Check if creation of 'temp_mat' was successful. Notify
	if(!create_matrix(&temp,"temp_mat", 5, 5)) {
		perror("The creation of temp_mat failed\n");
		return -1;
	}else{
		printf("\nMatrix %s was successfully created!\n",temp->name);
	}
	
	//Check if addtion of temp_mat to the array was successful.
	unsigned int pos = add_matrix_to_array(mats,temp, 10);
	if(pos > 10){
		perror("Addition to array of matrices failed\n");
		return -1;
	}
	else{
		printf("\n%s was added to the array of matrices successfully!",temp->name);
	}

	int mat_idx = find_matrix_given_name(mats,10,"temp_mat");

	if (mat_idx < 0) {
		perror("PROGRAM FAILED TO INIT\n");
		return -1;
	}
	
	random_matrix(mats[mat_idx], 10, 15);
	
	//Check if matrix written to file.
	if(!write_matrix("temp_mat", mats[mat_idx])){ 
		perror("Writing temp_mat to a file failed\n");
		return -1;
	}else{
		printf("\ntemp_mat was successfully written to a file!\n");
	}
	
	line = readline("> ");
	while(strncmp(line,"exit", strlen("exit")  + 1) != 0) {
		
		if (!parse_user_input(line,&cmd)) {
			printf("\nERROR:Failed at parsing command\n");
		}
		
		if (cmd->num_cmds > 1) {	
			run_commands(cmd,mats,10);
		}
		if (line) {
			free(line);
		}
		destroy_commands(&cmd);
		line = readline("> ");
	}
	free(line);
	destroy_remaining_heap_allocations(mats,10);
	return 0;	
}

/* 
 * PURPOSE: Parses the user input, takes the necessary steps to execute the user's
 * 			commands.
 * INPUTS: 
 * 		   cmd : list of commands
 * 	       mats : list of matrices
 * 	       num_mats : is the number of matrices in the list.
 * RETURN: void
 **/
 
void run_commands (Commands_t* cmd, Matrix_t** mats, unsigned int num_mats) {
	//Check if cmd or first matrix (mat[0])is NULL
	if(!cmd){
		perror("\nCommand list is empty\n");
		return;
	}
	if(!(*mats)){
		perror("\nThere are no matrices in the array");
	}

	/*Parsing and calling of commands*/
	if (strncmp(cmd->cmds[0],"display",strlen("display") + 1) == 0
		&& cmd->num_cmds == 2) {
			/*find the requested matrix*/
			int idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
			if (idx >= 0) {
				display_matrix (mats[idx]);
			}
			else {
				printf("Matrix (%s) doesn't exist\n", cmd->cmds[1]);
				return;
			}
	}
	else if (strncmp(cmd->cmds[0],"add",strlen("add") + 1) == 0
		&& cmd->num_cmds == 4) {
			int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
			int mat2_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[2]);
			if (mat1_idx >= 0 && mat2_idx >= 0) {
				Matrix_t* c = NULL;
				if( !create_matrix (&c,cmd->cmds[3], mats[mat1_idx]->rows, 
						mats[mat1_idx]->cols)) {
					printf("Failure to create the result Matrix (%s)\n", cmd->cmds[3]);
					return;
				}
				
				//Check if add mat to array failed.
				int pos = add_matrix_to_array(mats,c, num_mats);
				printf("\npos = %d\n",pos);
				if(pos > num_mats){
					perror("matrix failed to be added to the array");
					return;
				}


				if (! add_matrices(mats[mat1_idx], mats[mat2_idx],c) ) {
					printf("Failure to add %s with %s into %s\n", mats[mat1_idx]->name, mats[mat2_idx]->name,c->name);
					return;	
				}
			}
	}
	else if (strncmp(cmd->cmds[0],"duplicate",strlen("duplicate") + 1) == 0
		&& cmd->num_cmds == 3 && strlen(cmd->cmds[1]) + 1 <= MATRIX_NAME_LEN) {
		int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
		if (mat1_idx >= 0 ) {
				Matrix_t* dup_mat = NULL;
				if( !create_matrix (&dup_mat,cmd->cmds[2], mats[mat1_idx]->rows, 
						mats[mat1_idx]->cols)) {
					return;
				}
				if(!duplicate_matrix(mats[mat1_idx], dup_mat)){
					perror("Matrix duplication failed\n");
					return;
				}else{
					if(add_matrix_to_array(mats,dup_mat,num_mats)){
						printf ("Duplication of %s into %s finished\n", mats[mat1_idx]->name, cmd->cmds[2]);
					}
				}
		}
		else {
			printf("Duplication Failed\n");
			return;
		}
	}
	else if (strncmp(cmd->cmds[0],"equal",strlen("equal") + 1) == 0
		&& cmd->num_cmds == 2) {
			int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
			int mat2_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[2]);
			if (mat1_idx >= 0 && mat2_idx >= 0) {
				if ( equal_matrices(mats[mat1_idx],mats[mat2_idx]) ) {
					printf("SAME DATA IN BOTH\n");
				}
				else {
					printf("DIFFERENT DATA IN BOTH\n");
				}
			}
			else {
				printf("Equal Failed\n");
				return;
			}
	}
	else if (strncmp(cmd->cmds[0],"shift",strlen("shift") + 1) == 0
		&& cmd->num_cmds == 4) {
		int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
		const int shift_value = atoi(cmd->cmds[3]);
			if (mat1_idx >= 0 ) {
				if(bitwise_shift_matrix(mats[mat1_idx],cmd->cmds[2][0], shift_value)){
				printf("Matrix (%s) has been shifted by %d\n", mats[mat1_idx]->name, shift_value);
			}else{
				perror("Bit shift failed.\n");
				return;
			}
		}
		else {
			printf("Matrix shift failed\n");
			return;
		}

	}
	else if (strncmp(cmd->cmds[0],"read",strlen("read") + 1) == 0
		&& cmd->num_cmds == 2) {
		Matrix_t* new_matrix = NULL;
		if(! read_matrix(cmd->cmds[1],&new_matrix)) {
			printf("Read Failed\n");
			return;
		}	
		
		int pos1 = add_matrix_to_array(mats,new_matrix, num_mats);
		if(pos1 > num_mats){
			printf("\nMatrix %s failed to be added to the array.\n",new_matrix->name);
		}
		printf("Matrix (%s) is read from the filesystem\n", cmd->cmds[1]);	
	}
	else if (strncmp(cmd->cmds[0],"write",strlen("write") + 1) == 0
		&& cmd->num_cmds == 2) {
		int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
		if(! write_matrix(mats[mat1_idx]->name,mats[mat1_idx])) {
			printf("Write Failed\n");
			return;
		}
		else {
			printf("Matrix (%s) is wrote out to the filesystem\n", mats[mat1_idx]->name);
		}
	}
	else if (strncmp(cmd->cmds[0], "create", strlen("create") + 1) == 0
		&& strlen(cmd->cmds[1]) + 1 <= MATRIX_NAME_LEN && cmd->num_cmds == 4) {
		Matrix_t* new_mat = NULL;
		const unsigned int rows = atoi(cmd->cmds[2]);
		const unsigned int cols = atoi(cmd->cmds[3]);
		
		//Check if creation failed
		if(!create_matrix(&new_mat,cmd->cmds[1],rows, cols)){
			printf("\nCreation of matrix %s failed.\n",cmd->cmds[2]);
			return;
		}
		//Check if matrix was added to array.
		int pos2 = add_matrix_to_array(mats,new_mat,num_mats);
		if(pos2>num_mats){ //means add failed
			printf("\nMatrix %s failed to be added to array",new_mat->name);
			return;
		}
		printf("Created Matrix (%s,%u,%u)\n", new_mat->name, new_mat->rows, new_mat->cols);
	}
	else if (strncmp(cmd->cmds[0], "random", strlen("random") + 1) == 0
		&& cmd->num_cmds == 4) {
		int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
		const unsigned int start_range = atoi(cmd->cmds[2]);
		const unsigned int end_range = atoi(cmd->cmds[3]);
		if(random_matrix(mats[mat1_idx],start_range, end_range)){
			printf("Attempts to fill matrix with random values failed. God save us.\n");
			return;
		}

		printf("Matrix (%s) is randomized between %u %u\n", mats[mat1_idx]->name, start_range, end_range);
	}
	else {
		printf("Not a command in this application\n");
	}

}

 
/* PURPOSE: Searches the array of matrices by comparing the name, given by the user to each
 * 			matrix's name.
 * INPUTS: 
 * 		   
 * RETURN: Either returns the index of matrix or returns -1 to indicate that the matrix was not found.
 **/
unsigned int find_matrix_given_name (Matrix_t** mats, unsigned int num_mats, const char* target) {
	//Check if mats empty
	if(!(*mats)){
		printf("\nArray of matrices empty\n");
		return -1;
	}

	for (int i = 0; i < num_mats; ++i) {
		if (strncmp(mats[i]->name,target,strlen(mats[i]->name)) == 0) {
			return i;
		}
	}
	return -1;
}

/* 
 * PURPOSE: Free memory allocated for matrices
 * INPUTS:
 *        mats : pointer to the list of matrices.
 *        num_mats : is the number of the matrices in the list.
 *
 * RETURN: void
 **/
void destroy_remaining_heap_allocations(Matrix_t **mats, unsigned int num_mats) {
	
	//Check if array empty.
	if(!(*mats)){
		printf("Matrix array empty or &MatrixArray[0] null\n");
		return;
		
	}

	for (int i = 0;i < num_mats;i++) {
		if (mats[i] != NULL) {
			free(mats[i]->data);
			free(mats[i]->name);
		}
	}
}
