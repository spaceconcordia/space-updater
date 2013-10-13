//******************************************/
//  SPACE CONCORDIA 2013
//
//
//******************************************/
#include <ProcessUpdater.h>
#include <cstdio>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>
#include <fileIO.h>
#include "shakespeare.h" 
//----------------------------------------------
//  Constructor
//----------------------------------------------
ProcessUpdater::ProcessUpdater(const char* newJob, const char* currentJob, const char* oldJob, FILE* logStream){
    initialize(newJob, currentJob, oldJob, logStream);
}

void ProcessUpdater::initialize(const char* newJob, const char* currentJob, const char* oldJob, FILE* logStream){
    newJobPath = newJob;
    oldJobPath = oldJob;
    currentJobPath = currentJob;

    if (logStream == NULL){
        perror("The FILE* for the logs is NULL... exit");
        exit(-1);
    }else{
        log = logStream;
    }


    if (CheckIfNewJobIsNotEmpty() == true){
        DIR* testExist = opendir(currentJobPath);
        if (testExist == NULL){
            mkdir(currentJobPath, S_IRWXU);
        }
        closedir(testExist);
        testExist = NULL;
        testExist = opendir(oldJobPath);
        if (testExist == NULL){
            mkdir(oldJobPath, S_IRWXU);
        }
        closedir(testExist);
    }
}
//----------------------------------------------
//  Destructor
//----------------------------------------------
ProcessUpdater::~ProcessUpdater(){
    //*/
}
//----------------------------------------------
//  StartUpdate
//----------------------------------------------
bool ProcessUpdater::StartUpdate() const{
    bool isSuccess = false;
    int retry = 0;
    if (CheckIfNewJobIsNotEmpty() == true){
        while(CheckIfNewJobIsNotEmpty() == true && isSuccess == false && retry < MAX_RETRY){
            mkdir("tempo", S_IRWXU);
            
            isSuccess = CopyDirRecursively(oldJobPath, "tempo");

            if (isSuccess == true){
                isSuccess = DeleteDirectoryContent(oldJobPath);
            }
            if (isSuccess == true){
                if ((isSuccess = CopyDirRecursively(currentJobPath, oldJobPath)) == false){
                    CopyDirRecursively("tempo", oldJobPath);
                }
            }
            if (isSuccess == true){
                if ((isSuccess = DeleteDirectoryContent(currentJobPath)) == false){
                    DeleteDirectoryContent(oldJobPath);
                    CopyDirRecursively("tempo", oldJobPath);
                }
            }
            if (isSuccess == true){
                if ((isSuccess = CopyDirRecursively(newJobPath, currentJobPath)) == false){
                    CopyDirRecursively(oldJobPath,currentJobPath);
                    DeleteDirectoryContent(oldJobPath);
                    CopyDirRecursively("tempo", oldJobPath);
                }
            }
            if (isSuccess == true){
                if ((isSuccess = DeleteDirectoryContent(newJobPath)) == false){
                    DeleteDirectoryContent(currentJobPath);
                    CopyDirRecursively(oldJobPath,currentJobPath);
                    DeleteDirectoryContent(oldJobPath);
                    CopyDirRecursively("tempo", oldJobPath);
                }
            }

            DeleteDirectoryContent("tempo");
            rmdir("tempo");
            retry += 1;
        }
    }else{
        string msg = "Empty folder in new... discard it : ";
        msg.append(string(newJobPath));
        Log(log, WARNING, "ProcessUpdater", msg);
        isSuccess = true;
    }
    return isSuccess;
}
//----------------------------------------------
//  CheckIfNewJobIsNotEmpty
//----------------------------------------------
bool ProcessUpdater::CheckIfNewJobIsNotEmpty() const{
   return IsDirectoryNotEmpty(newJobPath); 
}
