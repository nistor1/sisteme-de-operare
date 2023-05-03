#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <pthread.h>
#include <semaphore.h>

#define NR_THREADS 5

typedef struct _THREAD_DATA{
    int threadProcess;
    int threadIndex;
    pthread_t* t;
} THREAD_DATA;

sem_t semP2[5];

void* threadFunction(void* arg) {
    THREAD_DATA tempT = *(THREAD_DATA*)arg;
    int threadId = tempT.threadIndex;
    threadId++;
    info(BEGIN, tempT.threadProcess, threadId);
    info(END, tempT.threadProcess, threadId);

    pthread_exit(NULL);
}
void* threadFunction2(void* arg) {
    THREAD_DATA tempT = *(THREAD_DATA*)arg;
    int threadId = tempT.threadIndex;
    threadId++;

    if(threadId == 0) {
            info(BEGIN, tempT.threadProcess, threadId);

            for(int i = 0; i < NR_THREADS; i++) {
                sem_wait(&semP2[0]);
            }
            info(END, tempT.threadProcess, threadId);

        } else {
            sem_post(&semP2[0]);
                if(threadId == 1) {
                    info(BEGIN, tempT.threadProcess, threadId);
                    sem_post(&semP2[3]);


                    sem_wait(&semP2[1]);
                    info(END, tempT.threadProcess, threadId);
                } else {
                    if(threadId == 3) {
                        sem_wait(&semP2[3]);
                        info(BEGIN, tempT.threadProcess, threadId );

                        
                        info(END, tempT.threadProcess, threadId);
                        sem_post(&semP2[1]);
                    } else {
                        info(BEGIN, tempT.threadProcess, threadId);
                        info(END, tempT.threadProcess, threadId);
                    }

                }
            }

    pthread_exit(NULL);
}
int main() {
    init();
    pid_t pid2, pid3, pid4, pid5, pid6, pid7, pid8;
    info(BEGIN, 1, 0);


    pid2 = fork();
    if (pid2 == -1) {
        return -1;
    } else if (pid2 == 0) {
        info(BEGIN, 2, 0);

        pid3 = fork();
        if (pid3 == -1) {
            return -1;
        } else if (pid3 == 0) {
            info(BEGIN, 3, 0);

            pid6 = fork();
            if (pid6 == -1) {
                return -1;
            } else if (pid6 == 0) {
                info(BEGIN, 6, 0);
                pid7 = fork();
                if (pid7 == -1) {
                    return -1;
                } else if (pid7 == 0) {
                    info(BEGIN, 7, 0);

                    pid8 = fork();
                    if (pid8 == -1) {
                        return -1;
                    } else if (pid8 == 0) {
                        info(BEGIN, 8, 0);
                        pthread_t t8[NR_THREADS];
                        THREAD_DATA params8[NR_THREADS];

                        for (int i = 0; i < NR_THREADS; i++) {
                            params8[i].threadIndex = i;
                            params8[i].threadProcess = 8;
                            params8[i].t = NULL;
                            pthread_create(&t8[i], NULL, threadFunction, &params8[i]);
                        }

                        for (int i = 0; i < NR_THREADS; i++) {
                            pthread_join(t8[i], NULL);
                        }

                        info(END, 8, 0);
                        exit(0);
                    } else {
                        waitpid(pid8, NULL, 0);
                    } 
                    pthread_t t7[NR_THREADS];
                    THREAD_DATA params7[NR_THREADS];

                    for (int i = 0; i < NR_THREADS; i++) {
                        params7[i].threadIndex = i;
                        params7[i].threadProcess = 7;
                        params7[i].t = NULL;
                        pthread_create(&t7[i], NULL, threadFunction, &params7[i]);
                    }

                    for (int i = 0; i < NR_THREADS; i++) {
                        pthread_join(t7[i], NULL);
                    }
                    info(END, 7, 0);
                    exit(0);     
                } else {
                    waitpid(pid7, NULL, 0);
                }
                pthread_t t6[NR_THREADS];
                THREAD_DATA params6[NR_THREADS];

                for (int i = 0; i < NR_THREADS; i++) {
                    params6[i].threadIndex = i;
                    params6[i].threadProcess = 6;
                    params6[i].t = NULL;
                    pthread_create(&t6[i], NULL, threadFunction, &params6[i]);
                }

                for (int i = 0; i < NR_THREADS; i++) {
                    pthread_join(t6[i], NULL);
                }

                info(END, 6, 0);
                exit(0);      
            } else {
                waitpid(pid6, NULL, 0);
            }

            pthread_t t3[NR_THREADS];
            THREAD_DATA params3[NR_THREADS];

            for (int i = 0; i < NR_THREADS; i++) {
                params3[i].threadIndex = i;
                params3[i].threadProcess = 3;
                params3[i].t = NULL;
                pthread_create(&t3[i], NULL, threadFunction, &params3[i]);
            }

            for (int i = 0; i < NR_THREADS; i++) {
                pthread_join(t3[i], NULL);
            }

            info(END, 3, 0);
            exit(0);
        } else {
            waitpid(pid3, NULL, 0); 
        }

        pthread_t t2[NR_THREADS];
        THREAD_DATA params2[NR_THREADS];
            sem_init(&semP2[0], 0, 1);
            sem_init(&semP2[1], 0, 0);
            sem_init(&semP2[2], 0, 1);
            sem_init(&semP2[3], 0, 0);
            sem_init(&semP2[4], 0, 1);
            sem_init(&semP2[5], 0, 1);

        for (int i = 0; i < NR_THREADS; i++) {
            params2[i].threadIndex = i;
            params2[i].threadProcess = 2;
            params2[i].t = t2;
            pthread_create(&t2[i], NULL, threadFunction2, &params2[i]);
        }

        for (int i = 0; i < NR_THREADS; i++) {
            pthread_join(t2[i], NULL);
            sem_destroy(&semP2[i]);
        }

        info(END, 2, 0);
        exit(0);
    } else {
        pid4 = fork();
        if (pid4 == -1) {
            return -1;
        } else if (pid4 == 0) {
            info(BEGIN, 4, 0);

            pid5 = fork();
            if (pid5 == -1) {
                return -1;
            } else if (pid5 == 0) {
                info(BEGIN, 5, 0);

                pthread_t t5[40];
                THREAD_DATA params5[40];

                for (int i = 0; i < 40; i++) {
                    params5[i].threadIndex = i;
                    params5[i].threadProcess = 5;
                    params5[i].t = NULL;
                    pthread_create(&t5[i], NULL, threadFunction, &params5[i]);
                }

                for (int i = 0; i < 40; i++) {
                    pthread_join(t5[i], NULL);
                }

                info(END, 5, 0);
                exit(0);
            } else {
                waitpid(pid5, NULL, 0); 
            }
            pthread_t t4[NR_THREADS];
            THREAD_DATA params4[NR_THREADS];

            for (int i = 0; i < NR_THREADS; i++) {
                params4[i].threadIndex = i;
                params4[i].threadProcess = 4;
                params4[i].t = NULL;
                pthread_create(&t4[i], NULL, threadFunction, &params4[i]);
            }

            for (int i = 0; i < NR_THREADS; i++) {
                pthread_join(t4[i], NULL);
            }

            info(END, 4, 0);
            exit(0);
        } else {
            waitpid(pid4, NULL, 0); 
        }
    }
    waitpid(pid2, NULL, 0);
    info(END, 1, 0);

    return 0;
}
