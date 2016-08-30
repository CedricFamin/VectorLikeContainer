#include <stdlib.h>

#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <unordered_map>

#include "VectorLikeContainer.h"

#include <intrin.h>
#include <map>
#include <functional>

class Benchmark
{
public:
    Benchmark(std::string const & name) : _name(name)
    {
        begin = __rdtsc();
    }

    ~Benchmark()
    {
        Report();
    }

    void Report()
    {
        uint64_t cycles = __rdtsc() - begin;
        std::cout << "Result (" << _name << ") : " << cycles << " cycles" << std::endl;
        begin = __rdtsc();
    }
private:
    std::string _name;
    uint64_t begin;
};

class TestConstruct
{
public:
    TestConstruct(int i)
    {
        std::cout << "class constructor" << std::endl;
    }

    TestConstruct(TestConstruct const & cpy)
    {
        std::cout << "Cpy constructor" << std::endl;
    }

    TestConstruct(TestConstruct&& rvalue)
    {
        std::cout << "Move constructor" << std::endl;
    }
};


class Test
{
    static int nextId;
public:
    Test()
        : _id(++nextId)
    {
        std::cout << "construct #" << _id << std::endl;
    }
    ~Test()
    {
        std::cout << "Destroy #" << _id << std::endl;
    }
private:
    int _id;
};

int Test::nextId = 0;
static const int N = 100;

void TestStackLocalVector()
{
    CContainers::VectorStackLocal<int, N> intVec;
    //b.Report();
    for (int i = 0; i < N; ++i) { intVec.emplace_back(i); }
    //b.Report();
    while (!intVec.empty()) { intVec.pop_back(); }
}

void TestMaxSizeAllocator()
{
    CContainers::MaxSizedVector<int, N> intVec;
    //b.Report();
    for (int i = 0; i < N; ++i) { intVec.emplace_back(i); }
    //b.Report();
    while (!intVec.empty()) { intVec.pop_back(); }
}

void TestStdVectorReserve()
{
    std::vector<int> intVec;
    intVec.reserve(N);
    //b.Report();
    for (int i = 0; i < N; ++i) { intVec.push_back(i); }
    //b.Report();
    while (!intVec.empty()) { intVec.pop_back(); }
}

void TestStdVector()
{
    std::vector<int> intVec;
    //b.Report();
    for (int i = 0; i < N; ++i) { intVec.push_back(i); }
    //b.Report();
    while (!intVec.empty()) { intVec.pop_back(); }
}

void TestConstructor()
{
    CContainers::VectorStackLocal<TestConstruct, 10> testConstructList;
    testConstructList.emplace_back(5);
    std::cout << "=====================================" << std::endl;
    testConstructList.push_back(TestConstruct(1));
    std::cout << "=====================================" << std::endl;
    testConstructList.push_back(testConstructList.front());
    std::cout << "=====================================" << std::endl;
}

void TestIteration()
{
    CContainers::VectorStackLocal<char, 10> intVec;
    CContainers::VectorStackLocal<char, 10> intVec2;
    for (int i = 0; i < 9; ++i)
    {
        intVec.push_back(i);
    }

    intVec2.assign(intVec.begin(), intVec.end());


    for (char& i : intVec2)
    {
        std::cout << static_cast<int>(i) << std::endl;
    }
}

struct StringNode
{
    typedef CContainers::MaxSizedVector<StringNode, (unsigned char)-1> Tree;
    std::string str;
    Tree subTree;

    void InitIFN()
    {
        if (subTree.size() == 0)
            subTree.resize(subTree.capacity());
    }
};

StringNode::Tree stringRootTree;
void RegisterString(StringNode::Tree & tree, std::string const & str, int n)
{
    StringNode & node = stringRootTree[str[n]];
    if (str[n + 1] == '\0')
    {
        if (node.str.size() == 0)
            node.str = str;
        return;
    }
    node.InitIFN();
    RegisterString(node.subTree, str, n + 1);
}

void RegisterString(std::string const & str)
{
    RegisterString(stringRootTree, str, 0);
}

std::string & StringLookup(StringNode::Tree & tree, std::string const & str, int n)
{
    StringNode & node = stringRootTree[str[n]];
    if (str[n + 1] == '\0')
    {
        return node.str;
    }
    return StringLookup(node.subTree, str, n + 1);
}

std::string & StringLookup(std::string const & str)
{
    return StringLookup(stringRootTree, str, 0);
}

std::map<std::string, std::string> mapString;
void RegisterString2(std::string const & str)
{
    mapString[str] = str;
}

std::string & StringLookup2(std::string const & str)
{
    return mapString[str];
}

namespace CContainers
{
    class String
    {
    public:
        String(char const * str)
            : _internString()
        {
            RegisterString(str);
            _internString = StringLookup(str);
        }

        bool operator==(String const & other)
        {
            return _internString == other._internString;
        }
    private:
        std::string * _internString;
    };
}

int main()
{
    std::map<std::string, std::function<void()> > nameWFuncMap;
    nameWFuncMap["StackLocalVector"] = TestStackLocalVector;
    nameWFuncMap["MaxSizedVector  "] = TestMaxSizeAllocator;
    nameWFuncMap["Vector + Resize "] = TestStdVectorReserve;
    nameWFuncMap["Vector          "] = TestStdVector;

    for (int i = 0; i < 10; ++i)
    {
        for (auto& nameWFunc : nameWFuncMap)
        {
            Benchmark b(nameWFunc.first);
            nameWFunc.second();
        }
        std::cout << std::endl << std::endl;
    }
    
    TestConstructor();
    TestIteration();
    std::cout << "============================ Test string ============================" << std::endl;
    
    {
        std::string* strJ;
        std::string* strB;
        {
            
            RegisterString("Bonjour");
            RegisterString("Jambon");
            RegisterString("Test1");
            RegisterString("Test2");
            RegisterString("azerty");
            RegisterString("qwerty");
            RegisterString("moijaimepaslejava");
            RegisterString("legarnagecollectortuelesperfs");
            RegisterString("iwannabetheverybest");
            RegisterString("Testtetsttest");
            Benchmark b("Test string ");
            for (int i = 0; i < 5120; ++i)
            {
                strJ = &StringLookup("Test1");
                strB = &StringLookup("Test1");
            }
        }
        std::cout << *strJ << *strB << std::endl;
    }

    {
        std::string* strJ;
        std::string* strB;
        {
            RegisterString2("Bonjour");
            RegisterString2("Jambon");
            RegisterString2("Test1");
            RegisterString2("Test2");
            RegisterString2("azerty");
            RegisterString2("qwerty");
            RegisterString2("moijaimepaslejava");
            RegisterString2("legarnagecollectortuelesperfs");
            RegisterString2("iwannabetheverybest");
            RegisterString2("Testtetsttest");
            Benchmark b("Test string2");
            for (int i = 0; i < 5120; ++i)
            {
                strJ = &StringLookup2("Test1");
                strB = &StringLookup2("Test1");
            }
        }
        std::cout << *strJ << *strB << std::endl;
    }
    

    system("PAUSE");
    return 0;
}