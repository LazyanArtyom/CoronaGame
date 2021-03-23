#ifndef CGRAPH_H
#define CGRAPH_H

#include <vector>
#include <set>

//template <typename T>
class TNode
{
public:
    int m_tValue;
    std::set<size_t> m_arrNeighbours;
    short bPlvac = 0;
};

//template <typename T>
class CGraph
{
public:

    CGraph(const std::vector<std::pair<int, std::set<size_t> > > &arrConnectionsMatrix);
    ~CGraph();
    TNode* getNodeById(size_t nId);
    size_t getNodeNeighboursCount(size_t nId);
private:
    TNode* m_pFirstNode = nullptr;
    std::vector<TNode *> m_arrIdToNodeMapping;
};

#endif // CGRAPH_H
