# By Tiago Franca | Miguel Franca
# Makefile to compile all .cpp files in a given folder where it is run.
# Uses dependency files and stored .d and .o in $MAIN_OBJ_DIR folder
# To add extra folders with .h or .cpp, define paths in variable $LIB_DIRS, as shown below

##############################################################################################
###################################### USER DEFINITIONS ######################################
##############################################################################################

# LIB_DIRS 	:= 	
# LIB_DIRS    := ../../GraphicsFramework/GraphicsFramework \
# 			   ../../GraphicsFramework/GraphicsFramework/sfml \
# 			   ../../GraphicsFramework/GraphicsFramework/holders \
# 			   ../../GraphicsFramework
				
# USER_FLAGS  := -std=c++11 -Wno-unused-parameter -Wno-unused-function -fopenmp

# ROOT_LD 	:= $(shell root-config --libs)
# ROOT_CXX 	:= $(shell root-config --cflags)
# SFML_ROOT must be defined on the user's machine
# SFML_LD 	:= -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -L$(SFML_ROOT)/lib
# SFML_CXX		:= -I$(SFML_ROOT)/include

# VALGRIND	:= -g -O0
VALGRIND	?= -O2

# KOKKOS_DEVICES := "Cuda, OpenMP"
# OMP_NUM_THREADS ?= 4

# J = 6 # runs in parallel

##############################################################################################
##############################################################################################
##############################################################################################

# reminders:
# $@ - left target being called
# $< - first dependency
# $^ - all dependencies
# %  - any pattern
# $$ - holds the $ pattern without expanding it
# |  - pipe: pre-requisites
# =  - lazy set (expanded every time used)
# := - immediate set (expanded immediately)
# ?= - set if not defined already
# += - append to existing
# @  - turn off output of that command


# below we set some default variables

ifneq (,$(findstring Cuda,$(KOKKOS_DEVICES)))
CXX = ${KOKKOS_PATH}/bin/nvcc_wrapper
KOKKOS_CUDA_OPTIONS := "enable_lambda"
KOKKOS_CXX_STANDARD := $(subst -std=,,$(filter -std=c++%,$(USER_FLAGS))) 
else
CXX := g++
endif
LINK := $(CXX)


MAIN_OBJ_DIR ?= bin
USER_FLAGS	 += -Wall -Wextra 

MF := Makefile
ifneq ($(MAKEFILE_DIR),)
MF += $(MAKEFILE_DIR)/Makefile
endif

##############################################################################################

# set paralellism
# set J variable to change it
OS := $(shell uname)
ifeq ($J,)

ifeq ($(OS),Linux)
  NPROCS := $(shell grep -c ^processor /proc/cpuinfo)
else ifeq ($(OS),Darwin)
  NPROCS := $(shell system_profiler | awk '/Number of CPUs/ {print $$4}{next;}')
endif # $(OS)
JOBS = $(shell echo $$(( $(NPROCS) / 2 )))

else
  JOBS := $J
endif # $J

# always run in parallel
MAKEFLAGS += --jobs=$(JOBS)

##############################################################################################
########################################## MAKEFILE ##########################################
##############################################################################################

WHITE  = "\033[0m"
GREY   = "\033[1;37m"
LBLUE  = "\033[1;36m"
PURPLE = "\033[1;35m"
BLUE   = "\033[1;34m"
YELLOW = "\033[1;33m"
GREEN  = "\033[1;32m"
RED    = "\033[1;31m"

ifneq (OMP_NUM_THREADS,)
  export OMP_NUM_THREADS
endif

ECHO?=@ # set this to null on the command line to increase verbosity

# executable has folder name
EXECUTABLE 		:= $(shell basename "$(CURDIR)").exe

INCLUDES 		:= $(addprefix -I,$(LIB_DIRS))

# same 'bin' folder for all libs
LIBS_OBJ_DIR 	:= $(addsuffix /$(MAIN_OBJ_DIR),$(LIB_DIRS))

MAIN_CPP_FILES 	:= $(wildcard *.cpp)
MAIN_OBJ_FILES 	:= $(addprefix $(MAIN_OBJ_DIR)/,$(notdir $(MAIN_CPP_FILES:.cpp=.o))) 
MAIN_DEP_FILES 	:= $(subst .o,.d,$(MAIN_OBJ_FILES))
LIBS_CPP_FILES 	:= $(foreach lib,$(LIB_DIRS),$(wildcard $(lib)/*.cpp))
LIBS_OBJ_FILES 	:= $(foreach file,$(LIBS_CPP_FILES), $(addprefix $(dir $(file)), $(addprefix $(MAIN_OBJ_DIR)/, $(notdir $(file:.cpp=.o)))))
LIBS_DEP_FILES 	:= $(subst .o,.d,$(LIBS_OBJ_FILES))
#LIBS_OBJ_FILES - replace .cpp by .o and add bin/ before the basename (but after the lib path)

LINK_FLAGS := $(USER_FLAGS) $(VALGRIND) $(ROOT_LD) $(SFML_LD)
CXX_FLAGS  := $(USER_FLAGS) $(VALGRIND) $(ROOT_CD) $(SFML_CXX) $(INCLUDES)

default: $(EXECUTABLE)

ifneq ($(KOKKOS_DEVICES),)

  # define folder where to build Kokkos
  ifneq (,$(findstring Cuda,$(KOKKOS_DEVICES)))
    ifneq (,$(findstring OpenMP,$(KOKKOS_DEVICES)))
      BUILD_DIR := $(KOKKOS_PATH)/build/cuda_omp
    else
      BUILD_DIR := $(KOKKOS_PATH)/build/cuda
    endif
  else
    ifneq (,$(findstring OpenMP,$(KOKKOS_DEVICES)))
      BUILD_DIR := $(KOKKOS_PATH)/build/omp
    else
      $(info What device are you using? Not sure I'll do this right.)
      BUILD_DIR := $(KOKKOS_PATH)/build/default
    endif
  endif

  KOKKOS_AUX_MAKEFILE = Makefile_kokkos_build
  KOKKOS_LIB_FILE = $(BUILD_DIR)/$(KOKKOS_LINK_DEPENDS)
  MF += $(KOKKOS_PATH)/$(KOKKOS_AUX_MAKEFILE)

include $(KOKKOS_PATH)/Makefile.kokkos
# automatic files that Kokkos creates need to be removed
$(shell rm -f KokkosCore_config.h KokkosCore_config.tmp)

  # remove standard from CXX flags to avoid duplication warning
  # Kokkos uses --std= , instead of -std=
  USER_STANDARD := $(filter -std=c++%,$(USER_FLAGS))
  ifneq (USER_STANDARD,)
    KOKKOS_CXXFLAGS := $(subst -$(USER_STANDARD),,$(KOKKOS_CXXFLAGS))
  endif

  LINK_FLAGS     += $(KOKKOS_LDFLAGS) $(KOKKOS_LIBS)      -L$(BUILD_DIR)
  CXX_FLAGS      += $(KOKKOS_CPPFLAGS) $(KOKKOS_CXXFLAGS) -I$(BUILD_DIR) -DKOKKOS # KOKKOS becomes a macro we can use to check if it is being used
  LIBS_OBJ_FILES := $(KOKKOS_LIB_FILE) $(LIBS_OBJ_FILES) # prepend Kokkos .a
  EXTRA_CLEAN    += clean-kokkos

# $(info $(KOKKOS_LDFLAGS))
# $(info $(KOKKOS_LIBS))
# $(info $(KOKKOS_CPPFLAGS))
# $(info $(KOKKOS_CXXFLAGS))

$(KOKKOS_LIB_FILE):
	$(ECHO)mkdir -p $(BUILD_DIR)
	$(ECHO)$(MAKE) -f ../../$(KOKKOS_AUX_MAKEFILE) -C $(BUILD_DIR) --no-print-directory	KOKKOS_DEVICES=$(KOKKOS_DEVICES) USER_FLAGS="$(USER_FLAGS)"  

clean-kokkos:
	@echo  cleaning $(RED) Kokkos $(WHITE) for $(PURPLE) $(KOKKOS_DEVICES) $(WHITE)
	$(ECHO)rm -rf $(BUILD_DIR)

endif

ifneq ($(filter -fopenmp,$(CXX_FLAGS)),)
  export OMP_PROC_BIND=spread
  export OMP_PLACES=threads
  ifeq ($(OMP_NUM_THREADS),)
    export OMP_NUM_THREADS=$(NPROCS)
  endif
endif

$(EXECUTABLE): $(LIBS_OBJ_FILES) $(MAIN_OBJ_FILES)
	@echo compiling $(BLUE) $(notdir $@) $(WHITE) depending $(GREEN) $(notdir $^) $(WHITE)
	@echo
	$(ECHO)$(LINK) $^ -o $@ $(LINK_FLAGS)

$(MAIN_OBJ_DIR)/%.o: %.cpp $(MF) | $(MAIN_OBJ_DIR)
	@echo compiling $(GREEN) $(notdir $@) $(WHITE) depending $(YELLOW) $(notdir $<) $(WHITE)
	@# -MMD creates dependency files. -MF name.d forces the name of the depencency file. Typically not needed
	@# but for Kokkos the nvcc compiler was not producing it without this specification
	$(ECHO)$(CXX) -o $@ -c $< $(CXX_FLAGS) -MMD -MF $(subst .o,.d,$@)

# function that needed to be defined so that two patterns can be caught and used in dependencies, $1 and $2 (all except the 'bin' part)
define LIBS_OBJ_RULE
$1/$$(MAIN_OBJ_DIR)/$2.o : $1/$2.cpp $(MF) | $1/$$(MAIN_OBJ_DIR)
	@echo compiling $$(GREEN) $$(notdir $$@) $$(WHITE) depending $$(YELLOW) $$(notdir $$<) $$(WHITE)
	$(ECHO)$$(CXX) -o $$@ -c $$< $$(CXX_FLAGS) -MMD -MF $$(subst .o,.d,$$@)
endef

# function that removes the 'bin' part and the file name from a path
# eg: abc/bin/f.cpp to abc
define SPLIT_LIB_DIR
$(subst /$(MAIN_OBJ_DIR)/,,$(dir $1))
endef

# definition of .o target rule for all object files in LIBS_OBJ_FILES, by means of the function LIBS_OBJ_RULE
$(foreach obj,$(LIBS_OBJ_FILES),$(eval $(call LIBS_OBJ_RULE,$(call SPLIT_LIB_DIR,$(obj)),$(subst .o,,$(notdir $(obj))))))

# dependency files 
-include $(MAIN_DEP_FILES)
-include $(LIBS_DEP_FILES)

# bin directories
$(MAIN_OBJ_DIR) $(LIBS_OBJ_DIR):
	$(ECHO)mkdir -p $@
	@echo $(LBLUE) created $@ $(WHITE)

# cleaners
c: clean
clean:
	$(ECHO)rm -rf $(MAIN_OBJ_DIR)
	$(ECHO)rm -f $(EXECUTABLE)
	@echo  cleaning $(RED) $(notdir $(EXECUTABLE)) $(WHITE)
	@echo  cleaning $(RED) $(MAIN_OBJ_DIR) $(WHITE)

rc: realclean

ifeq ($(LIBS_OBJ_DIR),)
realclean: $(EXTRA_CLEAN) clean
else
realclean: $(EXTRA_CLEAN) clean
	$(ECHO)rm -rf $(LIBS_OBJ_DIR)
	@echo  cleaning $(RED) $(LIBS_OBJ_DIR) $(WHITE)
endif

# runners
r: run
run: default
	$(ECHO)./$(EXECUTABLE)

t: time
time: default
	$(ECHO)time ./$(EXECUTABLE)

v: valgrinder
valgrinder: default
	$(ECHO)valgrind --leak-check=yes ./$(EXECUTABLE)

bt: backtrace
backtrace: default
	@echo "" | gdb $(EXECUTABLE) -ex run -ex bt | grep "#"
# 	@echo If gdb finds no error, make will give an error. Don\'t worry.

