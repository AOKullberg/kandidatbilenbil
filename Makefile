# Default target executed when no arguments are given to make.
default_target: all

#=============================================================================
# VARIABLER

#FLAGS
CXXFLAGS += -std=c++11 -pthread -Wall

#COMPILER
CC = g++

#Includes
INCLUDES = -I/usr/local/include/ -I../include -I/usr/include

#Libraries
LFLAGS = -L/usr/local/lib -L/usr/lib -L/var/lib -L/opt/lib
LIBS = -lopencv_objdetect -lopencv_highgui \
-lopencv_videoio -lopencv_imgcodecs -lopencv_imgproc -lopencv_core\
-lraspicam -lraspicam_cv -lopencv_features2d
NETLIBS = -lsfml-network -lsfml-system -lsfml-graphics
COMPRESS = -lz

#=============================================================================
# Targets

# The main all target
all: sensormodul.o bildbehandling.o kommunikation_klient.o compression.o
	$(CC) $(CXXFLAGS) $(INCLUDES) -o sensor sensormodul.o bildbehandling.o kommunikation_klient.o compression.o $(LFLAGS) $(NETLIBS) $(LIBS) $(COMPRESS)
.PHONY : all

#target to build kommunication
kommunikation: kommunikation_klient.o
	$(CC) $(CXXFLAGS) $(INCLUDES) -o kommunikation kommunikation_klient.o ../compression.o $(LFLAGS) $(NETLIBS) 
.PHONY : kommunikation

#target to build compress
compress: compression.o
	$(CC) $(CXXFLAGS) $(INCLUDES) -o compress compression.o $(LFLAGS) $(COMPRESS) $(NETLIBS) -lopencv_core
.PHONY : compress

#--------------------------------------------------------------------------#
#Object make-rules

#target to build object kommunikation
kommunikation_klient.o:
	$(CC) $(CXXFLAGS) $(INCLUDES) -c Kommunikation/kommunikation_klient.cpp $(LFLAGS) $(NETLIBS) 
.PHONY : kommunikation_klient.cpp.o

#target to build object bildbehandling
bildbehandling.o:
	$(CC) $(CXXFLAGS) $(INCLUDES) -c Bildbehandling/bildbehandling.cpp $(LFLAGS) $(LIBS)
.PHONY : bildbehandling.cpp.o

# target to build object sensor
sensormodul.o:
	$(CC) $(CXXFLAGS) $(INCLUDES) -c sensormodul.cpp $(LFLAGS) $(LIBS)
.PHONY : sensormodul.cpp.o

#target to build object compress
compression.o:
	$(CC) $(CXXFLAGS) $(INCLUDES) -c Compression/compression.cpp $(LFLAGS) $(COMPRESS) $(NETLIBS) -lopencv_core
.PHONY : compression.cpp.o

#clean
clean:
	$(RM) all *.o *~

#zap
zap:
	$(RM) all sensor kommunikation

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... kommunikation"
	@echo "... compress"
	@echo "... zap"
	@echo "... sensormodul.o"
	@echo "... kommunikation_klient.o"
	@echo "... compression.o"
	@echo "... bildbehandling.o"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

