/*
 * file:        part-2.c
 * description: Part 2, CS5600 Project 2, Fall 2023
 */

/* NO OTHER INCLUDE FILES */
#include "elf64.h"
#include "sysdefs.h"
#include <sys/mman.h>


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
char *global_argv[10];
int global_argc = 0;

void do_readline(char *buf, int len){
	int i;
    for (i = 0; i < len - 1; ++i) {
        read(0, &buf[i], 1);
        if (buf[i] == '\n') {
            break;
        }
    }
    buf[i] = '\0';
};

void do_print(char *buf){
	int i = 0;
    while (buf[i] != '\0') {
        ++i;
    }
    write(1, buf, i);
};

char *do_getarg(int i){
	if (i >= 0 && i < global_argc) {
        return global_argv[i];
    }
    return 0;
};         

int open(char *path, int flags) {
    return syscall(__NR_open, path, flags);
}

int close(int fd) {
    return syscall(__NR_close, fd);
}

int lseek(int fd, int offset, int flag) {
    return syscall(__NR_lseek, fd, offset, flag);
}

void *mmap(void *addr, int len, int prot, int flags, int fd, int offset) {
    return (void *) syscall(__NR_mmap, addr, len, prot, flags, fd, offset);
}

int munmap(void *addr, int len) {
    return syscall(__NR_munmap, addr, len);
}


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

	char buffer[200];
	while (1) {
		do_print("Please enter the name of the executable file to run: ");
		do_readline(buffer, 200);
		if (buffer[0] == 'q') {
			break;
		} else {
			exec(buffer);
		}
	}

	exit(0);

	/* YOUR CODE HERE AS DESCRIBED IN THE FILE DESCRIPTION*/
	/* When the user enters an executable_file, the main function should call exec(executable_file) */
}


void exec(char* filename) {
    int fd = open(filename, 0);
    struct elf64_ehdr e_hdr;
    read(fd, &e_hdr, sizeof(e_hdr));

    // Dynamically allocate Program Headers
    struct elf64_phdr *phdrs = malloc(e_hdr.e_phnum * sizeof(struct elf64_phdr));
    if (phdrs == NULL) {
        do_print("Memory allocation failed\n");
        exit(1);
    }

    lseek(fd, e_hdr.e_phoff, SEEK_SET);
    read(fd, phdrs, e_hdr.e_phnum * sizeof(struct elf64_phdr));

    // Array to store mmap addresses
    void *mmap_addrs[e_hdr.e_phnum];
    int mmap_lens[e_hdr.e_phnum];
    int mmap_count = 0;

    // Load PT_LOAD segments
    for (int i = 0; i < e_hdr.e_phnum; ++i) {
        if (phdrs[i].p_type == PT_LOAD) {
            int len = ROUND_UP(phdrs[i].p_memsz, 4096);
            void *addr = mmap((void *)(phdrs[i].p_vaddr + 0x80000000),
                              len,
                              PROT_READ | PROT_WRITE | PROT_EXEC,
                              MAP_PRIVATE | MAP_ANONYMOUS,
                              -1,
                              0);
            if (addr == MAP_FAILED) {
                do_print("mmap failed\n");
                exit(1);
            }

            // Record mmap address and length
            mmap_addrs[mmap_count] = addr;
            mmap_lens[mmap_count] = len;
            mmap_count++;

            lseek(fd, phdrs[i].p_offset, SEEK_SET);
            read(fd, addr, phdrs[i].p_filesz);
        }
    }

    // Execute entry point function
    void (*entry)() = (void (*)())(e_hdr.e_entry + 0x80000000);
    entry();

    // Clean up, use munmap
    for (int i = 0; i < mmap_count; ++i) {
        munmap(mmap_addrs[i], mmap_lens[i]);
    }

    // Free dynamically allocated memory
    free(phdrs);
}
