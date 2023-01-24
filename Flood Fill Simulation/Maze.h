#include <vector>

using namespace std;

class Maze{
public:
    static void initializeMaze(vector<vector<Cell>> &map, int mazeSize){

        int centerX = mazeSize/2, centerY= mazeSize/2;
        for (int i = 0; i < mazeSize; i++)
        {
            map[i].reserve(mazeSize);
        }
        for (int i = 0; i < mazeSize; i++)
        {
            for (int j = 0; j < mazeSize; j++)
            {
                map[i][j].setcellAddress(Coordinates{i, j});
                int minX = min(std::abs(i - centerX), abs(i - (centerX - 1)));

                int minY = min(std::abs(j - centerY), abs(j - (centerY - 1)));
                int floodFillCost = minX + minY;
                map[i][j].setFloodFillCost(floodFillCost);
                map[i][j].setReverseFloodFillCost(i + j);
            }
        }

        map[0][0].setSouthWall(true); // leads to errors
    }
};