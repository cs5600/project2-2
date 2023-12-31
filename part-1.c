/*
 * file:        part-1.c
 * description: Part 1, CS5600 Project 2, Fall 2023
 */

/* THE ONLY INCLUDE FILE */
#include "sysdefs.h"

/* write these functions */

int read(int fd, void *ptr, int len);
int write(int fd, void *ptr, int len);
void exit(int err);
 

/*The read function is given to you in the starter code, by analogy write the
  write() function and the exit() function. 
  __NR_read is defined in file sysdefs.h, it is the system call number 
  for the system function that performs reading. 
    A link is provided after each function, to read about this system call function
  in the Linux manual page */

int read(int fd, void *ptr, int len) {
	if (len < 0) {
		return -1;
	}

	return syscall(__NR_read, fd, ptr, len);
} /*https://man7.org/linux/man-pages/man2/read.2.html */


int write(int fd, void *ptr, int len){
	/* add your code here*/
	if (len < 0) {
		return -1;
	}

	return syscall(__NR_write, fd, ptr, len);
} /* https://man7.org/linux/man-pages/man2/write.2.html */



void exit(int err){
	/* add your code here*/
	syscall(__NR_exit, err);
} /* https://man7.org/linux/man-pages/man2/exit.2.html */



/* Factor, factor! Don't put all your code in main()!
   Write new functions using the functions you just wrote
*/


/* read one line from stdin (file descriptor 0) into a buffer pointed to by ptr. : */
void readline(int fd, void *ptr, int max_len) {
	/*reads a line from the keyboard by repeatedly (for each byte) calling function
	read() that you just wrote. 
	You will call read() to read 1 byte at a time, and sticking it in a buffer (pointed to by ptr); 
	if it is �\n� then add a '\0' on the end and you�re done)
    You can assign a (void *) pointer to a (char *) by using proper casting if needed.*/

	/* add your code here*/
	char* buf = (char *)ptr;

	// loop <max_len-1> times for an extra space for '\0'
	for (int i=0 ; i<max_len-1 ; ++i)
	{
		read(fd, buf + i, 1);
		if (buf[i] == '\n')
		{
			buf[i+1] = '\0';
			return;
		}
	}

	// If there's no breakline from fd, put '\0' in the end
	buf[max_len-1] = '\0';
	return;
}




/* print a string to stdout (file descriptor 1) */
void print_and_clean(int fd, void *ptr, int max_len) {
	/* This function prints a string pointed to by ptr to stdout by repeatedly calling
	function write() you wrote above.
	You will call write to print 1 byte at a time.
	After you print a character, clean the corresponding buffer location by replacing
	the character printed by 0 */


	/* add your code here*/
	char* buf = (char *)ptr;

	for (int i=0 ; i<max_len ; ++i)
	{
		write(fd, buf + i, 1);
		buf[i] = 0;
	}
}





void main(void)
{
	/* reads a line of input from standard input (file descriptor 0)
	   if the line starts with �q�, �u�, �i�, �t� then exit
	   otherwise print the line to standard output (file descriptor 1) and loop.
	   You can assume that input lines are never more than 200 bytes long. 
	   Remember that you can�t return from the main function � you have to call exit.
	   Remember that you can�t use any functions other than the ones that you write 
	   yourself. That means no printf � use GDB for your debugging (and no strcmp for
	   string comparisons, or malloc for allocating memory).   */

	/* add your code here */
	char buf[200];
	for (int i=0 ; i<200 ; ++i)
	{
		buf[i] = 0;
	}
	while (1)
	{
		readline(0, buf, 200);
		
		// Find the number of bytes in this line
		int length = 0;
		for (int i=0 ; i<200 ; ++i)
		{
			if (buf[i] == '\0')
			{
				length = i;
			}
		}

		// If the line starts with "quit", then exit
		if (length >= 4
		 && buf[0] == 'q'
		 && buf[1] == 'u'
		 && buf[2] == 'i'
		 && buf[3] == 't')
		{
			exit(0);
		}

		print_and_clean(1, buf, 200);
	}
}
