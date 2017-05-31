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
-lraspicam -lraspicam_cv -lopencv_features2d -lwiringPi
NETLIBS = -lsfml-network -lsfml-system -lsfml-graphics
COMPRESS = -lz

#=============================================================================
# Targets

# The main all target
all: sensormodul.o bildbehandling.o kommunikation_klient.o compression.o sensorer.o laser.o hallsensor.o ultraljud.o
	$(CC) $(CXXFLAGS) $(INCLUDES) -o sensor sensormodul.o bildbehandling.o kommunikation_klient.o compression.o sensorer.o laser.o hallsensor.o ultraljud.o $(LFLAGS) $(NETLIBS) $(LIBS) $(COMPRESS)
.PHONY : all

#target to build kommunication
kommunikation: kommunikation_klient.o
	$(CC) $(CXXFLAGS) $(INCLUDES) -o kommunikation kommunikation_klient.o ../compression.o $(LFLAGS) $(NETLIBS) 
.PHONY : kommunikation

#target to build compress
compress: compression.o
	$(CC) $(CXXFLAGS) $(INCLUDES) -o compress compression.o $(LFLAGS) $(COMPRESS) $(NETLIBS) -lopencv_core
.PHONY : compress

#target to build sensors
sensorer: sensorer.o bildbehandling.o
	$(CC) $(CXXFLAGS) $(INCLUDES) -o sensorer sensorer.o bildbehandling.o $(LFLAGS) $(COMPRESS) $(NETLIBS) $(LIBS)
#--------------------------------------------------------------------------#
#Object make-rules

#target to build object kommunikation
kommunikation_klient.o: Kommunikation/kommunikation_klient.cpp
	$(CC) $(CXXFLAGS) $(INCLUDES) -c Kommunikation/kommunikation_klient.cpp $(LFLAGS) $(NETLIBS) 
.PHONY : kommunikation_klient.cpp.o

#target to build object bildbehandling
bildbehandling.o: Bildbehandling/bildbehandling.cpp
	$(CC) $(CXXFLAGS) $(INCLUDES) -c Bildbehandling/bildbehandling.cpp $(LFLAGS) $(LIBS)
.PHONY : bildbehandling.cpp.o

# target to build object sensor
sensormodul.o: sensormodul.cpp
	$(CC) $(CXXFLAGS) $(INCLUDES) -c sensormodul.cpp $(LFLAGS) $(LIBS)
.PHONY : sensormodul.cpp.o

#target to build object compress
compression.o: Compression/compression.cpp
	$(CC) $(CXXFLAGS) $(INCLUDES) -c Compression/compression.cpp $(LFLAGS) $(COMPRESS) $(NETLIBS) -lopencv_core
.PHONY : compression.cpp.o

#target to build object sensorer
sensorer.o: Sensorer/sensorer.cpp
	$(CC) $(CXXFLAGS) $(INCLUDES) -c Sensorer/sensorer.cpp $(LFLAGS) $(LIBS)

#target laser.o
laser.o: Sensorer/laser.cpp
	$(CC) $(CXXFLAGS) $(INCLUDES) -c Sensorer/laser.cpp $(LFLAGS) $(LIBS)

#target ultraljud.o
ultraljud.o: Sensorer/ultraljud.cpp
	$(CC) $(CXXFLAGS) $(INCLUDES) -c Sensorer/ultraljud.cpp $(LFLAGS) $(LIBS)

#target hallsensor.o
hallsensor.o: Sensorer/hallsensor.cpp
	$(CC) $(CXXFLAGS) $(INCLUDES) -c Sensorer/hallsensor.cpp $(LFLAGS) $(LIBS)

#Clean rules------------------------------------------------

#clean sensor
clean_sensor:
	$(RM) all sensorer.o laser.o ultraljud.o hallsensor.o

#clean bildbehandling
clean_bild:
	$(RM) all bildbehandling.o

#clean kommunikation
clean_kom:
	$(RM) all kommunikation_klient.o compression.o

#clean main
clean_main:
	$(RM) all sensormodul.o

#clean all
clean:
	$(RM) all *.o *~

#zap
zap:
	$(RM) all *.o *~ sensor kommunikation

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... kommunikation"
	@echo "... compress"
	@echo "... sensorer"
	@echo "... clean"
	@echo "... clean_sensor"
	@echo "... clean_kom"
	@echo "... clean_bild"
	@echo "... zap"
	@echo "... [filename].o"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

