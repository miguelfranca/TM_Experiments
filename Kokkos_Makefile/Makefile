# By Tiago Franca | Miguel Franca
# Makefile to compile all .cpp files in a given folder where it is run.
# Uses dependency files and stored .d and .o in $MAIN_OBJ_DIR folder
# To add extra folders with .h or .cpp, define paths in variable $LIB_DIRS, as shown below

##############################################################################################
###################################### USER DEFINITIONS ######################################
##############################################################################################

# LIB_DIRS 	:= 	
				
USER_FLAGS  := -std=c++11 -Wno-unused-parameter -Wno-unused-function -fopenmp

# ROOT_LD 	:= $(shell root-config --libs)
# ROOT_CXX 	:= $(shell root-config --cflags)
# SFML_ROOT must be defined on the user's machine
SFML_LD 	:= -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -L$(SFML_ROOT)/lib
SFML_CXX		:= -I$(SFML_ROOT)/include

# VALGRIND	:= -g -O0
VALGRIND	:= -O2

# KOKKOS_DEVICES := "OpenMP"

# always run in parallel
MAKEFLAGS += --jobs=4

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

EXTRA_DEPENDS := Makefile
ifneq ($(MAIN_DIR),)
EXTRA_DEPENDS += $(MAIN_DIR)/Makefile
endif

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

#executable has folder name
EXECUTABLE 		:= $(shell basename "$(CURDIR)").exe

INCLUDES 		:= $(addprefix -I,$(LIB_DIRS))

#same 'bin' folder for all libs
LIBS_OBJ_DIR 	:= $(addsuffix /$(MAIN_OBJ_DIR),$(LIB_DIRS))

MAIN_CPP_FILES 	:= $(wildcard *.cpp)
MAIN_OBJ_FILES 	:= $(addprefix $(MAIN_OBJ_DIR)/,$(notdir $(MAIN_CPP_FILES:.cpp=.o))) 
MAIN_DEP_FILES 	:= $(subst .o,.d,$(MAIN_OBJ_FILES))
LIBS_CPP_FILES 	:= $(foreach lib,$(LIB_DIRS),$(wildcard $(lib)/*.cpp))
LIBS_OBJ_FILES 	:= $(foreach file,$(LIBS_CPP_FILES), $(addprefix $(dir $(file)), $(addprefix $(MAIN_OBJ_DIR)/, $(notdir $(file:.cpp=.o)))))
LIBS_DEP_FILES 	:= $(subst .o,.d,$(LIBS_OBJ_FILES))
#LIBS_OBJ_FILES - replace .cpp by .o and add bin/ before the basename (but after the lib path)

LINK_FLAGS := $(USER_FLAGS) $(VALGRIND) $(ROOT_LD) $(SFML_LD)
CXX_FLAGS  := $(USER_FLAGS) $(VALGRIND) $(ROOT_CD) $(SFML_CXX) $(INCLUDES) -MMD

default: $(EXECUTABLE)

ifneq ($(KOKKOS_DEVICES),)
include $(KOKKOS_PATH)/Makefile.kokkos
LINK_FLAGS     += $(KOKKOS_LDFLAGS) $(KOKKOS_LIBS)
CXX_FLAGS      += $(KOKKOS_CPPFLAGS) $(KOKKOS_CXXFLAGS)
LIBS_OBJ_FILES += $(KOKKOS_LINK_DEPENDS)
EXTRA_DEPENDS  += $(KOKKOS_CPP_DEPENDS)
EXTRA_CLEAN    += kokkos-clean
endif

$(EXECUTABLE): $(MAIN_OBJ_FILES) $(LIBS_OBJ_FILES)
	@echo compiling $(BLUE) $(notdir $@) $(WHITE) depending $(GREEN) $(notdir $^) $(WHITE)
	@echo
	@$(LINK) $^ -o $@ $(LINK_FLAGS)

$(MAIN_OBJ_DIR)/%.o: %.cpp $(EXTRA_DEPENDS) | $(MAIN_OBJ_DIR)
	@echo compiling $(GREEN) $(notdir $@) $(WHITE) depending $(YELLOW) $(notdir $<) $(WHITE)
	@$(CXX) -o $@ -c $< $(CXX_FLAGS)

#function that needed to be defined so that two patterns can be caught and used in dependencies, $1 and $2 (all except the 'bin' part)
define LIBS_OBJ_RULE
$1/$$(MAIN_OBJ_DIR)/$2.o : $1/$2.cpp $(EXTRA_DEPENDS) | $1/$$(MAIN_OBJ_DIR)
	@echo compiling $$(GREEN) $$(notdir $$@) $$(WHITE) depending $$(YELLOW) $$(notdir $$<) $$(WHITE)
	@$$(CXX) -o $$@ -c $$< $$(CXX_FLAGS)
endef

#function that removes the 'bin' part and the file name from a path
#eg: abc/bin/f.cpp to abc
define SPLIT_LIB_DIR
$(subst /$(MAIN_OBJ_DIR)/,,$(dir $1))
endef

#definition of .o target rule for all object files in LIBS_OBJ_FILES, by means of the function LIBS_OBJ_RULE
$(foreach obj,$(LIBS_OBJ_FILES),$(eval $(call LIBS_OBJ_RULE,$(call SPLIT_LIB_DIR,$(obj)),$(subst .o,,$(notdir $(obj))))))

-include $(MAIN_DEP_FILES)
-include $(LIBS_DEP_FILES)

# bin directories
$(MAIN_OBJ_DIR) $(LIBS_OBJ_DIR):
	@mkdir -p $@
	@echo $(LBLUE) created $@ $(WHITE)

# cleaners
c: clean
clean: $(EXTRA_CLEAN)
	@rm -rf $(MAIN_OBJ_DIR)
	@rm -f $(EXECUTABLE)
	@echo  cleaning $(RED) $(notdir $(EXECUTABLE)) $(WHITE)
	@echo  cleaning $(RED) $(MAIN_OBJ_DIR) $(WHITE)

rc: realclean

ifeq ($(LIBS_OBJ_DIR),)
realclean: clean
else
realclean: clean
	@rm -rf $(LIBS_OBJ_DIR)
	@echo  cleaning $(RED) $(LIBS_OBJ_DIR) $(WHITE)
endif

# runners
r: run
run: default
	@./$(EXECUTABLE)

t: time
time: default
	@time ./$(EXECUTABLE)

v: valgrinder
valgrinder: $(EXECUTABLE)
	@valgrind --leak-check=yes ./$(EXECUTABLE)

bt: backtrace
backtrace: default
	@echo "" | gdb $(EXECUTABLE) -ex run -ex bt | grep "#"