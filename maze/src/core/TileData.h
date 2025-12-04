#ifndef MAZE_TILE_DATA_H
#define MAZE_TILE_DATA_H

#include <vector>

#include "CellLoc.h"
#include "CellType.h"

//
// This is used to define the repeated "Tile" that will be used to
// build the maze. A tile is made up of one or more "Cells" each
// cell has a type and a list of connections (to other cell types)
// A Connection details the amount the Cell Location will change
// to get to the other cell.
//
// Cell Locations are simply a list of coordinates. How to interpret
// them (X,Y or Y,X or along and down) is up to you.
//
// E.g. To define the Tile to create a standard 2D square (4 exits
// per cell) maze and assuming 0,0 is the top left, 0,Y is the bottom
// left and X,0 top right. Then there would be 1 cell type and 4 Connections
// How the Connections are defined is up to you. Assuming North, South, East,
// West (for the four connections) then the tile would look like
//
//         Con0 (0,-1)
//          |
//        #####
//        #   #
//  Con3 -# 0 #- Con2
// (-1,0) #   #  (1,0)
//        #####
//          |
//         Con1 (0,1)
//
// i.e. Cell Type 0 has 4 Connections (0..3). Connection0 leads to
// Cell Type0 and changes the current location by 0 along and 1 up (North)
//
// To extend this to 3D the Cell Locations would have 3 values instead of 2
// and 2 more Connections would be needed (for the up and down exits).
//
// NOTE: Connections don't have any IDs/Types, instead the order you define
// them is their ID e.g. defining 4 Connections will mean each Node in the
// maze for that type will have 4 exits (0..3) corresponding to the order
// you added the Connections.
//
// NOTE: No validation is performed on the data. There must only be one
// of each Cell Type in the data. Also Connections must be consistant e.g.
// If there is a Connection from type 0 to type 1 then you need to define
// a connection from type1 to type0 (and locChange should be reversed).
// Done this way since might want to add support for one-way Connections.
//
namespace Maze {

class TileData
{
public:
    typedef struct {
        CellType fromCellType;
        CellLoc locChange;
        CellType toCellType;
    } Connection;

public:
    TileData();
    virtual ~TileData();

    // Add a Connection. Need to define both ways
    virtual void defineConnection(const Connection& rConnection);

    // Sets the type of Cell that will be used to create the first
    // Node in the generated maze.
    // If not called then fromType of first added Connected is used
    // NOTE: If calling it MUST be called after the first connection added
    virtual void setFirstCellType(const CellType& rCellType);
    virtual const CellType& getFirstCellType() const;

    // Returns the number of connections for that cell type
    virtual int getNumConnections(const CellType& rFromCellType) const;

    // Returns a Connection (conNum = 0...getNumConnections-1)
    // 0 returned if not found or conNum too large
    virtual const Connection* getConnection(const CellType& rFromCellType,
                                            int conNum) const;

protected:
    CellType                        m_firstCellType;

    typedef std::vector<Connection> ConnectionList;
    ConnectionList                  m_connections;
};

} // namespace

#endif
