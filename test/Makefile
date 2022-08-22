
BIN_DIR=.
SRC_DIR=.

tests: $(BIN_DIR)/test.exe $(BIN_DIR)/test.so

version:
	$(CXX) --version

$(BIN_DIR)/%.exe: $(SRC_DIR)/%Exe.cxx
	$(CXX) $(CXXFLAGS) -o $@ $<

$(BIN_DIR)/%.so: $(SRC_DIR)/%So.cxx
	$(CXX) --shared $(CXXFLAGS) -o $@ $<
