//******************************************/
//  JAMG @ SPACE CONCORDIA 2013
//  
//
//******************************************/
#include <fileIO.h>
#include <cstring>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>

const int BUFFER_SIZE = 100;
//----------------------------------------------
//  DeleteDirectory
//----------------------------------------------
bool DeleteDirectoryContent(const char* toDelete){
    bool isSuccess = true;
    char path_tempo[BUFFER_SIZE];
    DIR* dirToDelete  = opendir(toDelete);
    struct dirent* item;
    if (dirToDelete != NULL){
        while((item = readdir(dirToDelete)) && (isSuccess == true)){
            if (item->d_type == DT_DIR && strncmp(item->d_name, ".", 1) != 0 && strncmp(item->d_name,"..",2) !=0){
                safe_strcat(safe_strcat(safe_strcpy(path_tempo, toDelete, BUFFER_SIZE), "/", BUFFER_SIZE), item->d_name, BUFFER_SIZE);
                DeleteDirectoryContent(path_tempo);
                rmdir(path_tempo);

            }else if(item->d_type == DT_REG){
                safe_strcat(safe_strcat(safe_strcpy(path_tempo, toDelete, BUFFER_SIZE), "/", BUFFER_SIZE), item->d_name, BUFFER_SIZE);
                isSuccess = DeleteFile(path_tempo);
            }
        }

        closedir(dirToDelete);
    }else{
        isSuccess = false;
    }
    return isSuccess;
}
//----------------------------------------------
//  CopyDirRecursively
//----------------------------------------------
bool CopyDirRecursively(const char* src, const char* dest){  
    bool isSuccess = true;
    char path_tempo_src[BUFFER_SIZE];               // BUFFER_SIZE = 100
    char path_tempo_dest[BUFFER_SIZE];
    DIR* toCopy = opendir(src);
    struct dirent* item;
    
    DIR* destination = opendir(dest);
    if (destination == NULL){
        mkdir(dest, S_IRWXU);
    }
    closedir(destination);

    if (toCopy != NULL){
        while((item = readdir(toCopy)) && (isSuccess == true)){
            
            safe_strcat(safe_strcat(safe_strcpy(path_tempo_src,   src, BUFFER_SIZE), "/", BUFFER_SIZE), item->d_name, BUFFER_SIZE);
            safe_strcat(safe_strcat(safe_strcpy(path_tempo_dest, dest, BUFFER_SIZE), "/", BUFFER_SIZE), item->d_name, BUFFER_SIZE);
            
            if (item->d_type == DT_DIR && strncmp(item->d_name, ".", 1) != 0 && strncmp(item->d_name,"..",2) !=0){  // item is a directory.
                mkdir(path_tempo_dest, S_IRWXU );
                isSuccess = CopyDirRecursively(path_tempo_src, path_tempo_dest);
            }else if (item->d_type == DT_REG){                                                                      // item is a file.
                isSuccess = CopyFile(path_tempo_src, path_tempo_dest);
            }
        }
        closedir(toCopy);
    }else{
        isSuccess = false;
    }
    return isSuccess;
}
//----------------------------------------------
//  CopyFile
//----------------------------------------------
bool CopyFile(const char* src, const char* dest){
    bool isSuccess = true;
    FILE* in = fopen(src, "rb");
    FILE* out = fopen(dest, "w+b");
    char* buffer = (char*)malloc(sizeof(char));
    
    if (in==NULL || out==NULL){
        remove (dest);
        isSuccess = false;
    }else{
        while (fread(buffer, 1, 1, in)){            // TODO : DONT read 1 byte at time FIXME
            fwrite(buffer, 1, 1, out);
        }
        fclose(in);
        fclose(out);
    }
    free(buffer);

    return isSuccess;
}
//----------------------------------------------
//  DeleteFile
//----------------------------------------------
bool DeleteFile(const char* fileToDelete){
    bool isSuccess = true;
    if (remove(fileToDelete) != 0){                // 0 on success, -1 on failure
        perror("error deleting the file"); 
        isSuccess = false;
    }

    return isSuccess;
}
//----------------------------------------------
//  MoveFile
//----------------------------------------------
bool MoveFile(const char* src, const char* dest){
    bool isSuccess = true;
    if (CopyFile(src, dest) == false){
        isSuccess = false;
    }
    if (DeleteFile(src) == false){
        isSuccess = false;
    }

    return isSuccess;
}
//----------------------------------------------
//  IsDirectoryNotEmpty()
//----------------------------------------------
bool IsDirectoryNotEmpty(const char* dir){ 
    struct dirent *item;     
    DIR* dirToCheck = opendir(dir);
    int counter=0;

    if (dirToCheck != NULL){
        while (item = readdir(dirToCheck)){
            if (strncmp(item->d_name, ".", 1) != 0 && strncmp(item->d_name,"..",2)!=0 ){
                counter += 1;
            }
        }

    closedir(dirToCheck);                               // 0 on success, -1 on failure 
    }else{
        perror ("In IsDirectoryNotEmpty(), couldn't open the directory ");
    }

    return (counter != 0); 
}
//----------------------------------------------
//  IsDirectoryExists()
//----------------------------------------------
bool IsDirectoryExists(const char* path){
    DIR* testExist = opendir(path);
    if (testExist == NULL){
        return false;
    }

    closedir(testExist);
    return true;
}
//----------------------------------------------
//  safe_strcat() 
//----------------------------------------------
char* safe_strcat(char* buffer, const char* str, int buf_size){
    const int EOF_LENGTH = 1;
    int buffer_len = strlen(buffer);
    if (strlen(buffer) + strlen(str) + EOF_LENGTH > buf_size){
        fprintf(stderr, "[safe_strcat] : Overflow, the buffer is too small!\n");
        printf("buffer : %s\n", buffer);
        exit(1);
    }else{
        strncat(buffer, str, strlen(str));
        return buffer;
    }
}
//----------------------------------------------
//  safe_strcpy() 
//----------------------------------------------
char* safe_strcpy(char* buffer, const char* str, int buf_size){
    const int EOF_LENGTH = 1;
    if (strlen(str) + EOF_LENGTH > buf_size){
        fprintf(stderr, "[safe_strcpy] : Overflow, the buffer is too small!\n");
        exit(1);
    }else{
        strncpy(buffer, str, strlen(str));
        buffer[strlen(str)] = '\0';
        return buffer;
    }
}
