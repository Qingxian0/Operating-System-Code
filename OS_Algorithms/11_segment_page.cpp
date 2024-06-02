#include <iomanip> // 用于 setw
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int M = 1000;
const int N = 64;

bool allocTable[M * N]; // 主存分配表
int remained;           // 剩余块数
int MemorySize;         // 主存大小
int blockNum;           // 块数
int blockLength;        // 块长
int wordLength;         // 字长

// 页表
struct PageTable
{
    int num;   // 页号
    int block; // 块号
};

// 段表
struct SegmentTable
{
    int segmentNum;         // 段号
    string state;           // 状态
    int pageLength;         // 页长
    PageTable pageTable[M]; // 页表
};

// 作业
struct Job
{
    int size;    // 作业大小
    string name; // 作业名字
    // int address;//作业起址
    int segmentNum;               // 段数
    SegmentTable segmentTable[M]; // 段表
    struct Job *next;
};

Job *head = NULL; // 作业链表

// 函数名：Print() 参数：无
void printAllocationTable()
{
    // 函数功能：输出主存位示图，并打印作业分配情况。
    // 函数功能：打印系统内存信息
    int i = 0;
    cout << "主存位视图如下所示：" << endl;
    cout << "     ";
    for (i = 0; i < wordLength; i++)
        cout << setw(2) << i << ' ';
    cout << endl << setw(2) << " 0 | ";

    for (i = 0; i < blockNum - 1; i++)
    {
        if ((i + 1) % wordLength == 0)
        {
            cout << setw(2) << allocTable[i] << endl;
            cout << setw(2) << (i + 1) / wordLength << " | ";
        }
        else
        {
            cout << setw(2) << allocTable[i] << ' ';
        }
    }

    // 单独处理最后一个block
    cout << setw(2) << allocTable[blockNum - 1] << endl;

    cout << "剩余物理块数：" << remained << endl;
}

void printPageTable(const Job *job)
{
    cout << "作业" << job->name << "的分配情况如下: " << endl;
    for (int i = 0; i < job->segmentNum; ++i)
    {
        cout << "\t" << "第" << i << "段: " << endl;
        cout << "\t\t" << "页号\t" << "块号" << endl;
        for (int j = 0; j < job->segmentTable[i].pageLength; j++)
        {
            cout << "\t\t" << job->segmentTable[i].pageTable[j].num << "\t" << job->segmentTable[i].pageTable[j].block
                 << endl;
        }
    }
}

// 函数名：init() 参数：无
void init()
{
    // 函数功能：随机初始化位示图
    cout << "请输入内存大小为："; // 以字长为单位
    cin >> MemorySize;

    cout << "请输入系统字长大小(32/64)为: ";
    cin >> wordLength;

    cout << "请输入块长: "; // 以字长为单位
    cin >> blockLength;

    blockNum = MemorySize / blockLength; // 主存物理块数
    remained = 0;                        // 最后一个字用于存储剩余空闲块数

    for (int i = 0; i < blockNum; i++)
    {
        remained += !(allocTable[i] = rand() % 2);
    }
}

// 函数名：allocation() 参数：name(作业名),size（作业大小）
void allocation(string name, int size)
{
    // 函数功能：为作业随机分配物理块
    Job *newJob = new Job;
    newJob->name = name;
    newJob->size = size;
    newJob->next = nullptr;
    cout << "请输入要将作业的段数：";
    cin >> newJob->segmentNum;
    int remainedUnallocSize = size;
    for (int i = 0; i < newJob->segmentNum; i++)
    {
        newJob->segmentTable[i].segmentNum = i;
        newJob->segmentTable[i].state = "NO";
        cout << "剩余" << remainedUnallocSize << "的内存未分配，" << "请输入第" << i << "段的大小: ";

        int tmpSegmentSize = 0x7fffffff;
        do
        {
            cin >> tmpSegmentSize;
            if (tmpSegmentSize > remainedUnallocSize)
            {
                cout << "段过大，请重新输入！" << endl;
            }
        } while (tmpSegmentSize > remainedUnallocSize);
        newJob->segmentTable[i].pageLength = tmpSegmentSize;

        // 初始化页表
        int j = 0;
        for (j = 0; j < tmpSegmentSize; j++)
        {
            newJob->segmentTable[i].pageTable[j].num = j;
        }

        // 内存分配
        int pageNum = 0;
        for (j = 0; j < blockNum && pageNum < tmpSegmentSize; j++)
        {
            if (!allocTable[j])
            {
                newJob->segmentTable[i].pageTable[pageNum++].block = j;
                allocTable[j] = 1;
            }
        }

        // 修改相关参数
        remained -= tmpSegmentSize;
        remainedUnallocSize -= tmpSegmentSize;
        newJob->segmentTable[i].state = "YES";

        // 将作业加入作业队列
        newJob->next = head;
        head = newJob;

        cout << "内存分配成功！" << endl;
    }

    // 打印相关信息
    printAllocationTable();
    printPageTable(newJob);
}

// 函数名：recycle() 参数：str(作业名)
void recycle(string name)
{
    // 函数功能：匹配作业并回收改作业，释放空间，最后调用Print()
    // 匹配作业
    Job *current = head;
    Job *previous = NULL;
    while (current and current->name.compare(name) != 0)
    {
        previous = current;
        current = current->next;
    }

    if (current)
    {
        // 找到了，释放对应物理块并将其移出作业队列
        for (int i = 0; i < current->segmentNum; i++)
        {
            for (int j = 0; j < current->segmentTable[i].pageLength; j++)
            {
                allocTable[current->segmentTable[i].pageTable[j].block] = 0;
            }
            remained += current->segmentTable[i].pageLength;
        }

        // 移出作业队列
        if (previous == NULL)
            head = head->next;
        else
            previous->next = current->next;

        delete current;
        printAllocationTable();
    }
    else
    {
        // 没找到
        cout << "未找到对应作业！" << endl;
    }
}

// 函数名：menu() 参数：无
int menu()
{
    // 函数功能：打印分区管理菜单
    // 函数功能：打印操作菜单，选择需要进行的操作
    cout << "***************分页式管理***************" << endl;
    cout << "         *    1. 内存分配    *" << endl;
    cout << "         *    2. 内存去配    *" << endl;
    cout << "         *    0. 退出        *" << endl;
    cout << "请输入选项：";

    int choice;
    do
    {
        cin >> choice;
        if (choice != 1 && choice != 2 && choice != 0)
            cout << "非法选择！" << endl;
    } while (choice != 1 && choice != 2 && choice != 0);

    return choice;
}

int main()
{
    init();
    printAllocationTable();

    int choice;
    while (1)
    {
        int choice = menu();
        if (choice == 1)
        {
            // 内存分配
            printAllocationTable();
            cout << "请输入作业名: ";
            string name;
            cin >> name;
            cout << "请输入" << name << "所需主存大小: ";
            int size = 0x7FFFFFFF;
            cin >> size;

            if (size > remained * blockLength)
            {
                cout << "主存空间不足，无法装载！\n";
                continue;
            }
            else
            {
                allocation(name, size);
            }
        }
        else if (choice == 2)
        {
            // 内存去配
            string name;
            printf("请输入作业名:");
            cin >> name;
            recycle(name);
        }
        else if (choice == 0)
        {
            // 退出
            break;
        }
    }

    return 0;
}
