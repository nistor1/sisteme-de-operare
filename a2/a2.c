#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"

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
                        info(END, 8, 0);
                        exit(0);
                    } else {
                        waitpid(pid8, NULL, 0);
                    } 
                    info(END, 7, 0);
                    exit(0);     
                } else {
                    waitpid(pid7, NULL, 0);
                }
                info(END, 6, 0);
                exit(0);      
            } else {
                waitpid(pid6, NULL, 0);
            }
            info(END, 3, 0);
            exit(0);
        } else {
            waitpid(pid3, NULL, 0); 
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
                info(END, 5, 0);
                exit(0);
            } else {
                waitpid(pid5, NULL, 0); 
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
