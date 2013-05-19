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
//  deleteDirectory
//----------------------------------------------
bool DeleteDirectoryContent(const char* toDelete){
    bool isSuccess = true;
    char path_tempo[BUFFER_SIZE];
    DIR* dirToDelete  = opendir(toDelete);
    struct dirent* item;
    if (dirToDelete != NULL){
        while((item = readdir(dirToDelete)) && (isSuccess == true)){
            if (item->d_type == DT_DIR && strncmp(item->d_name, ".", 1) != 0 && strncmp(item->d_name,"..",2) !=0){
                strcat(strcat(strcpy(path_tempo, toDelete), "/"), item->d_name);
                DeleteDirectoryContent(path_tempo);
                rmdir(path_tempo);

            }else if(item->d_type == DT_REG){
                strcat(strcat(strcpy(path_tempo, toDelete), "/"), item->d_name);
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
            
            strcat(strcat(strcpy(path_tempo_src,   src), "/"), item->d_name);
            strcat(strcat(strcpy(path_tempo_dest, dest), "/"), item->d_name);
            
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
//  copyFile
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
        while (fread(buffer, 1, 1, in)){            // TODO : error checking here.
            fwrite(buffer, 1, 1, out);
        }
        fclose(in);
        fclose(out);
    }
    free(buffer);

    return isSuccess;
}
//----------------------------------------------
//  deleteFile
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
    if (!CopyFile(src, dest)){
        isSuccess = false;
    }
    if ( !DeleteFile(src) ){
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

