CXX = g++
MICROCC=microblazeel-xilinx-linux-gnu-g++
BB = arm-linux-gnueabi-g++

CPPUTEST_HOME = ../CppUTest
UPDATER_PATH  = ../space-updater
UTLS_DIR = 	../space-lib/utls
SPACE_LIB = ../space-lib
SPACE_SCRIPT = ../space-script

CPPFLAGS += -Wall -I$(CPPUTEST_HOME)/include
CXXFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h
CFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorMallocMacros.h
LD_LIBRARIES = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt
MICROCFLAGS=-mcpu=v8.40.b -mxl-barrel-shift -mxl-multiply-high -mxl-pattern-compare -mno-xl-soft-mul -mno-xl-soft-div -mxl-float-sqrt -mhard-float -mxl-float-convert -mlittle-endian -Wall

INCLUDE = -I$(UPDATER_PATH)/include -I$(SPACE_LIB)/shakespeare/inc -I$(SPACE_LIB)/include -L$(SPACE_LIB)/lib -L$(SPACE_LIB)/shakespeare/lib

make_dir:
	mkdir -p bin
LIBRARIES = -lshakespeare -lcs1_utls
LIBRARIESMBCC = -lshakespeare-mbcc -lcs1_utlsQ6

#
# 	Compilation for CppUTest
#

test : make_dir AllTests

bin/Date.o : $(UTLS_DIR)/src/Date.cpp $(UTLS_DIR)/include/Date.h $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) -I$(UTLS_DIR)/include/ $(CXXFLAGS) -c $(UTLS_DIR)/src/Date.cpp -o $@

bin/fileIO.o: src/fileIO.cpp include/fileIO.h
	$(CXX) $(INCLUDE) -c $< -o $@

bin/ProcessUpdater.o : src/ProcessUpdater.cpp include/ProcessUpdater.h include/fileIO.h
	$(CXX) $(INCLUDE) -c $< -o $@

bin/Updater.o : src/Updater.cpp include/Updater.h include/ProcessUpdater.h include/fileIO.h
	$(CXX) $(INCLUDE) -c $< -o $@

AllTests: src/AllTests.cpp tests/Updater-test.cpp ./bin/fileIO.o ./bin/ProcessUpdater.o ./bin/Updater.o
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE) -o AllTests $^ $(LD_LIBRARIES) $(LIBRARIES)

buildBin: make_dir bin/fileIO.o bin/Date.o bin/ProcessUpdater.o bin/Updater.o PC-Updater

PC-Updater : src/PC.cpp ./bin/fileIO.o ./bin/ProcessUpdater.o ./bin/Updater.o
	$(CXX) $(INCLUDE) $^ -o ./bin/PC-Updater $(LIBRARIES)

#
#	Compilation for the Q6. Microblaze.
#

buildQ6 : make_dir bin/fileIO-Q6.o bin/ProcessUpdater-Q6.o bin/Updater-Q6.o Updater-Q6

bin/fileIO-Q6.o: src/fileIO.cpp include/fileIO.h
	$(MICROCC) $(MICROCFLAGS) $(INCLUDE) -c $< -o $@

bin/ProcessUpdater-Q6.o : src/ProcessUpdater.cpp include/ProcessUpdater.h include/fileIO.h
	$(MICROCC) $(MICROCFLAGS) $(INCLUDE) -c $< -o $@

bin/Updater-Q6.o : src/Updater.cpp include/Updater.h include/ProcessUpdater.h include/fileIO.h
	$(MICROCC) $(MICROCFLAGS) $(INCLUDE) -c $< -o $@

Updater-Q6: src/Q6.cpp ./bin/fileIO-Q6.o ./bin/ProcessUpdater-Q6.o ./bin/Updater-Q6.o
	$(MICROCC) $(MICROCFLAGS) $(INCLUDE) -o ./bin/Updater-Q6 $^ $(LIBRARIESMBCC)

#
#	Compilation for Beaglebone Black
#

buildBB : make_dir bin/fileIO-bb.o bin/ProcessUpdater-bb.o bin/Updater-bb.o bin/Updater-bb

bin/fileIO-bb.o: src/fileIO.cpp include/fileIO.h
	$(BB) $(INCLUDE) $(INCLUDE) -c $< -o $@

bin/ProcessUpdater-bb.o : src/ProcessUpdater.cpp include/ProcessUpdater.h include/fileIO.h
	$(BB) $(INCLUDE) $(INCLUDE) -c $< -o $@

bin/Updater-bb.o : src/Updater.cpp include/Updater.h include/ProcessUpdater.h include/fileIO.h
	$(BB) $(INCLUDE) $(INCLUDE) -c $< -o $@

Updater-bb: src/Q6.cpp ./bin/fileIO-bb.o ./bin/ProcessUpdater-bb.o ./bin/Updater-bb.o
	$(BB) $(INCLUDE) $(INCLUDE) -o ./bin/Updater-bb $^ -lshakespeare-BB
#
# CleanUp
#
clean :
	rm -f *.o *~ ./bin/*.o

cleanAll:
	rm -f *.o *~ ./bin/*.o
	rm -f AllTests PC-Updater Updater-Q6
