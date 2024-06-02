# Producer-Consumer Model

在C语言中实现生产者-消费者模型通常涉及到多线程编程和同步机制，如互斥锁（mutexes）和条件变量（condition variables）。下面是一个简单的生产者-消费者模型的实现，使用了POSIX线程（pthread）库。

这个例子中，生产者会将数据放入一个共享的缓冲区，而消费者则会从该缓冲区取出数据。

```c
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
void* producer(void* arg) {
    int item;
    while (1) {
        // 生产数据
        item = rand() % 100;
        // 加锁
        pthread_mutex_lock(&mutex);
        // 如果缓冲区已满，等待消费者消费
        while (count == BUFFER_SIZE) {
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
void* consumer(void* arg) {
    int item;
    while (1) {
        // 加锁
        pthread_mutex_lock(&mutex);
        // 如果缓冲区为空，等待生产者生产
        while (count == 0) {
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

int main() {
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
```

在这个例子中，我们定义了一个缓冲区`buffer`，生产者和消费者通过`in`和`out`索引来存取数据。`count`变量用于跟踪缓冲区中的数据数量。
生产者和消费者函数都使用了`pthread_mutex_lock`和`pthread_mutex_unlock`来保护对共享资源的访问。当缓冲区满时，生产者会使用`pthread_cond_wait`等待`can_produce`条件变量；当缓冲区空时，消费者会等待`can_consume`条件变量。当相应的条件满足时，另一个线程会使用`pthread_cond_signal`来唤醒等待的线程。
在`main`函数中，我们初始化了互斥锁和条件变量，并创建了生产者和消费者线程。最后，我们等待这两个线程结束，并清理了分配的资源。
请注意，这个例子主要用于演示目的，实际应用中可能需要更复杂的错误处理和同步机制。此外，生产者和消费者的数量和行为的配置可能需要根据具体的应用场景来调整。
