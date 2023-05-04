#include <iostream>
#include <string>
#include <queue>
#include <stack>

using namespace std;

struct TreeNode
{
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(): val(0), left(nullptr), right(nullptr)
    {}
    TreeNode(int x): val(x), left(nullptr), right(nullptr)
    {}
    TreeNode(int x, TreeNode *l, TreeNode *r): val(x), left(l), right(r)
    {}
};


string tree2str(TreeNode* root) 
{

}

// 二叉树后序遍历——迭代
vector<int> postorderTraversal(TreeNode* root) {
    vector<int> res;
    if (!root)
    {
        return res;
    }
    stack<TreeNode*> st;
    st.push(root);
    while(!st.empty())
    {
        TreeNode* node = st.top();
        st.pop();
        if (node->left)
        {
            st.push(node->left);
        }
        if (node->right)
        {
            st.push(node->right);
        }
        res.insert(res.begin(), node->val);
        
    }
    return res;
}


// 二叉树中序遍历——迭代
vector<int> inorderTraversal(TreeNode* root)
{
    vector<int> res;
    if (!root)
    {
        return res;
    }
    stack<TreeNode*> st;
    TreeNode* cur = root;
    
    while (cur || !st.empty())
    {
        while (cur)
        {
            st.push(cur);
            cur = cur->left;
        }
        cur = st.top();
        res.push_back(cur->val);
        st.pop();
        cur = cur->right;
    }
    return res;
}

// 二叉树前序遍历——迭代
vector<int> preorderTraversal(TreeNode* root) 
{
    vector<int> res;    // init vector and not do anything might be null?
    // root is null
    if (!root)
    {
        return res;
    }
    // root is not null
    stack<TreeNode*> st;
    st.push(root);
    while(!st.empty())
    {
        TreeNode* node = st.top();  // for temp store 
        // root first, right then left
        res.push_back(node->val);
        st.pop();
        if (node->right)
        {
            st.push(node->right);
        }
        if (node->left)
        {
            st.push(node->left);
        }
    }
    return res;
}

// 二叉树前序遍历——迭代 bro Hang's method 
vector<int> preorderTraversal2(TreeNode* root) {
    stack<TreeNode*> st;
    TreeNode* cur = root;
    vector<int> res;

    // cur and st are both null, loop stop
    while (cur || !st.empty())
    {
        // 先将所有左子树入栈
        while(cur)
        {
            res.push_back(cur->val);
            st.push(cur);
            cur = cur->left;
        }
        // 最后入栈的叶子节点先处理
        TreeNode* top = st.top();
        st.pop();
        // 循环迭代子问题
        cur = top->right;
    }
    return res;
}

// 对称二叉树——递归
bool isSymmetric(TreeNode* root) {
    // root is null
    if (!root)
    {
        return true;
    }
    return isSymmetric(root->left, root->right);
}

bool isSymmetric(TreeNode* left, TreeNode* right)
{
    // left & right both null
    if (!left & !right)
    {
        return true;
    }
    // left is null or right is null
    if (!left || !right)
    {
        return false;
    }
    // left val != right val
    if (left->val != right->val)
    {
        return false;
    }
    return isSymmetric(left->left, right->right) && isSymmetric(left->right, right->left);
}

// 对称二叉树——递归
bool isSymmetric2(TreeNode* root)
{
    if (!root)
    {
        return true;
    }
    queue<TreeNode*> q;
    q.push(root->left);
    q.push(root->right);
    while(!q.empty())
    {
        TreeNode* node1 = q.front();
        q.pop();
        TreeNode* node2 = q.front();
        q.pop();
        if (!node1 && !node2)
        {
            continue;
        }
        if (!node1 || !node2)
        {
            return false;
        }
        if (node1->val != node2->val)
        {
            return false;
        }
        q.push(node1->left);
        q.push(node2->right);
        q.push(node1->right);
        q.push(node2->left);
    }
}

int main()
{

    return 0;
}