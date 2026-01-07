CXX = g++
CXXFLAGS = -std=c++17 -fopenmp -O2 -Wall
TARGET = integrated_system
SRCS = integrated_system.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean