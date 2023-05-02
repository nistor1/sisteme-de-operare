#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <pthread.h>

#define NR_THREADS 5

typedef struct _THREAD_DATA{
    int threadProcess;
    int threadIndex;
} THREAD_DATA;
void* threadFunction(void* arg) {
    THREAD_DATA tempT = *(THREAD_DATA*)arg;
    int threadId = tempT.threadIndex;
    info(BEGIN, tempT.threadProcess, threadId + 1);
    info(END, tempT.threadProcess, threadId + 1);
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

        for (int i = 0; i < NR_THREADS; i++) {
            params2[i].threadIndex = i;
            params2[i].threadProcess = 2;
            pthread_create(&t2[i], NULL, threadFunction, &params2[i]);
        }

        for (int i = 0; i < NR_THREADS; i++) {
            pthread_join(t2[i], NULL);
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

                pthread_t t5[NR_THREADS];
                THREAD_DATA params5[NR_THREADS];

                for (int i = 0; i < NR_THREADS; i++) {
                    params5[i].threadIndex = i;
                    params5[i].threadProcess = 5;
                    pthread_create(&t5[i], NULL, threadFunction, &params5[i]);
                }

                for (int i = 0; i < NR_THREADS; i++) {
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
