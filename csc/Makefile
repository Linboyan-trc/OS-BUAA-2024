# csc/Makefile

# Default target
all: fibo

# Subdirectory
CODE_DIR = code

# Targets in subdirectory
SUBTARGETS = $(CODE_DIR)/fibo.o $(CODE_DIR)/main.o

# Build executable
fibo: $(SUBTARGETS)
	$(CC) $(CFLAGS) -o $@ $^

# Clean .o files
clean:
	$(MAKE) -C $(CODE_DIR) clean

