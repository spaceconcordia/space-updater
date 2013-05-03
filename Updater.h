//******************************************/
//  SPACE CONCORDIA 2013
//  
//
//******************************************/
//  StartUpdate() 
//      - checks if there is a Rollback to do and performs it if there is one.
//      - checks if there is an update to do and performs it if there is one.
//      - Returns false only if the update fails.
//
#ifndef UPDATER_H_
#define UPDATER_H_
#include <dirent.h>
class Updater{
    private :
        static const int BUFFER_SIZE = 100;
        static const int BUFFER_NAME_SIZE = 30;
        static const int MAX_RETRY = 5;
        const char* newDirPath;
        const char* oldDirPath;
        const char* currentDirPath;
        const char* rollbackDirPath;
        
        static const char* rollbackFileName;
    public :

        Updater();
        Updater(const char*, const char*, const char*, const char*);
        ~Updater();

        bool StartRollback() const;
        bool StartRollback(const char*) const;
        bool StartUpdate() const;                                       // Returns false only if the update fails.
        bool CheckForUpdate() const;
        bool CheckForRollback() const;

    private :
        bool IsBackUpAvailable(const char*) const;
        void ExtractPathToRollback(char*) const;
        void ExtractJobName(char*) const;
};
#endif
