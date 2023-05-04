#include <iostream>
#include <list>
using namespace std;

int main()
{
    list<int> l2;
    l2.push_back(1);
    l2.push_back(2);
    l2.push_back(3);
    for (auto e: l2)
    {
        cout << e << " ";
    }
    cout << endl;
    return 0;
}