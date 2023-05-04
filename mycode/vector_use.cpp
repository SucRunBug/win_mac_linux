#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

using namespace std;

int main()
{
    // 利用迭代器区间构造vector
    vector<int> v1{1, 2, 3, 4, 5};
    // vector<int> v2(++v1.begin(), --v1.end());

    // 头插和查找到2后插入20
    v1.insert(v1.begin(), 0);
    auto pos = find(v1.begin(), v1.end(), 2);
    if (pos != v1.end())
    {
        v1.insert(pos, 20);
    }
    for (auto e: v1)
    {
        cout << e << " ";
    }
    cout << endl;
    // 排序
    sort(v1.begin(), v1.end());
    for (auto e: v1)
    {
        cout << e << " ";
    }
    cout << endl;
    sort(v1.begin(), v1.end(), greater<int>());
    for (auto e: v1)
    {
        cout << e << " ";
    }
    cout << endl;
    

    return 0;
}