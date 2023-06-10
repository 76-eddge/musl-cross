
SRC_DIR :=.
BIN_DIR :=bin
CC_DIR :=$(BIN_DIR)
TESTS :=$(CC_DIR)/test.exe $(CC_DIR)/test.so $(BIN_DIR)/testSo.exe $(CC_DIR)/testUuid.so $(BIN_DIR)/testUuidSo.exe
PRE_SO_TEST :=$(PRE_TEST)
LIBS :=-ldl -lm -lrt -luuid -pthread
MD :=mkdir -p
PATCH_ELF :=$(if $(findstring /, $(CXX)), $(dir $(CXX)))patchelf
CC_CXX :=$(CXX)
CC_CXXFLAGS :=$(CXXFLAGS)
CC_LIBS :=-luuid -pthread
CC_SO_LIBS :=$(CC_LIBS) -lstdc++ -lgabi

version:
	$(CC_CXX) --version

tests: $(TESTS)

run-%: export LD_LIBRARY_PATH :=$(CC_DIR)
run-%: $(BIN_DIR)/%.exe
	$(if $(filter %So, $*),$(PRE_SO_TEST),$(PRE_TEST)) $<

run-tests: $(TESTS) $(patsubst %.exe, run-%, $(notdir $(filter %.exe, $(TESTS))))

# System built
$(BIN_DIR):
	$(MD) $(BIN_DIR)

$(BIN_DIR)/%So.exe: $(SRC_DIR)/%SoExe.cxx $(CC_DIR)/%.so | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $< $(CC_DIR)/$*.so $(filter-out -luuid, $(LIBS))

# Toolchain built
ifneq ($(BIN_DIR),$(CC_DIR))
$(CC_DIR):
	$(MD) $(CC_DIR)

$(BIN_DIR)/test.exe: $(CC_DIR)/test.exe
	ln $< $@
endif

$(CC_DIR)/%.so: $(SRC_DIR)/%So.cxx | $(CC_DIR)
	$(CC_CXX) -shared -fPIC $(CC_CXXFLAGS) -o $@ $< $(CC_SO_LIBS)
	$(PATCH_ELF) --remove-needed libc.so $@

$(CC_DIR)/%.exe: $(SRC_DIR)/%Exe.cxx | $(CC_DIR)
	$(CC_CXX) -static $(CC_CXXFLAGS) -o $@ $< $(CC_LIBS)
