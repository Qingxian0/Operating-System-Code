#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int MAX = 2000;

int memorySize; // 系统内存大小（以基本字长为单位）
int wordLength; // 字长
int blockSize;  // 物理块大小（以基本字长为单位）
int blockNum;   // 物理块个数

// 作业节点
typedef struct LinkNode
{
    char name[10];         // 作业名
    int size;              // 大小
    int pageTable[MAX][2]; // 作业页表
    struct LinkNode *next;
} Job;

// 主存分配表
struct AllocationGragh
{
    bool graph[MAX]; // 位示图
    int remainedNum; // 剩余空闲块数
} allocationGragh;

Job *head;

// 函数名：OperationChoice       函数参数：无
int OperationChoice()
{
    // 函数功能：打印操作菜单，选择需要进行的操作
    printf("***************分页式管理***************\n");
    printf("         *    1. 内存分配    *\n");
    printf("         *    2. 内存去配    *\n");
    printf("         *    0. 退出        *\n");
    printf("请输入选项：");
    int choice;
    do
    {
        scanf("%d", &choice);
    } while (choice != 1 && choice != 2 && choice != 0);

    return choice;
}

// 函数名：printPageTable       函数参数：print Page Table
void printPageTable(Job *job)
{
    // 函数功能：打印作业页表信息
    printf("********打印作业%s的页表********\n", job->name);
    for (int i = 0; i < job->size; i++)
        printf("%8d%8d\n", job->pageTable[i][0], job->pageTable[i][1]);
}

// 函数名：printAllocationGraph       函数参数：print Allocation Graph
void printAllocationGraph()
{
    // 函数功能：打印系统内存信息
    printf("主存位视图如下所示：\n");
    printf("     ");
    for (int i = 0; i < wordLength - 1; i++)
        printf("%2d ", i);
    printf("%2d\n", wordLength - 1);

    // 单独处理第一个block
    printf(" 0 | ");
    printf("%2d ", allocationGragh.graph[0]);

    for (int i = 1; i < blockNum - 1; i++)
    {
        if ((i + 1) % wordLength == 0)
        {
            printf("%2d\n", allocationGragh.graph[i]);
            printf("%2d | ", (i + 1) / wordLength);
        }
        else
        {
            printf("%2d ", allocationGragh.graph[i]);
        }
    }

    // 单独处理最后一个block
    printf("%2d\n", allocationGragh.graph[blockNum - 1]);

    printf("剩余物理块数：%d\n", allocationGragh.remainedNum);
}

// 函数名：allocate    函数参数：
void allocate()
{
    /* 函数功能：建立作业节点，判断是否可以分配，可以进行下一步，进行内存分配，
       插入作业链表，打印该作业的页表信息，打印系统内存信息。
    */
    Job *newJob = (Job *)malloc(sizeof(Job));
    printf("请输入作业名字：");
    scanf("%s", newJob->name);

    do
    {
        printf("请输入作业需要分配的大小：");
        scanf("%d", &newJob->size);
        if (newJob->size > MAX)
        {
            printf("要分配的空间大小超过MAX！请重新输入！\n");
        }
    } while (newJob->size > MAX);

    for (int i = 0; i < newJob->size; i++)
        newJob->pageTable[i][0] = i + 1;

    if (allocationGragh.remainedNum >= newJob->size)
    {
        int pageNum = 0;
        for (int i = 0; i < blockNum && pageNum < newJob->size; i++)
        {
            if (allocationGragh.graph[i] == 0)
            {
                newJob->pageTable[pageNum++][1] = i;
                allocationGragh.graph[i] = 1;
                allocationGragh.remainedNum -= 1;
            }
        }

        // 头插法加入newJob
        newJob->next = head;
        head = newJob;

        printf("分配成功！\n");
        printAllocationGraph();
        printPageTable(newJob);
    }
    else
    {
        printf("没有足够的空间，给该作业的空间分配失败！\n");
        free(newJob);
    }
}

// 函数名：recycle      函数参数：
void recycle()
{
    // 函数功能：找到待回收的作业节点，释放内存，从作业链表中删除，打印系统内存信息
    char jobToRecycleName[10];
    printf("请输入你要回收的作业名：");
    scanf("%s", jobToRecycleName);

    // 查找该作业
    Job *current = head;
    Job *previous = NULL;

    while (current && strcmp(current->name, jobToRecycleName))
    {
        previous = current;
        current = current->next;
    }

    if (current)
    {
        // 回收对应的block
        for (int i = 0; i < current->size; i++)
        {
            allocationGragh.graph[current->pageTable[i][1]] = 0;
            allocationGragh.remainedNum += 1;
        }

        // 将改作业移出作业队列并free
        if (previous == NULL)
            head = head->next;
        else
            previous->next = current->next;
        free(current);

        printf("该作业回收成功！\n");
        printAllocationGraph();
    }
    else
    {
        printf("回收失败，未找到该作业！\n");
    }
}

// 函数名：initMemory       函数参数：无
void init()
{
    // 函数功能：定义内存大小，字长与物理块大小，初始化系统内存和内存分配表，打印系统内存信息
    printf("请输入系统内存空间的大小：");
    scanf("%d", &memorySize);
    printf("请输入字长（16/32/64）：");
    scanf("%d", &wordLength);
    printf("请输入物理块大小：");
    scanf("%d", &blockSize);

    blockNum = memorySize / blockSize;
    // initialize the allocation graph
    allocationGragh.remainedNum = 0;

    for (int i = 0; i < blockNum; i++)
    {
        int x = rand() % 2;
        allocationGragh.graph[i] = x;
        if (x == 0)
            allocationGragh.remainedNum += 1;
    }

    printAllocationGraph();
}

// 函数名：menu          函数参数：无
void menu()
{
    init();
    while (1)
    {
        int c;
        c = OperationChoice();
        switch (c)
        {
        case 1:
            allocate();
            break;
        case 2:
            recycle();
            break;
        case 0:
            return;
        }
    }
}

int main()
{
    menu();
    return 0;
}
