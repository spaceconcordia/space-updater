//******************************************/
//  JAMG @ SPACE CONCORDIA 2013
//
//
//******************************************/
#include <Updater.h>
#include <sys/stat.h>
#include <fileIO.h>
#include <unistd.h>     // rmdir
#include <ProcessUpdater.h>
#include <cstring>
#include <string>
#include "shakespeare.h" 
const char* Updater::rollbackFileName = "rollback.txt";   
//----------------------------------------------
//  Constructor
//----------------------------------------------
Updater::Updater(){                                 // Default : Q6 paths
    newDirPath = "/home/apps/new";
    oldDirPath = "/home/apps/old";
    currentDirPath = "/home/apps/current";
    rollbackDirPath = "/home/apps/rollback";
    const char* logsFolder = "/home/apps/logs";
    
    initialize(newDirPath, currentDirPath, oldDirPath, rollbackDirPath, logsFolder);
}

Updater::Updater(const char* newDir, const char* currentDir, const char* oldDir, const char* rollPath){
    initialize(newDir, currentDir, oldDir, rollPath, "logs");
}

Updater::Updater(const char* newDir, const char* currentDir, const char* oldDir, const char* rollPath, const char* logsFolder){
    initialize(newDir, currentDir, oldDir, rollPath, logsFolder);
}

void Updater::initialize(const char* newDir, const char* currentDir, const char* oldDir, const char* rollPath, const char* logsFolder){
    if (IsDirectoryExists(logsFolder) == false){
        mkdir(logsFolder, S_IRWXU);

    }

    string log_folder(logsFolder);
    string log_path = log_folder.append("/").append(get_filename(log_folder, "Updater.", ".log").c_str());
    log = fopen(log_path.c_str(), "w+");

    if (IsDirectoryExists(newDir) == true && IsDirectoryExists(currentDir) == true 
                                                    && IsDirectoryExists(oldDir) == true 
                                                        && IsDirectoryExists(rollPath) == true){
        newDirPath = newDir;
        oldDirPath = oldDir;
        currentDirPath = currentDir;
        rollbackDirPath = rollPath;
    }else{
        Log(log, ERROR, "Updater", "Directories pass to Updater() do not exist");
        exit(1);
    }
}

Updater::~Updater(){
    if (log != NULL){
        fclose(log);
    }
}
//----------------------------------------------
//  StartRollback
//----------------------------------------------
bool Updater::StartRollback() const{
    bool isSuccess = false;
    char job_name[BUFFER_NAME_SIZE] = "\0";                                                     
    if (CheckForRollback() == true){
        ExtractJobName(job_name);
        isSuccess = StartRollback(job_name);
        DeleteDirectoryContent(rollbackDirPath);                                                // Clear rollbackDirPath
    }else{
        isSuccess = true;
        Log(log, NOTICE, "Updater", "Rollback folder is empty, nothing to do.");
    }
    return isSuccess;
}
bool Updater::StartRollback(const char* appName) const{
    bool isSuccess = false;
    bool isBackUpAvailable = false;
    int retry = 0;
    char path_tempo_current[BUFFER_SIZE];
    char path_tempo_old[BUFFER_SIZE];
    char job_name[BUFFER_NAME_SIZE] = "\0";                                                     
            mkdir("tempo", S_IRWXU);
            strcat(job_name, appName);
            strcat(strcat(strcpy(path_tempo_current, currentDirPath), "/"), job_name);
            strcat(strcat(strcpy(path_tempo_old,     oldDirPath), "/"),     job_name);
        while (isSuccess == false && retry < MAX_RETRY){
            
            isSuccess = CopyDirRecursively(path_tempo_current, "tempo");

            if (isSuccess == true){
                isSuccess = DeleteDirectoryContent(path_tempo_current);       
            }
            if (isSuccess == true && IsBackUpAvailable(path_tempo_old) == true){
                if ((isSuccess = CopyDirRecursively(path_tempo_old, path_tempo_current)) == false){
                    CopyDirRecursively("tempo", path_tempo_current);                                // Restores the original content.
                }
            }
                        
            DeleteDirectoryContent("tempo");                                                        // Clear the temporary directory.
            rmdir("tempo");                                                                         // Remove temporary dir.
            retry += 1;
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
            Log(log, ERROR, "Updater", "Rollback failed");
        }else{
            Log(log, NOTICE, "Updater", "Rollback success");
        }
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
                    string msg = "Update succes : ";
                    msg.append(path_tempo_new);
                    Log(log, NOTICE, "Updater", msg); 
                }else{
                    string msg = "Update failure : ";
                    msg.append(path_tempo_new);
                    Log(log, ERROR, "Updater", msg); 
                }

                rmdir(path_tempo_new);
                delete current_process;
            }
        }
    }else{
        isSuccess = true;
        Log(log, NOTICE, "Updater", "System is up to date"); 
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
    if (IsDirectoryNotEmpty(path) == false){
        Log(log, WARNING, "Updater", "IsBackUpAvailable() : no back-up available."); 
        return false;
    }
    return true;
}
//----------------------------------------------
//  ExtractPathToRollback
//----------------------------------------------
void Updater::ExtractPathToRollback(char* path) const{
    char path_tempo_dirToUpdate[BUFFER_SIZE];
    FILE* file = fopen(strcat(strcat(strcpy(path_tempo_dirToUpdate,rollbackDirPath), "/"), rollbackFileName), "r");// Open rollbackFileName for Reading
    if (file != NULL){
        fscanf(file, "%s", path);
        fclose(file);
    }else{
        Log(log, ERROR, "Updater", "ExtractPathToRollback() : can't open the file."); 
    }
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
