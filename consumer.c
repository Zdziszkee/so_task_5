//
// Created by Kacper Kuchta on 5/13/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#define BUFF_LEN 10
#define DATA_FRAME 5    //has to be smaller than buffer length

#define FIFO_NAME argv[1]
#define CONSUMER_FILE argv[2]

int main(int argc, char *argv[]) {

    /* Bad input arguments in consumer program */
    if (argc != 3) {
        printf("Wrong number of arguments! Expected 2, received %d", argc - 1);
        return -1;
    }



    /* Open fifo pipe */
    int inputDescriptor = open(FIFO_NAME, O_RDONLY, 0666);
    if (inputDescriptor == -1) {
        perror("Cannot open fifo pipe in consumer program\n");
        exit(EXIT_FAILURE);
    }

    /* Open consumer file */
    int outputDescriptor = open(CONSUMER_FILE, O_TRUNC | O_CREAT | O_WRONLY, 0666);
    if (outputDescriptor == -1) {
        perror("Cannot open consumer file\n");
        exit(EXIT_FAILURE);
    }

    /* Buffer declaration */
    char buffer[BUFF_LEN];
    char termOut[BUFF_LEN + 24];

    /* Transferring loop*/
    ssize_t readData;
    while (1) {
        /* wait random time */
        sleep(rand() % 5);

        /* Read data from fifo pipe */
        if ((readData = read(inputDescriptor, &buffer, DATA_FRAME)) == -1) {
            perror("Failed to read fifo file in consumer program\n");
            exit(EXIT_FAILURE);
        }

        /* break loop */
        if (readData == 0) break;

        /* Write data to consumer file */
        if ((write(outputDescriptor, &buffer, readData)) == -1) {
            perror("Cannot write data to consumer file\n");
            exit(EXIT_FAILURE);
        }

        /* Write data to terminal */
        buffer[readData] = '\0';
        sprintf(termOut, "%s%s%c", "\x1b[31mDownloading...\x1b[0m", buffer, '\n');

        if (write(STDOUT_FILENO, termOut, strlen(termOut)) == -1) {
            perror("Write to terminal is impossible, write error\n");
            exit(EXIT_FAILURE);
        }
    }

    /* Close consumer file */
    if (close(outputDescriptor) == -1) {
        perror("Cannot close consumer file\n");
        exit(EXIT_FAILURE);
    }

    /* Close fifo pipe */
    if (close(inputDescriptor) == -1) {
        perror("Cannot close fifo pipe in consumer program\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}