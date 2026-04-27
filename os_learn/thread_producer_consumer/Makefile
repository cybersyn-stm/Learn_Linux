# User-mode Makefile (cross compile + bear/clangd friendly)

CC := /home/cybersyn/duo-buildroot-sdk-v2/host-tools/gcc/riscv64-linux-musl-x86_64/bin/riscv64-unknown-linux-musl-gcc
SYSROOT := /home/cybersyn/duo-buildroot-sdk-v2/host-tools/gcc/riscv64-linux-musl-x86_64/sysroot

APP  ?= duo-exe exe

SRCS := $(shell find . -name "*.c")
OBJS := $(SRCS:.c=.o)

COMMON := --sysroot=$(SYSROOT)

CPPFLAGS ?=
CPPFLAGS ?= -I./include/               

CFLAGS  ?= -O2 -Wall -Wextra $(COMMON)
LDFLAGS ?= $(COMMON)
LDLIBS  ?=

all: $(APP)
	#scp $^ root@192.168.42.1:/root/

$(APP): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	gcc $(shell find -name *.c) -o exe 

clean:
	rm -f $(APP) $(OBJS)
