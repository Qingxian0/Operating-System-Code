#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// 内存结构体
typedef struct memory_node
{
    int size;    // 内存大小
    int address; // 内存始址
} memoryNode;

memoryNode memory;

// 分区表
typedef struct link_node
{
    int address;   // 分区始址
    int size;      // 分区大小
    char flag[20]; // 分区状态，空闲或者占用作业名
    struct link_node *next;
} node;

node *allocated;
node *notAllocated;

// 作业结构体
typedef struct job_node
{
    int id;        // 作业编号
    char name[20]; // 作业名
    int size;      // 作业大小
} job;

// initialize allocated and notAllocated
void initialize()
{
    allocated = NULL;
    notAllocated = (node *)malloc(sizeof(node));
    notAllocated->address = memory.address;
    notAllocated->size = memory.size;
    strcpy(notAllocated->flag, "空闲");
    notAllocated->next = NULL;
}

void showMenu1()
{
    printf("***************可变分区管理***************\n");
    printf("          *    1. 内存分配    *\n");
    printf("          *    2. 内存去配    *\n");
    printf("          *    0. 退出        *\n");
}

void showMenu2()
{
    printf("******************分配算法******************\n");
    printf("          *    1. 最先分配算法    *\n");
    printf("          *    2. 最优分配算法    *\n");
    printf("          *    3. 最坏分配算法    *\n");
}

// 打印主存分配情况
void showAllocationState()
{
    printf("***************主存分配情况***************\n");
    node *p;

    if (allocated)
        p = allocated;
    printf("已分配：\n");
    printf("分区号  大小（KB）        起始          状态\n");
    int i = 1;
    while (p)
    {
        printf("%d\t%d\t\t  %d\t\t%s\n", i, p->size, p->address, p->flag);
        i++;
        p = p->next;
    }

    if (notAllocated)
        p = notAllocated;
    printf("未分配：\n");
    printf("分区号  大小（KB）        起始          状态\n");

    while (p)
    {
        printf("%d\t%d\t\t  %d\t\t%s\n", i, p->size, p->address, p->flag);
        i++;
        p = p->next;
    }
}

void insertToAllocated(node *partitionToInsert)
{
    node *current = allocated;
    node *previous = NULL;
    while (current != NULL && current->address < partitionToInsert->address)
    {
        previous = current;
        current = current->next;
    }
    if (previous == NULL)
    {
        partitionToInsert->next = allocated;
        allocated = partitionToInsert;
    }
    else
    {
        partitionToInsert->next = current;
        previous->next = partitionToInsert;
    }
}

void insertToNotAllocated(node *partitionToInsert)
{
    node *current = notAllocated;
    node *previous = NULL;
    while (current != NULL && current->address < partitionToInsert->address)
    {
        previous = current;
        current = current->next;
    }
    if (previous == NULL)
    {
        partitionToInsert->next = notAllocated;
        notAllocated = partitionToInsert;
    }
    else
    {
        partitionToInsert->next = current;
        previous->next = partitionToInsert;
    }
}

void firstFit(job *newJob)
{
    node *current = notAllocated;
    node *previous = NULL;

    // 遍历未分配分区表，找到第一个足够大的分区
    while (current != NULL && current->size < newJob->size)
    {
        previous = current;
        current = current->next;
    }

    // 如果找到了足够大的分区
    if (current != NULL)
    {
        // 如果分区大小等于作业大小，直接分配
        if (current->size == newJob->size)
        {
            strcpy(current->flag, newJob->name);

            // 将当前分区从未分配表移到已分配表 (头插法)
            if (previous = NULL)
                notAllocated = notAllocated->next;
            else
                previous->next = current->next;

            insertToAllocated(current);
        }
        else
        {
            // 如果分区大小大于作业大小，分裂分区
            node *newAllocatedPartition = (node *)malloc(sizeof(node));
            newAllocatedPartition->address = current->address;
            newAllocatedPartition->size = newJob->size;
            strcpy(newAllocatedPartition->flag, newJob->name);

            insertToAllocated(newAllocatedPartition);

            // 更新原分区的信息
            current->address += newJob->size;
            current->size -= newJob->size;

            printf("分配成功！\n");
        }
    }
    else
    {
        // 如果没有找到足够大的分区
        printf("没有找到足够大的分区来满足%s的需求\n", newJob->name);
    }
}

void bestFit(job *newJob)
{
    node *current = notAllocated;
    node *previous = NULL;
    node *bestPartition = NULL;
    node *bestPartitionPrevious = NULL;
    int bestSize = memory.size; // 初始化为内存的最大值

    // 遍历未分配分区表，找到最佳匹配的分区
    while (current != NULL)
    {
        // 如果当前分区大小大于等于作业大小且小于最佳匹配的大小
        if (current->size >= newJob->size && current->size < bestSize)
        {
            bestPartition = current;
            bestSize = current->size;
            bestPartitionPrevious = previous; // 保存最佳匹配分区的上一个节点
        }
        previous = current;
        current = current->next;
    }

    // 如果找到了最佳匹配的分区
    if (bestPartition != NULL)
    {
        // 如果最佳分区大小恰好等于作业的大小
        if (bestPartition->size == newJob->size)
        {
            strcpy(bestPartition->flag, newJob->name);

            if (bestPartitionPrevious == NULL)
                notAllocated = notAllocated->next;
            else
                bestPartitionPrevious = bestPartitionPrevious->next;

            // 把最佳分区加入到已分配分区表
            insertToAllocated(bestPartition);
        }
        else
        {
            // 如果最佳分区大小大于等于作业的大小，要把最佳分区分割
            node *newAllocatedPartition = (node *)malloc(sizeof(node));
            newAllocatedPartition->address = bestPartition->address;
            newAllocatedPartition->size = newJob->size;
            strcpy(newAllocatedPartition->flag, newJob->name);

            insertToAllocated(newAllocatedPartition);

            // 更新原分区
            bestPartition->address += newJob->size;
            bestPartition->size -= newJob->size;

            printf("分配成功！\n");
        }
    }
    else
    {
        // 如果没有找到足够大的分区
        printf("没有找到足够大的分区来满足%s的需求\n", newJob->name);
    }
}

void worstFit(job *newJob)
{
    node *current = notAllocated;
    node *previous = NULL;
    node *worstPartition = NULL;
    node *worstPartitionPrevious = NULL;

    // 遍历未分配分区表，找到第最大的分区
    while (current != NULL)
    {
        if (current->size >= newJob->size && newJob->size > worstPartition->size)
        {
            worstPartition = current;
            worstPartitionPrevious = previous;
        }
        previous = current;
        current = current->next;
    }

    // 如果找到了足够大的分区
    if (worstPartition->size)
    {
        // 如果分区大小等于作业大小，直接分配
        if (worstPartition->size == newJob->size)
        {
            strcpy(worstPartition->flag, newJob->name);

            // 将当前分区从未分配表移到已分配表 (头插法)
            if (worstPartitionPrevious = NULL)
                notAllocated = notAllocated->next;
            else
                worstPartitionPrevious->next = worstPartition->next;

            insertToAllocated(worstPartition);
        }
        else
        {
            // 如果分区大小大于作业大小，分裂分区
            node *newAllocatedPartition = (node *)malloc(sizeof(node));
            newAllocatedPartition->address = worstPartition->address;
            newAllocatedPartition->size = newJob->size;
            strcpy(newAllocatedPartition->flag, newJob->name);

            insertToAllocated(newAllocatedPartition);

            // 更新原分区的信息
            worstPartition->address += newJob->size;
            worstPartition->size -= newJob->size;

            printf("分配成功！\n");
        }
    }
    else
    {
        // 如果没有找到足够大的分区
        printf("没有找到足够大的分区来满足%s的需求\n", newJob->name);
    }
}

void allocatePartition()
{
    showMenu2();
    int allocationChoice;

    do
    {
        printf("请输入选项：");
        scanf("%d", &allocationChoice);
    } while (!(allocationChoice == 1 || allocationChoice == 2 || allocationChoice == 3));

    printf("请输入作业名：");
    job *newJob = (job *)(malloc(sizeof(job)));
    scanf("%s", newJob->name);
    printf("%s需要分配的主存大小（单位：KB）：");
    scanf("%d", &newJob->size);

    switch (allocationChoice)
    {
    case 1:
        firstFit(newJob);
        break;
    case 2:
        bestFit(newJob);
        break;
    case 3:
        worstFit(newJob);
        break;
    }

    showAllocationState();
}

// 内存去配
void freePartition()
{
    showAllocationState();
    char JobToFree[20];
    printf("请输入要回收的作业名：");
    scanf("%s", JobToFree);

    // 找到对应要回收的分区
    node *current = allocated;
    node *previous = NULL;
    while (current != NULL && strcmp(JobToFree, current->flag) != 0)
    {
        previous = current;
        current = current->next;
    }

    // 找到了要回收的分区
    if (current != NULL)
    {
        // 回收对应分区
        node *partitionToFree = current;
        strcpy(partitionToFree->flag, "空闲");

        // 将partitionToFree移出allocated
        if (previous == NULL)
            allocated = allocated->next;
        else
            previous->next = partitionToFree->next;

        // 看看有没有上邻空闲区、下邻空闲区
        current = notAllocated;
        previous = NULL;
        node *aboveNotAllocated = NULL;
        node *belowNotAllocated = NULL;

        while (current != NULL && (aboveNotAllocated == NULL || belowNotAllocated == NULL))
        {
            // 上邻空闲区
            if (current->address + current->size == partitionToFree->address)
            {
                aboveNotAllocated = current;

                // 移除上邻空闲区
                if (previous == NULL)
                    notAllocated = notAllocated->next;
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
                    notAllocated = notAllocated->next;
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
        insertToNotAllocated(partitionToFree);

        printf("回收成功！\n");
        showAllocationState();
    }
    else
    {
        // 没有找到要回收的分区
        printf("没有找到要回收的分区\n");
    }
}

int main()
{
    // system("chcp 65001");
    printf("请输入内存大小为：");
    scanf("%d", &(memory.size));
    printf("请输入起始地址大小为：");
    scanf("%d", &(memory.address));

    // initialize allocated and notAllocated
    initialize();

    while (1)
    {
        showMenu1();
        int ManagementChoice;

        printf("请输入选项：");
        scanf("%d", &ManagementChoice);
        switch (ManagementChoice)
        {
        case 1:
            allocatePartition();
            break;
        case 2:
            freePartition();
            break;
        case 0:
            return 0;
            break;
        default:
            printf("没有这个选项\n");
            break;
        }
    }
    return 0;
}