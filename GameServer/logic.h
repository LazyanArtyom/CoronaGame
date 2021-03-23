#ifndef CLOGIC_H
#define CLOGIC_H
//#include "matrix.h"
#include "graph.h"
#include <queue>
#include <map>

class CLogic
{
public:
    CLogic(size_t nColNum, size_t nRowNum, size_t nGridType);
    ~CLogic();
    std::map<std::pair<size_t, size_t>, std::pair<int, short>> checkField(size_t nH, size_t nW, size_t& nCount, bool bFirst=false);
    void initGraph();
    void generateGraphForMatrix(std::vector<std::pair<int, std::set<size_t> > > & connections);
    size_t count = 0;
private:
    CGraph* m_pField;
    std::queue<size_t> vertexesToVisit;
    size_t m_nColNum;
    size_t m_nRowNum;
    size_t m_nGridType;
};

#endif // CLOGIC_H
