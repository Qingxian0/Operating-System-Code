#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int n, m;       // 分区数量， 作业个数
int v[1000000]; // 存储作业大小

struct Y
{
    int id;       // 分区号
    int size;     // 分区大小
    int address;  // 分区始址
    char *status; // 状态
};

struct Y a[110];

// 函数名：input    函数参数：无
struct Y input()
{
    // 函数功能：输入一个分区信息包括分区号，分区大小和分区始址
    struct Y tmp;
    scanf("%d", &tmp.id);
    scanf("%d", &tmp.size);
    scanf("%d", &tmp.address);
    tmp.status = (char *)malloc(sizeof(5));
    if (tmp.status)
        strcpy(tmp.status, "0");
    return tmp;
}

// 函数名：print    函数参数：无
void print()
{
    // 函数功能：输出所有分区信息（包括此时的状态，未被占用输出0，被占	用输出占用该分区的作业名）
    printf("**********打印分区信息**********\n");
    printf("分区号  大小(KB)  起始(KB)  状态\n");
    for (int i = 1; i <= n; i++)
    {
        printf("%d\t%d\t  %d\t  %s\n", a[i].id, a[i].size, a[i].address, a[i].status);
    }
}

int main()
{
    system("chcp 65001");

    printf("请输入系统的分区块数：");
    scanf("%d", &n);
    printf("请依次输入：\n分区号 大小 起始\n");
    for (int i = 1; i <= n; i++)
        a[i] = input();
    print();
    printf("请输入作业的个数：");
    scanf("%d", &m);
    printf("请输入这%d个作业的信息：\n", m);
    for (int i = 1; i <= m; i++)
    {
        printf("请输入作业%d的大小：", i);
        scanf("%d", &v[i]);
    }
    printf("打印各作业的信息：\n");
    printf("作业名  作业大小\n");
    for (int i = 1; i <= m; i++)
    {
        printf("JOB%d\t%dKB\n", i, v[i]);
        for (int j = 1; j <= n; j++)
        {
            if (a[j].size >= v[i] && !strcmp(a[j].status, "0"))
            { // 找到可以容纳得下该作业且未被占用的分区
                sprintf(a[j].status, "JOB%d", i);
                break;
            }
        }
    }
    print();
    while (true)
    {
        char op[2];
        printf("是否需要回收(y/n)？");
        scanf("%s", op);
        if (op[0] == 'n')
            break;
        print();
        char str[5];
        printf("请输入回收的作业名：");
        scanf("%s", str);

        int pos = 0; // find the position of Job
        for (int i = 1; i <= n; i++)
            if (!strcmp(a[i].status, str))
                pos = i;

        if (pos == 0)
            printf("找不到该作业，回收失败\n");
        else
        {
            strcpy(a[pos].status, "0"); // 标记回未占用状态
            printf("回收成功\n");
        }
        print();
    }
    return 0;
}
