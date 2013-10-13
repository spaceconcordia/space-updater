//******************************************/
//  SPACE CONCORDIA 2013
//
//
//******************************************/
#ifndef PROCESSUPUPDATER_H_
#define PROCESSUPUPDATER_H_
#include <dirent.h>
#include <stdio.h>
class ProcessUpdater{
    private :
        static const int MAX_RETRY = 5;
        const char* newJobPath;
        const char* oldJobPath;
        const char* currentJobPath;

        FILE* log;
    public:
        ProcessUpdater(const char*, const char*, const char*, FILE*);    // newJobPath, currentJobPath, oldJobPath, logStream
        void initialize(const char*, const char*, const char*, FILE*);
        ~ProcessUpdater();

        bool StartUpdate() const;                               // Returns false only if the update fails.

    private :
        bool CheckIfNewJobIsNotEmpty() const; 

};
#endif
