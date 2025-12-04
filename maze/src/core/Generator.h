#ifndef MAZE_GENERATOR_H
#define MAZE_GENERATOR_H

#include <vector>

#include "MazeData.h"

//
// This generates a random maze of different Node types which are
// connected to each other via a list of exits (which are either
// open or closed).
// See TileData.h for details of how the repeated tiles of the maze
// are represented. This class takes the tile data and builds it
// into a maze. So each Node has a Cell Type and Location (from
// the TileData) and a list of exits (from the TileData Connections
// for that cell type)
//
// The Node always has the same number of exits as the Cell Type
// has connections (so you can tell which exit it is). An exit has
// the Node that it leads to, if there is no Node i.e. the exit is
// at the "edge" of the maze then the Node it points to is 0.
//
// Each Exit is either OPEN or CLOSED (with obvious meanings). Also
// the Node it leads to is either UPTREE or DOWNTREE from the current
// Node. This is used to navigate the tree of Nodes. Starting from
// the root node (returned by generate method) follow all the DOWNTREE
// exits to visit every Node in the Maze.
//
// The various generation parameters in the MazeData are
// - wrapRound = Exits which would lead to nodes outside of the maze
//               dimensions can be created and lead to the Node on the
//               "other side" e.g. in a 2D maze an exit off the top would
//               lead to the node at the bottom.
// - singlePath = Maze only contains a single solution between any two points.
//               If openPlanChance is non-0 or noDeadEnds is false then there
//               will be more than one solution
// - noDeadEnds = A random exit is opened in any dead-ends i.e. Nodes with
//               only one exit are changed to have 2 exits.
// - openPlanChance = Once all of the above has been done each closed exit
//               has an "openPlanChance" percent chance of being opened i.e.
//               setting this to 100 will make every exit open.
// - pRNG = Use your own RNG instead of the basic default
//
// NOTE: No checking it made see if the data is consistent since no assumptions
// are made about what you are trying to do. E.g. A set of 2D tile data that
// generates 2 cell types at (row,col) and (row,col+1) would need an even value
// in the 2nd value of the dimensions (i.e. the number of columns wide) if using
// wrapRound.
//
// If it had and odd value then the first column of the maze would contain the
// first cell type and so would the last column. Therefore exits from the last
// column would lead to the 2nd column (not the first) and likewise exits from
// the first would lead to the second last. But maybe that's what you want.
//
namespace RNG { class I_Random; }

namespace Maze
{
class Generator
{
public:
    Generator(const MazeData& rMazeData, RNG::I_Random* pRNG=0);

    virtual ~Generator();

    virtual void setRNG(RNG::I_Random* pRNG);

    // Generate a maze, if seed is given the pRNG will be init'ed to it
    // NOTE: This is a new MazeData which must be deleted by caller
    virtual MazeData* generate(unsigned int seed = 0);

protected:
    class Impl;
    Impl* pimpl;
};

} // namespace

#endif
