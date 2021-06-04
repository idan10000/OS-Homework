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

void enqueue(DIR *dir, char *path) {
pthread_t tid = pthread_self();
    printf("enqueue thread = %lu\n", tid);
    pthread_mutex_lock(&queueMutex);
    node *newNode = malloc(sizeof(node));
    newNode->dir = dir;
    newNode->next = NULL;
    newNode->path = path;
    if (queue->head == NULL) {
        queue->head = newNode;
        queue->tail = newNode;
        queue->len++;
    } else {
        queue->tail->next = newNode;
        queue->len++;
    }
    pthread_mutex_unlock(&queueMutex);
    pthread_cond_signal(&queueNotEmptyCV);

}

node *dequeue() {
pthread_t tid = pthread_self();

    printf("dequeue called thread = %lu\n", tid);
    while (!startFlag) {
        printf("start flag = 0 thread = %lu\n", tid);
        pthread_mutex_lock(&startMutex);
        printf("locked start thread = %lu\n", tid);
        if((!startFlag))
            pthread_cond_wait(&startThreadsCV, &startMutex);
        printf("unlocked start thread = %lu\n", tid);

        pthread_mutex_unlock(&startMutex);
    }
    printf("before empty lock, queue len = %d thread = %lu\n", tid, queue->len);

    pthread_mutex_lock(&queueMutex);
    while (queue->len == 0) { //TODO check if accurate active threads
        printf("entered queue=0 thread = %lu\n", tid);
        printf("cur active threads before dequeue: %d thread = %lu\n", tid, activeThreads);
        if (activeThreads == 1) {
            printf("exiting thread = %lu\n", tid);
            exitFlag = 1;
            pthread_cond_broadcast(&queueNotEmptyCV);
            pthread_cond_destroy(&queueNotEmptyCV);
            pthread_mutex_unlock(&queueMutex);
            pthread_exit(NULL);
        }
        activeThreads--;
        printf("cur active threads before wait: %d thread = %lu\n", tid, activeThreads);
        printf("deque before wait thread = %lu\n", tid);
        if(activeThreads != 0)
            pthread_cond_wait(&queueNotEmptyCV, &queueMutex);
        printf("deque after wait thread = %lu\n", tid);
        if (exitFlag) {
            pthread_mutex_unlock(&queueMutex);
            pthread_exit(NULL);
        }
        printf("cur active threads after wait before increase: %d thread = %lu\n", tid, activeThreads);

        activeThreads++;
        printf("cur active threads after wait after increase: %d thread = %lu\n", tid, activeThreads);
    }
    printf("dequeue running thread = %lu\n", tid);

    node *temp = queue->head;
    if (queue->tail == queue->head) {
        queue->head = NULL;
        queue->tail = NULL;
        queue->len--;
    } else {
        queue->head = queue->head->next;
        queue->len--;
        temp->next = NULL;
    }
    printf("unlocking mutex thread = %lu\n", tid);

    pthread_mutex_unlock(&queueMutex);
    return temp;
}

void createThreads(pthread_t *threads, int threadAmount, char *searchTerm) {
pthread_t tid = pthread_self();
    for (size_t i = 0; i < threadAmount; i++) {
        printf("create Threads %d thread = %lu\n", tid, i);
        activeThreads++;
        int rc = pthread_create(&threads[i], NULL, handleThreads, (void *) searchTerm);
        if (rc) {
            fprintf(stderr, "Failed creating thread: %s thread = %lu\n", tid, strerror(rc));
            exit(EXIT_FAILURE);
        }
    }
    startFlag = 1;
    printf("sending start signal thread = %lu\n", tid);
    pthread_cond_broadcast(&startThreadsCV);
    pthread_cond_destroy(&startThreadsCV);
}

char isDirSearchable(char *path) {
    return access(path, R_OK | X_OK);
}

void *handleThreads(void *searchTerm) {
pthread_t tid = pthread_self();

    while (1) {
        node *dirNode = dequeue();
        DIR *dir = dirNode->dir;
        char *curPath = dirNode->path;
        printf("Current path %s thread = %lu\n", curPath,tid);

        char *term = (char *) searchTerm;
        struct dirent *curEntry;

        while ((curEntry = readdir(dir)) != NULL) {
            printf("Current entry name %s thread = %lu\n", curEntry->d_name,tid);
            if (!strcmp(curEntry->d_name, ".") || !strcmp(curEntry->d_name, "..")) {
                printf("at . or .. directories thread = %lu\n", tid);
                continue;
            }
//        struct stat stats;
//        if(lstat(curEntry->d_name,&stats) == -1){
//            fprintf(stderr, "Failed using lstat on dir %s: %s thread = %lu\n", tid, curEntry->d_name ,strerror(errno));
//        }
//        if(S_IFDIR(stats.st_mode)){
//
//        }

            // check if curEntry is a directory
//            if(strcmp(curEntry->d_name,"first") || strcmp(curEntry->d_name,"second")){
            printf("before d_type thread = %lu\n", tid);
            if (curEntry->d_type == DT_DIR) {
                printf("entered as directory thread = %lu\n", tid);
//            if(1){
//                char *newPath = malloc(strlen(curPath) + strlen(curEntry->d_name) + 2);
                char* newPath = malloc(PATH_MAX);
                strcpy(newPath, curPath);
                strcat(newPath, "/");
                strcat(newPath, curEntry->d_name);

                // check if curEntry is a searchable directory
                if (isDirSearchable(newPath) == -1) {
                    printf("Directory %s: Permission denied. thread = %lu\n", newPath,tid);
                    continue;
                }
                printf("cur active threads: %d thread = %lu\n", tid, activeThreads);
                printf("opening dir thread = %lu\n", tid);
                printf("new path %s thread = %lu\n",newPath,tid );
                DIR *tempDir = opendir(newPath);

                if (tempDir == NULL) {
                    fprintf(stderr, "Directory failed to open: %s thread = %lu\n", tid, strerror(errno));
                    printf("cur active threads exiting before reduction: %d thread = %lu\n", tid, activeThreads);
                    activeThreads--;
                    printf("cur active threads exiting after reduction: %d thread = %lu\n", tid, activeThreads);

                    pthread_cond_signal(&queueNotEmptyCV);
                    pthread_exit(NULL);
                }
                enqueue(tempDir, newPath);
            }
                // if curEntry contains the search term
            else {
                printf("search term: %s thread = %lu\n", term, tid);
                if (strstr(curEntry->d_name, term) != NULL) {
                    printf("entered as matching search term thread = %lu\n", tid);
                    char *newPath = malloc(strlen(curPath) + strlen(curEntry->d_name) + 2);
                    strcpy(newPath, curPath);
                    strcat(newPath, "/");
                    strcat(newPath, curEntry->d_name);
                    printf("%s thread = %lu\n",newPath, tid);
                    filesFound++;
                }
            }
        }
//        free(dirNode->path);
//        closedir(dir);
//        free(dirNode);
        printf("finished dir thread = %lu\n", tid);
    }
}


int main(int argc, char *argv[]) {
pthread_t tid = pthread_self();

    if (argc < 4) {
        fprintf(stderr, "Not enough arguments inserted: %s thread = %lu\n", tid, strerror(EINVAL));
        exit(EXIT_FAILURE);
    }

    char *rootDir = argv[1];
    char *searchTerm = argv[2];
    int threadAmount = atoi(argv[3]);

    printf("check searchable thread = %lu\n", tid);
    if (isDirSearchable(rootDir) == -1) {
        printf("Directory %s: Permission denied. thread = %lu\n", tid, rootDir);
        fprintf(stderr, "Root directory is not searchable: %s thread = %lu\n", tid, strerror(EINVAL));
        exit(EXIT_FAILURE);
    }

    queue = malloc(sizeof(MyQueue));
    queue->len = 1;
    node *headNode = malloc(sizeof(node));

    printf("open dir thread = %lu\n", tid);
    headNode->dir = opendir(rootDir);
    if (headNode->dir == NULL) {
        fprintf(stderr, "Root directory failed to open: %s thread = %lu\n", tid, strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("test1 thread = %lu\n", tid);
    headNode->next = NULL;
    headNode->path = rootDir;
    printf("test2 thread = %lu\n", tid);

    queue->head = headNode;
    queue->tail = headNode;


    printf("init mutex thread = %lu\n", tid);
    // Initialize mutex and condition variable objects
    pthread_mutex_init(&queueMutex, NULL);
    pthread_cond_init(&queueNotEmptyCV, NULL);
    pthread_cond_init(&startThreadsCV, NULL);
    pthread_mutex_init(&startMutex, NULL);

    printf("init flags thread = %lu\n", tid);
    exitFlag = 0;
    startFlag = 0;
    activeThreads = 0;
    filesFound = 0;
    pthread_t threads[threadAmount];

    createThreads(threads, threadAmount, searchTerm);

    printf("wait threads thread = %lu\n", tid);
    for (int i = 0; i < threadAmount; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("Done searching, found %d\n", filesFound);
    pthread_mutex_destroy(&queueMutex);
    exit(EXIT_SUCCESS);
}

