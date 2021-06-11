#define _DEFAULT_SOURCE

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

// MINIMAL ERROR HANDLING FOR EASE OF READING

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Not enough arguments inserted: %s\n", strerror(EINVAL));
        exit(EXIT_FAILURE);
    }

    // Open file for reading

    int sockfd = -1;
    char *sendBuffer;

    // ----------------- open file and setup output buffer -----------------
    FILE *readFile = fopen(argv[3], "rb");
    if (readFile == NULL) {
        fprintf(stderr, "Input file failed to open %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    //get size of file
    fseek(readFile, 0, SEEK_END);
    uint32_t N = ftell(readFile);
    fseek(readFile, 0, SEEK_SET);

    // setup buffer
    sendBuffer = malloc(N);
    if (sendBuffer == NULL) {
        fprintf(stderr, "Failed allocating memory: %s\n", strerror(ENOMEM));
        exit(EXIT_FAILURE);
    }

    // read file
    if (fread(sendBuffer, 1, N, readFile) != N) {
        fprintf(stderr, "Error reading file %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    fclose(readFile);

    struct sockaddr_in serv_addr; // where we Want to get to

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2])); // Note: htons for endiannes
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]); // hardcoded...


    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("\n Error : Connect Failed. %s \n", strerror(errno));
        return 1;
    }

    uint32_t netInt = htonl(N);
    char* inBuff = (char *) &netInt;
    int bytesWritten = 0;

    // Write N to server
    while(bytesWritten < 4){
        int curBytes = write(sockfd, inBuff + bytesWritten, 4 - bytesWritten);
        if(curBytes < 0){
            fprintf(stderr, "Failed writing N to server: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        bytesWritten += curBytes;
    }

    // Write file to server
    bytesWritten = 0;
    while(bytesWritten < N){
        int curBytes = write(sockfd, sendBuffer + bytesWritten, N - bytesWritten);
        if(curBytes < 0){
            fprintf(stderr, "Failed writing file to server: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        bytesWritten += curBytes;
    }

    // Read C from server
    int bytesRead = 0;
    while(bytesRead < 4){
        int curBytes = read(sockfd, inBuff + bytesRead, 4 - bytesRead);
        if(curBytes < 0){
            fprintf(stderr, "Failed writing N to server: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        bytesRead += curBytes;

    }



    close(sockfd); // is socket really done here?
    free(sendBuffer);
    uint32_t C = ntohl(netInt);
    printf("# of printable characters: %u\n", C);
    exit(0);
}
