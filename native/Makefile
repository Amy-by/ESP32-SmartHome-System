CXX := g++
CXXFLAGS := -std=c++17 -O2 -I../src -I../src/core -I./include
LDFLAGS :=
SRC := ../src/core/DataProcessor.cpp main.cpp
BUILD := build
BIN := $(BUILD)/native_app
.PHONY: all run clean
all: $(BIN)
$(BUILD):
	@mkdir -p $(BUILD)
$(BIN): $(BUILD) $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN) $(LDFLAGS)
run: all
	@$(BIN)
clean:
	@rm -rf $(BUILD)
