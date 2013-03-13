cc = g++
CPPUTEST_HOME = /home/jamg85/CppUTest




CPPFLAGS += -I$(CPPUTEST_HOME)/include
CXXFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h
CFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorMallocMacros.h
LD_LIBRARIES = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt


All: AllTests.cpp
	$(cc) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) -I/home/jamg85/Concordia/Space/Updater -o AllTests AllTests.cpp tests/Updater-test.cpp  fileIO.cpp ProcessUpdater.cpp Updater.cpp $(LD_LIBRARIES) 

	
