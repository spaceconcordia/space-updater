CXX = g++
MICRO_BLAZE = microblaze-unknown-linux-gnu-g++
CPPUTEST_HOME = /home/jamg85/CppUTest




CPPFLAGS += -Wall -I$(CPPUTEST_HOME)/include
CXXFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h
CFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorMallocMacros.h
LD_LIBRARIES = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt
MICRO_FLAGS = -mcpu=v8.10.a -mxl-barrel-shift -mxl-multiply-high -mxl-pattern-compare -mno-xl-soft-mul -mno-xl-soft-div -mxl-float-sqrt -mhard-float -mxl-float-convert -ffixed-r31 --sysroot /usr/local/lib/mbgcc/microblaze-unknown-linux-gnu/sys-root

all:AllTests

fileIO.o: fileIO.cpp fileIO.h
	$(CXX) -c $< -o $@

ProcessUpdater.o : ProcessUpdater.cpp ProcessUpdater.h fileIO.h
	$(CXX) -c $< -o $@
		
Updater.o :Updater.cpp Updater.h ProcessUpdater.h fileIO.h
	$(CXX) -c $< -o $@

AllTests: AllTests.cpp tests/Updater-test.cpp fileIO.o ProcessUpdater.o Updater.o
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) -I/home/jamg85/Concordia/Space/Updater -o AllTests $^ $(LD_LIBRARIES) 

clean:
	rm -f *.o 

allPC: PC-Updater

PC-Updater : PC.cpp fileIO.o ProcessUpdater.o Updater.o
	$(CXX) $^ -o PC-Updater

allQ6:Q6-Updater

fileIO-Q6.o: fileIO.cpp fileIO.h
	$(MICRO_BLAZE) $(MICRO_FLAGS)  -c $< -o $@

ProcessUpdater-Q6.o : ProcessUpdater.cpp ProcessUpdater.h fileIO.h
	$(MICRO_BLAZE) $(MICRO_FLAGS)  -c $< -o $@
		
Updater-Q6.o :Updater.cpp Updater.h ProcessUpdater.h fileIO.h
	$(MICRO_BLAZE) $(MICRO_FLAGS)  -c $< -o $@

Q6-Updater: Q6.cpp fileIO-Q6.o ProcessUpdater-Q6.o Updater-Q6.o
	$(MICRO_BLAZE) $(MICRO_FLAGS) -o Q6-Updater $^ 
