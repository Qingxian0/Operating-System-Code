#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAXSIZE 100

typedef int datatype;
typedef struct
{
    datatype a[MAXSIZE];
    int front;
    int rear;
} sequence_queue;

void init(sequence_queue *sq)
{
    sq->front = sq->rear = 0;
}

bool empty(sequence_queue *sq)
{
    return sq->front == sq->rear;
}

bool full(sequence_queue *sq)
{
    return sq->front == (sq->rear + 1) % MAXSIZE;
}

void display(sequence_queue *sq)
{
    for (int i = sq->front; i % MAXSIZE != sq->rear; i++)
    {
        printf("%d ", sq->a[i % MAXSIZE]);
    }
    printf("\n");
}

datatype dequeue(sequence_queue *sq)
{
    if (empty)
    {
        int x = sq->a[sq->front];
        sq->front = (sq->front + 1) % MAXSIZE;
        return x;
    }
    else
        printf("队列为空，无法出队\n");
}

void insertQueue(sequence_queue *sq, datatype x)
{
    if (!full(sq))
    {
        sq->a[sq->rear] = x;
        sq->rear = (sq->rear + 1) % MAXSIZE;
    }
    else
        printf("队列已满，无法插入");
}

void clearQueue(sequence_queue *sq)
{
    while (!empty(sq))
        dequeue(sq);
}

int main()
{
    datatype x;
    sequence_queue *sq = (sequence_queue *)malloc(sizeof(sequence_queue));
    init(sq);
    int n;
    printf("请输入需要创建队列的元素个数:");
    scanf("%d", &n);
    for (int i = 1; i <= n; ++i)
    {
        printf("请输入需要插入的值:");
        scanf("%d", &x);
        insertQueue(sq, x);
    }
    bool flag;
    do
    {
        printf("是否进行出队操作(1: 是, 0: 否):");
        scanf("%d", &flag);
        if (flag)
        {
            x = dequeue(sq);
            printf("出队的元素为: %d\n", x);
        }
        display(sq);
    } while (flag);
    do
    {
        printf("是否进行插入操作(1: 是, 0: 否):");
        scanf("%d", &flag);
        if (flag)
        {
            printf("请输入需要插入的值:");
            scanf("%d", &x);
            insertQueue(sq, x);
        }
        display(sq);
    } while (flag);

    clearQueue(sq);

    return 0;
}