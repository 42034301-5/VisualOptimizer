#ifndef __INTERMEDIATE_HPP__
#define __INTERMEDIATE_HPP__

#include <fstream>
#include <sstream>
#include <queue>
#include "global.hpp"
#include "DAG.hpp"

struct BasicBlock
{
    DAG dag;
    int begin = -1;
    int end   = -1;
    int nextCode = -1;  //顺序执行的下一语句
    int jumpCode = -1;  //跳转执行的下一语句

    std::vector<size_t> previousList;
    std::vector<size_t> dominatorList;
    std::vector<std::string> def;
    std::vector<std::string> use;
    std::vector<std::string> inActive;
    std::vector<std::string> ouActive;
    
    void clear()
    {
        dag.release();
        begin = -1, end = -1;
        nextCode = -1, jumpCode = -1;
    }

    bool operator==(const BasicBlock& rhs)
    {
        return begin == rhs.begin && end == rhs.end;
    }

};

class IntermediateCode
{
private:
    std::vector<QuadExp> interCode;
    std::vector<BasicBlock> blocks;
    std::vector<std::vector<size_t>> loops;
    std::vector<ForeEndEdge> mforeEndEdges;
    static IntermediateCode* IC;

    int findLabel(const std::string& target)
    {
        size_t size = interCode.size();
        for(size_t i = 0; i < size; ++i)
        {
            if(interCode[i].op == "LABEL" && interCode[i].a1 == target)
                return i;
        }
        return -1;
    }
    
    void genDominanceSets()
    {
        bool changed = true;
        size_t size = blocks.size();

        //  假设共有x个结点, 对于每一个结点Ni，初始化 D(Ni) = {N0, N1, ..., N x-1}，除了D(n0) = {0}
        blocks[0].dominatorList.emplace_back(0);
        for(size_t i = 1; i < size; ++i)
        {
            for(size_t j = 0; j < size; ++j)
            {
                blocks[i].dominatorList.push_back(j);
            }
        }

        while(changed)
        {
            changed = false;

            for(size_t i = 0; i < size; ++i)
            {
                auto&& dmls = blocks[i].dominatorList;
                size_t lastSize = blocks[i].dominatorList.size();

                for(auto&& preNode : blocks[i].previousList)
                {
                    dmls = intersection(dmls, blocks[preNode].dominatorList);
                }

                std::vector<size_t> self = {i};
                dmls = setunion(dmls, self);

                if(dmls.size() != lastSize)
                    changed = true;

            }
        }

        
    }

    void genDefAndUseSets()
    {
        size_t size = blocks.size();

        for(size_t i = 0; i < size; ++i)
        {
            auto&& use = blocks[i].use;
            auto&& def = blocks[i].def;
            use.clear();
            def.clear();

            for(size_t j = blocks[i].begin; j <= blocks[i].end; ++j)
            {
                QuadExp E = interCode[j];   //E是基本块 i 中标号为 j 的语句

                if(E.a2 != "-"&& !isLiteral(E.a2) && !contain(def, E.a2))
                {
                    std::vector<std::string> tmp = {E.a2};
                    use = setunion(use, tmp);
                }

                if(E.a3 != "-"&& !isLiteral(E.a3) && !contain(def, E.a3))
                {
                    std::vector<std::string> tmp = {E.a3};
                    use = setunion(use, tmp);
                }

                if(!contain(use, E.a1))
                {
                    std::vector<std::string> tmp = {E.a1};
                    def = setunion(def, tmp);
                }

            }

        }
    }

    void findActive()
    {
        size_t size = blocks.size();

        for(size_t i = 0; i < size; ++i)
        {
            blocks[i].inActive.clear();
        }

        bool changed = true;
        
        while(changed)
        {
            changed = false;

            for(size_t i = 0; i < size; ++i)
            {
                blocks[i].ouActive.clear();

                if(blocks[i].nextCode != -1)
                {
                    size_t nextBlock = findBlockByCode(blocks[i].nextCode);
                    blocks[i].ouActive = setunion(blocks[i].ouActive, blocks[nextBlock].inActive);
                }

                if(blocks[i].jumpCode != -1)
                {
                    size_t jumpBlock = findBlockByCode(blocks[i].jumpCode);
                    blocks[i].ouActive = setunion(blocks[i].ouActive, blocks[jumpBlock].inActive);
                }

                size_t lastSize = blocks[i].inActive.size();

                blocks[i].inActive = setunion(blocks[i].use, difference(blocks[i].ouActive, blocks[i].def));

                if(lastSize != blocks[i].inActive.size())
                    changed = true;
                
            }
        }

        for(size_t i = 0; i < size; ++i)
        {
            if(interCode[blocks[i].end].op[0] == 'J')
            {
                std::vector<std::string> label = {interCode[blocks[i].end].a1};
                blocks[i].ouActive = setunion(blocks[i].ouActive, label);
            }
        }

    }

    int findBlockByCode(size_t begin_)
    {
        size_t size = blocks.size();
        for(size_t i = 0; i < size; ++i)
        {
            if(blocks[i].begin <= begin_ && blocks[i].end >= begin_)
                return (int)i;
        }
        return -1;
    }

    void connectBlocks()
    {
        size_t size = blocks.size();
        for(size_t i = 0; i < size; ++i)
        {
            int index = findBlockByCode(blocks[i].nextCode);   //找到blocks[i]的nextCode所在的基本块，blocks[i]为该基本块的前驱
            if(index != -1)
            {
                blocks[index].previousList.emplace_back(i);
            }

            index = findBlockByCode(blocks[i].jumpCode);
            if(index != -1)
            {
                blocks[index].previousList.emplace_back(i);
            }
        }
    }

    void findLoop()
    {
        std::vector<std::pair<size_t, size_t>> backEdges;   //所有的回边
        size_t size = blocks.size();

        //查找回边
        for(size_t i = 0; i < size; ++i)
        {
            for(auto&& j : blocks[i].dominatorList)
            {
                if(contain(blocks[j].previousList, i))
                {
                    backEdges.emplace_back(std::pair{i, j});    //如果 j 是 i 的必经结点，而 i 是 j 的一个直接前驱，则<i, j>是回边
                }
            }
        }

        for(size_t i = 0; i < backEdges.size(); ++i)    
        {
            std::vector<size_t> L; 
            size_t n = backEdges[i].first, d = backEdges[i].second; //回边 n->d 组成循环 L

            L.emplace_back(n);
            L.emplace_back(d);
            
            if(n == d)
            {
                loops.emplace_back(L);
                continue;
            }

            std::queue<size_t> blockQue;     //BFS向循环L中添加基本块
            for(auto&& parent : blocks[n].previousList)
            {
                blockQue.push(parent);
            }
            while(!blockQue.empty())
            {
                size_t cur = blockQue.front();
                blockQue.pop();

                if(cur == d)
                    continue;
                    
                if(!contain(L, cur))
                    L.insert(L.begin() + 1, cur);

                for(auto&& parent : blocks[cur].previousList)
                {
                    if(!contain(L, parent) && parent != d)
                        blockQue.push(parent);
                }
            }
            loops.emplace_back(L);

            
        }
    }

    bool isIndexJumperTarget(size_t indexOfExp)
    {
        if(indexOfExp == blocks.size() - 1)
            return false;
            
        for(size_t i = 0; i < interCode.size(); ++i)
        {
            auto&& E = interCode[i];
            if(
                E.op[0] == 'J' &&
                std::stoi(std::string(E.a1.begin() + 1, E.a1.end() - 1)) == indexOfExp + 1
            )
            {
                return true;
            }
            
        }
        return false;
    }

    //优化后的语句标号会发生改变，需要把JMP语句a1的目标语句序号改为目标基本块序号
    void adjustJumpArg()
    {
        for(size_t i = 0; i < interCode.size(); ++i)
        {
            if(interCode[i].op[0] == 'J' && interCode[i].a1[0] != '(')
            {
                interCode[i].a1 = "B" + std::to_string(findBlockByCode(findLabel(interCode[i].a1)));
            }

            if(interCode[i].op[0] == 'J' && interCode[i].a1[0] == '(')
            {
                interCode[i].a1 = "B" + std::to_string(findBlockByCode(
                    std::stoi(std::string(interCode[i].a1.begin() + 1, interCode[i].a1.end() - 1)) - 1
                ));
            }
        }
    }

    IntermediateCode() {}

public:

    ~IntermediateCode()
    {
        if(IC != nullptr)
        {
            for(auto&& blk : IC->blocks)
            {
                blk.dag.release();
            }
            delete IC;
            IC = nullptr;
        }
    }

    void read_code(const QuadExp& E)
    {
        interCode.emplace_back(E);
    }

    void read_code(const std::string& filename)
    {
        std::ifstream in(filename);

        while(in.peek() != EOF)
        {
            QuadExp e;
            in >> e;
            read_code(e);
        }
    }

    void split()
    {
        BasicBlock blk;
        size_t size = interCode.size();

        for(size_t i = 0; i < size; ++i)
        {
            QuadExp E = interCode[i];

            bool beginFlag = (i > 0 && interCode[i - 1].op[0] == 'J');    
            bool endFlag = ( (i < size - 1 && (interCode[i + 1].op == "LABEL" || E.op[0] == 'J'))
                            || i == size - 1);

            if(i == 0 || E.op == "LABEL" || beginFlag || isIndexJumperTarget(i))
            {
                blk.begin = i;
            }

            // if(E.op != "LABEL" && E.op != "JMP")
            // {
            //     blk.dag.readQuad(E);
            // }

            if(endFlag || isIndexJumperTarget(i + 1))
            {
                blk.end = i;
                if(E.op != "JMP")
                {
                    blk.nextCode = (i == size - 1 ? -1 : i + 1);
                }

                if(E.op[0] == 'J')
                {
                    blk.jumpCode = (E.a1[0] == '(' ? std::stoi(std::string(E.a1.begin() + 1, E.a1.end() - 1)) - 1 : findLabel(E.a1));
                }
                
                blocks.emplace_back(blk);
                blk = BasicBlock{};
            }
        }

        connectBlocks();
        adjustJumpArg();
        genDominanceSets();
        genDefAndUseSets();
        findLoop();
        findActive();

        for(auto&& block : blocks)
        {
            for(size_t i = block.begin; i <= block.end; ++i)
                //if(interCode[i].op != "JMP" && interCode[i].op != "LABEL")
                block.dag.readQuad(interCode[i]);
        }
    }  

    std::vector<std::vector<QuadExp>> blockOptimize()
    {
        std::vector<std::vector<QuadExp>> result;
        for(size_t i = 0; i < blocks.size(); ++i)
        {
            auto c = blocks[i].dag.genOptimizedCode(blocks[i].ouActive);
            result.emplace_back(c);
        }

        return result;
    }

    void print_dominance()
    {
        size_t size = blocks.size();
        for(size_t i = 0; i < size; ++i)
        {
            std::cout << "Block" << i << "'s D = {";
            for(auto&& dmt : blocks[i].dominatorList)
            {
                std::cout << dmt << ' ';
            }
            std::cout << "}";
            std::cout << std::endl;
        }
    }

    std::string printFlowGraph()
    {
        std::ostringstream os;
        size_t size = blocks.size();

        for(size_t i = 0; i < size; ++i)
        {
            os << "Block" << i + 1 << " :" << std::endl;

            for(size_t j = blocks[i].begin; j <= blocks[i].end; ++j)
                os << j << " " << interCode[j];
            
            os << "Next block:" << findBlockByCode(blocks[i].nextCode) << "\t";
            os << "Jump block:" << findBlockByCode(blocks[i].jumpCode) << "\t";
            os << std::endl << std::endl;
        }

        return os.str();
    }

    static std::tuple<std::vector<ForeEndNode>, std::vector<ForeEndEdge>>
    initWithCode(const std::string& code)
    {
        std::istringstream is(code);

        while(is.peek() != EOF)
        {
            QuadExp e;
            is >> e;
            if(!e.op.empty())
                IC->read_code(e);
        }

        IC->split();

        std::vector<ForeEndNode> foreNodes;
        std::vector<ForeEndEdge> foreEdges;

        for(size_t i = 0; i < IC->blocks.size(); ++i)
        {
            // 添加前端结点
            ForeEndNode n;
            n.id = i;
            
            std::ostringstream os;
            for(size_t j = IC->blocks[i].begin; j <= IC->blocks[i].end; ++j)
            {
                os << IC->interCode[j];
            }

            n.label = os.str();
            foreNodes.emplace_back(n);

            // 添加前端边
            ForeEndEdge e;
            if(IC->blocks[i].nextCode != -1)
            {
                e.from = i, e.to = IC->findBlockByCode(IC->blocks[i].nextCode);
                e.id = IC->mforeEndEdges.size();
                foreEdges.emplace_back(e);
                IC->mforeEndEdges.emplace_back(e);
                
            }
            if(IC->blocks[i].jumpCode != -1)
            {
                e.from = i, e.to = IC->findBlockByCode(IC->blocks[i].jumpCode);
                e.id = IC->mforeEndEdges.size();
                foreEdges.emplace_back(e);
                IC->mforeEndEdges.emplace_back(e);
            }

        }

        return std::make_tuple(foreNodes, foreEdges);
    }

    static IntermediateCode* getInstance()
    {
        return IC;
    }
};

IntermediateCode* IntermediateCode::IC = new IntermediateCode{};
#endif