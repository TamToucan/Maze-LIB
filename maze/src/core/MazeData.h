#ifndef MAZE_MAZEDATA_H
#define MAZE_MAZEDATA_H

#include "CellLoc.h"
#include "CellType.h"
#include "TileData.h"

//
// Used to pass all the info needed to create a maze to Generator
// which then populates the Root Node i.e. the actual maze
//
namespace Maze {
    class Node;

class MazeData
{
public:
    MazeData();

    MazeData(const TileData& rTileData,
             const CellLoc& rDimensions,
             const CellLoc& rStartLoc,
             bool wrapRound=false,
             bool singlePath=false,
             bool noDeadEnds=false,
             int openPlanChance=0);

    virtual ~MazeData();

    // Accessors
    virtual const TileData& getTileData() const { return m_tileData; }
    virtual const CellLoc& getDimensions() const { return m_dimensions; }
    virtual const CellLoc& getStartLoc() const { return m_startLoc; }
    virtual bool getWrapRoundOn() const { return m_wrapRoundOn; }
    virtual bool getSinglePath() const { return m_singlePath; }
    virtual bool getNoDeadEnds() const { return m_noDeadEnds; }
    virtual int  getOpenPlanChance() const { return m_openPlanChance; }

    // Settors
    virtual void setTileData(const TileData& rTileData);
    virtual void setDimensions(const CellLoc& rDimensions);
    virtual void setStartLoc(const CellLoc& rStartLoc);
    virtual void setWrapRound(bool wrapRoundOn);
    virtual void setSinglePath(bool singlePath);
    virtual void setNoDeadEnds(bool noDeadEnds);
    virtual void setOpenPlanChance(int openPlanChance);

    virtual Node* getRoot() const { return m_pRoot; }
    virtual void setRoot(Node* pRoot) { m_pRoot = pRoot; }

    // End loc only really makes sense for single path maze
    virtual void setEndLoc(const CellLoc& endLoc) { m_endLoc = endLoc; }
    virtual const CellLoc& getEndLoc() const { return m_endLoc; }

    // Helper
    virtual int getTotalCells() const;

protected:
    Node*    m_pRoot;

    TileData m_tileData;
    CellLoc  m_dimensions;
    CellLoc  m_startLoc;
    CellLoc  m_endLoc;

    bool     m_wrapRoundOn;
    bool     m_singlePath;
    bool     m_noDeadEnds;
    int      m_openPlanChance;
};

} // namespace

#endif
