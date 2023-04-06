#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>


typedef struct SECTION_HEADER_{
    char sectName[17];
    char sectType;
    int sectOffset;
    int sectSize;
}SECTION_HEADERS;

typedef struct HEADER_SECTION_FILE_{
    int version;
    char noOfSections;
    SECTION_HEADERS* sectionsHeaders;
    short int headerSize;
    char magic[2];

}HEADER_SECTION_FILE;

void displaySF(HEADER_SECTION_FILE headerSF) {
    printf("SUCCESS\n");
    printf("version=%d\n", headerSF.version);
    printf("nr_sections=%d\n", (int)headerSF.noOfSections);

    for(int i = 0; i < headerSF.noOfSections; i++) {
        printf("section%d: %s %d %d\n", i+1, headerSF.sectionsHeaders[i].sectName, (int)headerSF.sectionsHeaders[i].sectType,  (int)headerSF.sectionsHeaders[i].sectSize);
    }

}

int parseSF(char* path, HEADER_SECTION_FILE* headerSF, bool display) {

    int fd = -1;
    char buffer[27];

    fd = open(path, O_RDONLY);
    if(fd == -1) {
        printf("ERROR\ninvalid directory path");
        return -1;
    }

    lseek(fd, 0, SEEK_END);
    lseek(fd, -26, SEEK_END);
    
    int step = -1;
    int indexSections = 1;
    while(indexSections != 0) {
        int bufferCapacity = read(fd, buffer, 26);
        bufferCapacity--;
        if(bufferCapacity <= 0 || bufferCapacity > 25) {
            printf("ERROR\n");
            break;
        }
        if(bufferCapacity == 25) {     
            step++; 
            if(step == 0 && bufferCapacity - 2 >= 0) {
                headerSF->magic[1] = buffer[25];
                headerSF->magic[0] = buffer[24];
                //printf("(%c    %c)", headerSF.magic[0], headerSF.magic[1]);
                bufferCapacity -= 2;
                step++;

                if(headerSF->magic[0] != 'Z' || headerSF->magic[1] != 'F') { 
                    printf("ERROR\nwrong magic");
                    return -1;
                }

            }        
            if(step == 1 && bufferCapacity - 2 >= 0) {
                char tempShort[2];
                short int f = 0;
                tempShort[1] = buffer[23];
                tempShort[0] = buffer[22];
               // printf("(%d    %d)", tempShort[0], tempShort[1]);
                memcpy(&f, tempShort, sizeof(short int));
                //printf("%d\n", (int)f);
                headerSF->headerSize = f;
                bufferCapacity -= 2;
                step++;
            }
            if(step == 2) {
                lseek(fd, -headerSF->headerSize, SEEK_END);
            }
            if(step == 3) {
                memcpy(&headerSF->version, buffer, sizeof(int));
                if(!(headerSF->version >= 25 && headerSF->version <= 118)) {
                    printf("ERROR\nwrong version");
                    return -1;
                } 
                //printf("%d ", headerSF.version);

                //for(int i = 0; i < 25; i++) {
                //    printf("%d ", buffer[i]);
                //}


                memcpy(&headerSF->noOfSections, buffer + 4, sizeof(char));
                if(headerSF->noOfSections < 3 || headerSF->noOfSections > 11) {
                    printf("ERROR\nwrong sect_nr");
                    return -1;
                }
                indexSections = headerSF->noOfSections;
                bufferCapacity -= 5;
                lseek(fd, -headerSF->headerSize + 5, SEEK_END);
            }
            if(step == 4) {
                char tempSectType = 0;
                int tempSectOffset = 0;
                int tempSectSize = 0;
                if(headerSF->sectionsHeaders == NULL){
                   headerSF->sectionsHeaders = (SECTION_HEADERS*)malloc(headerSF->noOfSections * sizeof(SECTION_HEADERS));
                }
                if(headerSF->sectionsHeaders == NULL) {
                    return -1;
                }
                int index = headerSF->noOfSections - indexSections;
                for(int j = 0; j < 16; j++) {
                    headerSF->sectionsHeaders[index].sectName[j] = buffer[j];
                    //printf("%c", buffer[j]);
                }
                headerSF->sectionsHeaders[index].sectName[16] = 0;

                memcpy(&(tempSectType), buffer + 16, sizeof(char));
                memcpy(&(tempSectOffset), buffer + 17, sizeof(int));
                memcpy(&(tempSectSize), buffer + 21, sizeof(int));

                if(!(tempSectType == 71 || tempSectType == 55 || tempSectType == 93 ||tempSectType == 36 ||tempSectType == 82 ||tempSectType == 67)) {
                    printf("ERROR\nwrong sect_types");
                    return -1;   
                }

                headerSF->sectionsHeaders[index].sectType = tempSectType;
                headerSF->sectionsHeaders[index].sectOffset = tempSectOffset;
                headerSF->sectionsHeaders[index].sectSize = tempSectSize;
                //printf("%d\n", index);
                //printf("->%d->%d->%d\n", headerSF.sectionsHeaders[index].sectType, headerSF.sectionsHeaders[index].sectOffset, headerSF.sectionsHeaders[index].sectSize);
                lseek(fd, -1, SEEK_CUR);
                indexSections--;
                step--;
            }
        }
    }

    //    for(int i = 0; i < headerSF.noOfSections; i++) {
    //    printf("section%d: %s %d %d\n", i+1, headerSF.sectionsHeaders[i].sectName, (int)headerSF.sectionsHeaders[i].sectType,  (int)headerSF.sectionsHeaders[i].sectSize);
   // }
    if(display == true) {
            displaySF(*headerSF);
    }
    close(fd);
    return 0;
}

int displayBufferBackwards(char* buffer, int size, int cur) {
    if(cur > 0) {
        size = cur;
    }
    for(int i = size - 1; i >= 0; i--) {
        if(buffer[i] == '\n' || buffer[i] == 0) {
            return 0;
        }
        printf("%c", buffer[i]);
    }
    return 1;
}

int extractSF(char* path, int section, int line, HEADER_SECTION_FILE* h) {
    int isValid = parseSF(path, h, false);
    if(isValid == -1) {
        free(h->sectionsHeaders);
        return -1;
    }

    int fd = -1;
    char buffer[21];

    fd = open(path, O_RDONLY);
    if(fd == -1) {
        printf("ERROR\ninvalid directory path");
        free(h->sectionsHeaders);
        return -1;
    }
    
    int firstSuccess = 0;
    int index = 0;
    int cntNewLine = 0;
    section--;
    lseek(fd, h->sectionsHeaders[section].sectOffset, SEEK_SET);
    while(cntNewLine <= line && index < h->sectionsHeaders[section].sectSize) {

        read(fd, buffer, 20);
        int j = 0;
        for(j = 0; j < 20; j++) {
            if(cntNewLine == line) {
                break;
            }
            if(buffer[j] == '\n' || buffer[j] == 0) {
                cntNewLine++;
            }
            if(firstSuccess == 1) {
               index++;
            }
        }
        if(cntNewLine == line) {
            if(firstSuccess == 0) {
                printf("SUCCESS\n"); 
                firstSuccess++;
            }
            //buffer[20] == 0;
            //printf("%s", buffer);
            if(j-1 != 0) {
                if(displayBufferBackwards(buffer, 20, j-1) == 0) {
                    //printf("\nIo zic ca merge");
                    close(fd);
                    free(h->sectionsHeaders);
                    return 0;
                }
            } 
            if(lseek(fd, -40, SEEK_CUR) == -1) {
            lseek(fd, 0, SEEK_SET);
            } 
        }
        j= 0;
        //printf("\n%d\n", cntNewLine);
    }

    close(fd);
    free(h->sectionsHeaders);
    return -1;
}


int listDir(const char *path, char* filter, int* firstO, bool verifySF) {
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
        if (verifySF == true) {
            if (lstat(file_path, &statbuf) == 0) {
                if (S_ISREG(statbuf.st_mode)) {
                    HEADER_SECTION_FILE h;
                    if (parseSF(file_path, &h, false) == 0) {
                        for (int i = 0; i < h.noOfSections; i++) {
                            if (extractSF(file_path, i, 15, &h) == 0) {
                                printf("%s\n", file_path);
                                break;
                            }
                        }
                    }
                    if (h.sectionsHeaders != NULL) {
                        free(h.sectionsHeaders);
                    }
                }
                else if (S_ISDIR(statbuf.st_mode)) {
                    // Skip directories
                    continue;
                }
            }
        }else if(filter == NULL) {
             if(lstat(file_path, &statbuf) == 0) {
                        if(S_ISREG(statbuf.st_mode) || S_ISDIR(statbuf.st_mode)) {
                            printf("%s\n", file_path); 

                        }}
        } else {
            if(filter[0] == 'r') { // grater_than
               long int size = 0;
                sscanf(filter + 2, "%ld", &size);
                if(lstat(path, &statbuf) == 0) {
                    if(file_stat.st_size > size) {
                            printf("%s\n", file_path);
                        }
                }
            } else {// ends in
                int length = 0;
                int lengthFilter = strlen(filter+ 2);
                length = strlen(entry->d_name);
                if(strncmp(entry->d_name + length - lengthFilter, filter + 2, lengthFilter) == 0) {
                    printf("%s\n", file_path); 
                }
            }
        }
    }  
    closedir(dir);
    return 0;
    }   

    void listRec(const char *path, char* filter, int* firstO, bool verifySF) {
        DIR *dir = NULL;
        struct dirent *entry = NULL;
        char fullPath[1024];
        struct stat statbuf;
        if(listDir(path, filter, firstO, verifySF) == -1){
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
                                listRec(fullPath, filter, firstO, verifySF);
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
            bool parse = false;
            bool extract = false;
            bool findall = false;
            for (int i = 1; i < argc; i++) {
                if (strcmp(argv[i], "list") == 0) {
                    list = true;
                    break;
                }
                if (strcmp(argv[i], "parse") == 0) {
                    parse = true;
                    break;
                }
                if (strcmp(argv[i], "extract") == 0) {
                    extract = true;
                    break;
                }
                if (strcmp(argv[i], "findall") == 0) {
                    findall = true;
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
                        filter = argv[i] + 11;
                        break;
                    } else if (strncmp(argv[i], "name_ends_with=", 14) == 0) {
                        filter = argv[i] + 13;
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
                    listRec(path, filter, &firstO, false);
                } else {
                    listDir(path, filter, &firstO, false);
                } 
            } else if(parse == true) {
                char* path = NULL;
                for (int i = 1; i < argc; i++) {
                    if (strncmp(argv[i], "path=", 5) == 0) {
                        path = argv[i] + 5;
                        break;
                    }
                }
                HEADER_SECTION_FILE h;
                parseSF(path, &h, true);
                free(h.sectionsHeaders);
            } else if(extract == true) {
                char* path = NULL;
                int section = 0;
                int line = 0;
                for (int i = 1; i < argc; i++) {
                    if (strncmp(argv[i], "path=", 5) == 0) {
                        path = argv[i] + 5;
                    }
                    if (strncmp(argv[i], "section=", 8) == 0) {
                        sscanf(argv[i] + 8, "%d", &section);
                    }
                    if (strncmp(argv[i], "line=", 5) == 0) {
                        sscanf(argv[i] + 5, "%d", &line);
                    }
                }
                HEADER_SECTION_FILE h;
                extractSF(path, section, line, &h);
            } else if(findall == true) {
                char* path = NULL;
                for (int i = 1; i < argc; i++) {
                    if (strncmp(argv[i], "path=", 5) == 0) {
                        path = argv[i] + 5;
                        break;
                    }
                }
                int firstO = 0;
                listRec(path, NULL, &firstO, false);
            }
        }
    } 
    
    return 0;
}
