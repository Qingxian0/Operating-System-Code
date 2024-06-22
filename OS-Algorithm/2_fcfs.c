#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct
{
    int hour, min;
} Time;

typedef struct node
{
    int id;        // 编号你
    char name[20]; // 进程名
    Time arrive;   // 到达就绪队列时间
    int zx;        // 执行时间
    Time start;    // 开始时间
    Time finish;   // 完成时间
    int zz;        // 周转时间=完成时间-到达时间
    float zzxs;    // 带权周转系数=周转间/执行时间,下同
    struct node *next;
} node;

typedef struct
{
    node *front, *end;
} Q;

// 函数名：check，函数参数：Time x（当前进程的到达时，Time y（待插入进程的到达时间）
bool check(Time x, Time y)
{
    // 函数功能：比较到达时间的早晚
    if (x.hour < y.hour)
        return true;
    else if (x.hour == y.hour)
        return x.min < y.min;
    return false;
}

// 函数名：init，函数参数：Q &x（含有就绪队列的队头和队尾节点地址）
void init(Q *x)
{
    // 函数功能：初始化，为之后操作做准备
    x->front = x->end = NULL;
}

// 函数名：insert，函数参数：Q *x（指向含有就绪队列的队头和队尾节点的指针）,node tmp（）
void insert(Q *x, node *tmp)
{
    // 函数功能：构建并按到达时间先后插入进程节点放入就绪队列中
    if (x->front == NULL || x->end == NULL) // 空队列
    {
        x->front = x->end = tmp;
        return;
    }
    node *p = x->front, *q = NULL;
    if (check(tmp->arrive, x->front->arrive)) // 最早到达的进程
    {
        tmp->next = x->front;
        x->front = tmp;
        return;
    }
    if (check(x->end->arrive, tmp->arrive)) // 最晚到达的进程
    {
        x->end->next = tmp;
        x->end = tmp;
        return;
    }
    while (check(p->arrive, tmp->arrive))
    {
        q = p;
        p = p->next;
    }
    tmp->next = p;
    q->next = tmp;
}

// 函数名：dele，函数参数：Q &x（含有就绪队列的队头和队尾节点地址）
void dele(Q *x)
{
    // 函数功能：初始化，为下一个进程序列做准备
    node *p = x->front;
    while (p != NULL)
    {
        x->front = x->front->next;
        free(p);
        p = x->front;
    }
    x->end = NULL;
}

// 函数名：input，函数参数：Q *x（含有就绪队列的队头和队尾节点地址）
void input(Q *x)
{
    // 函数功能：输入节点信息，构建并按到达时间先后依次插入进程节点放入就绪队列中
    int n; // 进程数量
    printf("请输入进程数量: ");
    scanf("%d", &n);
    printf("id 姓名 到达时间 执行时间\n");
    while (n--)
    {
        node *tmp = (node *)malloc(sizeof(node));
        // id
        scanf("%d", &tmp->id);
        // 名称
        scanf("%s", tmp->name);
        // 到达时间
        scanf("%d:%d", &tmp->arrive.hour, &tmp->arrive.min);
        // 执行时间（分钟）
        scanf("%d", &tmp->zx);

        tmp->next = NULL;
        insert(x, tmp);
    }
}

// 函数名：cal，函数参数：Time x（当前进程的开始时间）, int y（当前进程的执行时间）
Time cal(Time x, int y)
{
    // 函数功能：计算当前进程的结束时间（开始时间+执行时间）
    Time tmp;
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

// 函数名：deline，函数参数：Time x（上一进程的结束时间）,Time y（当前进程的到达时间）
int decline(Time x, Time y)
{
    // 函数功能：计算进程的等待时间（即上一进程的结束时间与当前进程的到达时间的时间差）
    return (y.hour * 60 + y.min) - (x.hour * 60 + x.min);
}

// 函数名：fcfs，函数参数：Q *x（指向含有就绪队列的队头和队尾节点的指针）
void fcfs(Q *x)
{
    // 函数功能：计算进程相关时间信息，分别确定开始时间，结束时间（开始时间+执行时间），周转时间（结束时间-到达时间），周转系数（周转时间/执行时间）
    if (x->front == NULL || x->end == NULL)
        return;
    // 对最早的进程处理
    x->front->start = x->front->arrive;
    x->front->finish = cal(x->front->start, x->front->zx);
    x->front->zz = decline(x->front->arrive, x->front->finish);
    x->front->zzxs = 1.0 * x->front->zz / x->front->zx;

    // 对后续进程处理
    node *last = x->front;
    node *p = last->next;
    while (p != NULL)
    {
        if (check(last->finish, p->arrive)) // 如果当前进程来的时间比上一个进程结束时间早
            p->start = p->arrive;
        else
            p->start = last->finish;
        p->finish = cal(p->start, p->zx);
        p->zz = decline(p->arrive, p->finish);
        p->zzxs = 1.0 * p->zz / p->zx;
        last = p;
        p = p->next;
    }
}

// 函数名：output，函数参数：Q &x（含有就绪队列的队头和队尾节点的地址）
void output(Q *x)
{
    // 函数功能：按进程执行顺序分别输出每个进程全部结果信息和系统平均时间
    if (x->front == NULL || x->end == NULL)
    {
        printf("队列为空!\n");
        return;
    }
    node *p = x->front;
    printf("id\t名字\t到达时间 执行时间（分钟）开始时间 完成时间 周转时间（分钟）带权周转系数\n");
    float zz_sum = 0, zzxs_sum = 0;
    int cnt = 0;
    while (p != NULL)
    {
        printf("%d\t%s\t%02d:%02d\t %d（分钟）\t %02d:%02d\t  %02d:%02d\t   %d（分钟）\t   %.2f\t\n",
               p->id, p->name, p->arrive.hour, p->arrive.min, p->zx,
               p->start.hour, p->start.min, p->finish.hour, p->finish.min, p->zz, p->zzxs);
        cnt++;
        zz_sum += p->zz;
        zzxs_sum += p->zzxs;
        p = p->next;
    }
    printf("系统平均周转时间为：%14.2f\n", zz_sum / cnt);
    printf("系统带权平均周转系数为：%10.2f\n", zzxs_sum / cnt);
}

int main()
{
    Q x;
    while (1)
    {
        init(&x);
        input(&x);
        fcfs(&x);
        output(&x);
        while (x.front != NULL)
            dele(&x);
    }
    return 0;
}
