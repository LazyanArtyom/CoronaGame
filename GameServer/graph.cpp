#include "graph.h"
#include <QDebug>

CGraph::CGraph(const std::vector<std::pair<int, std::set<size_t> > > &arrConnectionsMatrix)
{
    for(auto vertex : arrConnectionsMatrix)
    {
        m_arrIdToNodeMapping.push_back(new TNode());
    }
    for(size_t i = 0; i < arrConnectionsMatrix.size(); ++i)
    {
        auto pNode = m_arrIdToNodeMapping[i];
        auto vertex = arrConnectionsMatrix[i];
        pNode->m_tValue = vertex.first;
        for(auto neighbour : vertex.second)
            pNode->m_arrNeighbours.insert(neighbour);
    }
}

CGraph::~CGraph()
{
    for(auto pNode : m_arrIdToNodeMapping)
        if(pNode != nullptr)
        {
            delete  pNode;
            pNode = nullptr;
        }
}

TNode *CGraph::getNodeById(size_t nId)
{
    return m_arrIdToNodeMapping[nId];
}

size_t CGraph::getNodeNeighboursCount(size_t nId)
{
    return m_arrIdToNodeMapping[nId]->m_arrNeighbours.size();
}

