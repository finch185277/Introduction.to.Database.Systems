CXX = g++
ARG = -O3 -std=c++17 -pthread -g -o
RAW_FILES = main.cpp
EXEC_FILE = main

TN = 16
DATA_FOLDER = data
IN_FILE = $(DATA_FOLDER)/data1
OUT_FILE = $(DATA_FOLDER)/data1_out
ANS_FILE = $(DATA_FOLDER)/data1_answer

all: $(RAW_FILES)
	$(CXX) $(ARG) $(EXEC_FILE) $^

.PHONY: test clean

test: all
	./$(EXEC_FILE) $(TN) $(OUT_FILE) < $(IN_FILE)
	diff $(OUT_FILE) $(ANS_FILE)

clean:
	rm -rf $(EXEC_FILE)
