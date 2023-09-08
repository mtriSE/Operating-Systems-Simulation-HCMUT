
INC = -Iinclude
LIB = -lpthread

SRC = src
OBJ = obj
INCLUDE = include

CC = gcc
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

vpath %.c $(SRC)
vpath %.h $(INCLUDE)

MAKE = $(CC) $(INC) 

# Object files needed by modules
MEM_OBJ = $(addprefix $(OBJ)/, paging.o mem.o cpu.o loader.o)
OS_OBJ = $(addprefix $(OBJ)/, cpu.o mem.o loader.o queue.o os.o sched.o timer.o mm-vm.o mm.o mm-memphy.o)
SCHED_OBJ = $(addprefix $(OBJ)/, cpu.o loader.o)
HEADER = $(wildcard $(INCLUDE)/*.h)

all: os
#mem sched os

# Just compile memory management modules
mem: $(MEM_OBJ)
	$(MAKE) $(LFLAGS) $(MEM_OBJ) -o mem $(LIB)

# Just compile scheduler
sched: $(SCHED_OBJ)
	$(MAKE) $(LFLAGS) $(MEM_OBJ) -o sched $(LIB)

# Compile the whole OS simulation
os: $(OS_OBJ)
	$(MAKE) $(LFLAGS) $(OS_OBJ) -o os $(LIB)

$(OBJ)/%.o: %.c ${HEADER} $(OBJ)
	$(MAKE) $(CFLAGS) $< -o $@

# Prepare objectives container
$(OBJ):
	mkdir -p $(OBJ)

clean:
	rm -f $(OBJ)/*.o os sched mem
	rm -r $(OBJ)

