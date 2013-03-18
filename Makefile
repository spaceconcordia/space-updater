CXX = g++
CPPUTEST_HOME = /home/jamg85/CppUTest




CPPFLAGS += -Wall -I$(CPPUTEST_HOME)/include
CXXFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h
CFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorMallocMacros.h
LD_LIBRARIES = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt

all:AllTests

fileIO.o: fileIO.cpp fileIO.h
	$(CXX) -c $< -o $@

ProcessUpdater.o : ProcessUpdater.cpp ProcessUpdater.h fileIO.h
	$(CXX) -c $< -o $@
		
Updater.o :Updater.cpp Updater.h ProcessUpdater.h fileIO.h
	$(CXX) -c $< -o $@

AllTests: AllTests.cpp fileIO.o ProcessUpdater.o Updater.o
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) -I/home/jamg85/Concordia/Space/Updater -o AllTests $^ tests/Updater-test.cpp $(LD_LIBRARIES) 

clean:
	rm *.o
