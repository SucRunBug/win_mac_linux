此文件记录我的每周刷题记录。

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
