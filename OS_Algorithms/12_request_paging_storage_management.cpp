#include <bits/stdc++.h>
using namespace std;
const int N = 1e3 + 10;

int n = 0; // n为物理块的块数
enum SCHEDUAL
{
    FIFO,
    LRU
};

struct Job
{
    string name;
    int num_visit;            // 访问页面次数
    vector<int> visit_order;  // 作业页面顺序
    vector<bool> miss_record; // 用于记录本次是否产生缺页面
    vector<int> out_record;   // 用于记录本次置换出去的页面
    vector<vector<int>> sched_record;
};

// 函数名：menu  参数：无
int menu()
{
    // 函数功能：输出选择界面菜单，1为FIFO，2为LFU，0退出
    cout << "**********请求分页式存储管理**********" << endl;
    cout << "       *     1. FIFO分配     *" << endl;
    cout << "       *     2. LRU分配      *" << endl;
    cout << "       *     0. 退出         *" << endl;
    cout << "请输入选项：";

    int choice;
    do
    {
        cin >> choice;
        if (choice != 1 && choice != 2 && choice != 0)
            cout << "输入的选项不存在！请重新输入：" << endl;
    } while (choice != 1 && choice != 2 && choice != 0);

    return choice;
}

// 函数名：input  参数：无
Job *inputNewJob()
{
    // 函数功能：输入作业信息
    Job *new_job = new Job;
    cout << "请输入作业名：";
    cin >> new_job->name;
    cout << "请输入作业页面访问次数：";
    cin >> new_job->num_visit;

    // initialize
    new_job->visit_order.resize(new_job->num_visit, -1);
    new_job->miss_record.resize(new_job->num_visit, false);
    new_job->out_record.resize(new_job->num_visit, -1);
    new_job->sched_record.resize(new_job->num_visit);

    cout << "请输入作业页面访问顺序：";
    for (int i = 0; i < new_job->num_visit; i++)
    {
        cin >> new_job->visit_order[i];
    }

    return new_job;
}

// 函数名：print    参数：无
void printSchedualProcess(Job *job, SCHEDUAL schedual)
{
    // 函数功能：输出页面置换详情和缺页中断率
    if (schedual == FIFO)
        cout << "**********打印作业FIFO调度进入主存的过程**********" << endl;
    else if (schedual == LRU)
        cout << "**********打印作业LRU调度进入主存的过程**********" << endl;

    cout << "作业名：" << job->name << endl;
    cout << "作业调度过程：" << endl;
    cout << setw(2) << "i:\t";
    for (int i = 0; i < job->num_visit; i++)
    {
        cout << setw(2) << i << " ";
    }
    cout << endl << "page:\t";

    for (int i = 0; i < job->num_visit; i++)
    {
        cout << setw(2) << job->visit_order[i] << " ";
    }

    cout << endl << "sched:\t";
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < job->num_visit; j++)
        {
            if (job->sched_record[j][i] != -1)
                cout << setw(2) << job->sched_record[j][i] << " ";
            else
                cout << "   ";
        }
        cout << endl << "\t";
    }

    int num_miss = 0;
    for (int i = 0; i < job->num_visit; i++)
    {
        if (job->miss_record[i])
        {
            cout << setw(2) << "+" << " ";
            num_miss++;
        }
        else
            cout << "   ";
    }

    cout << endl << "out:\t";
    for (int i = 0; i < job->num_visit; i++)
    {
        if (job->out_record[i] != -1)
            cout << setw(2) << job->out_record[i] << " ";
        else
            cout << "   ";
    }

    cout << endl << "缺页中断率为：" << (1.0 * num_miss) / job->num_visit * 100 << "%" << endl;
}

// 函数名：FIFOSchedualing   参数：无
void FIFOSchedualing(Job *job)
{
    // 函数功能：进行FIFO调度
    vector<int> sched_process;

    int i = 0;
    for (i = 0; i < n; i++)
    {
        job->miss_record[i] = true;
        sched_process.push_back(job->visit_order[i]);
        job->sched_record[i].assign(sched_process.begin(), sched_process.end());
        job->sched_record[i].resize(n, -1);
    }

    for (i = n; i < job->num_visit; i++)
    {
        if (find(sched_process.begin(), sched_process.end(), job->visit_order[i]) == sched_process.end())
        {
            job->miss_record[i] = true;
            job->out_record[i] = sched_process.front();
            sched_process.erase(sched_process.begin());
            sched_process.push_back(job->visit_order[i]);
        }
        job->sched_record[i].assign(sched_process.begin(), sched_process.end());
    }
}

// 函数名：LRUSchedualing   参数：无
void LRUSchedualing(Job *job)
{
    // 函数功能：进行LRU调度
    vector<int> sched_process;
    vector<int> visit_record;

    int i = 0;
    for (i = 0; i < n; i++)
    {
        job->miss_record[i] = true;
        sched_process.push_back(job->visit_order[i]);
        visit_record.push_back(1); // synchronize with "sched_process"

        for (auto it = visit_record.begin(); it != visit_record.end() - 1; it++)
            *it += 1;

        job->sched_record[i].assign(sched_process.begin(), sched_process.end());
        job->sched_record[i].resize(n, -1);
    }

    for (i = n; i < job->num_visit; i++)
    {
        // 距离上一次访问的时间 += 1
        for (auto it = visit_record.begin(); it != visit_record.end(); it++)
        {
            *it += 1;
        }

        if (find(sched_process.begin(), sched_process.end(), job->visit_order[i]) == sched_process.end())
        {
            job->miss_record[i] = true;
            int lru_index = 0; // 最长时间未被访问的元素下标
            for (int j = 0; j < n; j++)
            {
                if (visit_record[j] > visit_record[lru_index])
                    lru_index = j;
            }
            job->out_record[i] = sched_process[lru_index];
            sched_process.erase(sched_process.begin() + lru_index);
            visit_record.erase(visit_record.begin() + lru_index);
            sched_process.push_back(job->visit_order[i]);
            visit_record.push_back(1);
        }
        else
        {
            int visit_index = 0;
            while (visit_index < n && sched_process[visit_index] != job->visit_order[i])
                visit_index++;
            visit_record[visit_index] = 1;
        }
        job->sched_record[i].assign(sched_process.begin(), sched_process.end());
    }
}

int main()
{
    // system("chcp 65001");
    cout << "请输入主存物理块的块数：";
    cin >> n;

    int choice = 0;
    while (true)
    {
        choice = menu();
        switch (choice)
        {
        case 1: {
            Job *new_job = inputNewJob();
            FIFOSchedualing(new_job);
            printSchedualProcess(new_job, FIFO);
            delete new_job;
            break;
        }
        case 2: {
            Job *new_job = inputNewJob();
            LRUSchedualing(new_job);
            printSchedualProcess(new_job, LRU);
            delete new_job;
            break;
        }
        case 0: {
            return 0;
        }
        }
    }
    return 0;
}
