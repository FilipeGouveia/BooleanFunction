CXX = g++
CXXFLAGSLINUX = $(WARNINGS) -std=c++11 -ggdb3 -static
CXXFLAGSOSX = $(WARNINGS) -std=c++11 -ggdb3
WARNINGS = -Wall -Wextra -pedantic
LFLAGS = -std=c++11
LFLAGSLINUX = -std=c++11 -static

# Operating System
OS := $(shell uname)

CPPS = $(wildcard *.cpp)
LCPPS = $(filter-out main.cpp, $(CPPS))
HEADERS = $(wildcard *.h)
OBJS = $(CPPS:%.cpp=%.o)
LOBJS = $(LCPPS:%.cpp=%.o)
TARGET = booleanFunction
LTARGET = libBooleanFunction.a


default: all

.PHONY: all lib clean depend

all: depend $(LTARGET) $(TARGET)

lib: depend $(LTARGET)

$(TARGET): $(OBJS)
ifeq ($(OS),Linux)
	$(CXX) $(LFLAGSLINUX) -o $@ $^
else
	$(CXX) $(LFLAGS) -o $@ $^
endif

$(LTARGET): $(LOBJS)
	ar rvsu $@ $^

%.o: %.cpp
ifeq ($(OS),Linux)
	$(CXX) $(CXXFLAGSLINUX) -c $<
else
	$(CXX) $(CXXFLAGSOSX) -c $<
endif

clean:
	rm -f .makedeps *.o $(TARGET) $(LTARGET)

depend: $(CPPS) $(HEADERS)
ifeq ($(OS),Linux)
	$(CXX) $(CXXFLAGSLINUX) -MM $(CPPS) > .makedeps
else
	$(CXX) $(CXXFLAGSOSX) -MM $(CPPS) > .makedeps
endif
