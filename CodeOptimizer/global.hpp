#ifndef __GLOBAL_HPP__
#define __GLOBAL_HPP__

#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <set>
#include <stack>
#include <tuple>
#include <memory>



struct QuadExp
{
    std::string op, a1, a2, a3;

    int type()  const
    {
        if(op == "SET" || op == "ADDR")
            return 0;
        if(op == "TAR")
            return 3;
        if(a2 == "-" && a3 != "-")
            return 1;
        return 2;
    }

    std::string toString()
    {
        return (op + "\t" + a1 + "\t" + a2 + "\t" + a3 + "\t");
    }
};

std::istream& operator>>(std::istream& in, QuadExp& E)
{
    in >> E.op >> E.a1 >> E.a2 >> E.a3;
    return in;
};
std::ostream& operator<<(std::ostream& out, QuadExp& E)
{
    out << E.op << "\t" << E.a1 << "\t" << E.a2 << "\t" << E.a3 << " \n";
    return out;
};

template<typename T, typename F>
bool contain(const T& ls, const F& elem)
{
    for(auto&& e : ls)
    {
        if(e == elem)
            return true;
    }
    return false;
}

bool isLiteral(const std::string& arg)
{
    for(auto&& i : arg)
    {
        if(i > '9' || i < '0')
            return false;
    }
    return true;
}

template<typename T>
T intersection(const T& A, const T& B)
{
    T result;
    std::set_intersection(
        A.begin(), A.end(),
        B.begin(), B.end(),
        std::back_inserter(result)
    );

    return result;
}

template<typename T>
T setunion(const T& A, const T& B)
{
    T result;
    std::set_union(
        A.begin(), A.end(),
        B.begin(), B.end(),
        std::back_inserter(result)
    );

    return result;
}

template<typename T>
T difference(const T& A, const T& B)
{
    T result;
    std::set_difference(
        A.begin(), A.end(),
        B.begin(), B.end(),
        std::back_inserter(result)
    );

    return result;
}

template<typename T>
bool startWith(const T& A, const T& B)
{
    if(B.size() > A.size())
        return false;

    auto it1 = A.begin();
    auto it2 = B.begin();
    while(it2 != B.end())
    {
        if(*it1 != *it2)
            return false;
        ++it1;
        ++it2;
    }
    return true;
}

enum
{
    NORMAL  = 0,
    NEW     = 1,
    KILLED  = 2,
    LEAF    = 3,
    ACTIVE  = 4
};

struct ForeEndNode
{
    size_t id{};
    std::string label;
    size_t group = NORMAL;

};

struct ForeEndEdge
{
    size_t id{};
    size_t from{}, to{};
    size_t group = NORMAL;
};

#endif