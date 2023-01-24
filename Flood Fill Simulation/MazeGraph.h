
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <queue>
#include <limits>


using namespace std;


class MazeGraph
{
public:
    MazeGraph()
    {
        width = height = V = 0;
    }
    MazeGraph(vector<vector<Cell>> &map, int M, int N)
    {
        width = M;
        height = N;
        V = M * N;

        for (int i = 0; i < V; i++)
        {
        }
        for (int x = 0; x < M; x++)
        {
            for (int y = 0; y < N; y++)

            {

                if (!map[x][y].hasNorthWall() && inBounds(x, y + 1))
                {
                    this->addEdge(Coordinates{x, y}, Coordinates{x, y + 1});
                }
                if (!map[x][y].hasSouthWall() && inBounds(x, y - 1))
                {
                    this->addEdge(Coordinates{x, y}, Coordinates{x, y - 1});
                }
                if (!map[x][y].hasWestWall() && inBounds(x - 1, y))
                {
                    this->addEdge(Coordinates{x, y}, Coordinates{x - 1, y});
                }
                if (!map[x][y].hasEastWall() && inBounds(x + 1, y))
                {
                    this->addEdge(Coordinates{x, y}, Coordinates{x + 1, y});
                }
            }
        }
    }

    void addEdge(Coordinates u, Coordinates v)
    {
//TODO
        this->edges[u.x][u.y].push_back(v);
    }

    std::vector<Coordinates> neighbors(Coordinates id)
    {
        return edges[id.x][id.y];
    }
    void printGraph()
    {
        for (int i = 0; i < 16; ++i) {
            for (int j = 0; j < 16; ++j) {
                cerr << i <<" "<<j << ":\n";

                for (auto neighbour : edges[i][j])
                {
                    std::cerr << "(" + std::to_string(neighbour.x) + "," + std::to_string(neighbour.y) + ") ";
                }
                std::cerr << "\n\n";
            }
        }
    }
    int cost(vector<vector<Cell>> &map, Coordinates from, Coordinates to) const
    {
        // cost of moving along each edge would be t where t is the time but since we dont know
        //if there is a turn involved, cost will be constant
        return map[from.x][from.y].getHasBeenExplored() ? 1.0 : 1000;
        ;
    }
    // check for bounds where required because robot knows the maze area
    bool inBounds(int X, int Y)
    {
        if (X < 0 || X > width - 1)
            return false;
        if (Y < 0 || Y > height - 1)
            return false;
        return true;
    }

private:
    int width, height; //15 for a 16 by 16 maze
    int V;             // number of vertices
    vector<Coordinates> edges [16][16];
};
