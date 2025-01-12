# Directories
BIN_DIR = .
SRC_DIR = src

# C compiler and flags
cc := /usr/bin/gcc
#OPTIMISATIONS = -O2 -pg
CFLAGS = -g3 -O0
INCLUDE = -I./include/
LIBS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm


# For later when this becomes an actual project{
# # Source and object files
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:.c=.o)
# DEPS := depends.mk
#}


# # Executables
BINS := $(wildcard $(BIN_DIR)/*.c)
EXECS := $(BINS:.c=.x)


all: $(EXECS) #$(DEPS)

%.o : %.mod

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $< $(CFLAGS) $(INCLUDE) -o $@ $(LIBS)
	
# $(SRC_DIR)/%.o: $(SRC_DIR)/%.c
# 	$(CC) -c $< $(CFLAGS) $(INCLUDE) -o $@ $(LIBS)

$(BIN_DIR)/%.x: $(BIN_DIR)/%.c $(OBJS)
	$(CC) $< $(CFLAGS) $(INCLUDE) -o $@ $(SRCS) $(LIBS)

# $(DEPS):
# 	@python3 mkdeps.py

.phony: clean
clean:
	rm -f $(OBJS) $(SRC_DIR)/*.mod $(EXECS) $(DEPS) 

# Include dependencies
include $(DEPS)
