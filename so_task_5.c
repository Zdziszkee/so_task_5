//
// Created by Kacper Kuchta on 5/13/23.
//
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/stat.h>

#define PRODUCER_FILE argv[1]
#define PRODUCER_X argv[2]
#define CONSUMER_FILE argv[3]
#define CONSUMER_X argv[4]


char *FIFO_NAME;
void closeFile(void) {
    unlink(FIFO_NAME);
}

int main(int argc, char *argv[]) {

    if (argc != 6) {
        printf("Wrong number of arguments! Expected 5, received %d", argc - 1);
        return -1;
    }
    FIFO_NAME = argv[5];

    if (mkfifo(FIFO_NAME, 0666) != 0) {
        perror("Can't create fifo\n");
        exit(EXIT_FAILURE);
    }

    if (atexit(closeFile) != 0) {
        perror("Atexit error\n");
        _exit(EXIT_FAILURE);
    }

    int statLock = 0;

    switch (fork()) {

        /* error case */
        case -1:
            perror("Fork error\n");
            exit(EXIT_FAILURE);

        case 0:
            /* child process */
            switch (fork()) {
                case -1:
                    perror("Grandson's fork error\n");
                    exit(EXIT_FAILURE);

                case 0:
                    /* producer process */
                    /* run consumer program in new process */
                    if (execlp(CONSUMER_X, CONSUMER_X, FIFO_NAME, CONSUMER_FILE, NULL) != 0) {
                        perror("Execlp consumer error\n");
                        exit(EXIT_FAILURE);
                    }
                    _exit(0);

                default:
                    /* consumer process */
                    /* run producer program in this process */
                    if (execlp(PRODUCER_X, PRODUCER_X, FIFO_NAME, PRODUCER_FILE, NULL) != 0) {
                        perror("Execlp producer error\n");
                        exit(EXIT_FAILURE);
                    }
                    /* wait to close child process */
                    if (wait(&statLock) == -1) {
                        perror("Wait1 error\n");
                        exit(EXIT_FAILURE);
                    }
                    _exit(0);
            }

        default:
            /* parent process */
            /* wait to close child process*/
            if (wait(&statLock) == -1) {
                perror("Wait2 error\n");
                exit(EXIT_FAILURE);
            }
            break;
    }

    return 0;
}