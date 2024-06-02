#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct data
{
    int hour;
    int min;
} time;

typedef struct node
{
    int id;        // 进程编号
    char name[20]; // 进程名
    time arrive;   // 到达就绪队列的时间
    int zx;        // 执行时间
    time start;    // 开始执行时间
    time finish;   // 执行完成时间
    int zz;        // 周转时间=执行完成时间-到达就绪队列时间
    float zzxs;    // 带权周转时间=周转时间/执行时间
    time current;  // 当前进程开始执行的时间
    int ywcsj;     // 进程已经完成的时间
    int sysj;      // 当前进程的剩余时间
    struct node *next;
} Node;

typedef struct Queue
{
    Node *front;
    Node *tail;
} Queue;

Queue *init()
{
    Queue *p = (Queue *)malloc(sizeof(Queue));
    p->front = NULL;
    p->tail = NULL;
    return p;
}

// 函数名:timeCompare		函数参数:，tt（系统计时）,p（当前进程的到达时间）
bool timeCompare(time tt, time p)
{
    // 函数功能：比较时间大小，tt<p返回false，否则返回true
    return tt.hour * 60 + tt.min >= p.hour * 60 + p.min;
}

// 函数名:idCompare		函数参数:，p1（进程1的id）,p2（进程2的id）
bool idCompare(int p1, int p2)
{
    // 函数功能：比较id大小，p1<p2返回true，否则返回false
    return p1 < p2;
}

// 交换两个节点的数据
void swap(Node *a, Node *b)
{
    // 交换节点数据
    int temp_id = a->id;
    char temp_name[20];
    strcpy(temp_name, a->name);
    time temp_arrive = a->arrive;
    int temp_zx = a->zx;
    time temp_start = a->start;
    time temp_finish = a->finish;
    int temp_zz = a->zz;
    double temp_zzxs = a->zzxs;
    time temp_current = a->current;
    int temp_ywcsj = a->ywcsj, temp_sysj = a->sysj;

    a->id = b->id;
    strcpy(a->name, b->name);
    a->arrive = b->arrive;
    a->zx = b->zx;
    a->start = b->start;
    a->finish = b->finish;
    a->zz = b->zz;
    a->zzxs = b->zzxs;
    a->current = b->current;
    a->ywcsj = b->ywcsj;
    a->sysj = b->sysj;

    b->id = temp_id;
    strcpy(b->name, temp_name);
    b->arrive = temp_arrive;
    b->zx = temp_zx;
    b->start = temp_start;
    b->finish = temp_finish;
    b->zz = temp_zz;
    b->zzxs = temp_zzxs;
    b->current = temp_current;
    b->ywcsj = temp_ywcsj;
    b->sysj = temp_sysj;
}

// 函数名:choose		函数参数:，p（进程指针）,q（进程指针），flag（选择标志）
bool choose(Node *p, Node *q, int flag)
{
    // 函数功能：选择比较函数比较名字大小，flag==1返回时间比较结果取反，否则返回名字比较结果
    if (flag == 1)
        return !timeCompare(p->arrive, q->arrive);
    else
        return idCompare(p->id, q->id);
}

// 函数名:Time_or_Name_Sorted		函数参数:，que（队列指针），flag（选择标志）
void Time_or_Name_Sorted(Queue *que, int flag)
{
    // 函数功能：选择排序函数并对进程进行排序，flag==1返回时间排序，否则返回名字排序
    if (que->front == NULL)
        return;
    Node *p = que->front, *last = NULL;
    bool done = true;
    do
    {
        done = true;
        p = que->front;
        while (p->next != last)
        {
            if (choose(p->next, p, flag))
            {
                swap(p, p->next);
                done = false;
            }
            p = p->next;
        }
        last = p;
    } while (!done);
}

void insert(Queue *que, Node *s)
{
    // 函数功能:尾插入队
    if (que->front == NULL || que->tail == NULL) // 空队列
    {
        que->front = que->tail = s;
        return;
    }
    que->tail->next = s;
    que->tail = s;
}

// 函数名:ScanIn	函数参数:，wait（队列指针），n（进程数）
void ScanIn(Queue *wait, int n)
{
    // 函数功能：输入相关进程及信息
    printf("id 名字 到达时间 执行时间\n");
    while (n--)
    {
        Node *tmp = (Node *)malloc(sizeof(Node));
        // id
        scanf("%d", &tmp->id);
        // 名字
        scanf("%s", tmp->name);
        // 到达时间
        scanf("%d:%d", &tmp->arrive.hour, &tmp->arrive.min);
        // 执行时间（分钟）
        scanf("%d", &tmp->zx);

        tmp->current.hour = 0;
        tmp->current.min = 0;
        tmp->ywcsj = 0;
        tmp->sysj = tmp->zx;
        tmp->next = NULL;
        insert(wait, tmp);
    }
}

// 函数名：cal，函数参数：Time x（当前进程的开始时间）, int y（当前进程的执行时间）
time cal(time x, int y)
{
    // 函数功能：计算当前进程的结束时间（开始时间+执行时间）
    time tmp;
    if (x.min + y >= 60)
    {
        tmp.hour = x.hour + 1;
        tmp.min = x.min + y - 60;
    }
    else
    {
        tmp.hour = x.hour;
        tmp.min = x.min + y;
    }
    return tmp;
}

// 计算时间差
int difference(time a, time b)
{
    return abs(b.hour * 60 + b.min - a.hour * 60 - a.min);
}

// 函数名:computeTime		函数参数:，tt（时间指针），q（结点指针），tp（时间片）
time computeTime(time tt, Node *q, int tp)
{
    // 函数功能：
    // 进程还未开始，首次更新进程的开始时间，并且每次重新定义进程执行的当前时间
    // 根据剩余时间与时间片的关系进行讨论
    // 更新进程的完成时间和周转时间,带权周转时间 ，最后返回时间指针
    // 调用timecompare()函数来寻找已到达的进程
    // 调用computeTime()函数来计算相关时间并更新进程信息

    q->current = tt;
    // 在时间片内执行完毕，主动释放CPU，并结束进程。
    if (q->sysj <= tp)
    {
        tt = cal(tt, q->sysj);
        q->ywcsj = q->zx;
        q->sysj = 0;
        q->finish = tt;
        q->zz = difference(q->arrive, q->finish);
        q->zzxs = (float)q->zz / q->zx;
    }
    else
    {
        tt = cal(tt, tp);
        q->ywcsj += tp;
        q->sysj -= tp;
    }
    return tt;
}

// 函数名:printCurrent		函数参数:，que（队列指针），tt（系统时间），i（时间轮转次数）
void printCurrent(Queue *que, time tt, int i)
{
    // 函数功能：输出第i次时间轮转的进程信息，置前进程开始执行的时间为0
    printf("第%d轮执行和就绪队列的结果：\n", i);
    printf("id\t名字\t到达时间 总执行时间（分钟）当前开始时间 已完成时间（分钟） 剩余完成时间（分钟）\n");
    Node *p = que->front;
    while (p != NULL)
    {
        printf("%d\t%s\t%2d:%02d\t %d（分钟）\t   %2d:%02d\t%d（分钟）\t   %d（分钟）\n",
               p->id, p->name, p->arrive.hour, p->arrive.min, p->zx,
               p->current.hour, p->current.min, p->ywcsj, p->sysj);
        p = p->next;
    }
    que->front->current.hour = 0;
    que->front->current.min = 0;
}

// 把执行完的process加到结果队列中
void dequeToTail(Queue *que, Queue *res)
{
    if (que->front == NULL)
        return;
    Node *p = que->front;
    que->front = p->next;
    p->next = NULL;
    insert(res, p);
}

void deleQueue(Queue *que)
{
    if (que->front != NULL)
    {
        Node *p = que->front, *q = que->front;
        while (p != NULL)
        {
            p = q->next;
            free(q);
            q = p;
        }
    }
}

// 函数名:Time_Slice_Rotation	函数参数:，que（队列指针），tp（时间片）
Queue *Time_Slice_Rotation(Queue *que, int tp)
{
    // 函数功能：模拟时间片轮转算法，分三种情况讨论：
    // 当前进程执行完
    // 当前进程未执行完
    // 除当前进程以外就绪队列队列为空，当前进程继续执行
    // 另外还要考虑就绪队列为空，诞下一个进程还未达到的情况。
    if (que->front == NULL)
    {
        printf("The queue is empty!\n");
        return NULL;
    }
    int i = 0; // 第i轮
    Queue *res = init(), *ready = init();
    time tt = que->front->arrive;
    dequeToTail(que, ready);
    while (que->front != NULL || ready->front != NULL)
    {
        // 就绪队列为空，没有比tt早到的process
        if (ready->front == NULL)
        {
            tt = que->front->arrive;
            dequeToTail(que, ready);
        }
        // 进行一轮调度
        i++;
        if (ready->front->ywcsj == 0)
            ready->front->start = tt;
        tt = computeTime(tt, ready->front, tp);
        printCurrent(ready, tt, i);

        // 把que中已到达的process加入到ready队列里
        while (que->front != NULL && timeCompare(tt, que->front->arrive))
            dequeToTail(que, ready);

        if (ready->front->sysj == 0) // 执行完毕，撤销该进程
            dequeToTail(ready, res);
        else // 未执行完毕，放到队尾
            dequeToTail(ready, ready);
    }
    return res;
}

// 函数名:Print 	函数参数:，que（队列指针），n（进程数）
void Print(Queue *que, int n)
{
    // 函数功能：输出所有进程完成后的信息以及系统平均周转时间和系统平均带权周转系数
    if (que == NULL || que->front == NULL)
    {
        printf("The queue is empty!\n");
        return;
    }
    printf("模拟进程时间片轮转进程调度过程输出结果：\n");
    printf("id\t名字\t到达时间 执行时间（分钟） 首次开始时间 完成时间 周转时间（分钟） 带权周转系数\n");
    Node *p = que->front;
    int zz_sum = 0, cnt = 0;
    float zzxs_sum = 0;
    while (p != NULL)
    {
        printf("%d\t%s\t%2d:%02d\t %d（分钟）\t  %2d:%02d\t\t%2d:%02d\t%d\t\t %.2f\n",
               p->id, p->name, p->arrive.hour, p->arrive.min, p->zx,
               p->start.hour, p->start.min, p->finish.hour, p->finish.min,
               p->zz, p->zzxs);
        cnt++;
        zz_sum += p->zz;
        zzxs_sum += p->zzxs;
        p = p->next;
    }
    printf("系统平均周转时间为：%14.2f\n", (float)zz_sum / cnt);
    printf("系统带权平均周转系数为：%10.2f\n", zzxs_sum / cnt);
}

int main()
{

    Queue *wait;
    wait = init();
    int flag, n, t;
    time earlytime;

    while (1)
    {
        printf("请输入操作:(1:开始进程;0:结束进程):");
        scanf("%d", &flag);
        if (flag == 0)
        {
            printf("\n操作结束!\n");
            break;
        }
        else
        {
            printf("请输入进程数量:\n");
            scanf("%d", &n);
            printf("请输入时间片时间:\n");
            scanf("%d", &t);
            ScanIn(wait, n);

            Time_or_Name_Sorted(wait, 1); // timesort
            wait = Time_Slice_Rotation(wait, t);
            Time_or_Name_Sorted(wait, 0); // namesort
            Print(wait, n);
            deleQueue(wait);
            wait = init();
        }
    }
    return 0;
}
