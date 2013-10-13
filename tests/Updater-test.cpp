//******************************************/
//  SPACE CONCORDIA 2013
//
//
//******************************************/
#include "CppUTest/TestHarness.h"
#include <sys/stat.h>
#include <unistd.h>     // rmdir()
#include <dirent.h>     // DIR
#include <string>
#include "fileIO.h"
#include "ProcessUpdater.h"
#include "Updater.h"
#include "shakespeare.h" 
using namespace std;
//************************************************************
//************************************************************
//              UpdaterTestGroup
//************************************************************
//************************************************************
TEST_GROUP(UpdaterTestGroup)
{
    Updater* updater;
    void setup(){
        mkdir("test-new", S_IRWXU);
        mkdir("test-current", S_IRWXU);
        mkdir("test-old", S_IRWXU);
        mkdir("test-rollback", S_IRWXU);
        mkdir("test-logs", S_IRWXU);
        updater = new Updater("test-new", "test-current", "test-old", "test-rollback", "test-logs");

    }
    void teardown(){
        DeleteDirectoryContent("test-new");
        DeleteDirectoryContent("test-current");
        DeleteDirectoryContent("test-old");
        DeleteDirectoryContent("test-rollback");
        DeleteDirectoryContent("test-logs");
        rmdir("test-new");                          // rmdir() : the directory HAS to be empty.
        rmdir("test-old");
        rmdir("test-current");
        rmdir("test-rollback");//*/
        rmdir("test-logs");
        delete updater;
    }
};
//----------------------------------------------
//  StartUpdate 
//----------------------------------------------
TEST(UpdaterTestGroup, StartUpdate_NewFolderIsNotEmptyAndRollback_ReturnsTrue){
    printf("StartUpdate_NewFolderIsNotEmptyAndRollback_ReturnsTrue : ");
    mkdir("test-new/JobA", S_IRWXU);
        FILE* JobANew = fopen("test-new/JobA/new-JobA.txt", "w+");
        fprintf(JobANew,"some text to test");
        fclose(JobANew);
        mkdir("test-new/JobA/sub-dir", S_IRWXU);
    mkdir("test-new/JobB", S_IRWXU);
        FILE* fileJobB = fopen("test-new/JobB/new-JobB.txt", "w+");
        fprintf(fileJobB,"in fileJobB\n");
        fclose(fileJobB);
    mkdir("test-current/JobA", S_IRWXU);
        FILE* JobACurrent = fopen("test-current/JobA/currentJobA.txt","w+");
        fclose(JobACurrent);
    mkdir("test-current/JobC", S_IRWXU);
        FILE* file3 = fopen("test-current/JobC/currentJobC.txt", "w+");
        fprintf(file3, "JobC current");
        fclose(file3);
    mkdir("test-old/JobC", S_IRWXU);
        FILE* file2 = fopen("test-old/JobC/oldJobC.txt", "w+");
        fprintf(file2, "JobC Old");
        fclose(file2);
    FILE* rollbackFile = fopen("test-rollback/rollback.txt", "w+");
    fprintf(rollbackFile, "asdf////test-old/JobC");
    fclose(rollbackFile);

    CHECK(updater->StartUpdate() == true);
}
TEST(UpdaterTestGroup, StartUpdate_NewFolderIsEmptyNoRollBack_ReturnsTrue){
    CHECK(updater->StartUpdate() == true);
}
//----------------------------------------------
//  StartRollback 
//----------------------------------------------
TEST(UpdaterTestGroup, StartRollback_RollbackAndBackupInOld_ReturnsTrue){               // UPDTR-07
    mkdir("test-current/JobC", S_IRWXU);
    mkdir("test-current/JobC/C", S_IRWXU);
        FILE* file3 = fopen("test-current/JobC/C/currentJobC.txt", "w+");
        fprintf(file3, "JobC current");
        fclose(file3);
    mkdir("test-old/JobC", S_IRWXU);
    mkdir("test-old/JobC/C", S_IRWXU);
        FILE* file2 = fopen("test-old/JobC/C/oldJobC.txt", "w+");
        fprintf(file2, "JobC Old");
        fclose(file2);
    FILE* rollbackFile = fopen("test-rollback/rollback.txt", "w+");
    fprintf(rollbackFile, "apath////test-old/JobC/C");
    fclose(rollbackFile);
    CHECK(updater->StartRollback() == true);
}
TEST(UpdaterTestGroup, StartRollback_RollbackFolderIsEmpty_ReturnsTrue){
    CHECK(updater->StartRollback() == true);
}
TEST(UpdaterTestGroup, StartRollback_RollbackAndNoBackupInOld_ReturnsTrue){             // UPDTR-06
    FILE* rollbackFile = fopen("test-rollback/rollback.txt", "w+");
    fprintf(rollbackFile, "asdf////test-old/JobC");
    fclose(rollbackFile);
    mkdir("test-current/JobC", S_IRWXU);
    CHECK(updater->StartRollback() == true);
}
TEST(UpdaterTestGroup, StartRollback_RollbackUnexistingFile_ReturnsFalse){              // UPDTR-05
    FILE* rollbackFile = fopen("test-rollback/rollback.txt", "w+");
    fprintf(rollbackFile, "asdf////test-old/JobC");
    fclose(rollbackFile);
    CHECK(updater->StartRollback() == false);
}
//************************************************************
//************************************************************
//                  ProcessTestGroup
//************************************************************
//************************************************************
TEST_GROUP(ProcessTestGroup){
    ProcessUpdater* process_updater;
    FILE* log;

    void setup(){
        const char* logsFolder = "test-ProcessUpdaterLogs";
        if (IsDirectoryExists(logsFolder) == false){
            mkdir(logsFolder, S_IRWXU);
        }

        string log_folder(logsFolder);
        string log_path = log_folder.append("/").append(get_filename(log_folder, "ProcessUpdater.", ".log").c_str());
        log = fopen(log_path.c_str(), "w+");

        mkdir("test-new", S_IRWXU);
        mkdir("test-old", S_IRWXU);
        mkdir("test-current", S_IRWXU);
        process_updater = new ProcessUpdater("test-new", "test-current", "test-old", log);
    }

    void teardown(){ //*
        if (log != NULL){
            fclose(log);
            log = NULL;
        }

        DeleteDirectoryContent("test-new");
        DeleteDirectoryContent("test-current");
        DeleteDirectoryContent("test-old");
        DeleteDirectoryContent("test-ProcessUpdaterLogs");
        rmdir("test-new");
        rmdir("test-old");
        rmdir("test-current");
        rmdir("test-ProcessUpdaterLogs");

        if (process_updater != NULL){
            delete process_updater;
            process_updater = NULL;
        }
    }
};
//----------------------------------------------
//  StartUpdate 
//----------------------------------------------
TEST(ProcessTestGroup, StartUpdate_newIsNotEmpty_ReturnsTrue){
    mkdir("test-new/update", S_IRWXU);
        FILE* file = fopen("test-new/update/new-test-file.txt", "w+");
        fprintf(file,"some text to test");
        fclose(file);
        mkdir("test-new/update/sub-dir", S_IRWXU);
    mkdir("test-old/back-up", S_IRWXU);
        FILE* file2 = fopen("test-old/back-up/backup-file-test.txt", "w+");
        fprintf(file2, "back up version");
        fclose(file2);
    mkdir("test-current/v1", S_IRWXU);
        FILE* file3 = fopen("test-current/v1/current-file-test.txt", "w+");
        fprintf(file3, "version 1 test");
        fclose(file3);
    CHECK(process_updater->StartUpdate() == true);
}
TEST(ProcessTestGroup, StartUpdate_newIsEmpty_ReturnsTrue){
    mkdir("test-old/back-up", S_IRWXU);
        FILE* file2 = fopen("test-old/back-up/backup-file-test.txt", "w+");
        fprintf(file2, "back up version");
        fclose(file2);
    mkdir("test-current/v1", S_IRWXU);
        FILE* file3 = fopen("test-current/v1/current-file-test.txt", "w+");
        fprintf(file3, "version 1 test");
        fclose(file3);
    CHECK(process_updater->StartUpdate() == true);
}
//************************************************************
//************************************************************
//                  FileIOTestGroup                             
//************************************************************
//************************************************************
TEST_GROUP(FileIOTestGroup){
    void setup(){                                       
        mkdir("test-src", S_IRWXU);
            FILE* file = fopen("test-src/potato.txt", "w+");
            fprintf(file, "copy test");
            fclose(file);
            mkdir("test-src/1", S_IRWXU);
        mkdir("test-dest", S_IRWXU);
    }
    void teardown(){
        DeleteDirectoryContent("test-src");
        DeleteDirectoryContent("test-dest");
        rmdir("test-src");
        rmdir("test-dest");
  
    }
};
//----------------------------------------------
//  DeleteDirectoryContent
//----------------------------------------------
TEST(FileIOTestGroup, DeleteDirectoryContent_DirectoryExists_ReturnTrue){
    CHECK(DeleteDirectoryContent("test-src") == true)
}
//----------------------------------------------
//  CopyDirRecursively
//----------------------------------------------
TEST(FileIOTestGroup, CopyDirRecursively_SourceIsPresentAndDestDirExists_ReturnsTrue){
    CHECK(CopyDirRecursively("test-src","test-dest") == true);
}
//----------------------------------------------
//  CopyFile
//----------------------------------------------
TEST(FileIOTestGroup, CopyFile_FileIsPresent_ReturnsTrue){
    char src[]  = "src.txt";
    char dest[] = "dest.txt";
    
    FILE* file = fopen(src, "w+");                              // Creates and open the file.
    fprintf(file, "copy test\n");                               // Writes text in the file.
    fclose(file);

    CHECK(CopyFile(src, dest) == true);
    
    file = fopen(dest, "r");
    CHECK(file != NULL);
    fclose(file);

    remove(src);
    remove(dest);
}
//----------------------------------------------
//  CopyFile
//----------------------------------------------
TEST(FileIOTestGroup, CopyFile_FileIsNotPresent_ReturnsFalse){
    CHECK(CopyFile("does_not_exist.txt", "dest.txt") == false);
}
//----------------------------------------------
//  IsDirectoryNotEmpty
//----------------------------------------------
TEST(FileIOTestGroup, IsDirectoryNotEmpty_NewIsEmpty_ReturnsFalse){
    mkdir("tempo",S_IRWXU);                                         // S_IRWXU : user RWX
    CHECK( IsDirectoryNotEmpty("tempo") == false );
    rmdir("tempo");
}
//----------------------------------------------
//  IsDirectoryNotEmpty 
//----------------------------------------------
TEST(FileIOTestGroup, IsDirectoryNotEmpty_NewIsNotEmpty_ReturnsTrue){
    CHECK( IsDirectoryNotEmpty("./") == true );                      // Current directory is not empty.
}
//----------------------------------------------
//  MoveFile_FileIsPresent_ReturnsTrue 
//----------------------------------------------
TEST(FileIOTestGroup, MoveFile_FileIsPresent_ReturnsTrue){
    char src[]  = "ExampleA.txt";
    char dest[] = "ExampleB.txt";

    FILE* file = fopen(src,"w+");
    fprintf(file, "test Moving file\n");
    fclose(file);
    
    CHECK( MoveFile(src,dest) == true);

    remove(dest);
}
//----------------------------------------------
//  MoveFile_FileIsNotPresent_ReturnsFalse
//----------------------------------------------
TEST(FileIOTestGroup, MoveFile_FileIsNotPresent_ReturnsFalse){      
    char src[] = "a.txt";
    char dest[] = "b.txt";
    CHECK(MoveFile(src, dest) == false);                            // Prints an error message... OK
}
