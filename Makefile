ifeq ($(OS),Windows_NT)
	SEP := \\
	CC_FLAGS := -Wall -g -O0 -std=c++17 -I. -L C:$(SEP)Windows$(SEP)System32
	CC_LIBS := -lws2_32
else
	CC_FLAGS := -Wall -g -O0 -std=c++17 -I.
	SEP := /
	CC_LIBS := 
endif

CC := g++
OBJS := \
	build$(SEP)bn$(SEP)app.o \
	build$(SEP)bn$(SEP)socket.o \

PROG := bitnation
RMDIR := rmdir /s /q
MKDIR := mkdir
CPR := xcopy /E

all: bn 

.PHONY: clean
clean:
	$(RMDIR) build

.PHONY: init
init:
	$(MKDIR) build
	$(MKDIR) build$(SEP)bn

.PHONY: full
full:
	make clean
	make init
	make
	.$(SEP)$(PROG)

.PHONY: a
a:
	make
	.$(SEP)$(PROG)

bn: build$(SEP)bn$(SEP)main.o $(OBJS)
	$(CC) $(CC_FLAGS) build$(SEP)bn$(SEP)main.o $(OBJS) -o $(PROG) $(CC_LIBS)

build$(SEP)bn$(SEP)main.o: bn$(SEP)main.cpp bn$(SEP)main.hpp
	$(CC) $(CC_FLAGS) -c $< -o $@ $(CC_LIBS)

build$(SEP)bn$(SEP)app.o: bn$(SEP)app.cpp bn$(SEP)app.hpp
	$(CC) $(CC_FLAGS) -c $< -o $@ $(CC_LIBS)

build$(SEP)bn$(SEP)socket.o: bn$(SEP)socket.cpp bn$(SEP)socket.hpp
	$(CC) $(CC_FLAGS) -c $< -o $@ $(CC_LIBS)
