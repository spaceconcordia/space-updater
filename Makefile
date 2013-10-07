CXX = g++
MICRO_BLAZE = microblaze-unknown-linux-gnu-g++
CPPUTEST_HOME = /home/jamg85/CppUTest
UPDATER_PATH  = /home/jamg85/Concordia/Space/Updater
SPACE_LIB = /home/jamg85/Git/space-lib

CPPFLAGS += -Wall -I$(CPPUTEST_HOME)/include
CXXFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h
CFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorMallocMacros.h
LD_LIBRARIES = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt
MICRO_FLAGS = -mcpu=v8.10.a -mxl-barrel-shift -mxl-multiply-high -mxl-pattern-compare -mno-xl-soft-mul -mno-xl-soft-div -mxl-float-sqrt -mhard-float -mxl-float-convert -ffixed-r31 --sysroot /usr/local/lib/mbgcc/microblaze-unknown-linux-gnu/sys-root
INCLUDE = -I$(UPDATER_PATH)/include -I$(SPACE_LIB)/inc -L$(SPACE_LIB)/lib

#
# 	Compilation for CppUTest
#

test : AllTests

fileIO.o: src/fileIO.cpp include/fileIO.h
	$(CXX) $(INCLUDE) -c $< -o $@

ProcessUpdater.o : src/ProcessUpdater.cpp include/ProcessUpdater.h include/fileIO.h
	$(CXX) $(INCLUDE) -c $< -o $@
		
Updater.o :src/Updater.cpp include/Updater.h include/ProcessUpdater.h include/fileIO.h
	$(CXX) $(INCLUDE) -c $< -o $@ 

AllTests: src/AllTests.cpp tests/Updater-test.cpp fileIO.o ProcessUpdater.o Updater.o
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE) -I/home/jamg85/Concordia/Space/Updater -o AllTests $^ $(LD_LIBRARIES) -lshakespeare

allPC: PC-Updater

PC-Updater : src/PC.cpp fileIO.o ProcessUpdater.o Updater.o
	$(CXX) $(INCLUDE) $^ -o PC-Updater -lshakespeare

#
#	Compilation for the Q6. Microblaze.
#

Q6 : Updater-Q6

fileIO-Q6.o: src/fileIO.cpp include/fileIO.h
	$(MICRO_BLAZE) $(MICRO_FLAGS) $(INCLUDE) -c $< -o $@

ProcessUpdater-Q6.o : src/ProcessUpdater.cpp include/ProcessUpdater.h include/fileIO.h
	$(MICRO_BLAZE) $(MICRO_FLAGS) $(INCLUDE) -c $< -o $@
		
Updater-Q6.o : src/Updater.cpp include/Updater.h include/ProcessUpdater.h include/fileIO.h
	$(MICRO_BLAZE) $(MICRO_FLAGS) $(INCLUDE) -c $< -o $@

Updater-Q6: src/Q6.cpp fileIO-Q6.o ProcessUpdater-Q6.o Updater-Q6.o
	$(MICRO_BLAZE) $(MICRO_FLAGS) $(INCLUDE) -o Updater-Q6 $^ -lshakespeare-mbcc

#
# CleanUp
#
clean :
	rm -f *.o *~

cleanAll:
	rm -f *.o *~
	rm -f AllTests PC-Updater Updater-Q6
