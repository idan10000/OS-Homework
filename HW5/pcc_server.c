#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <assert.h>
#include <signal.h>

int connfd = -1;
int sigFlag = 0;
uint32_t pcc_total[95] = {0};

void end(int);

void signalHandler() {
    if (connfd < 0) {
        end(0);
    } else {
        sigFlag = 1; // will set it up so the server will exit once it finishes the current client handling.
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Invalid amount of arguments inserted: %s\n", strerror(EINVAL));
        exit(EXIT_FAILURE);
    }

    struct sigaction sigUSR1;
    sigUSR1.sa_handler = &signalHandler;
    if (sigaction(SIGUSR1, &sigUSR1, NULL) != 0) {
        fprintf(stderr, "Signal handler registration failed. %s\n", strerror(errno));
        return -1;
    }

    uint32_t pcc_temp[95] = {0};
    int listenfd = -1;
    struct sockaddr_in serv_addr;


    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (0 != bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) {
        printf("\n Error : Bind Failed. %s \n", strerror(errno));
        return 1;
    }

    if (0 != listen(listenfd, 10)) {
        printf("\n Error : Listen Failed. %s \n", strerror(errno));
        return 1;
    }

    // Start listening and handling clients
    while (1) {

        if (sigFlag)
            end(0);

        connfd = accept(listenfd, NULL, NULL);
        if (connfd < 0) {
            printf("\n Error : Accept Failed. %s \n", strerror(errno));
            return 1;
        }

        // ----------------- Read N -----------------

        uint32_t netInt;
        char *intBuff = (char *) &netInt;
        int bytesRead = 0;
        int curBytes = 1;
        while (curBytes > 0) {
            curBytes = read(connfd, intBuff + bytesRead, 4 - bytesRead);
            bytesRead += curBytes;
        }
        if (curBytes < 0) {
            if (!(errno == ETIMEDOUT || errno == ECONNRESET || errno == EPIPE)) {
                fprintf(stderr, "Failed to read N: %s\n", strerror(errno));
                exit(1);
            } else {
                fprintf(stderr, "TCP Error while reading N: %s\n", strerror(errno));
                close(connfd);
                connfd = -1;
                continue;
            }
        } else { // (curBytes == 0)
            if (bytesRead != 4) { // client process killed unexpectedly
                fprintf(stderr, "Client connection closed unexpectedly. Server still running: %s\n", strerror(errno));
                close(connfd);
                connfd = -1;
                continue;
            }
        }

        // ----------------- Read file -----------------
        int N = ntohl(netInt); // Amount of bits to be received
        char *readBuf = malloc(N);
        bytesRead = 0;
        curBytes = 1;
        while (curBytes > 0) {
            curBytes = read(connfd, readBuf + bytesRead, N - bytesRead);
            bytesRead += curBytes;
        }
        if (curBytes < 0) {
            if (!(errno == ETIMEDOUT || errno == ECONNRESET || errno == EPIPE)) {
                fprintf(stderr, "Failed to read file: %s\n", strerror(errno));
                exit(1);
            } else {
                fprintf(stderr, "TCP Error while reading N: %s\n", strerror(errno));
                close(connfd);
                connfd = -1;
                continue;
            }
        } else { // (curBytes == 0)
            if (bytesRead != N) { // client process killed unexpectedly
                fprintf(stderr, "Client connection closed unexpectedly. Server still running: %s\n", strerror(errno));
                close(connfd);
                connfd = -1;
                continue;
            }
        }

        // ----------------- Calculate C -----------------
        int C = 0;


        for (int i = 0; i < 95; ++i) { // reset temp buffer
            pcc_temp[i] = 0;
        }
        // count readable chars of each type
        for (int i = 0; i < N; i++) {
            if (32 <= readBuf[i] && readBuf[i] <= 126) {
                C++;
                pcc_temp[(int) (readBuf[i] - 32)]++;
            }
        }
        free(readBuf);

        // ----------------- Write C -----------------
        netInt = htonl(C);
        int bytesWritten = 0;
        curBytes = 1;
        while (curBytes > 0) {
            curBytes = write(connfd, intBuff + bytesWritten, 4 - bytesWritten);
            bytesWritten += curBytes;
        }
        if (curBytes < 0) {
            if (!(errno == ETIMEDOUT || errno == ECONNRESET || errno == EPIPE)) {
                fprintf(stderr, "Failed to read N: %s\n", strerror(errno));
                exit(1);
            } else {
                fprintf(stderr, "TCP Error while reading N: %s\n", strerror(errno));
                close(connfd);
                connfd = -1;
                continue;
            }
        } else { // (curBytes == 0)
            if (bytesWritten != 4) { // client process killed unexpectedly
                fprintf(stderr, "Client connection closed unexpectedly. Server still running: %s\n", strerror(errno));
                close(connfd);
                connfd = -1;
                continue;
            }
        }

        // if no error has occured until this point, update pcc_total from pcc_temp
        for (int i = 0; i < 95; i++) {
            pcc_total[i] += pcc_temp[i];
        }

        // close socket
        close(connfd);
        connfd = -1;
    }
}


void end(int signum) {
    for (int i = 0; i < 95; i++) {
        printf("char '%c' : %u times\n", (i + 32), pcc_total[i]);
    }
    exit(0);
}


