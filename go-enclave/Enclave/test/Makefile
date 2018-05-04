CC      = gcc
CFLAGS  = -g
LIB_PATH = /home/user99/Documents/Antoine/sgx/go-enclave/Enclave/go-lib
INC_PATH = /home/user99/Documents/Antoine/sgx/go-enclave/Enclave/go-code/libcalc
FLAGS = -Wall -I$(INC_PATH) -L$(LIB_PATH)
RM      = rm -f


default: all

all: Main

Main: main.c
	@$(CC) $(CFLAGS) $(FLAGS) main.c -lcalc -o main

clean:
	$(RM) main
