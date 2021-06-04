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

int isQueueEmpty(void) {
    return (queue->head == NULL) && (queue->tail == NULL);
}

void enqueue(DIR *dir, char *path) {
    printf("enqueue\n");
    pthread_mutex_lock(&queueMutex);
    node *newNode = malloc(sizeof(node));
    if (newNode == NULL) {
        printf("mem error at enqueue");
        fprintf(stderr, "Failed allocating memory: %s\n", strerror(ENOMEM));
        pthread_exit(NULL);
    }
    newNode->dir = dir;
    newNode->next = NULL;
    newNode->path = path;
    if (!isQueueEmpty()) {
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
    printf("dequeue called\n");
    while (!startFlag) {
        printf("start flag = 0\n");
        pthread_mutex_lock(&startMutex);
        printf("locked start\n");
        if ((!startFlag))
            pthread_cond_wait(&startThreadsCV, &startMutex);
        printf("unlocked start\n");

        pthread_mutex_unlock(&startMutex);
    }
    printf("before empty lock, queue len = %d\n", queue->len);

    pthread_mutex_lock(&queueMutex);
    while (queue->len == 0) { //TODO check if accurate active threads
        printf("entered queue=0\n");
        printf("cur active threads before dequeue: %d\n", activeThreads);
        if (activeThreads == 1) {
            printf("exiting\n");
            exitFlag = 1;
            pthread_cond_broadcast(&queueNotEmptyCV);
            pthread_cond_destroy(&queueNotEmptyCV);
            pthread_mutex_unlock(&queueMutex);
            pthread_exit(NULL);
        }
        activeThreads--;
        printf("cur active threads before wait: %d\n", activeThreads);
        printf("deque before wait\n");
        if (activeThreads != 0)
            pthread_cond_wait(&queueNotEmptyCV, &queueMutex);
        printf("deque after wait\n");
        if (exitFlag) {
            pthread_mutex_unlock(&queueMutex);
            pthread_exit(NULL);
        }
        printf("cur active threads after wait before increase: %d\n", activeThreads);

        activeThreads++;
        printf("cur active threads after wait after increase: %d\n", activeThreads);
    }
    printf("dequeue running\n");

    node *temp = queue->head;
    printf("is node null? : %d\n", temp == NULL);


    queue->head = queue->head->next;
    queue->len--;
    if(queue->head == NULL)
        queue->tail = NULL;

    temp->next = NULL;

    printf("unlocking mutex\n");
    printf("is node null? : %d\n", temp == NULL);

    pthread_mutex_unlock(&queueMutex);
    return temp;
}

void createThreads(pthread_t *threads, int threadAmount, char *searchTerm) {
    for (size_t i = 0; i < threadAmount; i++) {
        printf("create Threads %d\n", i);
        activeThreads++;
        int rc = pthread_create(&threads[i], NULL, handleThreads, (void *) searchTerm);
        if (rc) {
            fprintf(stderr, "Failed creating thread: %s\n", strerror(rc));
            exit(EXIT_FAILURE);
        }
    }
    startFlag = 1;
    printf("sending start signal\n");
    pthread_cond_broadcast(&startThreadsCV);
    pthread_cond_destroy(&startThreadsCV);
}

char isDirSearchable(char *path) {
    return access(path, R_OK | X_OK);
}

void *handleThreads(void *searchTerm) {

    while (1) {
        node *dirNode = dequeue();
        printf("test1\n");
        DIR *dir = dirNode->dir;
        printf("test2\n");
        char *curPath = dirNode->path;
        printf("test3\n");
        char *term = (char *) searchTerm;
        printf("test4\n");
        struct dirent *curEntry;

        while ((curEntry = readdir(dir)) != NULL) {
            printf("Current entry name %s\n", curEntry->d_name);
            if (!strcmp(curEntry->d_name, ".") || !strcmp(curEntry->d_name, "..")) {
                printf("at . or .. directories\n");
                continue;
            }


            // check if curEntry is a directory
            printf("before d_type\n");
            if (curEntry->d_type == DT_DIR) {
                printf("entered as directory\n");
//            if(1){
                char *newPath = malloc(PATH_MAX);
                strcpy(newPath, curPath);
                strcat(newPath, "/");
                strcat(newPath, curEntry->d_name);

                // check if curEntry is a searchable directory
                if (isDirSearchable(newPath) == -1) {
                    printf("Directory %s: Permission denied.\n", newPath);
                    continue;
                }
                printf("cur active threads: %d\n", activeThreads);
                printf("opening dir\n");
                printf("new path %s\n", newPath);
                DIR *tempDir = opendir(newPath);

                if (tempDir == NULL) {
                    fprintf(stderr, "Directory failed to open: %s\n", strerror(errno));
                    printf("cur active threads exiting before reduction: %d\n", activeThreads);
                    activeThreads--;
                    printf("cur active threads exiting after reduction: %d\n", activeThreads);

                    pthread_cond_signal(&queueNotEmptyCV);
                    pthread_exit(NULL);
                }
                enqueue(tempDir, newPath);
            }
                // if curEntry contains the search term
            else {
                printf("search term: %s\n", term);
                if (strstr(curEntry->d_name, term) != NULL) {
                    printf("entered as matching search term\n");
                    char *newPath = malloc(PATH_MAX);
                    strcpy(newPath, curPath);
                    strcat(newPath, "/");
                    strcat(newPath, curEntry->d_name);
                    printf("%s\n", newPath);
                    filesFound++;
                }
            }
        }
        printf("freeing memory\n");
//        free(curPath); TODO: check why it crashes with this line
        closedir(dir);
        free(dirNode);
        printf("finished dir\n");
    }
}


int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Not enough arguments inserted: %s\n", strerror(EINVAL));
        exit(EXIT_FAILURE);
    }

    char *rootDir = argv[1];
    char *searchTerm = argv[2];
    int threadAmount = atoi(argv[3]);

    printf("check searchable\n");
    if (isDirSearchable(rootDir) == -1) {
        printf("Directory %s: Permission denied.\n", rootDir);
        fprintf(stderr, "Root directory is not searchable: %s\n", strerror(EINVAL));
        exit(EXIT_FAILURE);
    }

    queue = malloc(sizeof(MyQueue));
    queue->len = 1;
    node *headNode = malloc(sizeof(node));

    printf("open dir\n");
    headNode->dir = opendir(rootDir);
    if (headNode->dir == NULL) {
        fprintf(stderr, "Root directory failed to open: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("test1\n");
    headNode->next = NULL;
    headNode->path = rootDir;
    printf("test2\n");

    queue->head = headNode;
    queue->tail = headNode;


    printf("init mutex\n");
    // Initialize mutex and condition variable objects
    pthread_mutex_init(&queueMutex, NULL);
    pthread_cond_init(&queueNotEmptyCV, NULL);
    pthread_cond_init(&startThreadsCV, NULL);
    pthread_mutex_init(&startMutex, NULL);

    printf("init flags\n");
    exitFlag = 0;
    startFlag = 0;
    activeThreads = 0;
    filesFound = 0;
    pthread_t threads[threadAmount];

    createThreads(threads, threadAmount, searchTerm);

    printf("wait threads\n");
    for (int i = 0; i < threadAmount; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("Done searching, found %d files\n", filesFound);
    pthread_mutex_destroy(&queueMutex);
    exit(EXIT_SUCCESS);
}
