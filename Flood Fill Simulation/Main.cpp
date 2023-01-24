#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include "API.h"
#include "Cell.h"
#include "Coordinates.h"
#include "Maze.h"
#include "MazeGraph.h"

using namespace std;


template <typename T, typename priority_t>
struct PriorityQueue
{
    typedef std::pair<priority_t, T> PQElement;
    std::priority_queue<PQElement, std::vector<PQElement>,
            std::greater<PQElement>>
            elements;

    inline bool empty() const
    {
        return elements.empty();
    }
    inline int size() const
    {
        return elements.size();
    }

    inline void put(T item, priority_t priority)
    {
        elements.emplace(priority, item);
    }

    T get()
    {
        T best_item = elements.top().second;
        elements.pop();
        return best_item;
    }
};


void log(const string& text);
void exploreCell(vector<vector<Cell>> &map, int x, int y);
void findStartToGoal(vector<vector<Cell>> &map, Coordinates goal);
bool isGoal(int x, int y);
bool matchGoal(Coordinates current, Coordinates goal);
Cell &getFrontCell(vector<vector<Cell>> &map);
Cell &getLeftCell(vector<vector<Cell>> &map);
Cell &getRightCell(vector<vector<Cell>> &map);
void getMinDistanceDirection(vector<vector<Cell>> &map, int &minDistance, char &minDirection);
void floodOpenNeighbours(vector<vector<Cell>> &map, Coordinates goal);
void moveInDirection(vector<vector<Cell>> &map, char direction);
void updatePosition(char currentMove);
void re_estimateCosts(vector<vector<Cell>> &map, Coordinates goal);
void route(MazeGraph &graph,
           vector<vector<Cell>> &map,
           Coordinates start,
           std::stack<Coordinates> &path);
void followPath(vector<vector<Cell>> &map, std::stack<Coordinates> &path);
void moveToNeighbouringCoordinates(vector<vector<Cell>> &map, int targetX, int targetY);
void resetCost();
const int mazeSize = 16; //16x16 maze

char direction = 'N'; // current direction

enum mode
{
    FIND_CENTRE,
    FIND_START
};

mode searchMode = FIND_CENTRE;
int x=0, y=0;
Coordinates came_from[16][16];
int cost_so_far [16][16] = {INT_MAX};
bool done = false; // done flood fill
int main(int argc, char* argv[]) {
    API::setText(0, 0, "Starting Point");

    vector<vector<Cell>> map(mazeSize, vector<Cell>(mazeSize));

    resetCost();
    log("Started flood fill to explore  the maze...");

    Maze::initializeMaze(map, mazeSize);

    findStartToGoal(map, Coordinates{8, 8});

    // get all unexplored cells
    queue<Coordinates> unexploredCells;
    for (int i = 0; i < mazeSize; i++)
    {
        for (int j = 0; j < mazeSize; j++)
        {
            if (!map[i][j].getHasBeenExplored())
            {
                unexploredCells.push(Coordinates{i, j});
            }
        }
    }

    // generate complete map while (!unexploredCells.empty())
    while (!unexploredCells.empty())
    {
        Coordinates goal = unexploredCells.front();
        unexploredCells.pop();
        if (!map[goal.x][goal.y].getHasBeenExplored())
        {
            re_estimateCosts(map, goal);
            findStartToGoal(map, goal);
        }
    }

    // Ended flood fill now return to start position
    done = true;
    MazeGraph graph(map, mazeSize, mazeSize);
    stack<Coordinates> path;
    log("Back to starting point...");

    searchMode = FIND_START;
    route(graph, map, Coordinates{x, y}, path);
    followPath(map, path);

    // Route to goal
    resetCost();
    searchMode = FIND_CENTRE;
    route(graph, map, Coordinates{0, 0}, path);

    API::ackReset();
    direction = 'N';
    followPath(map, path);


    log("Ended");
}

void log(const string& text) {
    cerr << text << endl;
}
void resetCost(){
    for (auto & i : cost_so_far) {
        for (int & j : i) {
            j=INT_MAX;
        }
    }
}
void route(MazeGraph &graph,
           vector<vector<Cell>> &map,
           Coordinates start,
           std::stack<Coordinates> &path)
{


    PriorityQueue<Coordinates, int> frontier;
    frontier.put(start, 0);

    came_from[start.x][start.y] = start;
    cost_so_far[start.x][start.y] = 0;
    Coordinates G = {-1, -1};
    while (!frontier.empty())
    {
        // get

        Coordinates current = frontier.get();
        if (isGoal(current.x, current.y))
        {
            G = current;
            break;
        }

        for (Coordinates next : graph.neighbors(current))
        {
            int new_cost = cost_so_far[current.x][current.y] + graph.cost(map, current, next);
            if (cost_so_far[next.x][next.y] == INT_MAX || new_cost < cost_so_far[next.x][next.y])
            {
                cost_so_far[next.x][next.y] = new_cost;

                frontier.put(next, new_cost);
                came_from[next.x][next.y] = current;
            }
        }
    }

    // reconstrucnt path
    Coordinates current = G;
    int t = 0;
    while (!(current == start))
    {
        path.push(current);
        current = came_from[current.x][current.y];
        t++;
    }

    path.push(current);
}
void findStartToGoal(vector<vector<Cell>> &map, Coordinates goal)
{

    bool destinationFound = false;
    int minDistance;
    char minDirection;
    // alias for conveneience
    while (!destinationFound)
    {
        exploreCell(map, x, y); // explore current cell
        minDistance = mazeSize * 5;

        // if goal fount-> exit while loop
        if (matchGoal(Coordinates{x, y}, goal))
        {
            destinationFound = true;
        }
        if (!destinationFound)
        {
            getMinDistanceDirection(map, minDistance, minDirection);
            // check if reflooding is required

            if ((map[x][y].getFloodFillCost() != 1 + minDistance))
            {
                // reflood
                floodOpenNeighbours(map, goal);

                // after reflooding get new min distance neighbours
                getMinDistanceDirection(map, minDistance, minDirection); //get neighbour with lowest distance
            }

            moveInDirection(map, minDirection);
        }
    }
}

void exploreCell(vector<vector<Cell>> &map, int x, int y)
{
    // map[x][y].setVisited(); // first visit is straight, then right, then left, then back
    if (map[x][y].getHasBeenExplored())
    {
        return;
    }

    map[x][y].sethasBeenExplored(true);
    switch (direction)
    {
        case 'N':

            map[x][y].setNorthWall(API::wallFront());

            map[x][y].setEastWall(API::wallRight());
            map[x][y].setWestWall(API::wallLeft());

            break;
        case 'S':

            map[x][y].setSouthWall(API::wallFront());
            map[x][y].setEastWall(API::wallLeft());
            map[x][y].setWestWall(API::wallRight());
            break;
        case 'W':

            map[x][y].setWestWall(API::wallFront());
            map[x][y].setSouthWall(API::wallLeft());
            map[x][y].setNorthWall(API::wallRight());
            break;
        case 'E':

            map[x][y].setEastWall(API::wallFront());
            map[x][y].setNorthWall(API::wallLeft());
            map[x][y].setSouthWall(API::wallRight());
            break;
        default:
            int a = 0;
    }
}

bool isGoal(int x, int y)
{
    if (searchMode == FIND_CENTRE)
        return ((x == 7 || x == 8) && (y == 7 || y == 8));
    else
        return ((x == 0) && (y == 0));
}

bool matchGoal(Coordinates current, Coordinates goal)
{
    return (current.x == goal.x && current.y == goal.y);
}
bool isSafe(int X, int Y)
{
    if (X < 0 || X > mazeSize - 1)
        return false;
    if (Y < 0 || Y > mazeSize - 1)
        return false;
    return true;
}
void getMinDistanceDirection(vector<vector<Cell>> &map, int &minDistance, char &minDirection)
{
    switch (searchMode)
    {
        case FIND_CENTRE:
            if (!API::wallFront())
            {
                Cell front = getFrontCell(map);

                if (minDistance > front.getFloodFillCost())
                {
                    minDistance = front.getFloodFillCost();
                    minDirection = 'f';
                }
            }
            if (!API::wallLeft())
            {
                Cell left = getLeftCell(map);
                if (minDistance > left.getFloodFillCost())
                {
                    minDistance = left.getFloodFillCost();
                    minDirection = 'l';
                }
            }
            if (!API::wallRight())
            {
                Cell right = getRightCell(map);
                if (minDistance > right.getFloodFillCost())
                {
                    minDistance = right.getFloodFillCost();
                    minDirection = 'r';
                }
            }
            if (map[x][y].getPrevVisitedCell() != nullptr)
            {
                Cell *back = map[x][y].getPrevVisitedCell();
                if (minDistance > back->getFloodFillCost())
                {
                    minDistance = back->getFloodFillCost();
                    minDirection = 'b';
                }
            }
            break;
        case FIND_START:
            if (!API::wallFront())
            {
                Cell front = getFrontCell(map);

                if (minDistance > front.getReverseFloodFillCost())
                {
                    minDistance = front.getReverseFloodFillCost();
                    minDirection = 'f';
                }
            }
            if (!API::wallLeft())
            {
                Cell left = getLeftCell(map);
                if (minDistance > left.getReverseFloodFillCost())
                {
                    minDistance = left.getReverseFloodFillCost();
                    minDirection = 'l';
                }
            }
            if (!API::wallRight())
            {
                Cell right = getRightCell(map);
                if (minDistance > right.getReverseFloodFillCost())
                {
                    minDistance = right.getReverseFloodFillCost();
                    minDirection = 'r';
                }
            }
            if (map[x][y].getPrevVisitedCell() != nullptr)
            {
                Cell *back = map[x][y].getPrevVisitedCell();
                if (minDistance > back->getReverseFloodFillCost())
                {
                    minDistance = back->getReverseFloodFillCost();
                    minDirection = 'b';
                }
            }

            break;
        default:
            int a = 0; // do nothing
    }
}
void floodOpenNeighbours(vector<vector<Cell>> &map, Coordinates goal)
{

    stack<Coordinates> floodStack;
    int minDistance = mazeSize * 2;
    char minDirection;
    int cellX, cellY;
    floodStack.push(Coordinates({x, y}));
    while (!floodStack.empty())
    {
        cellX = floodStack.top().x;
        cellY = floodStack.top().y;

        floodStack.pop();
        if (matchGoal(Coordinates{cellX, cellY}, goal))
            continue;

        Cell cell = map[cellX][cellY];

        if (cell.getHasBeenExplored())
        {

            minDistance = mazeSize * 2;
            int D1 = (!map[cellX][cellY].hasNorthWall()) ? map[cellX][cellY + 1].getFloodFillCost() : mazeSize * 2;
            int D2 = (!map[cellX][cellY].hasSouthWall()) ? map[cellX][cellY - 1].getFloodFillCost() : mazeSize * 2;
            int D3 = (!map[cellX][cellY].hasWestWall()) ? map[cellX - 1][cellY].getFloodFillCost() : mazeSize * 2;
            int D4 = (!map[cellX][cellY].hasEastWall()) ? map[cellX + 1][cellY].getFloodFillCost() : mazeSize * 2;
            minDistance = min(D1, D2);
            minDistance = min(minDistance, D3);
            minDistance = min(minDistance, D4);
            //----------add to stack

            if (map[cellX][cellY].getFloodFillCost() != 1 + minDistance)
            {
                map[cellX][cellY].setFloodFillCost(1 + minDistance);

                if (!map[cellX][cellY].hasNorthWall())
                {
                    floodStack.push(Coordinates{cellX, cellY + 1});
                }
                if (!map[cellX][cellY].hasSouthWall())
                {
                    floodStack.push(Coordinates{cellX, cellY - 1});
                }
                if (!map[cellX][cellY].hasWestWall())
                {
                    floodStack.push(Coordinates{cellX - 1, cellY});
                }
                if (!map[cellX][cellY].hasEastWall())
                {
                    floodStack.push(Coordinates{cellX + 1, cellY});
                }
            }
        }
        else
        {

            int minD = mazeSize * 2;
            // a cell that has not been explored has no walls so all neighbours are accessible

            int d1 = isSafe(cellX + 1, cellY) ? map[cellX + 1][cellY].getFloodFillCost() : mazeSize * 2;
            int d2 = isSafe(cellX - 1, cellY) ? map[cellX - 1][cellY].getFloodFillCost() : mazeSize * 2;
            int d3 = isSafe(cellX, cellY + 1) ? map[cellX][cellY + 1].getFloodFillCost() : mazeSize * 2;
            int d4 = isSafe(cellX, cellY - 1) ? map[cellX][cellY - 1].getFloodFillCost() : mazeSize * 2;
            minD = min(d1, d2);
            minD = min(minD, d3);
            minD = min(minD, d4);
            if (map[cellX][cellY].getFloodFillCost() != 1 + minD)
            {
                map[cellX][cellY].setFloodFillCost(1 + minD);

                if (isSafe(cellX + 1, cellY))
                {
                    floodStack.push(Coordinates{cellX + 1, cellY});
                }
                if (isSafe(cellX - 1, cellY))
                {
                    floodStack.push(Coordinates{cellX - 1, cellY});
                }
                if (isSafe(cellX, cellY + 1))
                {
                    floodStack.push(Coordinates{cellX, cellY + 1});
                }
                if (isSafe(cellX, cellY - 1))
                {
                    floodStack.push(Coordinates{cellX, cellY - 1});
                }
            }
        }
    }
}
void moveInDirection(vector<vector<Cell>> &map, char direction)
{
    // move to the  neighbouring cell with the lowest distance cost
    int prevX = x, prevY = y;
    if (direction == 'f')
    {
        API::moveForward();
        updatePosition('f');
    }
    else if (direction == 'l')
    {
        API::turnLeft();
        updatePosition('l');
        API::moveForward();
        updatePosition('f');
    }

    else if (direction == 'r')
    {

        API::turnRight();
        updatePosition('r');
        API::moveForward();
        updatePosition('f');
    }
    else if (direction == 'b')
    {
        API::turnRight();
        updatePosition('r');
        API::turnRight();
        updatePosition('r');
        API::moveForward();
        updatePosition('f');
    }

    map[x][y].setPrevVisitedCell(&map[prevX][prevY]);

    if(!done)
        API::setColor(x, y, 'Y');
    else if(searchMode == FIND_CENTRE)
        API::setColor(x,y,'R');
    else
        API::setColor(x, y, 'B');
}
void updatePosition(char currentMove)
{
    // update the direction and coordinates
    switch (direction)
    {
        case 'N':
            if (currentMove == 'l')
                direction = 'W';
            else if (currentMove == 'r')
                direction = 'E';

            else if (currentMove == 'f')
                ++y;
            break;
        case 'S':
            if (currentMove == 'l')
                direction = 'E';
            else if (currentMove == 'r')
                direction = 'W';
            else if (currentMove == 'f')
                --y;
            break;
        case 'E':
            if (currentMove == 'l')
                direction = 'N';
            else if (currentMove == 'r')
                direction = 'S';
            else if (currentMove == 'f')
                ++x;
            break;
        case 'W':
            if (currentMove == 'l')
                direction = 'S';
            else if (currentMove == 'r')
                direction = 'N';
            else if (currentMove == 'f')
                --x;
            break;
        default:
            x = 0, y = 0;
    }
}
void re_estimateCosts(vector<vector<Cell>> &map, Coordinates goal)
{
    for (int i = 0; i < mazeSize; i++)
    {
        for (int j = 0; j < mazeSize; j++)
        {

            int X = abs(i - goal.x);

            int Y = abs(j - goal.y);
            int floodFillCost = X + Y;
            map[i][j].setFloodFillCost(floodFillCost);
        }
    }
}

void followPath(vector<vector<Cell>> &map, std::stack<Coordinates> &path)
{
    // while stack not empty get next coordinate
    // move to next neighbour
    while (!path.empty())
    {
        moveToNeighbouringCoordinates(map, path.top().x, path.top().y);
        path.pop();
    }
}
void moveToNeighbouringCoordinates(vector<vector<Cell>> &map, int targetX, int targetY)
{
    switch (direction)
    {

        case 'N':
            if (targetX == (x - 1))
                moveInDirection(map, 'l');
            else if (targetX == (x + 1))
                moveInDirection(map, 'r');
            else if (targetY == (y + 1))
                moveInDirection(map, 'f');
            else if (targetY == (y - 1))
                moveInDirection(map, 'b');
            break;

        case 'S':
            if (targetX == (x - 1))
                moveInDirection(map, 'r');
            else if (targetX == (x + 1))
                moveInDirection(map, 'l');
            else if (targetY == (y + 1))
                moveInDirection(map, 'b');
            else if (targetY == (y - 1))
                moveInDirection(map, 'f');
            break;

        case 'E':
            if (targetX == (x - 1))
                moveInDirection(map, 'b');
            else if (targetX == (x + 1))
                moveInDirection(map, 'f');
            else if (targetY == (y + 1))
                moveInDirection(map, 'l');
            else if (targetY == (y - 1))
                moveInDirection(map, 'r');
            break;

        case 'W':
            if (targetX == (x - 1))
                moveInDirection(map, 'f');
            else if (targetX == (x + 1))
                moveInDirection(map, 'b');
            else if (targetY == (y + 1))
                moveInDirection(map, 'r');
            else if (targetY == (y - 1))
                moveInDirection(map, 'l');
            break;

        default:
            return;
    }
}

// doesnt check if coordinates are within boundaries
Cell &getFrontCell(vector<vector<Cell>> &map)
{

    switch (direction)
    {
        case 'N':
            return map[x][y + 1];
            break;
        case 'S':
            return map[x][y - 1];
            break;
        case 'E':
            return map[x + 1][y];
            break;
        case 'W':
            return map[x - 1][y];
            break;
        default:
            return map[x][y];
    }
}

Cell &getLeftCell(vector<vector<Cell>> &map)
{

    switch (direction)
    {
        case 'N':
            return map[x - 1][y];
            break;
        case 'S':
            return map[x + 1][y];
            break;
        case 'E':
            return map[x][y + 1];
            break;
        case 'W':
            return map[x][y - 1];
            break;
        default:
            return map[x][y];
            ;
    }
}

Cell &getRightCell(vector<vector<Cell>> &map)
{

    switch (direction)
    {
        case 'N':
            return map[x + 1][y];
            break;
        case 'S':
            return map[x - 1][y];
            break;
        case 'E':
            return map[x][y - 1];
            break;
        case 'W':
            return map[x][y + 1];
            break;
        default:
            return map[x][y];
    }
}

