CXX?=g++
CXXFLAGS:=-O3 -pipe -fPIC -march=native -mtune=native -Wa,-q -std=c++11 -pg -ggdb


BIN_DIR:=bin
BUILD_DIR:=build

LD_LIB_FLAGS=-L./src/ -L./
LD_INC_FLAGS=-I./src/ -I./ -I./src/tinyFA -I./$(BUILD_DIR)

gfak: $(BUILD_DIR)/main.o libgfakluge.a src/tinyFA/pliib.hpp src/tinyFA/tinyfa.hpp | $(BUILD_DIR) $(BIN_DIR)
	+$(CXX) $(CXXFLAGS) -o $@ $< $(LD_LIB_FLAGS) $(LD_INC_FLAGS) -lgfakluge

$(BUILD_DIR)/main.o: src/main.cpp src/gfakluge.hpp src/tinyFA/pliib.hpp src/tinyFA/tinyfa.hpp | $(BUILD_DIR) $(BIN_DIR)
	+$(CXX) $(CXXFLAGS) -c -o $@ $< $(LD_LIB_FLAGS) $(LD_INC_FLAGS) -lgfakluge

libgfakluge.a: $(BUILD_DIR)/gfakluge.o src/tinyFA/pliib.hpp src/tinyFA/tinyfa.hpp | $(BUILD_DIR) $(BIN_DIR)
	ar -rs $@ $<

$(BUILD_DIR)/gfakluge.o: src/gfakluge.cpp src/gfakluge.hpp src/tinyFA/pliib.hpp src/tinyFA/tinyfa.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LD_LIB_FLAGS) $(LD_INC_FLAGS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)


.PHONY: clean all


clean:
	$(RM) gfak
	$(RM) src/*.o
	$(RM) *.a
	$(RM) x.sort
	$(RM) y.sort
	rm -rf $(BIN_DIR)
	rm -rf $(BUILD_DIR)
	$(RM) test_test.gfa
	$(RM) q_test.gfa
