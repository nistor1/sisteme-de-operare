#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>


int listDir(const char *path, char* filter, int* firstO)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    dir = opendir(path);

    struct stat statbuf;

    if(dir == NULL) {
        printf("ERROR\ninvalid directory path");
        return -1;
    }
    if(*firstO == 0) {
        printf("SUCCESS\n");
        (*firstO)++;
    }
    while((entry = readdir(dir)) != NULL) {


        char file_path[1024];
        sprintf(file_path, "%s/%s", path, entry->d_name);
        struct stat file_stat;
        stat(file_path, &file_stat);

        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        if(filter == NULL) {
             if(lstat(file_path, &statbuf) == 0) {
                        if(S_ISREG(statbuf.st_mode) || S_ISDIR(statbuf.st_mode)) {
                            printf("%s\n", file_path); 

                        }}
        } else {
            if(filter[0] == 'r') { // grater_than
               long int size = 0;
                sscanf(filter + 2, "%ld", &size);
                if(lstat(path, &statbuf) == 0) {
                if(S_ISREG(statbuf.st_mode)) {
                            
                    if(file_stat.st_size > size) {
                        printf("%s\n", file_path);
                        }
                    }

                } else {// ends in
                    int length = 0;
                    int lengthFilter = strlen(filter+ 2);
                    length = strlen(entry->d_name);
                    if(strncmp(entry->d_name + length - lengthFilter + 1, filter + 2, lengthFilter) == 0) {
                        printf("%s\n", file_path); 
                    }
                }
            }
            }
        }   
    closedir(dir);
    return 0;
}

    void listRec(const char *path, char* filter, int* firstO) {
        DIR *dir = NULL;
        struct dirent *entry = NULL;
        char fullPath[1024];
        struct stat statbuf;
        if(listDir(path, filter, firstO) == -1){
            return;
        }
        dir = opendir(path);
        if(dir == NULL) {
            printf("ERROR\ninvalid directory path");
            return;
        }
        if(*firstO == 0) {
            printf("SUCCESS\n");
            (*firstO)++;
        }
        
        while((entry = readdir(dir)) != NULL) {
            if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                snprintf(fullPath, 1024, "%s/%s", path, entry->d_name);
                    if(lstat(fullPath, &statbuf) == 0) {
                            if(S_ISDIR(statbuf.st_mode) || S_ISLNK(statbuf.st_mode)) {
                                listRec(fullPath, filter, firstO);
                            }               
                        }
                }
        }
        closedir(dir);
    }

int main(int argc, char **argv){
    if(argc >= 2){
        if(strcmp(argv[1], "variant") == 0){
            printf("19799\n");
        } else {
            bool list = false;
            for (int i = 1; i < argc; i++) {
                if (strcmp(argv[i], "list") == 0) {
                    list = true;
                    break;
                }
            }
            if(list == true) {
                bool isRecursive = false;
                char* path = NULL;
                char* filter = NULL;
                for (int i = 1; i < argc; i++) {
                    if (strncmp(argv[i], "path=", 5) == 0) {
                        path = argv[i] + 5;
                        break;
                    }
                }
                for (int i = 1; i < argc; i++) {
                    if (strcmp(argv[i], "recursive") == 0) {
                        isRecursive = true;
                        break;
                    }
                }
                 
                for (int i = 1; i < argc; i++) {
                    if (strncmp(argv[i], "size_greater=", 12) == 0) {
                        filter = argv[i] + 10;
                        break;
                    } else if (strncmp(argv[i], "name_ends_with=", 14) == 0) {
                        filter = argv[i] + 12;
                        break;
                    }
                }

                if(filter != NULL) {
                    if(filter[2] == '\0') {
                        filter = NULL;
                    }                    
                }
                int firstO = 0;
                if(isRecursive == true) {
                    listRec(path, filter, &firstO);
                } else {
                    listDir(path, filter, &firstO);
                } 
            }
        }
    } 
    
    return 0;
}
