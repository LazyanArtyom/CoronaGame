#include "logic.h"
#include <QDebug>
#include <assert.h>

CLogic::CLogic(size_t nColNum, size_t nRowNum, size_t nGridType)
    : m_nColNum(nColNum), m_nRowNum(nRowNum), m_nGridType(nGridType)
{
    initGraph();
    qDebug() << "Column " << m_nColNum << "Row" << m_nRowNum;
}

CLogic::~CLogic()
{
    if (m_pField)
    {
        delete m_pField;
        m_pField = nullptr;
    }
}

std::map<std::pair<size_t, size_t>, std::pair<int, short>> CLogic::checkField(size_t nH, size_t nW, size_t& nCount, bool bFirst)
{
    std::map<std::pair<size_t, size_t>, std::pair<int, short>> changedValues;

    if (bFirst)
    {
        for (size_t i = 0; i < m_nRowNum; ++i)
        {
            for (size_t j = 0; j < m_nColNum; ++j)
            {
                changedValues[std::make_pair(i, j)] = std::make_pair(m_pField->getNodeById(i * m_nColNum + j)->m_tValue,
                                                                     m_pField->getNodeById(i * m_nColNum + j)->bPlvac);
            }
        }

        return changedValues;
    }

    size_t nNodeId = m_nColNum * nH + nW;
    vertexesToVisit.push(nNodeId);
    m_pField->getNodeById(nNodeId)->m_tValue += 2;
    //changedValues[std::make_pair(nH, nW)] = m_pField->getNodeById(nNodeId)->m_tValue;
    do
    {
        size_t nId = vertexesToVisit.front();
        if(m_nGridType == 2) //open
        {
            if(m_pField->getNodeById(nId)->m_tValue >= 4)
            {
                if(m_pField->getNodeById(nId)->bPlvac == 0)
                {
                    m_pField->getNodeById(nId)->bPlvac = 1;
                    ++count;
                }

                for(auto nNeighbourId : m_pField->getNodeById(nId)->m_arrNeighbours)
                {
                    m_pField->getNodeById(nNeighbourId)->m_tValue += m_pField->getNodeById(nId)->m_tValue / 4;
                    vertexesToVisit.push(nNeighbourId);
                }
                m_pField->getNodeById(nId)->m_tValue = m_pField->getNodeById(nId)->m_tValue % 4;
            }
        }
        else
        {
            if(m_pField->getNodeById(nId)->m_tValue >= (int)m_pField->getNodeNeighboursCount(nId))
            {
                if(m_pField->getNodeById(nId)->bPlvac == 0)
                {
                    m_pField->getNodeById(nId)->bPlvac = 1;
                    ++count;
                }

                for(auto nNeighbourId : m_pField->getNodeById(nId)->m_arrNeighbours)
                {
                    m_pField->getNodeById(nNeighbourId)->m_tValue += m_pField->getNodeById(nId)->m_tValue / (int)m_pField->getNodeNeighboursCount(nId);
                    vertexesToVisit.push(nNeighbourId);
                }
                m_pField->getNodeById(nId)->m_tValue = m_pField->getNodeById(nId)->m_tValue % (int)m_pField->getNodeNeighboursCount(nId);
            }
        }
        vertexesToVisit.pop();
        changedValues[std::make_pair((size_t)(nId / m_nColNum), (size_t)(nId % m_nColNum))]
                = std::make_pair(m_pField->getNodeById(nId)->m_tValue, m_pField->getNodeById(nId)->bPlvac);

        if(count == m_nColNum * m_nRowNum)
        {
            nCount = count;
            break;
        }
        qDebug()<<"vertices " << vertexesToVisit.size();
    }
    while(!vertexesToVisit.empty());
    nCount = count;
    return changedValues;
}

void CLogic::initGraph()
{
    std::vector<std::pair<int, std::set<size_t> > > arrConnections;
    generateGraphForMatrix(arrConnections);
    m_pField = new CGraph(arrConnections);
}

void CLogic::generateGraphForMatrix(std::vector<std::pair<int, std::set<size_t> > > & connections)
{
    connections.resize(m_nColNum * m_nRowNum);
    switch (m_nGridType)
    {
    case 0:
        {
            for(size_t i = 0; i < m_nRowNum; ++i)
            {
                for(size_t j = 0; j < m_nColNum; ++j)
                {
                    if(i != 0)
                        connections[i * m_nColNum + j].second.insert((i - 1) * m_nColNum + j);
                    else
                        connections[i * m_nColNum + j].second.insert((m_nRowNum - 1) * m_nColNum + j);
                    if(i != m_nRowNum - 1)
                        connections[i * m_nColNum + j].second.insert((i + 1) * m_nColNum + j);
                    else
                        connections[i * m_nColNum + j].second.insert(j);
                    if(j != 0)
                        connections[i * m_nColNum + j].second.insert(i * m_nColNum + j - 1);
                    else
                        connections[i * m_nColNum + j].second.insert(i * m_nColNum + m_nColNum - 1);
                    if(j != m_nColNum - 1)
                        connections[i * m_nColNum + j].second.insert(i * m_nColNum + j + 1);
                    else
                        connections[i * m_nColNum + j].second.insert(i * m_nColNum);
                   connections[i * m_nColNum + j].first = 2;
                }
            }
        }
        break;
    case 1:
        {
            for(size_t i = 0; i < m_nRowNum; ++i)
            {
                for(size_t j = 0; j < m_nColNum; ++j)
                {
                    if(i != 0)
                        connections[i * m_nColNum + j].second.insert((i - 1) * m_nColNum + j);
                    if(i != m_nRowNum - 1)
                        connections[i * m_nColNum + j].second.insert((i + 1) * m_nColNum + j);
                    if(j != 0)
                        connections[i * m_nColNum + j].second.insert(i * m_nColNum + j - 1);
                    if(j != m_nColNum  - 1)
                        connections[i * m_nColNum + j].second.insert(i * m_nColNum + j + 1);

                    if(i == 0 || j == 0 || j == m_nColNum - 1 || i == m_nRowNum - 1)
                        connections[i * m_nColNum + j].first = 1;
                    else
                        connections[i * m_nColNum + j].first = 2;
                }
            }
            connections[0].first = 0;
            connections[m_nColNum - 1].first = 0;
            connections[(m_nRowNum - 1) * m_nColNum].first = 0;
            connections[m_nColNum * m_nRowNum - 1].first = 0;
        }
        break;
    case 2:
        {
            for(size_t i = 0; i < m_nRowNum; ++i)
            {
                for(size_t j = 0; j < m_nColNum; ++j)
                {
                    if(i != 0)
                        connections[i * m_nColNum + j].second.insert((i - 1) * m_nColNum + j);
                    if(i != m_nRowNum - 1)
                        connections[i * m_nColNum + j].second.insert((i + 1) * m_nColNum + j);
                    if(j != 0)
                        connections[i * m_nColNum + j].second.insert(i * m_nColNum + j - 1);
                    if(j != m_nColNum  - 1)
                        connections[i * m_nColNum + j].second.insert(i * m_nColNum + j + 1);
                    connections[i * m_nColNum + j].first = 2;
                }
            }
        }
    default:
        break;
    }
}
