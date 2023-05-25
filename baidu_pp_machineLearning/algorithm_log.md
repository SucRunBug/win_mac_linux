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

