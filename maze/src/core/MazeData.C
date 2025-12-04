#include "MazeData.h"
#include "Generator.h"
#include "MazeHelper.h"

namespace Maze {

MazeData::MazeData() :
    m_pRoot(0),
    m_wrapRoundOn(false),
    m_noDeadEnds(false),
    m_openPlanChance(0)
{
}

MazeData::MazeData(
        const TileData& rTileData,
        const CellLoc& rDimensions,
        const CellLoc& rStartLoc,
        bool wrapRound,
        bool singlePath,
        bool noDeadEnds,
        int openPlanChance) :
            m_pRoot(0)
{
    setTileData(rTileData);
    setDimensions(rDimensions);
    setStartLoc(rStartLoc);
    setWrapRound(wrapRound);
    setSinglePath(singlePath);
    setNoDeadEnds(noDeadEnds);
    setOpenPlanChance(openPlanChance);
}

MazeData::~MazeData()
{
    MazeHelper::deleteMaze(m_pRoot);
}

///////////////////////////////////////////////////////////////////////////

void MazeData::setTileData(const TileData& rTileData)
{
    m_tileData = rTileData;
}
void MazeData::setDimensions(const CellLoc& rDimensions)
{
    m_dimensions = rDimensions;
}

void MazeData::setStartLoc(const CellLoc& rStartLoc)
{
    m_startLoc = rStartLoc;
}

void MazeData::setWrapRound(bool wrapRoundOn)
{
    m_wrapRoundOn = wrapRoundOn;
}

void MazeData::setNoDeadEnds(bool noDeadEnds)
{
    m_noDeadEnds = noDeadEnds;
}

void MazeData::setSinglePath(bool singlePath)
{
    m_singlePath = singlePath;
}

void MazeData::setOpenPlanChance(int openPlanChance)
{
    m_openPlanChance = openPlanChance;
}

int MazeData::getTotalCells() const
{
    int l_total = 1;
    for (CellLoc::const_iterator l_itr = m_dimensions.begin();
         l_itr != m_dimensions.end();
         ++l_itr)
    {
        l_total *= *l_itr;
    }
    return l_total;
}

} // namespace
