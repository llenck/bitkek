CXXFLAGS ?= -Wall -Wextra -std=c++20
LDFLAGS ?=
CXX ?= g++

SOURCES := main.cpp requests.cpp
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))
HEADERS := requests.hpp req_types.hpp
EXECUTABLE := main

debug : CXXFLAGS += -Og -g
release : CXXFLAGS += -O3
debug : LDFLAGS +=
release : LDFLAGS += -O

debug: $(EXECUTABLE)
release: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(HEADERS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $< -c

.PHONY: clean
clean:
	$(RM) $(EXECUTABLE) $(OBJECTS)
