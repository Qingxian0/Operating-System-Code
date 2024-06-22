#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#define BUFFER_SIZE 10

// 缓冲区
int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;
int count = 0;

// 互斥锁和条件变量
pthread_mutex_t mutex;
pthread_cond_t can_produce;
pthread_cond_t can_consume;

// 生产者函数
void *producer(void *arg)
{
    int item;
    for (int i = 0; i < 100; i++)
    {
        // 生产数据
        item = rand() % 100;
        // 加锁
        pthread_mutex_lock(&mutex);
        // 如果缓冲区已满，等待消费者消费
        while (count == BUFFER_SIZE)
        {
            pthread_cond_wait(&can_produce, &mutex);
        }
        // 将数据放入缓冲区
        buffer[in] = item;
        in = (in + 1) % BUFFER_SIZE;
        count++;
        printf("Produced: %d\n", item);
        // 通知消费者可以消费了
        pthread_cond_signal(&can_consume);
        // 解锁
        pthread_mutex_unlock(&mutex);
        // 生产者休息一会儿
        sleep(rand() % 3);
    }
    return NULL;
}

// 消费者函数
void *consumer(void *arg)
{
    int item;
    for (int i = 0; i < 100; i++)
    {
        // 加锁
        pthread_mutex_lock(&mutex);
        // 如果缓冲区为空，等待生产者生产
        while (count == 0)
        {
            pthread_cond_wait(&can_consume, &mutex);
        }
        // 从缓冲区取出数据
        item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        count--;
        printf("Consumed: %d\n", item);
        // 通知生产者可以生产了
        pthread_cond_signal(&can_produce);
        // 解锁
        pthread_mutex_unlock(&mutex);
        // 消费者休息一会儿
        sleep(rand() % 3);
    }
    return NULL;
}

int main()
{
    pthread_t producer_thread, consumer_thread;
    // 初始化互斥锁和条件变量
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&can_produce, NULL);
    pthread_cond_init(&can_consume, NULL);
    // 创建生产者和消费者线程
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);
    // 等待线程结束
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);
    // 清理资源
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&can_produce);
    pthread_cond_destroy(&can_consume);
    return 0;
}