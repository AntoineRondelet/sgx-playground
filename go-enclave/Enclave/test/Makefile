CC      = gcc
CFLAGS  = -g
# Go in the root of the sgx playground repo and run: export SGX_REPO=$(pwd)
SGX_REPO ?= .
LIB_PATH = $(SGX_REPO)/go-enclave/Enclave/go-lib
INC_PATH = $(SGX_REPO)/go-enclave/Enclave/go-code/libcalc
FLAGS = -Wall -I$(INC_PATH) -L$(LIB_PATH)
RM      = rm -f

default: all

all: Main

Main: main.c
	@$(CC) $(CFLAGS) $(FLAGS) main.c -lcalc -o main

clean:
	$(RM) main
