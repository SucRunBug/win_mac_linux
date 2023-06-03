此文件记录我的每周刷题记录。

## 第一周

现在有 n 个蜂巢，每一个蜂巢都对应了一个蜂蜜值 Si。 有一些蜂巢相互联结，使得他们可以共享蜂蜜值，即该蜂巢的蜂蜜值变为：它和它连接(直接连接或间接连接）的蜂巢的蜂蜜值的和。 现在小码哥想要查询一下一些蜂巢的蜂蜜值。  输入格式：第一行有两个数 n（蜂巢的数量）、m（操作的数量） 第二行有 n 个数字（s1,…, sn)：分别表示了每一个蜂巢的蜂蜜值。 随后有 m行：第一个数字如果是 1，则后面还有两个数字a，b，表示此次发现蜂巢a和b是相连的。第一个数字如果是2，则后面只有一个数字c，表示查询所有与蜂巢c相连的蜂巢(包括自己)的总蜂蜜值。 输出格式：对每一次的查询操作输出查询的蜂巢的蜂蜜值。

```c++
#include <iostream>
#include <vector>

using namespace std;

// 并查集的数据结构
class UnionFind {
public:
    vector<int> parent;
    vector<int> honey;

    UnionFind(int n, const vector<int>& honeyValues) {
        parent.resize(n + 1);
        honey.resize(n + 1);

        for (int i = 1; i <= n; i++) {
            parent[i] = i;
            honey[i] = honeyValues[i - 1];
        }
    }

    int find(int x) {
        if (parent[x] == x)
            return x;
        return parent[x] = find(parent[x]);
    }

    void unite(int a, int b) {
        int rootA = find(a);
        int rootB = find(b);

        if (rootA != rootB) {
            parent[rootB] = rootA;
            honey[rootA] += honey[rootB];
        }
    }

    int getHoney(int x) {
        int root = find(x);
        return honey[root];
    }
};

int main() {
    int n, m;
    cin >> n >> m;

    vector<int> honeyValues(n);
    for (int i = 0; i < n; i++) {
        cin >> honeyValues[i];
    }

    UnionFind uf(n, honeyValues);

    for (int i = 0; i < m; i++) {
        int op;
        cin >> op;

        if (op == 1) {
            int a, b;
            cin >> a >> b;
            uf.unite(a, b);
        } else if (op == 2) {
            int c;
            cin >> c;
            int honey = uf.getHoney(c);
            cout << honey << endl;
        }
    }

    return 0;
}

```

这段代码首先定义了一个`UnionFind`类来表示并查集的数据结构，其中`parent`数组用于记录每个蜂巢的父节点，`honey`数组用于记录每个蜂巢的蜂蜜值。构造函数会初始化并查集，将每个蜂巢的父节点初始化为自身，蜂蜜值初始化为给定的输入值。

接下来的`find`函数用于查找节点的根节点，实现了路径压缩来提高查询效率。`unite`函数用于合并两个蜂巢，将其中一个蜂巢的根节点设置为另一个蜂巢的根节点，并更新蜂蜜值。

在主函数中，首先读取输入的蜂巢数量`n`和操作数量`m`，然后读取每个蜂巢的蜂蜜值。接下来，根据操作类型进行相应的处理。如果是操作类型1，读取两个蜂巢的编号并调用`unite`函数进行合并。如果是操作类型2，读取一个蜂巢的编号并调用`getHoney`函数获取与该蜂巢相连的蜂巢的总蜂蜜值，并输出结果。





讲一个比较有意思的故事：约瑟夫是犹太军队的一个将军，在反抗罗马的起义中，他所率领的军队被击溃，只剩下残余的部队40余人，他们都是宁死不屈的人，所以不愿投降做叛徒。一群人表决说要死，所以用一种策略来先后杀死所有人。 于是约瑟夫建议：每次由其他两人一起杀死一个人，而被杀的人的先后顺序是由抽签決定的，约瑟夫有预谋地抽到了最后一签，在杀了除了他和剩余那个人之外的最后一人，他劝服了另外一个没死的人投降了罗马。 我们这个规则是这么定的： 在一间房间总共有n个人(编号 1~n），最后只能有一个人活下来。 按照如下规则去杀人： 所有人围成一圈；从1号开始顺时针（1一＞n） 报数，每次报到q的人将被杀掉，被杀掉的人将从房间内被移走。然后从被杀掉的下一个人重新报数，继续报到q，再清除，直到剩余一人。 你要做的是：当你在这一群人之间时，你必须选择一个位置以使得你变成那剩余的最后一人，也就是活下来。 实现约瑟夫环首先需要判断选择的那个人是否是最后一个幸存的人，不是的话再进行约瑟夫环行动。 输入格式一行为n, q 输出格式为剩下人的编号

```cpp
#include<bits/stdc++.h> 

using namespace std;

int josephus(int n, int q) {
    vector<int> people(n);
    // 编号
    for (int i = 0; i < n; ++i) {
        people[i] = i + 1;
    }
    int index = 0;  // 记录报数的位置
    while (people.size() > 1) {
        // (0+2-1) % 5 = 1  -> kill people[1]
        index = (index + q - 1) % people.size();
        people.erase(people.begin() + index);
    }
    return people[0];
}

int main( )
{
    int n, q;
    cin >> n >> q;
    cout << josephus(n, q) << endl;
    return 0;
}
```



给出两个一元多项式 LA 和LB，请你将这两个一元多项式相加，得到新的一元多项式LC 输入格式：第一行两个整数n和 m。 分别表示 LA和LB的项数 接下来n行，每行输入LA的每一项的信息，两个整数分别表示该项的系数coef 和次数expn，输入保证次数递增； 接下来m行，每行输入LB的每一项的信息，两个整数分别表示该项的系数coef 和次数  expn，输入保证次数递增。 输出格式：输出k行，k为LC的项数，每行输出 LC 的每一项的信息，两个整数分别表示 该项的系数 coef 和次数 expn，输出按次数递增。

n,m在0到1000000取值，coef在-1000到1000取值，expn在-1000000000到1000000000取值



```cpp

```

这题就是按照视频讲解里的来也过不了，而且视频里的代码很糟糕，无意义的命名、没有一点注释和使用全局变量并不初始化。

我写的又有几个测试用例超时过不了。





在n个城市之间原本要规划修建许多条下水管道，管理人员发现这些管道会形成一条回路，而下水道只要将城市联通即可，所以回路会加大施工的成本。所以希望你来帮忙找出多余的管道来进行优化。当然管道和管道之问是有区别的，比如用Sij来表示i到j的管道管理费用，Sij越小则表示该管道管理费用越低。能否去除一些管线，使得总管理成本最低。求出最低的管理成本（不存在自身与自身成为回路的管道）。

输入格式：第一行有两个数n和k表示城市数量和管道数量;
接下来k行，每行都有三个数i, j, c表示城市i和城市j之间的管道
成本为c。

输出格式：一个正整数，最低管理成本。

除了Prim算法，还可以使用Kruskal算法来解决这个问题。Kruskal算法是另一种常用的最小生成树算法。

Kruskal算法的基本思想是从成本最低的边开始选择，并逐步添加边到最小生成树中，直到最小生成树包含所有的城市或者所有的边都被考虑过。

下面是使用Kruskal算法解决这个问题的C++代码示例：

```cpp
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

struct Edge {
    int city1, city2, cost;
};

bool compareEdges(const Edge& e1, const Edge& e2) {
    return e1.cost < e2.cost;
}

int findParent(vector<int>& parent, int city) {
    if (parent[city] == -1)
        return city;
    return findParent(parent, parent[city]);
}

void unionSets(vector<int>& parent, int city1, int city2) {
    int parent1 = findParent(parent, city1);
    int parent2 = findParent(parent, city2);
    parent[parent1] = parent2;
}

int findMinCost(vector<Edge>& edges, int n) {
    sort(edges.begin(), edges.end(), compareEdges);

    vector<int> parent(n + 1, -1); // 记录城市的父节点，用于判断是否形成回路

    int minCostSum = 0; // 最低管理成本

    for (const auto& edge : edges) {
        int parent1 = findParent(parent, edge.city1);
        int parent2 = findParent(parent, edge.city2);

        if (parent1 != parent2) {
            minCostSum += edge.cost;
            unionSets(parent, parent1, parent2);
        }
    }

    return minCostSum;
}

int main() {
    int n, k;
    cin >> n >> k;

    vector<Edge> edges(k);

    for (int i = 0; i < k; ++i) {
        cin >> edges[i].city1 >> edges[i].city2 >> edges[i].cost;
    }

    int minCost = findMinCost(edges, n);
    cout << minCost << endl;

    return 0;
}
```

## 第二周

**给你n个数字，通过交换相邻的两个数字，实现升序，请求出最少交换的次数**

超时的算法

```py
def bubble_sort(arr):
    n = len(arr)
    swaps = 0
    for i in range(n):
        # 提前退出标志，如果本轮没有进行交换，则已经有序
        swapped = False
        for j in range(0, n-i-1):
            if arr[j] > arr[j+1]:
                arr[j], arr[j+1] = arr[j+1], arr[j]
                swaps += 1
                swapped = True
        if not swapped:
            break
    return swaps

def main():
    #code here
    n = int(input())
    input_string = input()
    numbers = [int(num) for num in input_string.split()]
    swaps = bubble_sort(numbers)
    print(swaps)
    pass


if __name__ == '__main__':
    main();
```



高效的算法需要利用到树状数组的相关概念，需要求逆序对之类的。

整体算法的思路是：将数组元素离散化（离散化的意思是，例如1 2 999 3的序列，从大到小依次标上号，这4个数分别为4 3 1 2），然后遍历这个新数组，查看每一个数之前有没有比它小的数，如果有，逆序对就增加1，最终逆序对的数量就是最少需要交换的数量。



```cpp
#include <iostream>

using namespace std;

const int N = 2e5 + 7;
int a[N];   // 离散化原数组
int b[N];   // 原数组
int c[N];   // 离散化管理数组
int n;
long long ans;

// 树状数组模版
int lowbit(int x) { return x & -x; }
void add(int i, int x) {
    for(; i <=n; i += lowbit(i))
        c[i] += x;
}
int sum(int i) {
    int ans = 0;
    for (; i > 0; i -= lowbit(i))
        ans += c[i];
    return ans;
}
// 离散化
bool cmp(const int x, const int y) {
    if (b[x] == b[y])
        return x > y;
    return b[x] > b[y];
}

int main() {
    cin >> n;
    // 离散化
    for (int i = 1; i <= n; i++) {
        cin >> b[i];
        a[i] = i;
    }
    sort(a + 1, a + n + 1, cmp);
    for (int i = 1; i <= n; ++i) {
        add(a[i], 1);
        ans += sum(a[i] - 1);
    }
    cout << ans << endl;
    return 0;
}
```



泡泡，输入n,m，分别表示泡泡数量和碰撞次数，然后输入碰撞两个泡泡的碰撞节点。每个泡泡是由链表环组成，泡泡之间碰撞规则是，碰撞节点会相连，然后碰撞节点a的右节点会和碰撞节点b的左节点相连。最后输出所有泡泡碰撞后的情况。



```cpp
using namespace std;

const int N = 1e6+7;
struct NODE {
    int l, r;
    bool flag;
}bubb[N];

int main() {
    int n, m;
    cin >> n >> m;
    for (int i = 1; i <= n; ++i) {
        bubb[i].l = bubb[i].r = i;  // init bubb, the i is val
    }
    while (m--) {
        int x, y;
        cin >> x >> y;
        // 碰撞
        int t1 = bubb[x].r;
        int t2 = bubb[y].l;
        bubb[x].r = y;
        bubb[y].l = x;
        bubb[t1].l = t2;
        bubb[t2].r = t1;
    }
    // 按照字典序求出泡泡环
    for (int i = 1; i <= n; i++) {
        if (!bubb[i].flag) {
            int temp = i;
            while (!bubb[temp].flag) {
                cout << temp << " ";
                bubb[temp].flag = true;
                temp = bubb[temp].r;
            }
            cout << endl;
        }
    }
    return 0;
}
```

我也是头一次知道双向链表左右指针是不用是指针的，直接用类似于数组元素的下标进行表示。
