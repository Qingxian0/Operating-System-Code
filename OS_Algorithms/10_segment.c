#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 分区结构体
typedef struct Partition
{
    int size;      // 分区长度
    int address;   // 分区始址
    char flag[20]; // 分区状态，空闲或者占用作业名
    struct Partition *next;
} Partition;

Partition *allocatedPartitions = NULL;
Partition *freePartitions = NULL;
int remainedSize;

typedef struct Job
{
    char name[20];
    int size;
    int segmentCount;
    int segmentTable[10][2];
    Job *next;
} Job;

Job *jobList = NULL;

// 函数名 :initMemory  参数:无
void initMemory()
{
    int startAddr;
    // 函数功能：初始化内存空间
    printf("请输入内存大小：");
    scanf("%d", &remainedSize);
    printf("请输入起始地址：");
    scanf("%d", &startAddr);

    freePartitions = (Partition *)malloc(sizeof(Partition));
    freePartitions->size = remainedSize;
    freePartitions->address = startAddr;
    strcpy(freePartitions->flag, "空闲");
    freePartitions->next = NULL;
    allocatedPartitions = NULL;
}

// 打印主存分配情况
void printAllocations()
{
    printf("***************主存分配情况***************\n");
    Partition *p = NULL;
    int i = 1;

    if (allocatedPartitions)
    {
        p = allocatedPartitions;
        printf("已分配：\n");
        printf("分区号  大小（KB）        起始          状态\n");
        while (p)
        {
            printf("%d\t%d\t\t  %d\t\t%s\n", i, p->size, p->address, p->flag);
            i++;
            p = p->next;
        }
    }
    if (freePartitions)
    {
        p = freePartitions;
        printf("未分配：\n");
        printf("分区号  大小（KB）        起始          状态\n");

        while (p)
        {
            printf("%d\t%d\t\t  %d\t\t%s\n", i, p->size, p->address, p->flag);
            i++;
            p = p->next;
        }
    }
}

void printSegmentTable(Job *job)
{
    printf("**********打印%s段表**********\n", job->name);
    printf("段号\t段长\t基址\n");

    for (int i = 0; i < job->segmentCount; i++)
    {
        printf("%d\t%d\t%d\n", i, job->segmentTable[i][0], job->segmentTable[i][1]);
    }
}

void insertToAllocatedPartitions(Partition *partitionToInsert)
{
    Partition *current = allocatedPartitions;
    Partition *previous = NULL;
    while (current != NULL && current->address < partitionToInsert->address)
    {
        previous = current;
        current = current->next;
    }
    if (previous == NULL)
    {
        partitionToInsert->next = allocatedPartitions;
        allocatedPartitions = partitionToInsert;
    }
    else
    {
        partitionToInsert->next = current;
        previous->next = partitionToInsert;
    }
}

void insertToFreePartitions(Partition *partitionToInsert)
{
    Partition *current = freePartitions;
    Partition *previous = NULL;
    while (current != NULL && current->address < partitionToInsert->address)
    {
        previous = current;
        current = current->next;
    }
    if (previous == NULL)
    {
        partitionToInsert->next = freePartitions;
        freePartitions = partitionToInsert;
    }
    else
    {
        partitionToInsert->next = current;
        previous->next = partitionToInsert;
    }
}

bool firstFit(int *segment, char *name)
{
    Partition *current = freePartitions;
    Partition *previous = NULL;

    // 遍历未分配分区表，找到第一个足够大的分区
    while (current != NULL && current->size < segment[0])
    {
        previous = current;
        current = current->next;
    }

    // 如果找到了足够大的分区
    if (current != NULL)
    {
        // 如果分区大小等于分段大小，直接分配
        if (current->size == segment[0])
        {
            strcpy(current->flag, name);

            // 将当前分区从未分配表移到已分配表 (头插法)
            if (previous = NULL)
                freePartitions = freePartitions->next;
            else
                previous->next = current->next;
            insertToAllocatedPartitions(current);
            segment[1] = current->address;
        }
        else
        {
            // 如果分区大小大于作业大小，分裂分区
            Partition *newAllocatedPartition = (Partition *)malloc(sizeof(Partition));
            newAllocatedPartition->address = current->address;
            newAllocatedPartition->size = segment[0];
            strcpy(newAllocatedPartition->flag, name);

            insertToAllocatedPartitions(newAllocatedPartition);
            segment[1] = newAllocatedPartition->address;

            // 更新原分区的信息
            current->address += segment[0];
            current->size -= segment[0];
        }
        remainedSize -= segment[0];
        return true;
    }
    else
    {
        // 如果没有找到足够大的分区
        return false;
    }
}

void allocate()
{
    printf("请输入作业名：");
    Job *newJob = (Job *)(malloc(sizeof(Job)));
    scanf("%s", newJob->name);
    printf("请输入%s需要分配的主存大小：", newJob->name);
    scanf("%d", &newJob->size);
    printf("请输入%s需要分成几段：", newJob->name);
    scanf("%d", &newJob->segmentCount);
    for (int i = 0; i < newJob->segmentCount; i++)
    {
        newJob->segmentTable[i][0] = -1;
        newJob->segmentTable[i][1] = -1;
    }

    newJob->next = jobList;
    jobList = newJob;

    int remained = newJob->size;
    for (int i = 0; i < newJob->segmentCount; i++)
    {
        printf("剩余%dKB内存，请输入第%d段的大小：", remained, i + 1);
        scanf("%d", &newJob->segmentTable[i][0]);
        if (firstFit(newJob->segmentTable[i], newJob->name))
        {
            printAllocations();
            remained -= newJob->segmentTable[i][0];
        }
        else
        {
            printf("没有找到足够大的分区来满足第%d段的需求\n", i + 1);
            return;
        }
    }
    printf("分配成功！");
    printSegmentTable(newJob);
}

// 段去配
void freeSegment(char *segmentToFree)
{
    // 找到对应要回收的分区
    Partition *current = allocatedPartitions;
    Partition *previous = NULL;
    while (current != NULL && strcmp(segmentToFree, current->flag) != 0)
    {
        previous = current;
        current = current->next;
    }

    // 找到了要回收的分区
    if (current != NULL)
    {
        // 回收对应分区
        Partition *partitionToFree = current;
        strcpy(partitionToFree->flag, "空闲");
        remainedSize += current->size;

        // 将partitionToFree移出allocated
        if (previous == NULL)
            allocatedPartitions = allocatedPartitions->next;
        else
            previous->next = partitionToFree->next;

        // 看看有没有上邻空闲区、下邻空闲区
        current = freePartitions;
        previous = NULL;
        Partition *aboveNotAllocated = NULL;
        Partition *belowNotAllocated = NULL;

        while (current != NULL && (aboveNotAllocated == NULL || belowNotAllocated == NULL))
        {
            // 上邻空闲区
            if (current->address + current->size == partitionToFree->address)
            {
                aboveNotAllocated = current;

                // 移除上邻空闲区
                if (previous == NULL)
                    freePartitions = freePartitions->next;
                else
                    previous->next = aboveNotAllocated->next;

                current = current->next;
            }
            else if (partitionToFree->address + partitionToFree->size == current->address)
            {
                // 下邻空闲区
                belowNotAllocated = current;
                // 移除下邻空闲区
                if (previous == NULL)
                    freePartitions = freePartitions->next;
                else
                    previous->next = belowNotAllocated->next;

                current = current->next;
            }
            else
            {
                // 更新previous和current
                previous = current;
                current = current->next;
            }
        }

        // 合并上邻空闲区、下邻空闲区
        if (aboveNotAllocated)
        {
            partitionToFree->address = aboveNotAllocated->address;
            partitionToFree->size += aboveNotAllocated->size;
            free(aboveNotAllocated);
        }
        if (belowNotAllocated)
        {
            partitionToFree->size += belowNotAllocated->size;
            free(belowNotAllocated);
        }

        // 将partitionToFree插入到未分配分区表
        insertToFreePartitions(partitionToFree);
    }
}

// 内存去配
void freePartition()
{
    char jobName[20];
    printf("请输入要回收的作业名：");
    scanf("%s", jobName);
    Job *current = jobList;
    Job *previous = NULL;
    while (current && strcmp(current->name, jobName) != 0)
    {
        previous = current;
        current = current->next;
    }
    if (current)
    {
        if (previous == NULL)
        {
            jobList = jobList->next;
        }
        else
        {
            previous->next = current->next;
        }
        for (int i = 0; i < current->segmentCount; i++)
        {
            if (current->segmentTable[i][1] != -1)
            {
                freeSegment(current->name);
                printf("回收%s的段%d成功！\n", current->name, i);
            }
        }
        printAllocations();
    }
    else
    {
        printf("要回收的作业不存在！\n");
    }
}

int operationChoice()
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

int main()
{
    // system("chcp 65001");
    initMemory();
    while (true)
    {
        int c = operationChoice();
        switch (c)
        {
        case 1: {
            printAllocations();
            allocate();
            break;
        }
        case 2: {
            freePartition();
            break;
        }
        case 0:
            return 0;
        }
    }
    return 0;
}
