##############################################################################################
###################################### USER DEFINITIONS ######################################
##############################################################################################

MAKEFILE_DIR = ..

LIB_DIRS 	:= 	./Tools \
				../../GraphicsFramework/GraphicsFramework \
				../../GraphicsFramework/GraphicsFramework/sfml \
				../../GraphicsFramework/GraphicsFramework/holders \
				../../GraphicsFramework \
				
USER_FLAGS  := -std=c++11 -Wno-unused-parameter -Wno-unused-function -fopenmp

# SFML_ROOT must be defined on the user's machine
SFML_LD 	:= -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -L$(SFML_ROOT)/lib
SFML_CXX	:= -I$(SFML_ROOT)/include

VALGRIND	= -O2

KOKKOS_DEVICES := "Cuda, OpenMP"
# OMP_NUM_THREADS ?= 4

# J = 6 # runs in parallel

include $(MAKEFILE_DIR)/Makefile