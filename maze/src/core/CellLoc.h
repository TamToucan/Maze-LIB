#ifndef MAZE_CELL_LOC_H
#define MAZE_CELL_LOC_H

#include <vector>

namespace Maze {

typedef std::vector<int> CellLoc;

CellLoc operator+(const CellLoc& rLHS, const CellLoc& rRHS);
CellLoc& operator+=(CellLoc& rLHS, const CellLoc rRHS);

} // namespace

#endif
