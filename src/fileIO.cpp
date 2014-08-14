//******************************************/
//  JAMG @ SPACE CONCORDIA 2013
//  
//
//******************************************/
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fileIO.h"

const int BUFFER_SIZE = 100;
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * NAME : DeleteDirectory
 *
 * DESCRIPTION : Delete all DT_REG and DT_FIFO inside the specified directory
 *
 *-----------------------------------------------------------------------------*/
bool DeleteDirectoryContent(const char* toDelete)
{
    bool isSuccess = true;
    char path_tempo[BUFFER_SIZE];
    DIR* dirToDelete  = opendir(toDelete);
    struct dirent* item;
    if (dirToDelete != NULL) {
        while((item = readdir(dirToDelete)) && (isSuccess == true)) {
            if (item->d_type == DT_DIR && strncmp(item->d_name, ".", 1) != 0 && strncmp(item->d_name,"..",2) !=0) {
                safe_strcat(safe_strcat(safe_strcpy(path_tempo, toDelete, BUFFER_SIZE), "/", BUFFER_SIZE), item->d_name, BUFFER_SIZE);
                DeleteDirectoryContent(path_tempo);
                rmdir(path_tempo);

            }else if(item->d_type == DT_REG || item->d_type == DT_FIFO) {
                safe_strcat(safe_strcat(safe_strcpy(path_tempo, toDelete, BUFFER_SIZE), "/", BUFFER_SIZE), item->d_name, BUFFER_SIZE);
                isSuccess = DeleteFile(path_tempo);
            }
        }

        closedir(dirToDelete);
    } else {
        isSuccess = false;
    }
    return isSuccess;
}

//----------------------------------------------
//  CopyDirRecursively
//----------------------------------------------
bool CopyDirRecursively(const char* src, const char* dest)
{  
    bool isSuccess = true;
    char path_tempo_src[BUFFER_SIZE];               // BUFFER_SIZE = 100
    char path_tempo_dest[BUFFER_SIZE];
    DIR* toCopy = opendir(src);
    struct dirent* item;

    DIR* destination = opendir(dest);
    if (destination == NULL) {
        mkdir(dest, 0777);
    }
    closedir(destination);

    if (toCopy != NULL) {
        while((item = readdir(toCopy)) && (isSuccess == true)) {

            safe_strcat(safe_strcat(safe_strcpy(path_tempo_src,   src, BUFFER_SIZE), "/", BUFFER_SIZE), item->d_name, BUFFER_SIZE);
            safe_strcat(safe_strcat(safe_strcpy(path_tempo_dest, dest, BUFFER_SIZE), "/", BUFFER_SIZE), item->d_name, BUFFER_SIZE);

            if (item->d_type == DT_DIR && strncmp(item->d_name, ".", 1) != 0 && strncmp(item->d_name,"..",2) !=0) {  // item is a directory.
                mkdir(path_tempo_dest, 0777);
                isSuccess = CopyDirRecursively(path_tempo_src, path_tempo_dest);
            }else if (item->d_type == DT_REG) {                                                                      // item is a file.
                isSuccess = CopyFile(path_tempo_src, path_tempo_dest);
            }
        }
        closedir(toCopy);
    } else {
        isSuccess = false;
    }
    return isSuccess;
}
//----------------------------------------------
//  CopyFile
//----------------------------------------------
bool CopyFile(const char* src, const char* dest) {
    bool isSuccess = true;
    FILE* in = fopen(src, "rb");
    FILE* out = fopen(dest, "w+b");
    char* buffer; 
    int file_size;
    int numByte;


    if (in==NULL || out==NULL) {
        remove (dest);
        isSuccess = false;
    } else {
        fseek (in , 0 , SEEK_END);
        file_size = ftell (in);
        rewind (in);
        buffer = (char*)malloc(sizeof(char) * file_size);

        if (buffer == NULL) { 
            fprintf(stderr, "[CopyFile] : malloc error.\n");
            isSuccess = false;
        } else { 
            fread(buffer, 1, file_size, in);
            numByte =  fwrite(buffer, 1, file_size, out);

            if (numByte != file_size) {
                fprintf(stderr, "[CopyFile] : fread() or fwrite() error.\n");
                isSuccess = false;
            }

            free(buffer);
        }

        fclose(in);
        fclose(out);
    }

    struct stat st = {0};
    stat(src, &st);
    chmod(dest, st.st_mode & 0777); // Sets permissions

    return isSuccess;
}
//----------------------------------------------
//  DeleteFile
//----------------------------------------------
bool DeleteFile(const char* fileToDelete) {
    bool isSuccess = true;
    if (remove(fileToDelete) != 0) {                // 0 on success, -1 on failure
        perror("error deleting the file"); 
        isSuccess = false;
    }

    return isSuccess;
}
//----------------------------------------------
//  MoveFile
//----------------------------------------------
bool MoveFile(const char* src, const char* dest) {
    bool isSuccess = true;
    if (CopyFile(src, dest) == false) {
        isSuccess = false;
    }
    if (DeleteFile(src) == false) {
        isSuccess = false;
    }

    return isSuccess;
}

//----------------------------------------------
//  IsDirectoryNotEmpty()
//----------------------------------------------
bool IsDirectoryNotEmpty(const char* dir) { 
    struct dirent *item;     
    DIR* dirToCheck = opendir(dir);
    int counter=0;

    if (dirToCheck != NULL) {
        while ((item = readdir(dirToCheck))) {
            if (strncmp(item->d_name, ".", 1) != 0 && strncmp(item->d_name,"..",2)!=0 ) {
                counter += 1;
            }
        }

        closedir(dirToCheck);                               // 0 on success, -1 on failure 
    } else {
        perror ("In IsDirectoryNotEmpty(), couldn't open the directory ");
    }

    return (counter != 0); 
}

//----------------------------------------------
//  IsDirectoryExists()
//----------------------------------------------
bool IsDirectoryExists(const char* path) {
    DIR* testExist = opendir(path);
    if (testExist == NULL) {
        return false;
    }

    closedir(testExist);
    return true;
}
//----------------------------------------------
//  safe_strcat() 
//----------------------------------------------
char* safe_strcat(char* buffer, const char* str, size_t buf_size)
{
    const int EOF_LENGTH = 1;

    if (strlen(buffer) + strlen(str) + EOF_LENGTH > buf_size) {
        fprintf(stderr, "[safe_strcat] : Overflow, the buffer is too small!\n");
        printf("buffer : %s\n", buffer);
        exit(1);
    } else {
        strncat(buffer, str, strlen(str));
        return buffer;
    }
}
//----------------------------------------------
//  safe_strcpy() 
//----------------------------------------------
char* safe_strcpy(char* buffer, const char* str, size_t buf_size)
{
    const int EOF_LENGTH = 1;
    if (strlen(str) + EOF_LENGTH > buf_size) {
        fprintf(stderr, "[safe_strcpy] : Overflow, the buffer is too small!\n");
        exit(1);
    } else {
        strncpy(buffer, str, strlen(str));
        buffer[strlen(str)] = '\0';
        return buffer;
    }
}

//----------------------------------------------
//  IsFileExists() 
//----------------------------------------------
bool IsFileExists(const char* file) {
   struct stat buffer;
   return (stat(file,&buffer) == 0); 
}
