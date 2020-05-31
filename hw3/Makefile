CXX = clang++
ARG = -O3 -std=c++17 -lpthread -g -o
RAW_FILES = main.cpp
EXEC_FILE = main

all: $(RAW_FILES)
	$(CXX) $(ARG) $(EXEC_FILE) $^

.PHONY: test clean

clean:
	rm -rf $(EXEC_FILE)
