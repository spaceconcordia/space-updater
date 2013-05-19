//******************************************/
//  SPACE CONCORDIA 2013
//
//
//******************************************/
#ifndef FILEIO_H
#define FILEIO_H
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
bool CopyFile(const char* src, const char* dest);
bool DeleteFile(const char* fileToDelete);
bool MoveFile(const char* src, const char* dest);
bool IsDirectoryNotEmpty(const char*); 
bool CopyDirRecursively(const char*, const char*);
bool DeleteDirectoryContent(const char*);
#endif
