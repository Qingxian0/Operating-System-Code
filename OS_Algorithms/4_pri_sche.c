#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct data
{
    int hour;
    int min;
} time;

typedef struct linknode
{

    int id;        // 进程编号
    char name[20]; // 进程名
    int good;      // 优先级
    time arrive;   // 到达就绪队列的时间
    int zx;        // 执行时间
    time start;    // 开始执行时间
    time finish;   // 执行完成时间
    int zz;        // 周转时间=执行完成时间-到达就绪队列时间
    float zzxs;    // 带权周转时间=周转时间/执行时间
    struct linknode *next;

} linknode;
typedef linknode *linklist;

typedef struct
{
    linklist front;
    linklist rear;
} queue; // 队列

queue *init()
{
    // 函数功能：初始化队列，返回队列指针
    queue *p = (queue *)malloc(sizeof(queue));
    p->front = p->rear = NULL;
    return p;
}

// 函数名：timecompare()          参数:tt 当前时间, p 进程到达时间
bool timeCompare(time tt, time p)
{
    // tt < p(时间没到) false    tt >= p true
    // 函数功能：比较进程到达时间和当前时间，若小于则返回false，否则返回true
    return tt.hour * 60 + tt.min >= p.hour * 60 + p.min;
}

// 函数名：Levelcompare()          参数:p,q 进程
bool levelCompare(linklist p, linklist q)
{
    // 函数功能：比较p,q的优先级，p的优先级高则返回true，低则返回false，否则比较到达时间，p先或同时到达则返回true，反之则false
    return p->good > q->good;
}

// 函数名：sortCompare()          参数:p,q 进程
bool sortCompare(linklist p, linklist q)
{
    // if same priority, return true if p arrives earlier
    if (p->good == q->good)
        return timeCompare(q->arrive, p->arrive);
    return levelCompare(p, q); // return true if p has greater priority
}

// 交换两个节点的数据
void swap(linklist a, linklist b)
{
    // 交换节点数据
    int temp_id = a->id;
    char temp_name[20];
    strcpy(temp_name, a->name);
    int temp_good = a->good;
    time temp_arrive = a->arrive;
    int temp_zx = a->zx;
    time temp_start = a->start;
    time temp_finish = a->finish;
    int temp_zz = a->zz;
    double temp_zzxs = a->zzxs;

    a->id = b->id;
    strcpy(a->name, b->name);
    a->good = b->good;
    a->arrive = b->arrive;
    a->zx = b->zx;
    a->start = b->start;
    a->finish = b->finish;
    a->zz = b->zz;
    a->zzxs = b->zzxs;

    b->id = temp_id;
    strcpy(b->name, temp_name);
    b->good = temp_good;
    b->arrive = temp_arrive;
    b->zx = temp_zx;
    b->start = temp_start;
    b->finish = temp_finish;
    b->zz = temp_zz;
    b->zzxs = temp_zzxs;
}

// 函数名：LevelSorted()          参数:que 进程队列指针
void levelSorted(queue *que)
{
    // 函数功能：对进程队列按优先级排序
    // Bubble Sort
    if (que->front == NULL || (que->front == que->rear)) // empty or only one node
        return;
    bool flag = true;
    linklist ptr = que->front, last_ptr = NULL;
    do
    {
        flag = true;
        ptr = que->front;
        while (ptr->next != last_ptr)
        {
            if (!sortCompare(ptr, ptr->next))
            {
                swap(ptr, ptr->next);
                flag = false;
            }
            ptr = ptr->next;
        }
        last_ptr = ptr;
    } while (!flag);
}

// 函数名：insert  参数：Queue *res, node *x
void insert(queue *que, linklist s)
{
    // 函数功能:尾插入队
    if (que->front == NULL || que->rear == NULL) // 空队列
    {
        que->front = que->rear = s;
        return;
    }
    que->rear->next = s;
    que->rear = s;
}

// 函数名：ScanIn()    参数:wait进程队列指针, n 进程数
time ScanIn(queue *que, int n)
{
    // 函数功能：输入进程信息，返回最早的进程到达时间
    time earlist; // The process arrived earlist, initialized with the largest value.
    earlist.hour = 24;
    earlist.min = 24;
    printf("id 名字 优先级 到达时间 执行时间\n");
    while (n--)
    {
        linklist tmp = (linklist)malloc(sizeof(linknode));
        // id
        scanf("%d", &tmp->id);
        // 名称
        scanf("%s", tmp->name);
        // 优先级
        scanf("%d", &tmp->good);
        // 到达时间
        scanf("%d:%d", &tmp->arrive.hour, &tmp->arrive.min);
        // 执行时间（分钟）
        scanf("%d", &tmp->zx);

        tmp->next = NULL;
        insert(que, tmp);
        if (!timeCompare(tmp->arrive, earlist)) // return true tmp->arrive is earlier than "earlist" strictly
            earlist = tmp->arrive;
    }
    return earlist; // the first earlist process (with greatest priority)
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

// 函数名：computeTime()    参数:tt 当前时间的指针,q 当前进程的指针
time computeTime(time tt, linklist p)
{
    // 函数功能：更新当前时间和进程的各项时间
    if (timeCompare(tt, p->arrive))
        p->start = tt;
    else
        p->start = p->arrive;
    p->finish = cal(p->start, p->zx);
    p->zz = (p->finish.hour * 60 + p->finish.min) - (p->arrive.hour * 60 + p->arrive.min);
    p->zzxs = 1.0 * p->zz / p->zx;

    return p->finish;
}

// 函数名：priority()    参数:que进程队列指针,tt当前时间 n 进程数
queue *priority(queue *wait, time tt, int n)
{
    // 函数功能：进行优先级进程调度，并同时更新当前时间。
    if (wait->front == NULL)
    {
        printf("队列为空!\n");
        return NULL;
    }
    queue *res = init();
    while (wait->front != NULL)
    {
        linklist p = wait->front, left_p = NULL, choice = NULL, left_choice = NULL, earlist = wait->front, left_earlist = NULL;
        while (p != NULL)
        {
            if (!timeCompare(p->arrive, earlist->arrive)) // p arrived earlier strictly
            {
                earlist = p;
                left_earlist = left_p;
            }
            if (timeCompare(tt, p->arrive)) // if p in waiting queue
            {
                if (choice == NULL) // if there is no choosen process yet
                {
                    choice = p;
                    left_choice = left_p;
                }
                else if (levelCompare(p, choice)) // choose the first process with greatest priority
                {
                    choice = p;
                    left_choice = left_p;
                }
            }
            left_p = p;
            p = p->next;
        }
        // there is a choice in waiting queue
        if (choice != NULL)
        {
            // remove choosen process from waiting queue
            if (left_choice == NULL)
                wait->front = choice->next;
            else
                left_choice->next = choice->next;
            choice->next = NULL;
            tt = computeTime(tt, choice);
            insert(res, choice);
        }
        else // there is no choice. Execute the earlist one
        {
            // remove the earlist process from waiting queue
            if (left_earlist == NULL)
                wait->front = earlist->next;
            else
                left_earlist->next = earlist->next;
            earlist->next = NULL;
            tt = computeTime(tt, earlist);
            insert(res, earlist);
        }
    }
    wait->rear = NULL;
    return res;
}

// 函数名：Print()    参数:que进程队列指针, n 进程数
void print(queue *res, int n)
{
    // 函数功能：打印输出进程优先进程调度结果
    if (res == NULL)
        return;
    linklist p = res->front;
    int zz_sum = 0;
    float zzxs_sum = 0;
    int cnt = 0;
    printf("id\t名字\t到达时间 执行时间（分钟）开始时间 完成时间 周转时间（分钟）带权周转系数\n");
    while (p != NULL)
    {
        cnt++;
        printf("%d\t%s\t%2d:%02d\t %d（分钟）\t %2d:%02d\t  %2d:%02d\t   %d（分钟）\t   %.2f\t\n",
               p->id, p->name, p->arrive.hour, p->arrive.min, p->zx,
               p->start.hour, p->start.min, p->finish.hour, p->finish.min, p->zz, p->zzxs);
        zz_sum += p->zz;
        zzxs_sum += p->zzxs;
        p = p->next;
    }
    printf("系统平均周转时间为：%14.2f\n", zz_sum / cnt);
    printf("系统带权平均周转系数为：%10.2f\n", zzxs_sum / cnt);
}

// 函数名：destory()    参数:que进程队列指针
void destory(queue *que)
{
    if (que->front == NULL)
    {
        free(que);
        return;
    }
    linklist p = que->front;
    while (p != NULL)
    {
        que->front = p->next;
        free(p);
        p = que->front;
    }
    free(que);
}

int main()
{
    queue *wait, *res;
    int flag, n;
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
            wait = init();
            printf("请输入进程数量:");
            scanf("%d", &n);
            earlytime = ScanIn(wait, n);

            levelSorted(wait);
            res = priority(wait, earlytime, n);
            print(res, n);
            destory(wait);
            destory(res);
        }
    }

    return 0;
}
