#include "GDMaze.hpp"
#include "MazeData.h"
#include "Node.h"
#include "RandSimple.h"
#include "Generator.h"
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <gdextension_interface.h>
#include <godot_cpp/godot.hpp>

using namespace godot;

static Maze::Node* findNode(Maze::Node* pRoot, int x, int y);
static Maze::Node* findNode(Maze::Node* pCurNode, Maze::CellLoc& l_loc);

void GDMaze::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_rooms", "mazeSize"), &GDMaze::setRooms);
	ClassDB::bind_method(D_METHOD("set_room_cell_size", "cells"), &GDMaze::setRoomCellSize);
	ClassDB::bind_method(D_METHOD("set_wall_cell_size", "cells"), &GDMaze::setWallCellSize);
	ClassDB::bind_method(D_METHOD("set_door_cell_size", "cells"), &GDMaze::setDoorCellSize);
	ClassDB::bind_method(D_METHOD("set_start_room", "x", "y"), &GDMaze::setStartRoom);
	ClassDB::bind_method(D_METHOD("set_single_path", "singlePath"), &GDMaze::setSinglePath);
	ClassDB::bind_method(D_METHOD("set_no_dead_ends", "noDeadEnds"), &GDMaze::setNoDeadEnds);
	ClassDB::bind_method(D_METHOD("set_wrap_around", "wrapAround"), &GDMaze::setWrapAround);
	ClassDB::bind_method(D_METHOD("set_open_plan", "openPlanChance"), &GDMaze::setOpenPlan);
	ClassDB::bind_method(D_METHOD("set_smooth_walls", "smoothWalls"), &GDMaze::setSmoothWalls);
	ClassDB::bind_method(D_METHOD("set_floor", "floorTileCoords"), &GDMaze::setFloor);
	ClassDB::bind_method(D_METHOD("set_wall", "wallTileCoords"), &GDMaze::setWall);
	ClassDB::bind_method(D_METHOD("make_maze", "pTileMap", "layer", "seed"), &GDMaze::make_it);
}

GDMaze::GDMaze() {
	mFloor = Vector2i(0,0);
	mWall = Vector2i(0,1);
}

GDMaze::~GDMaze() {
}


GDMaze* GDMaze::setRooms(Vector2i mazeSize) {
	mRoomsWide = mazeSize.x;
	mRoomsTall = mazeSize.y;
	return this;
}

GDMaze* GDMaze::setRoomCellSize(Vector2i cells) {
	mRoomWidth = cells.x;
	mRoomHeight = cells.y;
	return this;
}
GDMaze* GDMaze::setWallCellSize(Vector2i cells) {
	mWallWidth = cells.x;
	mWallHeight = cells.y;
	return this;
}
GDMaze* GDMaze::setDoorCellSize(Vector2i cells) {
	mDoorWidth = cells.x;
	mDoorHeight = cells.y;
	return this;
}
GDMaze* GDMaze::setStartRoom(int x, int y) {
	mStartRoomX = x;
	mStartRoomY = y;
	return this;
}

GDMaze* GDMaze::setSinglePath(bool singlePath) {
	mSinglePath = singlePath;
	return this;

}
GDMaze* GDMaze::setNoDeadEnds(bool noDeadEnds) {
	mNoDeadEnds = noDeadEnds;
	return this;
}
GDMaze* GDMaze::setWrapAround(bool wrapAround) {
	mWrapAround = wrapAround;
	return this;
}
GDMaze* GDMaze::setOpenPlan(int chance) {
	mOpenPlanChance = chance;
	return this;
}

GDMaze* GDMaze::setSmoothWalls(bool smoothWalls) {
	mSmoothWalls = smoothWalls;
	return this;
}

GDMaze* GDMaze::setFloor(godot::Vector2i floor) {
	mFloor = floor;
	return this;
}
GDMaze* GDMaze::setWall(godot::Vector2i wall) {
	mWall = wall;
	return this;
}

void GDMaze::make_it(TileMapLayer* pTileMap, int layer, int seed) {
	const int DIM=2;
	const int l_north[DIM]  = {  0, -1 };
	const int l_south[DIM]  = {  0, +1 };
	const int l_east [DIM]  = { +1,  0 };
	const int l_west [DIM]  = { -1,  0 };
    Maze::TileData l_tileData;
    Maze::TileData::Connection l_con;
	Maze::CellLoc l_northCon(l_north, l_north+DIM);
	Maze::CellLoc l_southCon(l_south, l_south+DIM);
	Maze::CellLoc l_eastCon(l_east, l_east+DIM);
	Maze::CellLoc l_westCon(l_west, l_west+DIM);

	if (mDoorWidth > mRoomWidth) {
		ERR_PRINT("Door width must be <= Room width");
		return;
	}
	if (mDoorHeight > mRoomHeight) {
		ERR_PRINT("Door height must be <= Room height");
		return;
	}

    l_con.fromCellType = 1;
    l_con.toCellType = l_con.fromCellType;

    l_con.locChange = l_northCon;
    l_tileData.defineConnection(l_con);
    l_con.locChange = l_southCon;
    l_tileData.defineConnection(l_con);
    l_con.locChange = l_eastCon;
    l_tileData.defineConnection(l_con);
    l_con.locChange = l_westCon;
    l_tileData.defineConnection(l_con);

	Maze::CellLoc sizeDim;
	sizeDim.push_back(mRoomsWide);
	sizeDim.push_back(mRoomsTall);
	Maze::CellLoc start;
	start.push_back(mStartRoomX);
	start.push_back(mStartRoomY);


	RNG::RandSimple simple(seed);
    Maze::MazeData mazeData(l_tileData, sizeDim, start,
             mWrapAround,
             mSinglePath,
             mNoDeadEnds,
             mOpenPlanChance);

    Maze::Generator generator(mazeData, &simple);
    Maze::MazeData* pMaze = generator.generate(seed);

    // Make a grid of walls
    Maze::Node* pRoot = pMaze->getRoot();
    for (int ry=0; ry < mRoomsTall; ++ry) {
    	for (int rx=0; rx < mRoomsWide; ++rx) {
    		const Maze::Node* l_pNode = findNode(pRoot, rx, ry);
    		Vector2i roomCorner = roomToCell(rx, ry);

    		// North+South walls
    		for (int cy=0; cy < mWallHeight; ++cy) {
    			for (int cx=0; cx < mWallWidth*2+mRoomWidth; ++cx) {
    				// North
    				Vector2i coords(roomCorner.x+cx, roomCorner.y+cy);
    				pTileMap->set_cell(coords,0,mWall);
    				// South
    				coords.y += mRoomHeight+mWallHeight;
    				pTileMap->set_cell(coords,0,mWall);
    			}
    		}
    		// East+West walls
    		for (int cy=0; cy < mWallHeight*2+mRoomHeight; ++cy) {
    			for (int cx=0; cx < mWallWidth; ++cx) {
    				// West
    				Vector2i coords(roomCorner.x+cx, roomCorner.y+cy);
    				pTileMap->set_cell(coords,0,mWall);
    				// East
    				coords.x += mRoomWidth+mWallWidth;
    				pTileMap->set_cell(coords,0,mWall);
    			}
    		}
    	}
    }

    // Delete openings
    for (int ry=0; ry < mRoomsTall; ++ry) {
    	for (int rx=0; rx < mRoomsWide; ++rx) {
    		const Maze::Node* l_pNode = findNode(pRoot, rx, ry);
    		Vector2i roomCorner = roomToCell(rx, ry);

    		const int offsetX = mWallWidth + (mRoomWidth-mDoorWidth)/2;
    		// North exit
    		if (l_pNode->isOpen(0)) {
    			for (int cy=0; cy < mWallHeight; ++cy) {
    				for (int cx=0; cx < mDoorWidth; ++cx) {
    					Vector2i coords(roomCorner.x+offsetX+cx, roomCorner.y+cy);
    					pTileMap->set_cell(coords,0,mFloor);
    				}
    			}
    		}
    		// South exit
    		if (l_pNode->isOpen(1)) {
    			for (int cy=0; cy < mWallHeight; ++cy) {
    				for (int cx=0; cx < mDoorWidth; ++cx) {
    					Vector2i coords(roomCorner.x+offsetX+cx, roomCorner.y+mRoomHeight+mWallHeight+cy);
    					pTileMap->set_cell(coords,0,mFloor);
    				}
    			}
    		}

    		const int offsetY = mWallHeight + (mRoomHeight-mDoorHeight)/2;
    		// East exit
    		if (l_pNode->isOpen(2)) {
    			for (int cx=0; cx < mWallWidth; ++cx) {
    				for (int cy=0; cy < mDoorHeight; ++cy) {
    					Vector2i coords(roomCorner.x+mRoomWidth+mWallWidth+cx, roomCorner.y+offsetY+cy);
    					pTileMap->set_cell(coords,0,mFloor);
    				}
    			}
    		}
    		// West exit
    		if (l_pNode->isOpen(3)) {
    			for (int cx=0; cx < mWallWidth; ++cx) {
    				for (int cy=0; cy < mDoorHeight; ++cy) {
    					Vector2i coords(roomCorner.x+cx, roomCorner.y+offsetY+cy);
    					pTileMap->set_cell(coords,0,mFloor);
    				}
    			}
    		}

    	}
    }

    // Smooth wells
    if (mSmoothWalls) {
    	for (int ry=0; ry < mRoomsTall; ++ry) {
    		for (int rx=0; rx < mRoomsWide; ++rx) {
    			const Maze::Node* l_pNode = findNode(pRoot, rx, ry);

    			// CHeck for removing vert nub. So must be open to the east
    			if ((rx != mRoomsWide-1) && l_pNode->isOpen(2)) {
    				const Maze::Node* l_pNodeRight = findNode(pRoot, rx+1, ry);
    				if (l_pNode->isClosed(0) && l_pNodeRight->isClosed(0)) {
    					Vector2i roomCorner = roomToCell(rx+1, ry);
    					roomCorner.y += mWallHeight;
    					// if 2 north walls then remove the vert nub between them
    					for (int cy=0; cy < (mRoomHeight-mDoorHeight)/2; ++cy) {
    						for (int cx=0; cx < mWallWidth; ++cx) {
    							Vector2i coords(roomCorner.x+cx, roomCorner.y+cy);
    							pTileMap->set_cell(coords,0,mFloor);
    						}
    					}
    				}
    				if (l_pNode->isClosed(1) && l_pNodeRight->isClosed(1)) {
    					Vector2i roomCorner = roomToCell(rx+1, ry);
    					roomCorner.y += mWallHeight+ (mRoomHeight-(mRoomHeight-mDoorHeight)/2);
    					// if 2 south walls then remove the vert nub between them
    					for (int cy=0; cy < (mRoomHeight-mDoorHeight)/2; ++cy) {
    						for (int cx=0; cx < mWallWidth; ++cx) {
    							Vector2i coords(roomCorner.x+cx, roomCorner.y+cy);
    							pTileMap->set_cell(coords,0,mFloor);
    						}
    					}

    				}
    			}

    			// CHeck for removing horz nub. So must be open to the south
    			if (ry != mRoomsTall-1 && l_pNode->isOpen(1)) {
    				const Maze::Node* l_pNodeBelow = findNode(pRoot, rx, ry+1);
    				if (l_pNode->isClosed(3) && l_pNodeBelow->isClosed(3)) {
    					Vector2i roomCorner = roomToCell(rx, ry+1);
    					roomCorner.x += mWallWidth;
    					// if 2 west walls then remove the horz nub between them
    					for (int cy=0; cy < mWallHeight; ++cy) {
    						for (int cx=0; cx < (mRoomWidth-mDoorWidth)/2; ++cx) {
    							Vector2i coords(roomCorner.x+cx, roomCorner.y+cy);
    							pTileMap->set_cell(coords,0,mFloor);
    						}
    					}
    				}
    				if (l_pNode->isClosed(2) && l_pNodeBelow->isClosed(2)) {
    					Vector2i roomCorner = roomToCell(rx, ry+1);
    					roomCorner.x += mWallWidth+ (mRoomWidth-(mRoomWidth-mDoorWidth)/2);
    					// if 2 east walls then remove the horz nub between them
    					for (int cy=0; cy < mWallHeight; ++cy) {
    						for (int cx=0; cx < (mRoomWidth-mDoorWidth)/2; ++cx) {
    							Vector2i coords(roomCorner.x+cx, roomCorner.y+cy);
    							pTileMap->set_cell(coords,0,mFloor);
    						}
    					}
    				}
    			}
    		}
    	}
    }
    delete pMaze;
}

///////////////////////////////////////////////////

Vector2i GDMaze::roomToCell(int rx, int ry) {
	int x = rx * (mRoomWidth+mWallWidth);
	int y = ry * (mRoomHeight+mWallHeight);
	return Vector2i(x,y);
}

///////////////////////////////////////////////////

static Maze::Node* findNode(Maze::Node* pCurNode, Maze::CellLoc& l_loc)
{
    if (pCurNode->getCellLoc() != l_loc)
    {
        for (int i=0; i < pCurNode->getNumExits(); ++i)
        {
            if (pCurNode->isDownTree(i))
            {
                Maze::Node* l_pNext = pCurNode->getExitNode(i);
                if (l_pNext)
                {
                    Maze::Node* l_pFound = findNode(l_pNext, l_loc);
                    if (l_pFound)
                    {
                        return l_pFound;
                    }
                }
            }
        }
    }
    else
    {
        return pCurNode;
    }
    return 0;
}

static Maze::Node* findNode(Maze::Node* pRoot, int x, int y)
{
    Maze::CellLoc l_loc;
    l_loc.push_back(x);
    l_loc.push_back(y);
    return findNode(pRoot, l_loc);
}
