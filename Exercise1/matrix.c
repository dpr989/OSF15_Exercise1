#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


#include "matrix.h"


#define MAX_CMD_COUNT 50

/*protected functions*/
void load_matrix (Matrix_t* m, unsigned int* data);

/* 
 * PURPOSE: instantiates a new matrix with the passed name, rows, cols 
 * 
 * INPUTS: 
 *		   name : the name of the matrix limited to 50 characters 
 *  	   rows : the number of rows in the matrix
 *  	   cols : the number of cols in the matrix
 * 
 * RETURN:If no errors occurred during instantiation then trueelse false for an error in the process.
 *
 **/

bool create_matrix (Matrix_t** new_matrix, const char* name, const unsigned int rows,
						const unsigned int cols) {
	
	
	*new_matrix = calloc(1,sizeof(Matrix_t));
	if (!(*new_matrix)) {
		return false;
	}
	(*new_matrix)->data = calloc(rows * cols,sizeof(unsigned int));
	if (!(*new_matrix)->data) {
		return false;
	}
	(*new_matrix)->rows = rows;
	(*new_matrix)->cols = cols;
	unsigned int len = strlen(name) + 1; 
	if (len > MATRIX_NAME_LEN) {
		return false;
	}
	strncpy((*new_matrix)->name,name,len);
	return true;

}

	/* 
	 * PURPOSE: Dealocates the memory of the matrix and the pointer to it. Sets the
	 * 			pointer to the matrix to null.
	 * INPUTS: 
	 * 		  m : List of matrices. Note: *m should contain the pointer to 
	 * 			  the matrix to be deleted from the list **m
	 * 
	 * RETURN: void
	 **/
void destroy_matrix (Matrix_t** m) {

	if(!(*m)){
		printf("\nMatrix array empty.");
	}
	
	free((*m)->data);
	free(*m);
	*m = NULL;
}

	/* 
	 * PURPOSE: Compares 2 matrices to see if they are equivalent
	 * INPUTS: 
	 * 		   a : Pointer to the first matrix	
	 * 		   b : Pointer to the second matrix
	 * 
	 * RETURN: True if matrices are equal. False if matrices are not.
	 **/
bool equal_matrices (Matrix_t* a, Matrix_t* b) {

	//Check that both matrices aren't null.
	if(!a || !b){
		printf("Check inputs, a matrix is null");
		return false;
	}

	int result = memcmp(a->data,b->data, sizeof(unsigned int) * a->rows * a->cols);
	if (result == 0) {
		return true;
	}
	return false;
}

	/* 
	 * PURPOSE: To make a copy of a matrix
	 * INPUTS: 
	 * 		   src : Pointer to matrix that is the source for copying.
	 * 		   dest : Pointer to the matrix that is to be coppied into.
	 * 
	 * RETURN: True if the copy successful. False if either the source is NULL or if th
	 * 		   copy was unsuccessful.
	 **/
bool duplicate_matrix (Matrix_t* src, Matrix_t* dest) {

	if (!src) {
		printf("\nSource cannot be null.\n);
		return false;
	}
	/*
	 * copy over data
	 */
	unsigned int bytesToCopy = sizeof(unsigned int) * src->rows * src->cols;
	memcpy(dest->data,src->data, bytesToCopy);	
	return equal_matrices (src,dest);
}

	/* 
	 * PURPOSE: Bit shift inputed matrix 
	 * INPUTS: 
	 * 		   a : matrix to be shifted.
	 * 		   direction : character representing the direction to shift the matrix
	 * 		   shift : how many bits to shift the matrix,   
	 * RETURN:
	 **/
bool bitwise_shift_matrix (Matrix_t* a, char direction, unsigned int shift) {
	
	//If matrix is null
	if(!a){
		perror("Matrix is null and cannont be shifted\n");
		return false;
	}
	
	//Check direction is either l or r	
	if (direction == 'l' || direction == 'L') {
		unsigned int i = 0;
		for (; i < a->rows; ++i) {
			unsigned int j = 0;
			for (; j < a->cols; ++j) {
				a->data[i * a->cols + j] = a->data[i * a->cols + j] << shift;
			}
		}

	}
	else if(direction == 'r' || direction == 'R'){
		unsigned int i = 0;
		for (; i < a->rows; ++i) {
			unsigned int j = 0;
			for (; j < a->cols; ++j) {
				a->data[i * a->cols + j] = a->data[i * a->cols + j] >> shift;
			}
		}
	}else{
		printf("\nInvalid direction to shift\n");
		return false;
	}
	
	return true;
}

	/* 
	 * PURPOSE: To take 2 a,b matrices, of the same size and add them together to make a 3rd, c
	 * INPUTS: 
	 * 		   a : pointer to a matrix to be added
	 * 		   b : pointer to another matrix to be added
	 * 		   c : pointer to the resulting matrix of a+b
	 *  
	 * RETURN: Returns boolean value indicating whether the operation
	 **/
bool add_matrices (Matrix_t* a, Matrix_t* b, Matrix_t* c) {
	
	//Check to see if matrices are null
	if(!a || !b || !c){
		printf("\nCheck inputs a matrix pointer may me null.\n");
		return false;
	}
	
	

	if (a->rows != b->rows && a->cols != b->cols) {
		printf("\nIncompatible matrix sizes:\nMatrix 1 is: %d X %d\nMatrix 2 is: %d X %d",
				a->rows,a->cols,b->rows,b->cols);
		return false;
	}

	for (int i = 0; i < a->rows; ++i) {
		for (int j = 0; j < b->cols; ++j) {
			c->data[i * a->cols +j] = a->data[i * a->cols + j] + b->data[i * a->cols + j];
		}
	}
	return true;
}

	/* 
	 * PURPOSE: Displays the contents of the given matrix
	 * INPUTS: 
	 * 	       m : matrix pointer
	 * 
	 * RETURN: void
	 **/
void display_matrix (Matrix_t* m) {
	
	if(!m){
		printf("\nInput matrix is null\n");
		return;
	}


	printf("\nMatrix Contents (%s):\n", m->name);
	printf("DIM = (%u,%u)\n", m->rows, m->cols);
	for (int i = 0; i < m->rows; ++i) {
		for (int j = 0; j < m->cols; ++j) {
			printf("%u ", m->data[i * m->cols + j]);
		}
		printf("\n");
	}
	printf("\n");

}

	/* 
	 * PURPOSE: Opens stored matrix file, attempts to read from file. If successful, 
	 * 			creates a matrix and loads it into
	 * INPUTS: 
	 * 	       matrix_input_filename : file name of matrix to be read from file.
	 * 		   m : list of matrices
	 * RETURN: True if matrix read successfully. False if read failed.
	 **/
bool read_matrix (const char* matrix_input_filename, Matrix_t** m) {

	int fd = open(matrix_input_filename,O_RDONLY);
	if (fd < 0) {
		printf("FAILED TO OPEN FOR READING\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}
		return false;
	}

	/*read the wrote dimensions and name length*/
	unsigned int name_len = 0;
	unsigned int rows = 0;
	unsigned int cols = 0;
	
	if (read(fd,&name_len,sizeof(unsigned int)) != sizeof(unsigned int)) {
		printf("FAILED TO READING FILE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}
		return false;
	}
	char name_buffer[50];
	if (read (fd,name_buffer,sizeof(char) * name_len) != sizeof(char) * name_len) {
		printf("FAILED TO READ MATRIX NAME\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;	
	}

	if (read (fd,&rows, sizeof(unsigned int)) != sizeof(unsigned int)) {
		printf("FAILED TO READ MATRIX ROW SIZE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;
	}

	if (read(fd,&cols,sizeof(unsigned int)) != sizeof(unsigned int)) {
		printf("FAILED TO READ MATRIX COLUMN SIZE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;
	}

	unsigned int numberOfDataBytes = rows * cols * sizeof(unsigned int);
	unsigned int *data = calloc(rows * cols, sizeof(unsigned int));
	if (read(fd,data,numberOfDataBytes) != numberOfDataBytes) {
		printf("FAILED TO READ MATRIX DATA\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;	
	}

	if (!create_matrix(m,name_buffer,rows,cols)) {
		return false;
	}

	load_matrix(*m,data);
	free(data);
	if (close(fd)) {
		return false;

	}
	return true;
}

	/* 
	 * PURPOSE: To write a matrix to a file for usage latter.
	 * INPUTS: 
	 * 		   matrix_output_filename : name of file that matrix will be stored in.
	 * 
	 * RETURN: True if write successful. False if writing of matrix to file failed.
	 **/
bool write_matrix (const char* matrix_output_filename, Matrix_t* m) {

	int fd = open(matrix_output_filename, O_CREAT | O_RDWR | O_TRUNC, 0644);
	/* ERROR HANDLING USING errorno*/
	if (fd < 0) {
		printf("FAILED TO CREATE/OPEN FILE FOR WRITING\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXISTS\n");
		}
		return false;
	}
	/* Calculate the needed buffer for our matrix */
	unsigned int name_len = strlen(m->name) + 1;
	unsigned int numberOfBytes = sizeof(unsigned int) + (sizeof(unsigned int)  * 2) + name_len + sizeof(unsigned int) * m->rows * m->cols + 1;
	/* Allocate the output_buffer in bytes
	 * IMPORTANT TO UNDERSTAND THIS WAY OF MOVING MEMORY
	 */
	unsigned char* output_buffer = calloc(numberOfBytes,sizeof(unsigned char));
	unsigned int offset = 0;
	memcpy(&output_buffer[offset], &name_len, sizeof(unsigned int)); // IMPORTANT C FUNCTION TO KNOW
	offset += sizeof(unsigned int);	
	memcpy(&output_buffer[offset], m->name,name_len);
	offset += name_len;
	memcpy(&output_buffer[offset],&m->rows,sizeof(unsigned int));
	offset += sizeof(unsigned int);
	memcpy(&output_buffer[offset],&m->cols,sizeof(unsigned int));
	offset += sizeof(unsigned int);
	memcpy (&output_buffer[offset],m->data,m->rows * m->cols * sizeof(unsigned int));
	offset += (m->rows * m->cols * sizeof(unsigned int));
	output_buffer[numberOfBytes - 1] = EOF;

	if (write(fd,output_buffer,numberOfBytes) != numberOfBytes) {
		printf("FAILED TO WRITE MATRIX TO FILE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}
		return false;
	}
	
	if (close(fd)) {
		return false;
	}
	free(output_buffer);

	return true;
}

	/* 
	 * PURPOSE: To fill a given matrix with random unsigned int data between
	 * 			a upper and lower bound set by the user. 
	 * INPUTS: 
	 * 	       m : matrix to fill
	 * 		   start_range : lower bound of random values to put in matrix
	 * 		   end_range : upper bound of random values to put in matrix
	 * RETURN: True if matrix is successfully filled.
	 **/
bool random_matrix(Matrix_t* m, unsigned int start_range, unsigned int end_range) {
	
	//Check if m is null
	if(!m){
		printf("\nInput matrix null\n");
		return false;
	}
	//Check if start_range is larger than end_range
	if(start_range > end_range){
		printf("\nError starting range cannot be greater than end_range");
		return false;
	}

	for (unsigned int i = 0; i < m->rows; ++i) {
		for (unsigned int j = 0; j < m->cols; ++j) {
			m->data[i * m->cols + j] = rand() % (end_range + 1 - start_range) + start_range;
		}
	}
	return true;
}

/*Protected Functions in C*/

	/* 
	 * PURPOSE: Loads matrix with ints from data[] 
	 * INPUTS: 
	 * 		   m : pointer to matrix to fill
	 * 		   data : pointer to list of data to fill matrix with
	 * RETURN: void
	 **/
void load_matrix (Matrix_t* m, unsigned int* data) {
	
	if(!m){
		printf("\nMatrix null.\n");
		return;
	}
	
	memcpy(m->data,data,m->rows * m->cols * sizeof(unsigned int));
}

	/* 
	 * PURPOSE: Adds a matrix to the array of matrices (Matrix_t** mats) 
	 * INPUTS: 
	 * 		   mats : pointer to array of matrices
	 * 		   new_matrix : pointer to matrix to be added
	 * 		   num_mats : number of matrices in array
	 * 
	 * RETURN: index of matrix that was added to the array.
	 **/
unsigned int add_matrix_to_array (Matrix_t** mats, Matrix_t* new_matrix, unsigned int num_mats) {
	
	if(!new_matrix){
		perror("New matrix is null\n");
		return (num_mats+1);
	}
	static long int current_position = 0;
	const long int pos = current_position % num_mats;
	if ( mats[pos] ) {
		destroy_matrix(&mats[pos]);
	} 
	mats[pos] = new_matrix;
	current_position++;
	return pos;
}
