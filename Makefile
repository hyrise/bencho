BENCHO_SOURCE_DIR = ./src
BENCHO_INCLUDE_DIR = ./include
BENCHO_BUILD_BASE_DIR = ./build

BENCHO_LIB_NAME = bencho
BENCHO_LIB_FULL_NAME = lib$(BENCHO_LIB_NAME).a
BENCHO_LIB_DIR = $(BUILD_DIR)/lib

CC = clang++
LIB = ar cr


BUILD_FLAGS = -I$(BENCHO_SOURCE_DIR) -Wno-deprecated
LINKER_FLAGS = -lpthread -ldl
INCLUDE = -I$(BENCHO_INCLUDE_DIR) $(shell python-config --includes)
VERSION=$(shell git describe --tags)



-include settings.conf
 
ifeq ($(PROD), 1)
	BUILD_FLAGS += -O3 -finline-functions -DNDEBUG -D USE_TRACE -g -pipe
	BUILD_DIR = $(BENCHO_BUILD_BASE_DIR)/prod
else
	BUILD_FLAGS += -O0 -g2 -pipe
	CXX_DEBUG += -Wno-long-long #-Wall -pedantic
	BUILD_DIR = $(BENCHO_BUILD_BASE_DIR)/debug
endif
 
ifeq ($(PAPI), 1)
	BUILD_FLAGS += -D USE_PAPI_TRACE
	LINKER_FLAGS += -lpapi 
endif

ifneq ($(VERBOSE_BUILD), 1)
	echo_cmd = @echo "$(1)";
else # Verbose output
	echo_cmd =
endif

ifeq ($(GNUPLOT), 1)
	BUILD_FLAGS += -D GNUPLOT
endif

ifeq ($(PYPLOT), 1)
	BUILD_FLAGS += -D PYPLOT
endif

ifeq ($(RPLOT), 1)
	BUILD_FLAGS += -D RPLOT
endif


full_build_dir := $(BUILD_DIR)
src_dir := $(BENCHO_SOURCE_DIR)
libbencho := $(BENCHO_LIB_DIR)/$(BENCHO_LIB_FULL_NAME)

 
# Objects and dependencies
sources := $(shell find $(src_dir) -type f -name "*.cpp" -and -not -path "*/benchmarks/*" -and -not -path "*/tools/*")
objects := $(subst $(src_dir),$(BUILD_DIR),$(subst .cpp,.o,$(sources)))
dependencies := $(subst .o,.d,$(objects))
 
 
.PHONY: dirs help clean config echo_config
 
all: libbencho
 
config:
	@./configure.sh
 
echo_config:
	@./configure.sh echo
 
settings.conf:
	@./configure.sh
 
# Bencho-library
libbencho: $(libbencho) settings.conf
	 
depend: $(dependencies)
 
objects: $(objects)
 
dirs:
	@mkdir -p $(BUILD_DIR)/lib
	@mkdir -p $(BUILD_DIR)/test
 
$(libbencho): $(objects) settings.conf
	$(MAKE) echo_config
	$(call echo_cmd,LIB $@) $(LIB) $@ $(objects)
 
$(objects): $(BUILD_DIR)/%.o: $(src_dir)/%.cpp $(BENCHO_INCLUDE_DIR)/%.h $(BUILD_DIR)/%.d settings.conf
	$(call echo_cmd,CC $@) $(CC) $(BUILD_FLAGS) $(CXX_DEBUG) -o"$@" -c "$<" $(INCLUDE) -D BENCHO_DIR=$(CURDIR)
 
$(dependencies): $(BUILD_DIR)/%.d: $(src_dir)/%.cpp
	$(call echo_cmd,DEPEND $@) $(CC) -MM -MT $@ $(BUILD_FLAGS) $(CXX_DEBUG) $< > $@ $(INCLUDE)

#cleaning up
clean:
	$(call echo_cmd,REMOVE $(BENCHO_BUILD_BASE_DIR)) rm -rf $(BENCHO_BUILD_BASE_DIR)
	@rm -rf benchmarks/*.pyc


 
-include $(dependencies)
-include dirs # needed to create dirs BEFORE trying to create dependencies
