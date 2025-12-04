#include <iostream>
#include <string>
#include <vector>


#include "MazeData.h"
#include "MazeHelper.h"
#include "Node.h"


int main() {
  const int roomsWide = 16;
  const int roomsTall = 16;
  const int roomWidth = 3;
  const int roomHeight = 3;
  const int wallWidth = 2;
  const int wallHeight = 2;
  const int startX = 0;
  const int startY = 0;
  const bool wrap = false;
  const bool singlePath = false;
  const bool noDeadEnds = true;
  const int openPlanChance = 0;
  const unsigned int seed = 12345;

  Maze::MazeData *pMaze = Maze::MazeHelper::generateSquareMaze(
      roomsWide, roomsTall, startX, startY, wrap, singlePath, noDeadEnds,
      openPlanChance, seed);

  if (!pMaze) {
    std::cerr << "Failed to generate maze!" << std::endl;
    return 1;
  }

  // Calculate grid dimensions
  // Width = rooms * roomWidth + (rooms + 1) * wallWidth ?
  // GDMaze logic:
  // North+South walls: mWallWidth*2 + mRoomWidth
  // East+West walls: mWallWidth*2 + mRoomHeight
  // It seems walls are shared.
  // Let's assume a grid where each cell is either wall or room.
  // The total size would be:
  // Width = roomsWide * (roomWidth + wallWidth) + wallWidth
  // Height = roomsTall * (roomHeight + wallHeight) + wallHeight

  int gridWidth = roomsWide * (roomWidth + wallWidth) + wallWidth;
  int gridHeight = roomsTall * (roomHeight + wallHeight) + wallHeight;

  std::vector<std::string> grid(gridHeight, std::string(gridWidth, ' '));

  // Fill with walls initially? No, GDMaze fills walls around rooms.
  // Let's replicate GDMaze logic but writing to char grid.

  Maze::Node *pRoot = pMaze->getRoot();
  const char WALL_CHAR = '#';
  const char FLOOR_CHAR = ' ';

  // Initialize grid with floor? Or walls?
  // GDMaze sets walls, then clears openings.
  // Let's initialize with FLOOR, then draw walls.
  // Wait, GDMaze iterates rooms and draws walls around them.

  for (int ry = 0; ry < roomsTall; ++ry) {
    for (int rx = 0; rx < roomsWide; ++rx) {
      // Calculate top-left of this room's block (including walls)
      // In GDMaze: roomToCell(rx, ry) -> x = rx * (mRoomWidth + mWallWidth), y
      // = ...
      int cellX = rx * (roomWidth + wallWidth);
      int cellY = ry * (roomHeight + wallHeight);

      // Draw North Wall
      for (int cy = 0; cy < wallHeight; ++cy) {
        for (int cx = 0; cx < wallWidth * 2 + roomWidth; ++cx) {
          int x = cellX + cx;
          int y = cellY + cy;
          if (x < gridWidth && y < gridHeight)
            grid[y][x] = WALL_CHAR;
        }
      }
      // Draw South Wall
      for (int cy = 0; cy < wallHeight; ++cy) {
        for (int cx = 0; cx < wallWidth * 2 + roomWidth; ++cx) {
          int x = cellX + cx;
          int y = cellY + roomHeight + wallHeight + cy;
          if (x < gridWidth && y < gridHeight)
            grid[y][x] = WALL_CHAR;
        }
      }
      // Draw West Wall
      for (int cy = 0; cy < wallHeight * 2 + roomHeight; ++cy) {
        for (int cx = 0; cx < wallWidth; ++cx) {
          int x = cellX + cx;
          int y = cellY + cy;
          if (x < gridWidth && y < gridHeight)
            grid[y][x] = WALL_CHAR;
        }
      }
      // Draw East Wall
      for (int cy = 0; cy < wallHeight * 2 + roomHeight; ++cy) {
        for (int cx = 0; cx < wallWidth; ++cx) {
          int x = cellX + roomWidth + wallWidth + cx;
          int y = cellY + cy;
          if (x < gridWidth && y < gridHeight)
            grid[y][x] = WALL_CHAR;
        }
      }
    }
  }

  // Now clear openings
  for (int ry = 0; ry < roomsTall; ++ry) {
    for (int rx = 0; rx < roomsWide; ++rx) {
      const Maze::Node *pNode = Maze::MazeHelper::findNode(pRoot, rx, ry);
      if (!pNode)
        continue;

      int cellX = rx * (roomWidth + wallWidth);
      int cellY = ry * (roomHeight + wallHeight);

      // Door dimensions (assume same as room for simplicity or smaller?)
      // GDMaze uses mDoorWidth/Height. Let's assume door size = room size for
      // full opening? Or just 1? User didn't specify door size. GDMaze
      // defaults? Let's use door size = room size - 2? Or just 1. Let's use
      // door size = 1 for visibility.
      int doorWidth = 1;
      int doorHeight = 1;

      int offsetX = wallWidth + (roomWidth - doorWidth) / 2;
      int offsetY = wallHeight + (roomHeight - doorHeight) / 2;

      // North exit (0)
      if (pNode->isOpen(0)) {
        for (int cy = 0; cy < wallHeight; ++cy) {
          for (int cx = 0; cx < doorWidth; ++cx) {
            int x = cellX + offsetX + cx;
            int y = cellY + cy;
            if (x < gridWidth && y < gridHeight)
              grid[y][x] = FLOOR_CHAR;
          }
        }
      }
      // South exit (1)
      if (pNode->isOpen(1)) {
        for (int cy = 0; cy < wallHeight; ++cy) {
          for (int cx = 0; cx < doorWidth; ++cx) {
            int x = cellX + offsetX + cx;
            int y = cellY + roomHeight + wallHeight + cy;
            if (x < gridWidth && y < gridHeight)
              grid[y][x] = FLOOR_CHAR;
          }
        }
      }
      // East exit (2)
      if (pNode->isOpen(2)) {
        for (int cx = 0; cx < wallWidth; ++cx) {
          for (int cy = 0; cy < doorHeight; ++cy) {
            int x = cellX + roomWidth + wallWidth + cx;
            int y = cellY + offsetY + cy;
            if (x < gridWidth && y < gridHeight)
              grid[y][x] = FLOOR_CHAR;
          }
        }
      }
      // West exit (3)
      if (pNode->isOpen(3)) {
        for (int cx = 0; cx < wallWidth; ++cx) {
          for (int cy = 0; cy < doorHeight; ++cy) {
            int x = cellX + cx;
            int y = cellY + offsetY + cy;
            if (x < gridWidth && y < gridHeight)
              grid[y][x] = FLOOR_CHAR;
          }
        }
      }
    }
  }

  // Print grid
  for (const auto &row : grid) {
    std::cout << row << std::endl;
  }

  delete pMaze;
  return 0;
}
