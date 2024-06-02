#include <iomanip> // 用于 std::setw
#include <iostream>
#include <memory>
#include <string>
#include <vector>

const int MAX = 2000;

class Job; // 前向声明

class Memory
{
    // private:
    int memorySize;
    int wordLength;
    int blockSize;
    int blockNum;
    std::vector<bool> graph;
    int remainedNum;

  public:
    Memory(int memorySize, int wordLength, int blockSize)
        : memorySize(memorySize), wordLength(wordLength), blockSize(blockSize), blockNum(memorySize / blockSize),
          remainedNum(0), graph(blockNum)
    {
        for (int i = 0; i < blockNum; ++i)
        {
            remainedNum += !(graph[i] = rand() % 2);
        }
    }

    int getBlockNum() const
    {
        return blockNum;
    }

    int getRemainedNum() const
    {
        return remainedNum;
    }

    bool getBlock(int index) const
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

    bool isSafe(int newJobSize) const
    {
        return newJobSize <= remainedNum;
    }

    void printAllocationGraph() const
    {
        std::cout << "主存位视图如下所示：" << std::endl;
        std::cout << "     ";
        for (int i = 0; i < wordLength; ++i)
            std::cout << std::setw(2) << i << ' ';
        std::cout << std::endl << std::setw(2) << " 0 | ";

        for (int i = 0; i < blockNum - 1; ++i)
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

        std::cout << std::setw(2) << graph[blockNum - 1] << std::endl;
        std::cout << "剩余物理块数：" << remainedNum << std::endl;
    }

    void recycle(const std::string &jobToRecycleName);
};

class Job
{
  private:
    std::string name;
    int size;
    std::unique_ptr<int[]> pageTable;
    std::shared_ptr<Job> next;

    static std::shared_ptr<Job> head;

  public:
    Job(const std::string &name, int size) : name(name), size(size), pageTable(new int[size]())
    {
        std::fill(pageTable.get(), pageTable.get() + size, -1);
    }

    ~Job() = default;

    static std::shared_ptr<Job> getHead()
    {
        return head;
    }

    static void setHead(const std::shared_ptr<Job> &newHead)
    {
        head = newHead;
    }

    std::string getName() const
    {
        return name;
    }

    int getSize() const
    {
        return size;
    }

    std::shared_ptr<Job> getNext() const
    {
        return next;
    }

    int getPageTable(int pageIndex) const
    {
        return pageTable[pageIndex];
    }

    void setNext(const std::shared_ptr<Job> &newNext)
    {
        next = newNext;
    }

    void setPageTable(int pageIndex, int blockIndex)
    {
        pageTable[pageIndex] = blockIndex;
    }

    static std::shared_ptr<Job> createJob()
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

        return std::make_shared<Job>(newJobName, newJobSize);
    }

    static void allocate(const std::shared_ptr<Job> &newJob, const std::shared_ptr<Memory> &memory)
    {
        if (memory->isSafe(newJob->getSize()))
        {
            int pageIndex = 0;
            for (int i = 0; i < memory->getBlockNum() && pageIndex < newJob->getSize(); ++i)
            {
                if (!memory->getBlock(i))
                {
                    newJob->setPageTable(pageIndex++, i);
                    memory->setBlock(i, true);
                }
            }

            memory->setRemainedNum(-newJob->getSize());

            newJob->setNext(Job::getHead());
            Job::setHead(newJob);

            std::cout << "分配成功！" << std::endl;
            memory->printAllocationGraph();
            newJob->printPageTable();
        }
        else
        {
            std::cout << "没有足够的空间，给该作业的空间分配失败！" << std::endl;
        }
    }

    void printPageTable() const
    {
        std::cout << "页号\t块号" << std::endl;
        for (int i = 0; i < size; ++i)
        {
            std::cout << i << "\t" << pageTable[i] << std::endl;
        }
    }
};

std::shared_ptr<Job> Job::head = nullptr;

void Memory::recycle(const std::string &jobToRecycleName)
{
    auto current = Job::getHead();
    std::shared_ptr<Job> previous = current;

    while (current && current->getName() != jobToRecycleName)
    {
        previous = current;
        current = current->getNext();
    }

    if (current)
    {
        for (int i = 0; i < current->getSize(); ++i)
        {
            if (current->getPageTable(i) != -1)
            {
                setBlock(current->getPageTable(i), false);
            }
        }

        setRemainedNum(current->getSize());

        if (previous == nullptr)
        {
            Job::setHead(current->getNext());
        }
        else
        {
            previous->setNext(current->getNext());
        }

        std::cout << "该作业回收成功！" << std::endl;
        printAllocationGraph();
    }
    else
    {
        std::cout << "回收失败，未找到该作业！" << std::endl;
    }
}

int operationChoice()
{
    std::cout << "***************分页式管理***************" << std::endl;
    std::cout << "         *    1. 内存分配    *" << std::endl;
    std::cout << "         *    2. 内存回收    *" << std::endl;
    std::cout << "         *    0. 退出        *" << std::endl;
    std::cout << "请输入选项：";

    int choice;
    do
    {
        std::cin >> choice;
    } while (choice != 1 && choice != 2 && choice != 0);

    return choice;
}

void menu()
{
    int memorySize, wordLength, blockSize;
    std::cout << "请输入系统内存空间的大小：";
    std::cin >> memorySize;
    std::cout << "请输入字长（16/32/64）：";
    std::cin >> wordLength;
    std::cout << "请输入物理块大小：";
    std::cin >> blockSize;

    auto myMemory = std::make_shared<Memory>(memorySize, wordLength, blockSize);
    myMemory->printAllocationGraph();

    while (true)
    {
        int choice = operationChoice();
        switch (choice)
        {
        case 1: {
            auto newJob = Job::createJob();
            Job::allocate(newJob, myMemory);
            break;
        }
        case 2: {
            std::string jobToRecycleName;
            std::cout << "请输入你要回收的作业名：";
            std::cin >> jobToRecycleName;
            myMemory->recycle(jobToRecycleName);
            break;
        }
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
