CC := gcc
CFLAGS := 
LDFLAGS :=

export CC CFLAGS LDFLAGS

DEBUG ?= 1
ifeq (DEBUG, 1)
	CC += -DDEBUG -g
endif

all: common client server

common:
	make -C src/common

client:
	make -C src/client

server:
	make -C src/server

clean:
	make -C src/common clean
	make -C src/client clean
	make -C src/server clean
