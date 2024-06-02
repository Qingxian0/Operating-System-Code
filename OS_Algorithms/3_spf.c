#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct data
{
    int hour;
    int min;
} time;

typedef struct node
{
    int id;        // 编号
    char name[20]; // 名字
    time arrive;   // 到达时间
    int zx;        // 执行时间；
    time start;    // 开始时间
    time finish;   // 完成时间
    int zz;        // 周转时间=执行完成时间-到达就绪队列时间
    double zzxs;   // 带权周转时间=周转时间/执行时间
    struct node *next;
} linknode;

typedef linknode *linklist;

typedef struct
{
    linklist front;
    linklist rear;
} queue; // 队列

// 函数名：init   参数：无
queue *init()
{
    // 函数功能：初始化队列，返回队列指针
    queue *p = (queue *)malloc(sizeof(queue));
    p->front = p->rear = NULL;
    return p;
}

// 函数名：insert  参数：Queue *cc, node *x
void insert(queue *cc, linklist s)
{
    // 函数功能:尾插入队
    if (cc->front == NULL || cc->rear == NULL) // 空队列
    {
        cc->front = cc->rear = s;
        return;
    }
    cc->rear->next = s;
    cc->rear = s;
}

// 函数名：dele  参数：Queue *cc
void dele(queue *cc)
{
    if (cc->front == NULL)
        return;
    // 功能：队首结点出队
    linklist p = cc->front;
    cc->front = p->next;
    free(p);
}

// 函数名：input   参数：Queue *cc
void input(queue *cc)
{
    // 功能：实现作业队列的输入
    int n; // 进程数量
    printf("请输入进程数量: ");
    scanf("%d", &n);
    printf("id 姓名 到达时间 执行时间\n");
    while (n--)
    {
        linklist tmp = (linklist)malloc(sizeof(linknode));
        // id
        scanf("%d", &tmp->id);
        // 名称
        scanf("%s", tmp->name);
        // 到达时间
        scanf("%d:%d", &tmp->arrive.hour, &tmp->arrive.min);
        // 执行时间（分钟）
        scanf("%d", &tmp->zx);

        tmp->next = NULL;
        insert(cc, tmp);
    }
}

// 比较函数: based on arrival time and execution time
bool compare(linklist a, linklist b)
{
    if (a->arrive.hour == b->arrive.hour && a->arrive.min == b->arrive.min)
    {
        // 到达时间相同，按执行时间排序
        return a->zx < b->zx;
    }
    else
    {
        // 到达时间不同，按到达时间排序
        return (a->arrive.hour * 60 + a->arrive.min) < (b->arrive.hour * 60 + b->arrive.min);
    }
}
// 检查到达时间早晚
bool check(time a, time b)
{
    return a.hour * 60 + a.min <= b.hour * 60 + b.min;
}
// 交换两个节点的数据
void swap(linklist a, linklist b)
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

    a->id = b->id;
    strcpy(a->name, b->name);
    a->arrive = b->arrive;
    a->zx = b->zx;
    a->start = b->start;
    a->finish = b->finish;
    a->zz = b->zz;
    a->zzxs = b->zzxs;

    b->id = temp_id;
    strcpy(b->name, temp_name);
    b->arrive = temp_arrive;
    b->zx = temp_zx;
    b->start = temp_start;
    b->finish = temp_finish;
    b->zz = temp_zz;
    b->zzxs = temp_zzxs;
}
// 函数名：sort参数：Queue *cc
void sort(queue *cc)
{
    // 函数功能：对到达时间进行排序
    // Bubble Sort
    if (cc->front == NULL || (cc->front == cc->rear)) // empty or only one node
        return;
    bool flag = true;
    linklist ptr;
    linklist lptr = NULL;
    do
    {

        flag = true;
        ptr = cc->front;
        while (ptr->next != lptr)
        {
            if (!compare(ptr, ptr->next)) // 使用比较函数
            {
                swap(ptr, ptr->next);
                flag = false;
            }
            ptr = ptr->next;
        }
        lptr = ptr;
    } while (!flag);
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

int decline(time x, time y)
{
    // 函数功能：计算进程的等待时间（即上一进程的结束时间与当前进程的到达时间的时间差）
    return (y.hour * 60 + y.min) - (x.hour * 60 + x.min);
}

// 函数名：solve_SPF参数：Queue *cc
void solve_SPF(queue *cc)
{
    // 函数功能：解决短进程优先调度算法
    // SPF: 达到越早、服务时间越短越先调入
    if (cc->front == NULL)
    {
        printf("队列为空!\n");
        return;
    }

    // 如果有比last先到达的进程，找到他们中执行时间最短的一个执行；否则执行第一个比last晚到达的进程
    printf("id\t名字\t到达时间 执行时间（分钟）开始时间 完成时间 周转时间（分钟）带权周转系数\n");
    time last = cc->front->arrive;
    float zz_sum = 0, zzxs_sum = 0;
    int cnt = 0;
    while (cc->front != NULL)
    {
        cnt++;
        if (!check(cc->front->arrive, last)) // 第一个进程的到达时间就比last晚
        {
            cc->front->start = cc->front->arrive;
            cc->front->finish = cal(cc->front->start, cc->front->zx);
            cc->front->zz = decline(cc->front->arrive, cc->front->finish);
            cc->front->zzxs = 1.0 * cc->front->zz / cc->front->zx;
            last = cc->front->finish;
            zz_sum += cc->front->zz;
            zzxs_sum += cc->front->zzxs;
            printf("%d\t%s\t%2d:%02d\t %d（分钟）\t %2d:%02d\t  %2d:%02d\t   %d（分钟）\t   %.2f\t\n",
                   cc->front->id, cc->front->name, cc->front->arrive.hour, cc->front->arrive.min, cc->front->zx,
                   cc->front->start.hour, cc->front->start.min, cc->front->finish.hour, cc->front->finish.min,
                   cc->front->zz, cc->front->zzxs);
            dele(cc);
        }
        else // 第一个进程的到达时间比last早或者等于last
        {
            linklist first_later = cc->front, left_sp = NULL, sp = cc->front, left_p = NULL, p = cc->front;
            while (first_later != NULL && check(first_later->arrive, last))
                first_later = first_later->next;
            while (p != first_later) // 找到执行时间最短的进程
            {
                if (p->zx < sp->zx)
                {
                    sp = p;
                    left_sp = left_p;
                }
                left_p = p;
                p = p->next;
            }
            sp->start = last;
            sp->finish = cal(sp->start, sp->zx);
            sp->zz = decline(sp->arrive, sp->finish);
            sp->zzxs = 1.0 * sp->zz / sp->zx;
            last = sp->finish;
            zz_sum += sp->zz;
            zzxs_sum += sp->zzxs;
            printf("%d\t%s\t%2d:%02d\t %d（分钟）\t %2d:%02d\t  %2d:%02d\t   %d（分钟）\t   %.2f\t\n",
                   sp->id, sp->name, sp->arrive.hour, sp->arrive.min, sp->zx,
                   sp->start.hour, sp->start.min, sp->finish.hour, sp->finish.min,
                   sp->zz, sp->zzxs);
            if (sp == cc->front) // 最短执行时间的进程是front
                dele(cc);
            else // 最短执行时间的进程不是front
            {
                left_sp->next = sp->next;
                free(sp);
            }
        }
    }
    printf("系统平均周转时间为：%14.2f\n", zz_sum / cnt);
    printf("系统带权平均周转系数为：%10.2f\n", zzxs_sum / cnt);
}

int main()
{
    while (true)
    {
        int op;
        printf("请输入操作(1：开始进程调度；0：结束进程）");
        scanf("%d", &op);
        if (op == 0)
            break;
        queue *cc;
        cc = init();
        input(cc);
        sort(cc);
        // show(cc);
        solve_SPF(cc);
    }

    return 0;
}
