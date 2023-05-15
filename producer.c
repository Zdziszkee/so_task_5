//
// Created by Kacper Kuchta on 5/13/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BUFF_LEN 10
#define DATA_FRAME 5 //has to be smaller than BUFF_LEN

#define FIFO_NAME argv[1]
#define PRODUCER_FILE argv[2]

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Wrong number of arguments! Expected 2, received %d", argc - 1);
        return -1;
    }
    /* Open producer file*/
    int inputDescriptor = open(PRODUCER_FILE, O_RDONLY, 0666);
    if (inputDescriptor == -1) {
        perror("Cannot open producer file\n");
        exit(EXIT_FAILURE);
    }

    /* Open fifo pipe */
    int outputDescriptor = open(FIFO_NAME, O_WRONLY, 0666);
    if (outputDescriptor == -1) {
        perror("Cannot open fifo pipe in producer program\n");
        exit(EXIT_FAILURE);
    }

    /* Buffer declaration */
    char buffer[BUFF_LEN];
    char termOut[BUFF_LEN + 24];

    /* transferring loop */
    ssize_t readData;
    while (1) {
        /* wait random time */
        sleep(rand() % 5);

        /* read data from file*/
        if ((readData = read(inputDescriptor, &buffer, DATA_FRAME)) == -1) {
            perror("Failed to read producer file\n");
            exit(EXIT_FAILURE);
        }

        /* break loop */
        if (readData == 0) break;

        /* write to fifo pipe */
        if (write(outputDescriptor, &buffer, readData) == -1) {
            perror("Write to fifo pipe is impossible, write error\n");
            exit(EXIT_FAILURE);
        }

        /* write to terminal */
        buffer[readData] = '\0';
        sprintf(termOut, "%s%s%c", "\x1b[32mUploading...\x1b[0m", buffer, '\n');

        if (write(STDOUT_FILENO, termOut, strlen(termOut)) == -1) {
            perror("Write to terminal is impossible, write error\n");
            exit(EXIT_FAILURE);
        }

    }

    /* close producer file */
    if (close(inputDescriptor) == -1) {
        perror("Cannot close producer file\n");
        exit(EXIT_FAILURE);
    }

    /* close fifo pipe */
    if (close(outputDescriptor) == -1) {
        perror("Cannot close fifo pipe in producer program\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}