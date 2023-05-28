#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

#define FIFO_NAME1 "RESP_PIPE_19799"
#define FIFO_NAME2 "REQ_PIPE_19799"


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

int parseSF(char* dataFile, int sizeFile, HEADER_SECTION_FILE* headerSF) {
    int indexFile = sizeFile;
    int indexSections = 0;


                headerSF->magic[1] = dataFile[indexFile -1];
                headerSF->magic[0] = dataFile[indexFile -2];
   
                char tempShort[2];
                short int f = 0;
                tempShort[1] = dataFile[indexFile - 3];
                tempShort[0] = dataFile[indexFile - 4];
                memcpy(&f, tempShort, sizeof(short int));
                headerSF->headerSize = f;

                indexFile = sizeFile - headerSF->headerSize;

                memcpy(&headerSF->version, &dataFile[indexFile], sizeof(int));
                indexFile += 4;
                memcpy(&headerSF->noOfSections, &dataFile[indexFile], sizeof(char));
                indexFile++;

                indexSections = headerSF->noOfSections;

                int tempSectOffset = 0;
                int tempSectSize = 0;
                
                headerSF->sectionsHeaders = (SECTION_HEADERS*)malloc(headerSF->noOfSections * sizeof(SECTION_HEADERS));
                 if(headerSF->sectionsHeaders == NULL) {
                    return -1;
                }

                for( indexSections = 0; indexSections < headerSF->noOfSections; indexSections++) {
                    indexFile += 16;

                    indexFile++;
                    memcpy(&(tempSectOffset), &dataFile[indexFile], sizeof(int));
                    indexFile+=4;
                    memcpy(&(tempSectSize), &dataFile[indexFile], sizeof(int));
                    indexFile+=4;
            
                    headerSF->sectionsHeaders[indexSections].sectOffset = tempSectOffset;
                    headerSF->sectionsHeaders[indexSections].sectSize = tempSectSize;    
                }
       
     
       


    return 0;
}

int main() {
    unlink(FIFO_NAME1);
    int fdResp = -1;
    int fdReq = -1;
    unsigned int memSize = 0;
    volatile char *sharedMem = NULL;
    char *dataFile = NULL;
    off_t fileSize;


    if(mkfifo(FIFO_NAME1, 0600) != 0) {
        perror("ERROR\ncreating FIFO");
        return -1;
    }

    fdReq = open(FIFO_NAME2, O_RDONLY);
    if(fdReq == -1) {
        perror("ERROR\ncannot create the request PIPE");
        return -1;       
    }    
    fdResp = open(FIFO_NAME1, O_WRONLY);
    if(fdResp == -1) {
        perror("ERROR\ncannot create the response PIPE");
        return -1;       
    } 

    const char* requestMessage = "START!";
    if (write(fdResp, requestMessage, strlen(requestMessage)) == -1) {
        perror("ERROR\ncannot write to the response pipe");
        exit(EXIT_FAILURE);
    }

    printf("SUCCESS");

    for(;;) {
        char request[256] = {0};
        int i = -1;

        do {   
             i++;
            if(read(fdReq, &request[i], sizeof(request[i])) == -1) {
                perror("ERROR: cannot read from the request pipe");
                exit(EXIT_FAILURE);
            }

        }while(request[i] != '!');
 
        
        if (strcmp(request, "PING!") == 0) {
            const char* ping = "PING!";
            const char* pong = "PONG!";


            unsigned int id =19799;

            write(fdResp, ping, strlen(ping));
            write(fdResp, pong, strlen(pong));
            write(fdResp, &id, sizeof(unsigned int));
        } else if(strcmp(request, "CREATE_SHM!") == 0) {
            if(read(fdReq, &memSize, sizeof(unsigned int)) == -1) {
                perror("ERROR: cannot read from the request pipe");
                exit(EXIT_FAILURE);
            }

            const char* createSHM = "CREATE_SHM!";
            const char* success = "SUCCESS!";
            const char* error = "ERROR!";
            write(fdResp, createSHM, strlen(createSHM));

            int shmFd;
            shm_unlink("/08dWiY");
            shmFd = shm_open("/08dWiY", O_CREAT | O_RDWR, 0644);
            if(shmFd < 0) {
                write(fdResp, error, strlen(error));
                perror("Could not aquire shm");
                return 1;
            }
            
            ftruncate(shmFd, memSize);
            sharedMem = (volatile char*)mmap(0, memSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
            if(sharedMem == (void*)-1){
                write(fdResp, error, strlen(error));               
                perror("Could not map the shared memory");
                return 1;
            }

            write(fdResp, success, strlen(success));

        } else if(strcmp(request, "WRITE_TO_SHM!") == 0) {
            unsigned int offset = 0;
            unsigned int value = 0;
        
            if(read(fdReq, &offset, sizeof(unsigned int)) == -1) {
                perror("ERROR: cannot read from the request pipe");
                exit(EXIT_FAILURE);
            }           
            if(read(fdReq, &value, sizeof(unsigned int)) == -1) {
                perror("ERROR: cannot read from the request pipe");
                exit(EXIT_FAILURE);
            }
            const char* writeToSHM = "WRITE_TO_SHM!";
            const char* success = "SUCCESS!";
            const char* error = "ERROR!";
            write(fdResp, writeToSHM, strlen(writeToSHM));
            
            if(!(offset >= 0 && offset <= memSize - sizeof(unsigned int))){
                write(fdResp, error, strlen(error));
                continue;
            }

            memcpy((void*)(sharedMem + offset), &value, sizeof(unsigned int));

            write(fdResp, success, strlen(success));
        
        } else if(strcmp(request, "MAP_FILE!") == 0) {
       
            int i = -1;
            char fileName[128] = {0};
            do {   
                i++;
                if(read(fdReq, &fileName[i], sizeof(fileName[i])) == -1) {
                    perror("ERROR: cannot read from the request pipe");
                    exit(EXIT_FAILURE);
                }
            }while(fileName[i] != '!');
            const char* mapFile = "MAP_FILE!";
            const char* success = "SUCCESS!";
            const char* error = "ERROR!";
            write(fdResp, mapFile, strlen(mapFile));
            fileName[i] = 0;
            int fd;
            fd = open(fileName, O_RDONLY);
            if(fd == -1) {

                write(fdResp, error, strlen(error));
                return 1;
            }        

            fileSize = lseek(fd, 0, SEEK_END);
            lseek(fd, 0, SEEK_SET);

            dataFile = (char*)mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);
            if(dataFile == (void*)-1) {
                write(fdResp, error, strlen(error));
                perror("Could not map file");
                close(fd);
                return 1;
            }
            
            write(fdResp, success, strlen(success));
        } else if(strcmp(request, "READ_FROM_FILE_OFFSET!") == 0) {
            unsigned int offsetFile = 0;
            unsigned int numberOfBytes = 0;

            const char* readFromFIleOffset = "READ_FROM_FILE_OFFSET!";
            const char* success = "SUCCESS!";
            const char* error = "ERROR!";
        
            if(read(fdReq, &offsetFile, sizeof(unsigned int)) == -1) {
                perror("ERROR: cannot read from the request pipe");
                exit(EXIT_FAILURE);
            }           
            if(read(fdReq, &numberOfBytes, sizeof(unsigned int)) == -1) {
                perror("ERROR: cannot read from the request pipe");
                exit(EXIT_FAILURE);
            }      

            write(fdResp, readFromFIleOffset, strlen(readFromFIleOffset));
            if((offsetFile + numberOfBytes) >= fileSize) {
                write(fdResp, error, strlen(error));
                continue;
            }  
            if(numberOfBytes > memSize) {
                write(fdResp, error, strlen(error));
                continue;
            }  

            for (int i = 0; i < numberOfBytes; i++) {
                sharedMem[i] = dataFile[offsetFile + i];
            }

            write(fdResp, success, strlen(success));

        } else if(strcmp(request, "READ_FROM_FILE_SECTION!") == 0) {
            unsigned int offset = 0;
            unsigned int noOfBytes = 0;
            unsigned int sectionNo = 0;

            if(read(fdReq, &sectionNo, sizeof(unsigned int)) == -1) {
                perror("ERROR: cannot read from the request pipe");
                exit(EXIT_FAILURE);
            }           
            if(read(fdReq, &offset, sizeof(unsigned int)) == -1) {
                perror("ERROR: cannot read from the request pipe");
                exit(EXIT_FAILURE);
            }
            if(read(fdReq, &noOfBytes, sizeof(unsigned int)) == -1) {
                perror("ERROR: cannot read from the request pipe");
                exit(EXIT_FAILURE);
            }
            const char* readFromFileSection = "READ_FROM_FILE_SECTION!";
            const char* success = "SUCCESS!";
            const char* error = "ERROR!";
            write(fdResp, readFromFileSection, strlen(readFromFileSection));
            HEADER_SECTION_FILE h;
            if(parseSF(dataFile, fileSize, &h) == -1) {
                write(fdResp, error, strlen(error));
                continue;
            }

            if(noOfBytes > memSize) {
                write(fdResp, error, strlen(error));
                continue;
            }  
            
            if(sectionNo < 1 || sectionNo > h.noOfSections) {
                write(fdResp, error, strlen(error));
                continue;
            }
            for (int i = 0; i < noOfBytes; i++) {
                sharedMem[i] = dataFile[h.sectionsHeaders[sectionNo-1].sectOffset+ offset + i];
            }
            if(h.sectionsHeaders != NULL)
            free(h.sectionsHeaders);
        
            write(fdResp, success, strlen(success));
        
        } else if(strcmp(request, "READ_FROM_LOGICAL_SPACE_OFFSET!") == 0) {
            unsigned int offset = 0;
            unsigned int noOfBytes = 0;
       
            if(read(fdReq, &offset, sizeof(unsigned int)) == -1) {
                perror("ERROR: cannot read from the request pipe");
                exit(EXIT_FAILURE);
            }
            if(read(fdReq, &noOfBytes, sizeof(unsigned int)) == -1) {
                perror("ERROR: cannot read from the request pipe");
                exit(EXIT_FAILURE);
            }
            const char* readFromFileSection = "READ_FROM_LOGICAL_SPACE_OFFSET!";
            const char* success = "SUCCESS!";
            const char* error = "ERROR!";
            write(fdResp, readFromFileSection, strlen(readFromFileSection));
            HEADER_SECTION_FILE h;
            if(parseSF(dataFile, fileSize, &h) == -1) {
                write(fdResp, error, strlen(error));
                continue;
            }

            if(noOfBytes > memSize) {
                write(fdResp, error, strlen(error));
                continue;
            }  
            
            unsigned int logicalOffset = offset;

            for(int i = 0; i < h.noOfSections; i++){
                int secSize = h.sectionsHeaders[i].sectSize;
                int secOffset = h.sectionsHeaders[i].sectOffset;
                for(int j = 0; j < secSize; j++) {
                    sharedMem[logicalOffset] = dataFile[secOffset + j];
                    logicalOffset++;
                }
                logicalOffset = ((logicalOffset + 1023) / 1024) * 1024;
            }
            
            if(h.sectionsHeaders != NULL)
            free(h.sectionsHeaders);
        
            write(fdResp, success, strlen(success));
        
        }else if (strcmp(request, "EXIT!") == 0) {
            close(fdReq);
            close(fdResp);
            unlink(FIFO_NAME1);

            return 0;
        } else {
            close(fdReq);
            close(fdResp);
            unlink(FIFO_NAME1);
            return 0;
        }

    }



}