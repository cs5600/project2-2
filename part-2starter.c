/*
 * file:        part-2.c
 * description: Part 2, CS5600 Project 2, Fall 2023
 */

/* NO OTHER INCLUDE FILES */
#include "elf64.h"
#include "sysdefs.h"

extern void *vector[];

/* ---------- */

/* write these functions 
*/
int read(int fd, void *ptr, int len);           //done in part1
int write(int fd, void *ptr, int len);          //done in part1
void exit(int err);                             //done in part1
int open(char *path, int flags);
int close(int fd);
int lseek(int fd, int offset, int flag);
void *mmap(void *addr, int len, int prot, int flags, int fd, int offset);
int munmap(void *addr, int len);

/* ---------- */

/* Write the three 'system call' functions - do_readline, do_print, do_getarg 
 * Adjust the functions readline and print-and-clean functions written in part 1, to obtain
 * the 'system call' functions do_readline and do_print
 * hints: 
 *  - read() or write() one byte at a time. It's OK to be slow.
 *  - stdin is file desc. 0, stdout is file descriptor 1
 *  - use global variables for getarg
 */

/* your code here */
void do_readline(char *buf, int len);
void do_print(char *buf);
char *do_getarg(int i);         

/* ---------- */



/* simple function to split a line:
 *   char buffer[200];
 *   <read line into 'buffer'>
 *   char *argv[10];
 *   int argc = split(argv, 10, buffer);
 *   ... pointers to words are in argv[0], ... argv[argc-1]
 */
int split(char **argv, int max_argc, char *line)
{
	int i = 0;
	char *p = line;

	while (i < max_argc) {
		while (*p != 0 && (*p == ' ' || *p == '\t' || *p == '\n'))
			*p++ = 0;
		if (*p == 0)
			return i;
		argv[i++] = p;
		while (*p != 0 && *p != ' ' && *p != '\t' && *p != '\n')
			p++;
	}
	return i;
}

/* ---------- */

/* This is where you write the details of the function exec(char* filename) called by main()
* Follow instructions listed in project description.
* the guts of part 2
*   read the ELF header
*   for each segment, if b_type == PT_LOAD:
*     create mmap region
*     read from file into region
*   function call to hdr.e_entry
*   munmap each mmap'ed region so we don't crash the 2nd time
*
*   don't forget to define offset, and add it to virtual addresses read from ELF file
*
*               your code here
*/



/* ---------- */
void main(void)
{   // The vector array is defined as a global array. It plays the role of a system call vector table 
	// (similar to the interrupt vector table seen in class). Each entry in this array/table holds the address
	// of the corresponding system function. Check out call-vector.S and Makefile to see how the vector table is built.
	
	vector[0] = do_readline;
	vector[1] = do_print;
	vector[2] = do_getarg;

	/* YOUR CODE HERE AS DESCRIBED IN THE FILE DESCRIPTION*/
	/* When the user enters an executable_file, the main function should call exec(executable_file) */
}

