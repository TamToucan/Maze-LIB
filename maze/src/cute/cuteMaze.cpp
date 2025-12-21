#include <cute.h>
#include "MazeHelper.h"
#include "MazeData.h"

// This class is just a wrapper to prove both libraries can talk to each other
class CuteMaze {

public:
    CuteMaze(int width, int height) {
	    pMaze = Maze::MazeHelper::generateSquareMaze(width, height, 0, 0, false, false, false, 50, 4242);
    }
    ~CuteMaze() {
	    delete pMaze;
    }

    void draw() {
	    Cute::draw_push_color(Cute::color_white());
	    Cute::draw_line(Cute::v2(0,0), Cute::v2(100, 100), 2.0f);
    }

private:
    Maze::MazeData* pMaze;
};
