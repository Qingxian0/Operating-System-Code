#include <iomanip> // 用于 std::setw
#include <iostream>
#include <string>
const int MAX = 2000;

int memorySize; // 系统内存大小（以基本字长为单位）
int wordLength; // 字长
int blockSize;  // 物理块大小（以基本字长为单位）
int blockNum;   // 物理块个数

class Memory
{
  private:
    int memorySize;  // 系统内存大小（以基本字长为单位）
    int wordLength;  // 字长
    int blockSize;   // 物理块大小（以基本字长为单位）
    int blockNum;    // 物理块个数
    bool *graph;     // 位示图
    int remainedNum; // 剩余空闲块数
  public:
    Memory(int memorySize, int wordLength, int blockSize)
        : memorySize(memorySize), wordLength(wordLength), blockSize(blockSize), blockNum(memorySize / blockSize),
          remainedNum(0)
    {
        graph = new bool[blockNum];
        for (int i = 0; i < blockNum; i++)
        {
            remainedNum = remainedNum + 1 - (graph[i] = rand() % 2);
        }
    }

    ~Memory()
    {
        delete[] graph;
    }

    int getBlockNum()
    {
        return blockNum;
    }

    int getRemainedNum()
    {
        return remainedNum;
    }

    bool getBlock(int index)
    {
        return graph[index];
    }

    void setRemainedNum(int x)
    {
        remainedNum += x;
    }

    void setBlock(int index, bool x)
    {
        graph[index] = x;
    }

    bool isSafe(int newJobSize)
    {
        return newJobSize <= blockNum;
    }

    void printAllocationGraph()
    {
        // 函数功能：打印系统内存信息
        int i = 0;
        std::cout << "主存位视图如下所示：" << std::endl;
        std::cout << "     ";
        for (i = 0; i < wordLength; i++)
            std::cout << std::setw(2) << i << ' ';
        std::cout << std::endl << std::setw(2) << " 0 | ";

        for (i = 0; i < blockNum; i++)
        {
            if ((i + 1) % wordLength == 0)
            {
                std::cout << std::setw(2) << graph[i] << std::endl;
                std::cout << std::setw(2) << (i + 1) / wordLength << " | ";
            }
            else
            {
                std::cout << std::setw(2) << graph[i] << ' ';
            }
        }

        // 单独处理最后一个block
        std::cout << std::setw(2) << graph[blockNum - 1] << std::endl;

        std::cout << "剩余物理块数：" << remainedNum << std::endl;
    }
};

class Job
{
  private:
    std::string name; // 作业名
    int size;         // 大小
    int *pageTable;   // 作业页表
    Job *next;
    static Job *head;

  public:
    Job(std::string name, int size, Job *next = nullptr) : name(name), size(size), next(next)
    {
        // 初始化页表，-1表示未分配块
        pageTable = new int[size]{-1};

        if (head == nullptr)
        {
            head = this;
        }
    }

    ~Job()
    {
        delete[] pageTable;
    }

    static Job *getHead()
    {
        return head;
    }

    static void setHead(Job *newHead)
    {
        head = newHead;
    }

    std::string getName()
    {
        return name;
    }

    int getSize()
    {
        return size;
    }

    Job *getNext()
    {
        return next;
    }

    int getPageTable(int pageIndex)
    {
        return pageTable[pageIndex];
    }

    void setNext(Job *newNext)
    {
        next = newNext;
    }

    void setPageTable(int pageIndex, int blockIndex)
    {
        pageTable[pageIndex] = blockIndex;
    }

    static Job *createJob()
    {
        std::string newJobName;
        int newJobSize = 0;

        std::cout << "请输入作业名字：";
        std::cin >> newJobName;

        do
        {
            std::cout << "请输入作业需要分配的大小：";
            std::cin >> newJobSize;
            if (newJobSize > MAX)
            {
                std::cout << "要分配的空间大小超过MAX！请重新输入！" << std::endl;
            }
        } while (newJobSize > MAX);

        return new Job(newJobName, newJobSize);
    }

    static void allocate(Job *newJob, Memory *memory)
    {
        /* 函数功能：建立作业节点，判断是否可以分配，可以进行下一步，进行内存分配，
           插入作业链表，打印该作业的页表信息，打印系统内存信息。
        */

        // 判断是否可以分配空间
        if (memory->isSafe(newJob->getSize()))
        {
            int i = 0, pageIndex = 0;
            for (i = 0; i < memory->getBlockNum() && pageIndex < newJob->getSize(); i++)
            {
                if (!memory->getBlock(i))
                {
                    newJob->setPageTable(pageIndex++, i);
                    memory->setBlock(i, true);
                }
            }

            memory->setRemainedNum(-newJob->getSize());

            // 头插法插入newJob
            newJob->setNext(Job::getHead());
            Job::setHead(newJob);

            // 打印相关信息
            std::cout << "分配成功！" << std::endl;
            memory->printAllocationGraph();
            newJob->printPageTable();
        }
        else
        {
            std::cout << "没有足够的空间，给该作业的空间分配失败！" << std::endl;
            delete newJob;
        }
    }

    static void recycle(Memory *memory)
    {
        // 函数功能：找到待回收的作业节点，释放内存，从作业链表中删除，打印系统内存信息
        std::string jobToRecycleName;
        std::cout << "请输入你要回收的作业名：";
        std::cin >> jobToRecycleName;

        // 查找该作业
        Job *current = getHead();
        Job *previous = nullptr;

        while (current && current->getName().compare(jobToRecycleName) != 0)
        {
            previous = current;
            current = current->getNext();
        }

        if (current)
        {
            // 回收对应的block
            for (int i = 0; i < current->getSize(); i++)
            {
                if (current->getPageTable(i) != -1)
                {
                    memory->setBlock(current->getPageTable(i), false);
                }
            }

            memory->setRemainedNum(current->getSize());

            // 将改作业移出作业队列并free
            if (previous == nullptr)
                Job::setHead(current->getNext());
            else
                previous->setNext(current->getNext());

            std::cout << "该作业回收成功！" << std::endl;
            memory->printAllocationGraph();
            delete current;
        }
        else
        {
            std::cout << "回收失败，未找到该作业！" << std::endl;
        }
    }

    void printPageTable()
    {
        std::cout << "页号\t块号" << std::endl;
        for (int i = 0; i < size; i++)
        {
            std::cout << i << "\t" << pageTable[i] << std::endl;
        }
    }
};

// 定义静态成员变量
Job *Job::head = nullptr;

// 函数名：OperationChoice       函数参数：无
int operationChoice()
{
    // 函数功能：打印操作菜单，选择需要进行的操作
    std::cout << "***************分页式管理***************" << std::endl;
    std::cout << "         *    1. 内存分配    *" << std::endl;
    std::cout << "         *    2. 内存去配    *" << std::endl;
    std::cout << "         *    0. 退出        *" << std::endl;
    std::cout << "请输入选项：";

    int choice;
    do
    {
        std::cin >> choice;
    } while (choice != 1 && choice != 2 && choice != 0);

    return choice;
}

// 函数名：menu          函数参数：无
void menu()
{
    // 函数功能：定义内存大小，字长与物理块大小，初始化系统内存和内存分配表，打印系统内存信息
    std::cout << "请输入系统内存空间的大小：";
    std::cin >> memorySize;
    std::cout << "请输入字长（16/32/64）：";
    std::cin >> wordLength;
    std::cout << "请输入物理块大小：";
    std::cin >> blockSize;
    Memory *myMemory = new Memory(memorySize, wordLength, blockSize);

    myMemory->printAllocationGraph();

    while (true)
    {
        int c = operationChoice();
        switch (c)
        {
        case 1: {
            Job *newJob = Job::createJob();
            Job::allocate(newJob, myMemory);
            break;
        }
        case 2: {
            Job::recycle(myMemory);
            break;
        }
        case 0:
            delete myMemory;
            return;
        }
    }
}

int main()
{
    menu();
    return 0;
}