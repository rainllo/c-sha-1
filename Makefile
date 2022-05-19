# Make file with variables
# Lisa Miller 1/15/2020
CXX=gcc
CXXFLAGS= -std=c99 -g -Wall -Wshadow -Wpedantic -Wvla -Werror
# add some standard arguments to gcc
# -std=c99 means to use the C99 version of the C language.
# -g means to enable gdb by storing information such as your variable names
#   in the executable
#  -Wall, -Wshadow, -Wpedantic, Wvla, -Werror turn on extra warnings to let
#   you know about anomolies in your code might indicate a bug.
SOURCES=final_rjavillo.c printbits.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=final_rjavillo

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ 
#add -lm after @ to include c math library

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.c.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $<
