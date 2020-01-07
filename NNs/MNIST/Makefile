# By Tiago Franca | Miguel Franca
# Makefile to compile all .cpp files in a given folder where it is run.
# Uses dependency files and stored .d and .o in $MAIN_OBJ_DIR folder
# To add extra folders with .h or .cpp, define paths in variable $LIB_DIRS, as shown below

##############################################################################################
########## USER DEFINITIONS
##############################################################################################

LIB_DIRS 	:= 	../Lib \
				../Tools \
				../../../Gnuplot
				
# USER_FLAGS  := -Wall -Wextra -std=c++11 -Wno-unused-parameter -Wno-unused-function -fopenmp -pthread
# USER_FLAGS  := -std=c++14 -Wno-unused-parameter -Wno-unused-function -DPROFILING
# USER_FLAGS  := -std=c++14 -Wno-unused-parameter -Wno-unused-function -DEIGEN_NO_DEBUG
USER_FLAGS  := -std=c++14 -Wno-unused-parameter -Wno-unused-function

# ROOT_LD 	:= $(shell root-config --libs)
# ROOT_CC 	:= $(shell root-config --cflags)
# SFML_ROOT must be defined on the user's machine
# SFML_LD 	:= -ltgui -L$(TGUI_ROOT)/build/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -L$(SFML_ROOT)/lib
# SFML_CC		:= -I$(SFML_ROOT)/include -I$(TGUI_ROOT)/include
GNUPLOT_LD  := -lutil -lboost_iostreams -lboost_system -lboost_filesystem

# VALGRIND	:= -g -O0
VALGRIND	:= -O3

##############################################################################################
##############################################################################################

#possibly defined in other makefiles that import this one
ifneq (,$(filter $(CC),cc)) #for some reason CC appears to be already defined as 'cc', C-compiler
CC := g++
endif
ifeq ($(MAIN_OBJ_DIR),)
MAIN_OBJ_DIR 	:= bin
endif
ifeq ($(USER_FLAGS),)
USER_FLAGS		:= -Wall -Wextra -std=c++11
endif

ifeq ($(MAIN_DIR),)
MF := Makefile
else
MF := Makefile $(MAIN_DIR)/Makefile
endif

# old way of doing it -> removed cause a change in the Makefile now forces everything to recompile
# #define variables in runtime to activate ROOT, SFML or VALGRIND (e.g.: makefile r=1, s=1, v=1; or simply set r=1 in your local makefile)
# ifneq ($(or $(r), $(R), $(ROOT)),)
# ROOT_LD 	:= $(shell root-config --libs)
# ROOT_CC 	:= $(shell root-config --cflags)
# # $(info Compiling with ROOT)
# endif

# ifneq ($(or $(s), $(S), $(SFML)),)
# SFML_LD 	:= -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
# # $(info Compiling with SFML)
# endif

# ifneq ($(or $(v), $(V), $(VALGRIND)),)
# VALGRIND	:= -g -O0
# # $(info Compiling with VALGRIND)
# else
# VALGRIND	:= -O2
# endif

##############################################################################################
########## MAKEFILE
##############################################################################################

# reminders:
# $@ - left target being called
# $< - first dependency
# $^ - all dependencies
# %  - any pattern
# $$ - holds the $ pattern without expanding it

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
#LIBS_OBJ_FILES - replace .cpp by .o and add bin/ before then basename (but after the lib path)

LD_FLAGS := $(USER_FLAGS) $(VALGRIND) $(ROOT_LD) $(SFML_LD) $(GNUPLOT_LD)
CC_FLAGS := $(USER_FLAGS) $(VALGRIND) $(ROOT_CD) $(SFML_CC) $(INCLUDES) -MMD

$(EXECUTABLE): $(MAIN_OBJ_FILES) $(LIBS_OBJ_FILES)
	@echo compiling $(BLUE) $(notdir $@) $(WHITE) depending $(GREEN) $(notdir $^) $(WHITE)
	@echo
	@$(CC) $^ -o $@ $(LD_FLAGS)

#print info to user
ifneq ($(or $(r), $(R), $(ROOT)),)
	@echo Compiled using ROOT
endif
ifneq ($(or $(s), $(S), $(SFML)),)
	@echo Compiled using SFML
endif
ifneq ($(or $(v), $(V), $(filter-out -O3,$(VALGRIND))),)
	@echo Compiled using VALGRIND
endif

$(MAIN_OBJ_DIR)/%.o: %.cpp $(MF) | $(MAIN_OBJ_DIR)
	@echo compiling $(GREEN) $(notdir $@) $(WHITE) depending $(YELLOW) $(notdir $<) $(WHITE)
	@$(CC) -o $@ -c $< $(CC_FLAGS)

#function that needed to be defined so that two patterns can be caught and used in dependencies, $1 and $2 (all except the 'bin' part)
define LIBS_OBJ_RULE
$1/$$(MAIN_OBJ_DIR)/$2.o : $1/$2.cpp $(MF) | $1/$$(MAIN_OBJ_DIR)
	@echo compiling $$(GREEN) $$(notdir $$@) $$(WHITE) depending $$(YELLOW) $$(notdir $$<) $$(WHITE)
	@$$(CC) -o $$@ -c $$< $$(CC_FLAGS)
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

$(MAIN_OBJ_DIR) $(LIBS_OBJ_DIR):
	@mkdir -p $@
	@echo $(LBLUE) created $@ $(WHITE)

c: clean
clean:
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