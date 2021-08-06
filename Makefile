CXXFLAGS ?= -Wall -Wextra
LDFLAGS ?=
CXX ?= g++

SOURCES := main.cpp
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))
HEADERS :=
EXECUTABLE := main

debug : CXXFLAGS += -Og -g
release : CXXFLAGS += -O3
debug : LDFLAGS +=
release : LDFLAGS += -O

debug: $(EXECUTABLE)
release: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(HEADERS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

%.o: %.c $(HEADERS)
	$(CXX) $(CXXFLAGS) $< -c

.PHONY: clean
clean:
	$(RM) $(EXECUTABLE) $(OBJECTS)
