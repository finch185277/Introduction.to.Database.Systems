CXX = g++
ARG = -O3 -std=c++17 -pthread -g -o
RAW_FILES = main.cpp
EXEC_FILE = main

TEST_SCRIPT = test.sh

all: $(RAW_FILES)
	$(CXX) $(ARG) $(EXEC_FILE) $^

.PHONY: test test_time clean

test: all
	./$(TEST_SCRIPT)

time:
	$(CXX) $(ARG) $(EXEC_FILE) time.cpp
	./$(TEST_SCRIPT)

sample:
	$(CXX) $(ARG) $(EXEC_FILE) sample.cpp
	./$(TEST_SCRIPT)

clean:
	rm -rf $(EXEC_FILE)
