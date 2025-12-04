/*
 * MazeHelper.C
 *
 *  Created on: 28-Aug-2010
 *      Author: Tam
 */
#include <algorithm>

#include "MazeHelper.h"
#include "Node.h"

#include "DeleteIt.h"

#include "Debug.h"
#include "Generator.h"
#include "MazeData.h"
#include "RandSimple.h"

namespace Maze {

///////////////////////////////////////////////////////////////////////////

//
// Iterative method of getting all the Nodes in the tree
// Again, if maze is very big the recursive depth could be too much
//
void MazeHelper::makeNodeList(Node *pRoot, NodeList &rNodeList) {
  rNodeList.clear();
  if (pRoot) {
    rNodeList.push_back(pRoot);
    for (unsigned int i = 0; i < rNodeList.size(); ++i) {
      Node *l_pNode = rNodeList[i];
      int l_numExits = l_pNode->getNumExits();
      for (int e = 0; e < l_numExits; ++e) {
        if (l_pNode->isDownTree(e)) {
          Node *l_pChild = l_pNode->getExitNode(e);
          if (l_pChild) {
            rNodeList.push_back(l_pChild);
          }
        }
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////

void MazeHelper::deleteMaze(Node *pRoot) {
  LOG_INFO("Generator::deleteMaze");
  NodeList l_allNodes;
  makeNodeList(pRoot, l_allNodes);
  for_each(l_allNodes.begin(), l_allNodes.end(), Util::DeleteIt<Node *>());
}

///////////////////////////////////////////////////////////////////////////

void MazeHelper::makeSquareTileData(TileData &outData,
                                    const CellType roomType) {
  // Define the TileData
  const int DIM = 2;
  const int l_north[DIM] = {0, -1};
  const int l_south[DIM] = {0, +1};
  const int l_east[DIM] = {+1, 0};
  const int l_west[DIM] = {-1, 0};
  CellLoc l_northCon(l_north, l_north + DIM);
  CellLoc l_southCon(l_south, l_south + DIM);
  CellLoc l_eastCon(l_east, l_east + DIM);
  CellLoc l_westCon(l_west, l_west + DIM);

  TileData::Connection l_con;
  // Cell type doesn't actually matter since only is one type
  l_con.fromCellType = roomType;
  l_con.toCellType = l_con.fromCellType;

  l_con.locChange = l_northCon;
  outData.defineConnection(l_con);
  l_con.locChange = l_southCon;
  outData.defineConnection(l_con);
  l_con.locChange = l_eastCon;
  outData.defineConnection(l_con);
  l_con.locChange = l_westCon;
  outData.defineConnection(l_con);
}

///////////////////////////////////////////////////////////////////////////

const Maze::Node *MazeHelper::findNode(const Maze::Node *pCurNode,
                                       const Maze::CellLoc &loc) {
  if (pCurNode->getCellLoc() != loc) {
    for (int i = 0; i < pCurNode->getNumExits(); ++i) {
      if (pCurNode->isDownTree(i)) {
        Maze::Node *l_pNext = pCurNode->getExitNode(i);
        if (l_pNext) {
          const Maze::Node *l_pFound = findNode(l_pNext, loc);
          if (l_pFound) {
            return l_pFound;
          }
        }
      }
    }
  } else {
    return pCurNode;
  }
  return 0;
}

const Maze::Node *MazeHelper::findNode(const Maze::Node *pRoot, int x, int y) {
  Maze::CellLoc l_loc;
  l_loc.push_back(x);
  l_loc.push_back(y);
  return findNode(pRoot, l_loc);
}

MazeData *MazeHelper::generateSquareMaze(int width, int height, int startX,
                                         int startY, bool wrap, bool singlePath,
                                         bool noDeadEnds, int openPlanChance,
                                         unsigned int seed) {
  TileData l_tileData;
  makeSquareTileData(l_tileData);

  Maze::CellLoc sizeDim;
  sizeDim.push_back(width);
  sizeDim.push_back(height);
  Maze::CellLoc start;
  start.push_back(startX);
  start.push_back(startY);

  RNG::RandSimple simple(seed);
  Maze::MazeData mazeData(l_tileData, sizeDim, start, wrap, singlePath,
                          noDeadEnds, openPlanChance);

  Maze::Generator generator(mazeData, &simple);
  return generator.generate(seed);
}

} // namespace Maze
