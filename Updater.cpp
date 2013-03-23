//******************************************/
//  SPACE CONCORDIA 2013
//
//
//******************************************/
#include "Updater.h"
#include <sys/stat.h>
#include "fileIO.h"
#include <unistd.h>     // rmdir
#include "ProcessUpdater.h"
#include <cstring>
const char* Updater::rollbackFileName = "rollback.txt";         // TODO : change file name
//----------------------------------------------
//  Constructor
//----------------------------------------------
Updater::Updater(){ //TODO : hardcode the path.
    newDirPath = NULL;
    oldDirPath = NULL;
    currentDirPath = NULL;
    rollbackDirPath = NULL;
}
Updater::Updater(const char* newDir, const char* currentDir, const char* oldDir, const char* rollPath){
    newDirPath = newDir;
    oldDirPath = oldDir;
    currentDirPath = currentDir;
    rollbackDirPath = rollPath;
}
Updater::~Updater(){
}
//----------------------------------------------
//  StartRollback
//----------------------------------------------
bool Updater::StartRollback() const{
    bool isSuccess = false;
    int retry = 0;
    char path_tempo_current[BUFFER_SIZE];
    char path_tempo_old[BUFFER_SIZE];
    char job_name[BUFFER_NAME_SIZE] = "\0";                                                     //
    if (CheckForRollback() == true){
        while (CheckForRollback() == true && isSuccess == false && retry < MAX_RETRY){
            mkdir("tempo", S_IRWXU);
            ExtractJobName(job_name);
            strcat(strcat(strcpy(path_tempo_current, currentDirPath), "/"), job_name);
            strcat(strcat(strcpy(path_tempo_old,     oldDirPath), "/"),     job_name);
            if ((isSuccess = IsBackUpAvailable(path_tempo_old)) == false){                      // checks if there is a backup available
                DeleteDirectoryContent(rollbackDirPath);                                        // if not delete rollback.
            }
            if (isSuccess == true){
                isSuccess = CopyDirRecursively(path_tempo_current, "tempo");
            }
            if (isSuccess == true){
                isSuccess = DeleteDirectoryContent(path_tempo_current);       
            }
            if (isSuccess == true){
                if ((isSuccess = CopyDirRecursively(path_tempo_old, path_tempo_current)) == false){
                    CopyDirRecursively("tempo", path_tempo_current);                                // Restores the original content.
                }
            }
                        
            DeleteDirectoryContent(rollbackDirPath);                                                // Clear rollbackDirPath
            DeleteDirectoryContent("tempo");                                                        // Clear the temporary directory.
            rmdir("tempo");                                                                         // Remove temporary dir.
            retry += 1;
        }
    }else{
        isSuccess = true;
        printf("Rollback folder is empty... nothing to do.");
    }
    return isSuccess;
}
//----------------------------------------------
//  StartUpdate
//----------------------------------------------
bool Updater::StartUpdate() const {
    bool isSuccess = true;
    char path_tempo_new[BUFFER_SIZE];
    char path_tempo_current[BUFFER_SIZE];
    char path_tempo_old[BUFFER_SIZE];
    DIR* dir = opendir(newDirPath);
    struct dirent* item;
    if (CheckForRollback() == true){
        if (StartRollback() == false){
            //TODO if StartRollback() fails... LOG 
        }
        //TODO log success
        puts("Rollback success");
    }
     
    if (CheckForUpdate() == true && CheckForRollback() == false){
        ProcessUpdater* current_process;
        while((item=readdir(dir))){
            strcat(strcat(strcpy(path_tempo_new,     newDirPath),     "/"), item->d_name);
            strcat(strcat(strcpy(path_tempo_current, currentDirPath), "/"), item->d_name);
            strcat(strcat(strcpy(path_tempo_old,     oldDirPath),     "/"), item->d_name);
            
            if (item->d_type == DT_DIR && strncmp(item->d_name, ".", 1) != 0 && strncmp(item->d_name,"..",2) !=0){  
                current_process = new ProcessUpdater(path_tempo_new, path_tempo_current, path_tempo_old);  
                isSuccess = current_process->StartUpdate();
                if (isSuccess == true){
                    printf("Update success : %s\n", path_tempo_new);
                }else{
                    printf("Update failure : %s\n", path_tempo_new);
                }
                //TODO   if ProcessUpdater fails ... message/log and delete new Job 
                rmdir(path_tempo_new);
                delete current_process;
            }
        }
    }else{
        isSuccess = true;
        printf("System is up to date\n");
    }
    closedir(dir);
    return isSuccess;
}
//----------------------------------------------
//  CheckForUpdate
//----------------------------------------------
bool Updater::CheckForUpdate() const{
   return IsDirectoryNotEmpty(newDirPath);  
}
//----------------------------------------------
//  CheckForRollback
//----------------------------------------------
bool Updater::CheckForRollback() const{
    return IsDirectoryNotEmpty(rollbackDirPath);
}
//----------------------------------------------
//  IsBackUpAvailable
//----------------------------------------------
bool Updater::IsBackUpAvailable(const char* path) const{
    return IsDirectoryNotEmpty(path);
}
//----------------------------------------------
//  ExtractPathToRollback
//----------------------------------------------
void Updater::ExtractPathToRollback(char* path) const{
    char path_tempo_dirToUpdate[BUFFER_SIZE];
    FILE* file = fopen(strcat(strcat(strcpy(path_tempo_dirToUpdate,rollbackDirPath), "/"), rollbackFileName), "r");// Open rollbackFileName for Reading
    fscanf(file, "%s", path);
    fclose(file);
}
//----------------------------------------------
//  ExtractJobName
//----------------------------------------------
void Updater::ExtractJobName(char* jobName) const{
    char path[BUFFER_SIZE];
    ExtractPathToRollback(path);
/*    while(strchr(path,'/') != NULL){                        // We keep what is after the last '/'
        strcpy(path, strchr(path,'/') + 1);                 // +1 to get rid of the '/'
    }*/
    if (strstr(path,"old/") != NULL){
        strcpy(path, strstr(path, "old/") + 4);
    }else if(strstr(path, "current/") != NULL){
        strcpy(path, strstr(path, "current/") + 8);
    }
    strcpy(jobName, path);
//printf("jobName : %s\n", jobName);
}
