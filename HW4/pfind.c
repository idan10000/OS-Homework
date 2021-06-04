#define _GNU_SOURCE

#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>


void *handleThreads(void *searchTerm);

typedef struct queueNode {
    DIR *dir;
    char *path;
    struct queueNode *next;
} node;

typedef struct queue {
    int len;
    node *head;
    node *tail;
} MyQueue;

pthread_mutex_t queueMutex;
pthread_cond_t queueNotEmptyCV;

pthread_cond_t startThreadsCV;
pthread_mutex_t startMutex;


static atomic_int startFlag;
static atomic_int exitFlag;

static atomic_int activeThreads;
static atomic_int filesFound;
static MyQueue *queue;


// add a node to the queue
void enqueue(DIR *dir, char *path) {

    pthread_mutex_lock(&queueMutex);
    node *newNode = malloc(sizeof(node));
    if (newNode == NULL) {
        fprintf(stderr, "Failed allocating memory: %s\n", strerror(ENOMEM));
        pthread_exit(NULL);
    }

    // create new node
    newNode->dir = dir;
    newNode->next = NULL;
    newNode->path = path;

    if (queue->len != 0) {
        queue->tail->next = newNode;
        queue->tail = queue->tail->next;
        queue->len++;

    } else {
        queue->head = newNode;
        queue->tail = newNode;
        queue->len++;
    }
    pthread_mutex_unlock(&queueMutex);
    pthread_cond_signal(&queueNotEmptyCV);

}

node *dequeue() {
    // stop threads from running until all threads are created
    while (!startFlag) {
        pthread_mutex_lock(&startMutex);
        if ((!startFlag))
            pthread_cond_wait(&startThreadsCV, &startMutex);

        pthread_mutex_unlock(&startMutex);
    }


    pthread_mutex_lock(&queueMutex);
    //if queue is empty either wait or close the program
    while (queue->len == 0) {
        // if queue is empty and the thread is the last active thread, close program
        if (activeThreads == 1) {
            exitFlag = 1;
            pthread_cond_broadcast(&queueNotEmptyCV);
            pthread_cond_destroy(&queueNotEmptyCV);
            pthread_mutex_unlock(&queueMutex);
            pthread_exit(NULL);
        }
        activeThreads--;
        if (activeThreads != 0)
            pthread_cond_wait(&queueNotEmptyCV, &queueMutex); // wait until queue isn't empty and the signal was sent.

        if (exitFlag) { // if the closing procedure has started
            pthread_mutex_unlock(&queueMutex);
            pthread_exit(NULL);
        }
        activeThreads++;
    }

    // dequeue the head node
    node *temp = queue->head;

    queue->head = queue->head->next;
    queue->len--;
    if (queue->head == NULL)
        queue->tail = NULL;

    temp->next = NULL;

    pthread_mutex_unlock(&queueMutex);
    return temp;
}

// create the working threads of the program
void createThreads(pthread_t *threads, int threadAmount, char *searchTerm) {
    for (size_t i = 0; i < threadAmount; i++) {
        activeThreads++;
        int rc = pthread_create(&threads[i], NULL, handleThreads, (void *) searchTerm);
        if (rc) {
            fprintf(stderr, "Failed creating thread: %s\n", strerror(rc));
            exit(EXIT_FAILURE);
        }
    }
    // send starting signal and update flag
    startFlag = 1;
    pthread_cond_broadcast(&startThreadsCV);
    pthread_cond_destroy(&startThreadsCV);
}

// check if dir is searchable
char isDirSearchable(char *path) {
    return access(path, R_OK | X_OK);
}

void *handleThreads(void *searchTerm) {

    while (1) {
        node *dirNode = dequeue();
        DIR *dir = dirNode->dir;
        char *curPath = dirNode->path;
        char *term = (char *) searchTerm;
        struct dirent *curEntry;

        while ((curEntry = readdir(dir)) != NULL) {
            // if on . or .. skip
            if (!strcmp(curEntry->d_name, ".") || !strcmp(curEntry->d_name, "..")) {
                continue;
            }


            // check if curEntry is a directory
            if (curEntry->d_type == DT_DIR) {
                //create path to the new dir
                char *newPath = malloc(PATH_MAX);
                if (newPath == NULL) {
                    fprintf(stderr, "Failed allocating memory: %s\n", strerror(ENOMEM));
                    pthread_exit(NULL);
                }
                strcpy(newPath, curPath);
                strcat(newPath, "/");
                strcat(newPath, curEntry->d_name);

                // check if curEntry is a searchable directory
                if (isDirSearchable(newPath) == -1) {
                    printf("Directory %s: Permission denied.\n", newPath);
                    continue;
                }

                // open dir and insert it into queue
                DIR *tempDir = opendir(newPath);

                if (tempDir == NULL) {
                    fprintf(stderr, "Directory failed to open: %s\n", strerror(errno));
                    activeThreads--;
                    pthread_cond_signal(&queueNotEmptyCV);
                    pthread_exit(NULL);
                }
                enqueue(tempDir, newPath);
            } else {   // if curEntry contains the search term
                if (strstr(curEntry->d_name, term) != NULL) {
                    char *newPath = malloc(PATH_MAX);
                    if (newPath == NULL) {
                        fprintf(stderr, "Failed allocating memory: %s\n", strerror(ENOMEM));
                        pthread_exit(NULL);
                    }
                    strcpy(newPath, curPath);
                    strcat(newPath, "/");
                    strcat(newPath, curEntry->d_name);
                    filesFound++;
                }
            }
        }
        closedir(dir);
        free(dirNode);
    }
}


int main(int argc, char *argv[]) {
    // check if enough arguments inserted
    if (argc < 4) {
        fprintf(stderr, "Not enough arguments inserted: %s\n", strerror(EINVAL));
        exit(EXIT_FAILURE);
    }

    char *rootDir = argv[1];
    char *searchTerm = argv[2];
    int threadAmount = atoi(argv[3]);

    // check if root dir is searchable
    if (isDirSearchable(rootDir) == -1) {
        printf("Directory %s: Permission denied.\n", rootDir);
        fprintf(stderr, "Root directory is not searchable: %s\n", strerror(EINVAL));
        exit(EXIT_FAILURE);
    }

    // create the queue and initialize it to start from root dir
    queue = malloc(sizeof(MyQueue));
    if (queue == NULL) {
        fprintf(stderr, "Failed allocating memory: %s\n", strerror(ENOMEM));
        pthread_exit(NULL);
    }

    queue->len = 1;
    node *headNode = malloc(sizeof(node));
    if (headNode == NULL) {
        fprintf(stderr, "Failed allocating memory: %s\n", strerror(ENOMEM));
        pthread_exit(NULL);
    }

    headNode->dir = opendir(rootDir);
    if (headNode->dir == NULL) {
        fprintf(stderr, "Root directory failed to open: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    headNode->next = NULL;
    headNode->path = rootDir;

    queue->head = headNode;
    queue->tail = headNode;


    // Initialize mutex and condition variable objects
    pthread_mutex_init(&queueMutex, NULL);
    pthread_cond_init(&queueNotEmptyCV, NULL);
    pthread_cond_init(&startThreadsCV, NULL);
    pthread_mutex_init(&startMutex, NULL);

    // Init flags
    exitFlag = 0;
    startFlag = 0;
    activeThreads = 0;
    filesFound = 0;
    pthread_t threads[threadAmount];

    //start program
    createThreads(threads, threadAmount, searchTerm);

    // wait for all threads to exit
    for (int i = 0; i < threadAmount; i++) {
        pthread_join(threads[i], NULL);
    }

    // print result
    printf("Done searching, found %d files\n", filesFound);
    pthread_mutex_destroy(&queueMutex);
    exit(EXIT_SUCCESS);
}
