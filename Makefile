#
# Makefile for project 2
# You do not need to make any changes to this file.
#

# Assembly language files
#
syscall.o: syscall.S
	gcc -g -c -o syscall.o syscall.S

vector.o: vector.S
	gcc -g -c -o vector.o vector.S

call-vector.o: call-vector.S
	gcc -g -c -o call-vector.o call-vector.S



# these flags work for all the C files:
#  -ffreestanding: don't include even implicit system include files
#  turn off various features that complicate the executable

CFLAGS = -Wall -ggdb3 -ffreestanding -fno-stack-protector -fstack-check=no -nostdlib

# Main C programs - compile
#
part-1.o: part-1.c
	gcc $(CFLAGS) -c part-1.c -o part-1.o 

part-2.o: part-2.c
	gcc $(CFLAGS) -c part-2.c -o part-2.o 



# Main C programs - link
#
LDFLAGS = --entry=main -ffreestanding -nostdlib

part-1: part-1.o syscall.o
	gcc -ggdb3 part-1.o syscall.o -o part-1 $(LDFLAGS) -static

VECTOR = -Wl,--section-start=.vector=0x5000000

part-2: part-2.o syscall.o vector.o wait hello ugrep
	gcc -ggdb3 part-2.o syscall.o vector.o -o part-2 $(LDFLAGS) $(VECTOR) -static



# Part 2 micro-program - compile and link
# add -fpie - create position-independent executable

wait: wait.c call-vector.S
	gcc -fpie $(CFLAGS)  wait.c call-vector.S -o wait $(LDFLAGS)

hello: hello.c call-vector.S
	gcc -fpie $(CFLAGS)  hello.c call-vector.S -o hello $(LDFLAGS)

ugrep: ugrep.c call-vector.S
	gcc -fpie $(CFLAGS)  ugrep.c call-vector.S -o ugrep $(LDFLAGS)



# put this at the end so 'make' doesn't clean everything
#
clean:
	rm -f *.o part-1 part-2 wait hello ugrep 

