#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int INNERMOST = 199;
const int OUTERMOST = 0;
int length;           // 访问序列长度
vector<int> sequence; // 访问柱面的顺序（从0开始）
vector<int> result;   // 存放最短优先的结果
int cur_cylinder;     // 当前柱面

// 函数名：input 参数：无
void input()
{
    // 函数功能:输入访问序列信息
    cout << "Input the length of accessing sequence: ";
    cin >> length;
    sequence.resize(length, 0);
    result.resize(length, 0);
    cout << "Input the order of accessing cylinders: ";
    for (int i = 0; i < length; i++)
        cin >> sequence[i];
    cout << "Input the cylinder accessed currently: ";
    cin >> cur_cylinder;
}

// 函数名：fcfs 参数：无
void fcfs()
{
    // 函数功能: 调用先来先服务算法
    cout << "The order of moving arm: ";
    cout << cur_cylinder << " ";
    int sum = 0;
    for (int cyl : sequence)
    {
        cout << cyl << " ";
        sum += abs(cur_cylinder - cyl);
        cur_cylinder = cyl;
    }
    cout << endl << "The sum of moving cylinders is: " << sum << endl;
}

// 函数名：sstf参数：无
void sstf()
{
    // 函数功能: 调用短进程优先调度算法
    cout << "The order of moving arm: ";
    cout << cur_cylinder << " ";
    int sum = 0;

    while (!sequence.empty())
    {
        auto nearest = sequence.begin();
        int min_distance = 0x7fffffff;

        for (auto it = sequence.begin(); it != sequence.end(); it++)
        {
            int distance = abs(cur_cylinder - *it);
            if (distance < min_distance)
            {
                nearest = it;
                min_distance = distance;
            }
        }

        cur_cylinder = *nearest;
        sum += min_distance;

        cout << cur_cylinder << " ";

        sequence.erase(nearest);
    }
    cout << endl << "The sum of moving cylinders is: " << sum << endl;
}

// 函数名：oneWayScan 参数：无
void oneWayScan()
{
    // 函数功能:调用单向扫描算法
    vector<bool> book(length, false);
    sort(sequence.begin(), sequence.end());
    cout << "The order of moving arm: ";
    cout << cur_cylinder << " ";
    int sum = 0;

    for (int i = 0; i < length; i++)
    {
        if (sequence[i] > cur_cylinder)
        {
            sum += abs(cur_cylinder - sequence[i]);
            cur_cylinder = sequence[i];
            cout << cur_cylinder << " ";
            book[i] = true;
        }
    }

    if (cur_cylinder != INNERMOST)
    {
        sum += abs(cur_cylinder - INNERMOST);
        cout << INNERMOST << " ";
    }

    cur_cylinder = OUTERMOST;
    cout << OUTERMOST << " ";
    sum += INNERMOST;

    for (int i = 0; i < length; i++)
    {
        if (!book[i] && sequence[i] > cur_cylinder)
        {
            sum += abs(cur_cylinder - sequence[i]);
            cur_cylinder = sequence[i];
            cout << cur_cylinder << " ";
            // book[i] = true;
        }
    }

    cout << endl << "The sum of moving cylinders is: " << sum << endl;
}

// 函数名：twoWayScan 参数：无
void twoWayScan()
{
    // 函数功能:调用双向扫描算法
    vector<bool> book(length, false);
    sort(sequence.begin(), sequence.end());
    cout << "The order of moving arm: ";
    cout << cur_cylinder << " ";
    int sum = 0;

    for (int i = 0; i < length; i++)
    {
        if (sequence[i] > cur_cylinder)
        {
            sum += abs(cur_cylinder - sequence[i]);
            cur_cylinder = sequence[i];
            cout << cur_cylinder << " ";
            book[i] = true;
        }
    }

    if (cur_cylinder != INNERMOST)
    {
        sum += abs(cur_cylinder - INNERMOST);
        cout << INNERMOST << " ";
    }

    cur_cylinder = INNERMOST;
    for (int i = length - 1; i >= 0; i--)
    {
        if (!book[i] && sequence[i] < cur_cylinder)
        {
            sum += abs(cur_cylinder - sequence[i]);
            cur_cylinder = sequence[i];
            cout << cur_cylinder << " ";
            // book[i] = true;
        }
    }

    cout << endl << "The sum of moving cylinders is: " << sum << endl;
}

// 函数名：elevator 参数：无
void elevator()
{
    // 函数功能:调用电梯调度算法
    vector<bool> book(length, false);
    sort(sequence.begin(), sequence.end());
    int cur_cylinder_1 = cur_cylinder, cur_cylinder_2 = cur_cylinder;

    // From OUTER to INNER
    cout << endl << "*****Elevator Schedualing (From OUTER to INNER)*****" << endl;
    cout << "The order of moving arm: ";
    cout << cur_cylinder_1 << " ";
    int sum = 0;

    for (int i = 0; i < length; i++)
    {
        if (sequence[i] > cur_cylinder_1)
        {
            sum += abs(cur_cylinder_1 - sequence[i]);
            cur_cylinder_1 = sequence[i];
            cout << cur_cylinder_1 << " ";
            book[i] = true;
        }
    }

    for (int i = length - 1; i >= 0; i--)
    {
        if (!book[i] && sequence[i] < cur_cylinder_1)
        {
            sum += abs(cur_cylinder_1 - sequence[i]);
            cur_cylinder_1 = sequence[i];
            cout << cur_cylinder_1 << " ";
            // book[i] = true;
        }
    }

    cout << endl << "The sum of moving cylinders is: " << sum << endl;

    reverse(sequence.begin(), sequence.end());
    book = vector<bool>(length, false);
    // From Inner to OUTER
    cout << endl << "*****Elevator Schedualing (From Inner to Outer)*****" << endl;
    cout << "The order of moving arm: ";
    cout << cur_cylinder_2 << " ";
    sum = 0;

    for (int i = 0; i < length; i++)
    {
        if (sequence[i] < cur_cylinder_2)
        {
            sum += abs(cur_cylinder_2 - sequence[i]);
            cur_cylinder_2 = sequence[i];
            cout << cur_cylinder_2 << " ";
            book[i] = true;
        }
    }

    for (int i = length - 1; i >= 0; i--)
    {
        if (!book[i] && sequence[i] > cur_cylinder_2)
        {
            sum += abs(cur_cylinder_2 - sequence[i]);
            cur_cylinder_2 = sequence[i];
            cout << cur_cylinder_2 << " ";
            // book[i] = true;
        }
    }

    cout << endl << "The sum of moving cylinders is: " << sum << endl;
}

// 函数名：Out参数：无
void Menu()
{
    // 函数功能:输出选项菜单
    cout << "**********Menu of Disk Schedualing**********" << endl;
    cout << "     *             1.FCFS             *" << endl;
    cout << "     *             2.SSTF             *" << endl;
    cout << "     *             3.One-Way Scan     *" << endl;
    cout << "     *             4.Two-Way Scan     *" << endl;
    cout << "     *             5.Elevator         *" << endl;
    cout << "     *             0.exit             *" << endl;
    cout << "Please input the choice: ";
}

int main()
{
    int c = 0;
    while (1)
    {
        Menu(); // 给出选项
        cin >> c;

        switch (c) // 选择算法
        {
        case 1:
            input();
            fcfs();
            break;
        case 2:
            input();
            sstf();
            break;
        case 3:
            input();
            oneWayScan();
            break;
        case 4:
            input();
            twoWayScan();
            break;
        case 5:
            input();
            elevator();
            break;
        default:
            cout << "End of Schedualing!" << endl;
            exit(1);
        }
    }
    return 0;
}
