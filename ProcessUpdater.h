//******************************************/
//  SPACE CONCORDIA 2013
//
//
//******************************************/
#ifndef PROCESSUPUPDATER_H_
#define PROCESSUPUPDATER_H_
#include <dirent.h>
class ProcessUpdater{
    private :
        static const int MAX_RETRY = 5;
        const char* newJobPath;
        const char* oldJobPath;
        const char* currentJobPath;

    public:
        ProcessUpdater();
        ProcessUpdater(const char*, const char*, const char*);  // newJobPath, currentJobPath, oldJobPath
        ~ProcessUpdater();

        bool StartUpdate() const;                               // Returns false only if the update fails.

    private :
        bool CheckIfNewJobIsNotEmpty() const; 

};
#endif
