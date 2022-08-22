
BIN_DIR=.
SRC_DIR=.

tests: $(BIN_DIR)/test.exe $(BIN_DIR)/test.so $(BIN_DIR)/testSo.exe

version:
	$(CXX) --version

$(BIN_DIR)/%.so: $(SRC_DIR)/%So.cxx
	$(CXX) --shared -static -fPIC $(CXXFLAGS) -o $@ $<

$(BIN_DIR)/%.exe: $(SRC_DIR)/%Exe.cxx
	$(CXX) -static $(CXXFLAGS) -o $@ $<

$(BIN_DIR)/%So.exe: $(SRC_DIR)/%SoExe.cxx $(BIN_DIR)/%.so
	$(CXX) -static $(CXXFLAGS) -o $@ $< -Wl,-Bdynamic $(BIN_DIR)/$*.so -Wl,-Bstatic
