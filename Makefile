CC := gcc
CFLAGS := 
LDFLAGS :=

export CC CFLAGS LDFLAGS

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
