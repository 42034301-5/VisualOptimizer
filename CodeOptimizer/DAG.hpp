#ifndef __DAG_H__
#define __DAG_H__

#include <sstream>
#include <assert.h>
#include "global.hpp"

/*
* ADD
* SUB
* MUL
* DIV
* MOD
* TAR
* FAR
* SET
* JGT
* JLT
* JEQ
*/

struct DAGNode
{
    std::vector<std::string> symList;
    int left = -1;
    int right = -1;
    int tri = -1;
    std::string value;
    bool isKilled = false;

    static std::shared_ptr<DAGNode> createNode()
    {
        auto newNode = std::make_shared<DAGNode>();
        return newNode;
    }

    void removeSymbol(const std::string& target)
    {
        for(auto it = symList.begin(); it != symList.end(); )
        {
            if(*it == target)
                it = symList.erase(it);
            else
                ++it;
        }
    }

    void addSymbol(const std::string& target)
    {
        if(contain(symList, target))
            return;
        symList.emplace_back(target);
    }

    bool isLeaf()
    {
        return (left == -1 && right == -1 && tri == -1);
    }

};

class DAG
{
private:
    std::vector<std::shared_ptr<DAGNode>> nodes;

    int findNode(std::shared_ptr<DAGNode> target) const
    {
        for(size_t i = 0; i < nodes.size(); ++i)
        {
            if(nodes[i] == target)
                return i;
        }
        return -1;
    }

    std::shared_ptr<DAGNode> findNodeBySymbol(const std::string& target) const
    {
        for(auto&& node : nodes)
        {
            if(node == nullptr)
                continue;
            for(auto&& symbol : node->symList)
            {
                if(symbol == target)
                    return node;
            }
        }
        return nullptr;
    }

    std::shared_ptr<DAGNode> findNodeByValue(const std::string& target, int l, int r, int t) const
    {
        for(auto&& node : nodes)
        {
            if(node == nullptr)
                continue;
            if(node->value == target && node->left == l && node->right == r && node->tri == t)
                return node;
        }
        return nullptr;
    }

    bool isLiteralNode(const std::string& symbol) const
    {
        auto n = findNodeBySymbol(symbol);
        if(!n)
            return false;
        if(n->left != -1 && n->right == -1 && n->tri == -1 && isLiteral(nodes[n->left]->value))
            return true;
        if(n->right != -1 && n->left == -1 && n->tri == -1 && isLiteral(nodes[n->right]->value))
            return true;
        return false;
    }

    int getLiteral(const std::string& symbol) const
        {
        if(isLiteral(symbol))
            return std::stoi(symbol);

        auto n = findNodeBySymbol(symbol);
        assert(n != nullptr);

        if(n->left != -1)
            return std::stoi(nodes[n->left]->value);
        else
            return -std::stoi(nodes[n->right]->value);

    }

    void removeSymbol(const std::string& target)
    {
        for(auto&& node : nodes)
        {
            if(node != nullptr)
                node->removeSymbol(target);
        }
    }

    void killNodesDependingOn(std::shared_ptr<DAGNode> n)
    {
        int index = findNode(n);
        for(auto&& node : nodes)
        {
            if(node->left == index)
                node->isKilled = true;
        }
    }

    //(op, a1, a2, -)
    std::vector<size_t> readQuad0(const QuadExp& E)
    {
        std::vector<size_t> result;
        std::shared_ptr<DAGNode> n1 = nullptr, n2 = nullptr;
        auto n = findNodeBySymbol(E.a1);
        if(n != nullptr && !n->isKilled)
        {
            removeSymbol(E.a1);
        }

        n2 = findNodeBySymbol(E.a2);
        if(n2 != nullptr && !n2->isKilled)   // a2作为内部变量（可能活跃）出现过
        {
            n2->addSymbol(E.a1);
        }
        else                // a2没有作为内部变量出现过，还可能作为外部变量（叶子）出现过
        {
            n2 = findNodeByValue(E.a2, -1, -1, -1);
            if(n2 == nullptr)   //不存在n2 创建
            {
                n2 = DAGNode::createNode();
                n2->left = -1, n2->right = -1, n2->tri = -1;
                n2->value = E.a2;
                nodes.emplace_back(n2);
                result.emplace_back(nodes.size() - 1);
                
            }

            int indexn2 = findNode(n2);
            n1 = findNodeByValue(E.op, indexn2, -1, -1);   

            if(n1 != nullptr && !n1->isKilled)   // 已经存在 b = CONST这样的赋值，则直接附上a1
            {
                n1->addSymbol(E.a1);
            }
            else                //若不存在，创建a1
            {
                n1 = DAGNode::createNode();
                n1->left = indexn2, n1->right = -1, n1->tri = -1;
                n1->value = E.op;
                n1->addSymbol(E.a1);
                nodes.emplace_back(n1);
                result.emplace_back(nodes.size() - 1);
            }
        }
        return result;
    }

    // (op, a1, -, a3)
    std::vector<size_t> readQuad1(const QuadExp& E)
    {
        std::vector<size_t> result;
        std::shared_ptr<DAGNode> n1 =nullptr, n3 = nullptr;
        auto n = findNodeBySymbol(E.a1);
        if(n != nullptr && !n->isKilled)
        {
            removeSymbol(E.a1);
        }

        n3 = findNodeBySymbol(E.a3);
        if(n3 != nullptr)   // a3作为内部变量（可能活跃）出现过
        {
            n1 = findNodeByValue(E.op, -1, findNode(n3), -1);
            if(n1 != nullptr && !n1->isKilled)
                n1->addSymbol(E.a1);
            else
            {
                n1 = DAGNode::createNode();
                n1->left = -1, n1->right = findNode(n3), n1->tri = -1;
                n1->value = E.op;
                n1->addSymbol(E.a1);
                nodes.emplace_back(n1);
                result.emplace_back(nodes.size() - 1);
            }
        }
        else    // a3没有作为内部变量出现过，还可能作为外部变量（叶子）出现过
        {
            n3 = findNodeByValue(E.a3, -1, -1, -1);
            if(n3 == nullptr)
            {
                n3 = DAGNode::createNode();
                n3->left = -1, n3->right = -1, n3->tri = -1;
                n3->value = E.a3;
                nodes.emplace_back(n3);
                result.emplace_back(nodes.size() - 1);
            }

            int indexn3 = findNode(n3);
            n1 = findNodeByValue(E.op, -1, indexn3, -1);
            if(n1 != nullptr && !n1->isKilled)   // 已经存在 b = CONST这样的赋值，则直接附上a1
            {
                n1->addSymbol(E.a1);
            }
            else                //若不存在，创建a1
            {
                n1 = DAGNode::createNode();
                n1->left = -1, n1->right = indexn3, n1->tri = -1;
                n1->value = E.op;
                n1->addSymbol(E.a1);
                nodes.emplace_back(n1);
                result.emplace_back(nodes.size() - 1);
            }
        }      
        return result; 
    }

    //(op, a1, a2, a3)
    std::vector<size_t> readQuad2(const QuadExp& E)
    {
        std::vector<size_t> result;
        std::shared_ptr<DAGNode> n1 = nullptr, n2 = nullptr, n3 = nullptr;
        bool n2Literal = false, n3Literal = false;
        int indexn2 = -1, indexn3 = -1;

        n2 = findNodeByValue(E.a2, -1, -1, -1);
        n3 = findNodeByValue(E.a3, -1, -1, -1);
        if(isLiteral(E.a2) || isLiteralNode(E.a2))
            n2Literal = true;
        if(isLiteral(E.a3) || isLiteralNode(E.a3))
            n3Literal = true;
        //n2和n3均为值是常量的叶子结点，则直接计算n1
        if(n2Literal && n3Literal)
        {
            int val = 0, val2, val3;
            val2 = getLiteral(E.a2);
            val3 = getLiteral(E.a3);
            
            if(E.op == "ADD")
                val = val2 + val3;
            else if(E.op == "SUB")
                val = val2 - val3;
            else if(E.op == "MUL")
                val = val2 * val3;
            else if(E.op == "DIV")
                val = val2 / val3;
            else if(E.op == "MOD")
                val = val2 % val3;


            //已经存在值为val2 op val3的常量叶子，则a1 = val2 op val3
            //否则创建一个val2 op val3的常量叶子

            std::shared_ptr<DAGNode> n = findNodeByValue(std::to_string(val), -1, -1, -1);
            if(n == nullptr)
            {
                n = DAGNode::createNode();
                n->value = std::to_string(val);
                n->left = -1, n->right = -1, n->tri = -1;
                nodes.emplace_back(n);
                result.emplace_back(nodes.size() - 1);
            }
            
            n1 = findNodeByValue("SET", findNode(n), -1, -1);
            
            
            if(n1 != nullptr && !n1->isKilled)
            {
                removeSymbol(E.a1);
                n1->addSymbol(E.a1);
            }
            else
            {
                n1 = DAGNode::createNode();
                n1->left = findNode(n), n->right = -1, n->tri = -1;
                n1->value = "SET";
                removeSymbol(E.a1);
                n1->addSymbol(E.a1);
                nodes.emplace_back(n1);
                result.emplace_back(nodes.size() - 1);
            }
        }
        //n2和n3至少一个不是常量叶子（内部变量或外部变量）
        else
        {
            if(n2 == nullptr)
            {
                n2 = findNodeBySymbol(E.a2);
                if(n2 == nullptr)   // a2不存在, 说明a2是一个外部变量, 创建一个叶子表示它
                {
                    n2 = DAGNode::createNode();
                    n2->left = -1, n2->right = -1, n2->tri = -1;
                    n2->value = E.a2;
                    nodes.emplace_back(n2);
                    result.emplace_back(nodes.size() - 1);
                }
            }

            if(n3 == nullptr)
            {
                n3 = findNodeBySymbol(E.a3);
                if(n3 == nullptr)   // a3不存在, 说明a3是一个外部变量, 创建一个叶子表示它
                {
                    n3 = DAGNode::createNode();
                    n3->left = -1, n3->right = -1, n3->tri = -1;
                    n3->value = E.a3;
                    nodes.emplace_back(n3);
                    result.emplace_back(nodes.size() - 1);
                }
            }
            
            indexn2 = findNode(n2);
            indexn3 = findNode(n3);
            n1 = findNodeByValue(E.op, indexn2, indexn3, -1);
            if(n1 != nullptr && !n1->isKilled)
            {
                removeSymbol(E.a1);
                n1->addSymbol(E.a1);
            }
            else
            {
                n1 = DAGNode::createNode();
                n1->left = indexn2, n1->right = indexn3, n1->tri = -1;
                n1->value = E.op;
                removeSymbol(E.a1);
                n1->addSymbol(E.a1);
                nodes.emplace_back(n1);
                result.emplace_back(nodes.size() - 1);
            }
        }
        return result;
    }

    //(TAR, a1, a2, a3)  a1[a2] = a3
    std::vector<size_t> readQuad3(const QuadExp& E)
    {
        std::vector<size_t> result;
        std::shared_ptr<DAGNode> n1 = nullptr, n2 = nullptr, n3 = nullptr, n = nullptr;
        n1 = findNodeByValue(E.a1, -1, -1, -1);
        n2 = findNodeByValue(E.a2, -1, -1, -1);
        n3 = findNodeByValue(E.a3, -1, -1, -1);

        if(n1 == nullptr)
        {
            n1 = findNodeBySymbol(E.a1);
            if(n1 == nullptr)
            {
                n1 = DAGNode::createNode();
                n1->left = -1, n1->right = -1, n1->tri = -1;
                n1->value = E.a1;
                nodes.emplace_back(n1);
                result.emplace_back(nodes.size() - 1);
            }
        }

        if(n2 == nullptr)
        {
            n2 = findNodeBySymbol(E.a2);
            if(n2 == nullptr)
            {
                n2 = DAGNode::createNode();
                n2->left = -1, n2->right = -1, n2->tri = -1;
                n2->value = E.a2;
                nodes.emplace_back(n2);
                result.emplace_back(nodes.size() - 1);
            }
        }

        if(n3 == nullptr)
        {
            n3 = findNodeBySymbol(E.a3);
            if(n3 == nullptr)
            {
                n3 = DAGNode::createNode();
                n3->left = -1, n3->right = -1, n3->tri = -1;
                n3->value = E.a3;
                nodes.emplace_back(n3);
                result.emplace_back(nodes.size() - 1);
            }
        }

        n = DAGNode::createNode();
        n->left = findNode(n1), n->right = findNode(n2), n->tri = findNode(n3);
        n->value = E.op;
        nodes.emplace_back(n);
        result.emplace_back(nodes.size() - 1);
        killNodesDependingOn(n1);

        return result;
    }

    bool isRoot(std::shared_ptr<DAGNode> n) const
    {
        if(n == nullptr)
            return false;
        for(auto&& node : nodes)
        {
            if(node == nullptr)
                continue;
            if(
                (node->left != -1 && nodes[node->left]  == n)   ||
                (node->right != -1 && nodes[node->right]  == n) ||
                (node->tri != -1 && nodes[node->tri]  == n)
            )
            {
                return false;
            }
        }
        return true;
    }

    bool isActiveNode(std::shared_ptr<DAGNode> n, const std::vector<std::string>& outActive) const
    {
        if(n == nullptr)
            return false;

        if(n->isLeaf())
            return false;

        if(n->value == "TAR")
            return true;

        if(n->value[0] == 'J' && n->value != "JMP")
            return true;

        for(auto&& sym : n->symList)
        {
            if(contain(outActive, sym))
                return true;
        }

        return false;
    }

    std::vector<QuadExp> genCode(std::shared_ptr<DAGNode> n) const
    {
        std::vector<QuadExp> result;
        if(n == nullptr)
            return result;

        if(n->value == "TAR")
        {
            QuadExp e;
            e.op = "TAR";
            e.a1 = (nodes[n->left]->isLeaf() ? nodes[n->left]->value : nodes[n->left]->symList[0]);
            e.a2 = (nodes[n->right]->isLeaf() ? nodes[n->right]->value : nodes[n->right]->symList[0]);
            e.a3 = (nodes[n->tri]->isLeaf() ? nodes[n->tri]->value : nodes[n->tri]->symList[0]);
            result.emplace_back(e);
            return result;
        }

        QuadExp e;
        e.op = n->value;
        e.a1 = n->symList[0];
        if(n->left == -1)
            e.a2 = "-";
        else
            e.a2 = (nodes[n->left]->isLeaf() ? nodes[n->left]->value : nodes[n->left]->symList[0]);
        
        if(n->right == -1)
            e.a3 = "-";
        else
            e.a3 = (nodes[n->right]->isLeaf() ? nodes[n->right]->value : nodes[n->right]->symList[0]);

        result.emplace_back(e);

        for(auto it = n->symList.begin(); it != n->symList.end(); ++it)
        {
            if(it != n->symList.begin())
            {
                e.op = "SET";
                e.a1 = *it;
                e.a2 = n->symList[0];
                e.a3 = "-";
                result.emplace_back(e);
            }
        }

        return result;
    }

public:

    std::vector<size_t> readQuad(const QuadExp& E)
    {
        switch(const int T = E.type())
        {
            case 0:
                return readQuad0(E);
            case 1:
                return readQuad1(E);
            case 2:
                return readQuad2(E);
            case 3:
                return readQuad3(E);
            default:
                return std::vector<size_t>{};
        }
    }

    std::string print_DAG() const
    {
        std::ostringstream os;
        for(size_t i = 0; i < nodes.size(); ++i)
        {
            if(nodes[i] == nullptr)
                continue;
            os << "Node: n" << i << "\t\t";
            os << "Mark: " << nodes[i]->value << "\t\t";
            os << "Leaf:" << (nodes[i]->isLeaf() ? "Y" : "N") << "\t\t";
            os << "Symbols:";
            for(auto&& sym : nodes[i]->symList)
                os << sym << " ";
            os << "\t\t"; 
            os << "\n";
        }
        os << "\n";
        return os.str();
    }

    std::vector<QuadExp> genOptimizedCode(std::vector<std::string> outActive)
    {
        std::vector<QuadExp> result;
        bool changed = true;

        //删除不活跃的根结点
        while(changed)
        {
            changed = false;

            for(auto it = nodes.begin(); it != nodes.end(); ++it)
            {
                if(isRoot(*it) && !isActiveNode(*it, outActive))
                {
                    *it = nullptr;
                    changed = true;
                }
            }

        }

        //清除不活跃的标识符，为标识符为空的结点新增一个 Si 标识符
        size_t symSerial = 0;

        for(auto&& node : nodes)
        {
            if(node == nullptr)
                continue;
            if(node->value == "TAR")
                continue;

            for(auto it = node->symList.begin(); it < node->symList.end();)
            {
                if(!contain(outActive, *it))
                    it = node->symList.erase(it);
                else
                    ++it;
            }

            if(!node->isLeaf() && node->symList.empty())
            {
                node->symList.emplace_back(std::string{"S" + std::to_string(symSerial++)});
            }
        }

        //DFS自下而上生成代码
        std::vector<std::shared_ptr<DAGNode>> allRoots = [=]()->std::vector<std::shared_ptr<DAGNode>>{
            std::vector<std::shared_ptr<DAGNode>> roots;
            for(auto&& node : nodes)
            {
                if(node == nullptr)
                    continue;
                if(isRoot(node))
                    roots.emplace_back(node);
            }
            return roots;
        }();
        std::vector<bool> visited(nodes.size(), false);
        for(size_t i = 0; i < visited.size(); ++i)
        {
            if(nodes[i] == nullptr)
                continue;
            if(nodes[i]->isLeaf())
                visited[i] = true;
        }

        for(auto&& root : allRoots)
        {
            std::stack<std::shared_ptr<DAGNode>> stk;
            stk.push(root);

            while(!stk.empty())
            {
                std::shared_ptr<DAGNode> cur = stk.top();
                stk.pop();
                if(visited[findNode(cur)])
                    continue;
                if(
                    [=]() ->bool {
                        if(cur->left != -1 && !visited[cur->left])
                            return false;
                        if(cur->right != -1 && !visited[cur->right])
                            return false;
                        if(cur->tri != -1 && !visited[cur->right])
                            return false;
                        return true;
                    }()
                )
                {
                    std::vector<QuadExp> nodeCode = genCode(cur);
                    for(auto&& c : nodeCode)
                    {
                        result.emplace_back(c);
                    }
                    visited[findNode(cur)] = true;
                }
                else
                {
                    stk.push(cur);
                    if(cur->left != -1 && !visited[cur->left])
                        stk.push(nodes[cur->left]);

                    if(cur->right != -1 && !visited[cur->right])
                        stk.push(nodes[cur->right]);

                    if(cur->tri != -1 && !visited[cur->right])
                        stk.push(nodes[cur->right]);

                }
            }

        }

        return result;
    }

    void release()
    {
        for(auto&&node : nodes)
        {
            if(node != nullptr)
            {
                node = nullptr;
            }
        }
        nodes.clear();
    }

    friend class IntermediateCode;

};


#endif