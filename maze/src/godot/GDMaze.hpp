#ifndef GD_MAZE_H
#define GD_MAZE_H

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/tile_map_layer.hpp>

namespace godot {

class GDMaze : public Object {
	GDCLASS(GDMaze, Object)

protected:
	static void _bind_methods();

	int mRoomsWide = 1;
	int mRoomsTall = 1;

	int mRoomWidth = 3;
	int mRoomHeight = 3;
	int mDoorWidth = 1;
	int mDoorHeight = 1;
	int mWallWidth = 1;
	int mWallHeight = 1;
	int mStartRoomX = 0;
	int mStartRoomY = 0;

	bool mSinglePath = false;
	bool mNoDeadEnds = true;
	bool mWrapAround = false;
	int mOpenPlanChance = 0;

	bool mSmoothWalls = true;

	godot::Vector2i mFloor;
	godot::Vector2i mWall;

public:
	GDMaze();
	~GDMaze();

	GDMaze* setRooms(Vector2i mazeSize);

	GDMaze* setRoomCellSize(Vector2i cells);
	GDMaze* setWallCellSize(Vector2i cells);
	GDMaze* setDoorCellSize(Vector2i cells);
	GDMaze* setStartRoom(int x, int y);

	GDMaze* setSinglePath(bool singlePath);
	GDMaze* setNoDeadEnds(bool noDeadEnds);
	GDMaze* setWrapAround(bool wrapAround);
	GDMaze* setOpenPlan(int chance);

	GDMaze* setSmoothWalls(bool smoothWalls);

	GDMaze* setFloor(godot::Vector2i floor);
	GDMaze* setWall(godot::Vector2i wall);

	void make_it(TileMapLayer* pTileMap, int layer, int seed);

private:
    godot::Vector2i roomToCell(int rx, int ry);

};

}

#endif
