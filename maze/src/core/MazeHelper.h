/*
 * MazeHelper.h
 *
 *  Created on: 28-Aug-2010
 *      Author: Tam
 */

#ifndef MAZEHELPER_H_
#define MAZEHELPER_H_

//
// Helper functions
//

#include "TileData.h"
#include "CellType.h"
#include "CellLoc.h"

namespace Maze { class Node; }

namespace Maze {

class MazeHelper
{
public:

    //
    // Helper function to populate a vector with all the Nodes
    // Provided since navigating the tree is often using a recursive
    // function which if the Maze is very big can cause stack problems
    //
    typedef std::vector<Node*> NodeList;
    static void makeNodeList(Node* pRoot, NodeList& rNodeList);

    //
    // Helper function to delete all the Nodes
    //
    static void deleteMaze(Node* pRoot);

    //
    // Defines square tile data with exits 0..3 = NSEW
    //
    static void makeSquareTileData(TileData& outData, const CellType roomType=0);

    //
    // Find node with given CellLoc (not efficient!)
    //
    static const Maze::Node* findNode(const Maze::Node* pRoot, const Maze::CellLoc& loc);
};

} // namespace

#endif /* MAZEHELPER_H_ */
