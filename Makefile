CC = g++
COPTS = -lncurses

# Define the target executable
TARGET = a.out

# List of source files
SRCS = main.cc ui.cc

# Generate a list of object files from source files
OBJS = $(SRCS:.cc=.o)

# Main target to build the executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(COPTS) -o $(TARGET)

# Rules for compiling source files to object files
%.o: %.cc
	$(CC) -c $< $(COPTS) -o $@

# Clean up the generated object files and the executable
clean:
	rm -f $(OBJS) $(TARGET)
