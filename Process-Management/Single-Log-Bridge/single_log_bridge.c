#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 初始化信号量
pthread_mutex_t eastMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t westMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t bridge = PTHREAD_MUTEX_INITIALIZER;

int eastCount = 0;
int westCount = 0;

void* eastThreadFunc(void* arg) {
    while (1) {
        pthread_mutex_lock(&eastMutex); // P(eastMutex)
        eastCount++;
        if (eastCount == 1) {
            pthread_mutex_lock(&bridge); // P(bridge)
        }
        pthread_mutex_unlock(&eastMutex); // V(eastMutex)

        // 上桥
        printf("东方行人过桥\n");
        sleep(1); // 模拟过桥时间

        // 下桥
        pthread_mutex_lock(&eastMutex); // P(eastMutex)
        eastCount--;
        if (eastCount == 0) {
            pthread_mutex_unlock(&bridge); // V(bridge)
        }
        pthread_mutex_unlock(&eastMutex); // V(eastMutex)
    }
    return NULL;
}

void* westThreadFunc(void* arg) {
    while (1) {
        pthread_mutex_lock(&westMutex); // P(westMutex)
        westCount++;
        if (westCount == 1) {
            pthread_mutex_lock(&bridge); // P(bridge)
        }
        pthread_mutex_unlock(&westMutex); // V(westMutex)

        // 上桥
        printf("西方行人过桥\n");
        sleep(1); // 模拟过桥时间

        // 下桥
        pthread_mutex_lock(&westMutex); // P(westMutex)
        westCount--;
        if (westCount == 0) {
            pthread_mutex_unlock(&bridge); // V(bridge)
        }
        pthread_mutex_unlock(&westMutex); // V(westMutex)
    }
    return NULL;
}

int main() {
    system("chcp 65001");
    pthread_t eastThread, westThread;

    // 创建线程
    if (pthread_create(&eastThread, NULL, eastThreadFunc, NULL) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&westThread, NULL, westThreadFunc, NULL) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    // 等待线程结束
    pthread_join(eastThread, NULL);
    pthread_join(westThread, NULL);

    return 0;
}