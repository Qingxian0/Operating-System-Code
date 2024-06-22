#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX 10

int type_num;             // 资源种类
int N, M;                 // N是资源数,M是进程数
int Available[MAX];       // avaiable[i]表示系统中第i类资源的剩余量
int Max[MAX][MAX];        // Max[i][j]表示第i个进程对第j类资源的最大需求
int Need[MAX][MAX];       // need[i][j]表示第i个进程剩余所需第j类资源的数量
int Allocation[MAX][MAX]; // Allocation[i][j]表示第i个进程已占有第j类资源的数量
int Finish[MAX];          // finish[i]表示系统是否有能力使第i个进程执行完成
int Request[MAX][MAX];    // Request[i][j]表示第i个进程某次申请第j类资源的数量
int State[MAX];           // State[i]表示第i个进程所处状态（完成，进行中）
int Work[MAX];            // Work[i]表示第i个类资源数量
char Name[MAX][15];       // 记录进程的名字
int safe_arr[MAX];
int n; // 进程数

// 函数功能：根据进程名找到进程对应序号
int findIndex(char *name)
{
    for (int i = 1; i <= n; i++)
    {
        if (strcmp(name, Name[i]) == 0)
            return i;
    }
    return -1; // 没找到对应进程
}

// 函数功能：输入进程名并根据进程名找到进程对应序号
int inputName()
{
    char name[15];
    scanf("%s", name);
    int index = findIndex(name);

    while (index == -1) // 检查进程名是否存在
    {
        printf("名字不存在! 请重新输入!\n");
        printf("请输入进程名: ");
        scanf("%s", name);
        index = findIndex(name);
    }

    return index;
}

bool checkRequestNeed(int index)
{
    for (int i = 1; i <= type_num; i++)
    {
        if (Request[index][i] > Need[index][i])
            return false;
    }
    return true;
}

bool checkRequestAvailable(int index)
{
    for (int i = 1; i <= type_num; i++)
    {
        if (Request[index][i] > Available[i])
            return false;
    }
    return true;
}

bool checkNeedWork(int index)
{
    for (int i = 1; i <= type_num; i++)
    {
        if (Need[index][i] > Work[i])
            return false;
    }
    return true;
}

bool checkFinish()
{
    for (int i = 1; i <= n; i++)
    {
        if (Finish[i] == false)
            return false;
    }
    return true;
}

void printSafetySequence(int *safety_sequnece, int cnt)
{
    printf("申请成功！安全序列为: ");
    int i = 1;
    for (i = 1; i < cnt; i++)
        printf("%s->", Name[safety_sequnece[i]]);
    printf("%s\n", Name[safety_sequnece[i]]);
}

void printAvailable()
{
    for (int i = 1; i <= type_num; i++)
        printf("%d ", Available[i]);
    printf("\n");
}

void printNeed(int index)
{
    for (int i = 1; i <= type_num; i++)
        printf("%d ", Need[index][i]);
    printf("\n");
}

void updateState(int index)
{
    for (int i = 1; i <= type_num; i++)
    {
        if (Need[index][i] != 0)
        {
            State[index] = 0;
            return;
        }
    }
    // 归还资源
    for (int i = 1; i <= type_num; i++)
        Allocation[index][i] = 0;
    for (int i = 1; i <= type_num; i++)
        Available[i] += Max[index][i];
    State[index] = 1;
}

void printCurrentState()
{
    printf("进程名  最大需求量  尚需求量  已分配量  执行状态\n");
    for (int i = 1; i <= n; i++)
    {
        printf("%s\t", Name[i]);
        for (int j = 1; j <= type_num; j++)
            printf("%d ", Max[i][j]);
        printf("\t    ");
        for (int j = 1; j <= type_num; j++)
            printf("%d ", Need[i][j]);
        printf("    ");
        for (int j = 1; j <= type_num; j++)
            printf("%d ", Allocation[i][j]);
        printf("\t");
        if (!State[i])
            printf("Working\n");
        else
            printf("Finished\n");
    }
}

// Safety Algorithm
bool isSafe()
{
    // Work = Available
    for (int i = 1; i <= type_num; i++)
    {
        Work[i] = Available[i];
    }
    // Finish[i] = false if "State = 0" for i = 1, 2, ..., n
    for (int i = 1; i <= n; i++)
    {
        if (State[i] == 1)
            Finish[i] = true;
        else
            Finish[i] = false;
    }

    int cnt = 1;
    // find a T_i satisfy Need_i <= Work[i]
    while (!checkFinish() && cnt <= n)
    {
        int i = 1; // T_i
        while ((Finish[i] || !checkNeedWork(i)) && i <= n)
            i++;
        if (i == n + 1)
        {
            printf("无安全序列，请重新输入！\n");
            return false;
        }
        else
        {
            // Work = Work + Allocation_i
            for (int j = 1; j <= type_num; j++)
                Work[j] += Allocation[i][j];
            Finish[i] = true;
            safe_arr[cnt++] = i;
        }
    }
    printSafetySequence(safe_arr, cnt - 1);
    return true;
}

// Resource-Request Algorithm for Process P_i
int resoueceRequest(int index)
{
    printf("请输入该进程当前申请的各个资源数量: ");
    for (int i = 1; i <= type_num; i++)
        scanf("%d", &Request[index][i]);
    // 1. If Request_i <= Need_i || 2. If Request_i <= Available
    if (!checkRequestNeed(index) || !checkRequestAvailable(index))
    {
        printf("申请异常！\n");
        printf("系统可用资源为：");
        printAvailable();
        printf("当前进程剩余所需资源为：");
        printNeed(index);
        printf("请重新输入！");
        return 0;
    }
    // 3. Pretend to allocate requested resources to T_i by modifying the state
    for (int i = 1; i <= type_num; i++)
    {
        Available[i] = Available[i] - Request[index][i];
    }
    for (int i = 1; i <= type_num; i++)
    {
        Allocation[index][i] = Allocation[index][i] + Request[index][i];
    }
    for (int i = 1; i <= type_num; i++)
    {
        Need[index][i] = Need[index][i] - Request[index][i];
    }
    // If safe => the resources are allocated to T_i
    // If unsafe => input the request again
    if (!isSafe())
    {
        // recovery
        for (int i = 1; i <= type_num; i++)
        {
            Available[i] = Available[i] + Request[index][i];
        }
        for (int i = 1; i <= type_num; i++)
        {
            Allocation[index][i] = Allocation[index][i] - Request[index][i];
        }
        for (int i = 1; i <= type_num; i++)
        {
            Need[index][i] = Need[index][i] + Request[index][i];
        }
        return 1;
    }
    // 更新当前进程的State
    updateState(index);
    printCurrentState();
    printf("系统剩余资源：");
    printAvailable();
    return 2;
}

int main()
{
    /*--------------------------------初始化进程和系统资源--------------------------------*/
    printf("请输入进程数: ");
    scanf("%d", &n);

    printf("请输入资源种类数量: ");
    scanf("%d", &type_num);

    printf("请输入%d类资源初始化的资源数: ", type_num);
    for (int i = 1; i <= type_num; i++)
    {
        scanf("%d", &Available[i]);
    }

    printf("请输入进程的相关信息：\n");
    printf("进程名  最大需求量\n");
    for (int i = 1; i <= n; i++)
    {
        scanf("%s", Name[i]);
        for (int j = 1; j <= type_num; j++)
        {
            scanf("%d", &Max[i][j]);
            Need[i][j] = Max[i][j];
        }
    }
    /*--------------------------------进程开始执行--------------------------------*/
    printf("进程开始执行!\n");
    printf("请为进程分配相关资源!\n");
    char ifRequest = 'Y';
    do
    {
        printf("请输入进程名: ");

        int index = inputName();

        int res = resoueceRequest(index);

        // 申请异常或者没找到安全序列
        if (res == 0)
            continue;

        printf("询问是否需要申请资源? (Y/N)");
        getchar();
        scanf("%c", &ifRequest);
    } while (ifRequest == 'Y');

    printCurrentState();
    printf("系统剩余资源：");
    printAvailable();
    return 0;
}