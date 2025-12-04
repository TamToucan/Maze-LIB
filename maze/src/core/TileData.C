#include "TileData.h"
#include "CellLoc.h"
#include "CellType.h"

namespace Maze {

///////////////////////////////////////////////////////////////////////////

TileData::TileData() : m_firstCellType(0)
{
}

///////////////////////////////////////////////////////////////////////////

TileData::~TileData()
{
}

///////////////////////////////////////////////////////////////////////////

void TileData::defineConnection(const Connection& rConnection)
{
    if (m_connections.empty())
    {
        setFirstCellType(rConnection.fromCellType);
    }

    m_connections.push_back(rConnection);
}

///////////////////////////////////////////////////////////////////////////

void TileData::setFirstCellType(const CellType& rCellType)
{
    m_firstCellType = rCellType;
}

///////////////////////////////////////////////////////////////////////////

const CellType& TileData::getFirstCellType() const
{
    return m_firstCellType;
}

///////////////////////////////////////////////////////////////////////////

int TileData::getNumConnections(const CellType& rFromType) const
{
    int l_numCons = 0;
    for (ConnectionList::const_iterator l_itr = m_connections.begin();
            l_itr != m_connections.end();
            ++l_itr)
    {
        if (l_itr->fromCellType == rFromType)
        {
            ++l_numCons;
        }
    }
    return l_numCons;
}

///////////////////////////////////////////////////////////////////////////

const TileData::Connection* TileData::getConnection(const CellType& rFromType,
                                                    int conNum) const
{
    int l_curConNum = 0;
    for (ConnectionList::const_iterator l_itr = m_connections.begin();
            l_itr != m_connections.end();
            ++l_itr)
    {
        if (l_itr->fromCellType == rFromType)
        {
            if (l_curConNum == conNum)
            {
                return &(*l_itr);
            }
            ++l_curConNum;
        }
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////

} // namespace

