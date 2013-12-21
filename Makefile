CXX = g++
MICROCC=microblazeel-xilinx-linux-gnu-g++
CPPUTEST_HOME = /home/spaceconcordia/space/space-updater
UPDATER_PATH  = /home/spaceconcordia/space/space-updater
SPACE_LIB = /home/spaceconcordia/space/space-lib

CPPFLAGS += -Wall -I$(CPPUTEST_HOME)/include
CXXFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h
CFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorMallocMacros.h
LD_LIBRARIES = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt
MICROCFLAGS=-mcpu=v8.40.b -mxl-barrel-shift -mxl-multiply-high -mxl-pattern-compare -mno-xl-soft-mul -mno-xl-soft-div -mxl-float-sqrt -mhard-float -mxl-float-convert -mlittle-endian -Wall

INCLUDE = -I$(UPDATER_PATH)/include -I$(SPACE_LIB)/shakespeare/inc -L$(SPACE_LIB)/shakespeare/lib

#
# 	Compilation for CppUTest
#

test : AllTests

fileIO.o: src/fileIO.cpp include/fileIO.h
	$(CXX) $(INCLUDE) -c $< -o ./bin/$@

ProcessUpdater.o : src/ProcessUpdater.cpp include/ProcessUpdater.h include/fileIO.h
	$(CXX) $(INCLUDE) -c $< -o ./bin/$@
		
Updater.o :src/Updater.cpp include/Updater.h include/ProcessUpdater.h include/fileIO.h
	$(CXX) $(INCLUDE) -c $< -o ./bin/$@ 

AllTests: src/AllTests.cpp tests/Updater-test.cpp ./bin/fileIO.o ./bin/ProcessUpdater.o ./bin/Updater.o
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE) -o AllTests $^ $(LD_LIBRARIES) -lshakespeare

buildPC: fileIO.o ProcessUpdater.o Updater.o PC-Updater

PC-Updater : src/PC.cpp ./bin/fileIO.o ./bin/ProcessUpdater.o ./bin/Updater.o
	$(CXX) $(INCLUDE) $^ -o ./bin/PC-Updater -lshakespeare

#
#	Compilation for the Q6. Microblaze.
#

buildQ6 : fileIO-Q6.o ProcessUpdater-Q6.o Updater-Q6.o Updater-Q6 

fileIO-Q6.o: src/fileIO.cpp include/fileIO.h
	$(MICROCC) $(MICROCFLAGS) $(INCLUDE) -c $< -o ./bin/$@

ProcessUpdater-Q6.o : src/ProcessUpdater.cpp include/ProcessUpdater.h include/fileIO.h
	$(MICROCC) $(MICROCFLAGS) $(INCLUDE) -c $< -o ./bin/$@
		
Updater-Q6.o : src/Updater.cpp include/Updater.h include/ProcessUpdater.h include/fileIO.h
	$(MICROCC) $(MICROCFLAGS) $(INCLUDE) -c $< -o ./bin/$@

Updater-Q6: src/Q6.cpp ./bin/fileIO-Q6.o ./bin/ProcessUpdater-Q6.o ./bin/Updater-Q6.o
	$(MICROCC) $(MICROCFLAGS) $(INCLUDE) -o ./bin/Updater-Q6 $^ -lshakespeare-mbcc

#
# CleanUp
#
clean :
	rm -f *.o *~ ./bin/*.o

cleanAll:
	rm -f *.o *~ ./bin/*.o
	rm -f AllTests PC-Updater Updater-Q6
