# Maze Solving Algorithms
There are now two distinct types of maze solving algorithms. The first type of algorithm is intended to be employed within a maze. It denotes that the visitor who is about to explore a maze has no prior knowledge of its layout. The second group of algorithms, on the other hand, is intended to be employed by a computer or a human who is familiar with labyrinth construction and can study it off-line.
In the latter situation, the most efficient path out may be established before entering the maze. All dead ends, which represent the greatest difficulty to the first group of algorithms, are automatically avoided.

Unfortunately, real-world situations are not as pleasant as the assumptions of the second type of algorithm. In the majority of circumstances, a visitor has no perspective on the entire region that should be examined. As a result, algorithms that are beneficial in real-world circumstances fall into the first of the major groups.

In this project, we are going to solve a maze without a prior knowledge of of its layout. So we did some research on three well-known algorithms to find the best fit for our project.

## Wall Follower
The most frequent maze solution method is the wall follower, whose fundamental principle is to follow walls in the maze region. The solver robot looks at the right or left wall and goes around the maze till it finds its way out. The wall follower has two alternative rules: the left-hand rule and right-hand rule. Depending on the regulation, the turning precedence will be to the left or to the right.
The corresponding algorithm involves the following steps:
- **Step 1**: Sense the left wall.
- **Step 2**: If left wall present, then set _flag1_ to 1, else set
_flag1_ to 0.
- **Step 3**: If _flag1_ is 1, then go to **Step 4**, else turn left by *90* degrees.
- **Step 4**: Sense the front wall.
- **Step 5**: If front wall present, then set _flagf_ to 1, else set
_flagf_ to 0.
- **Step 6**: If _flagf_ is 0, then move straight, else turn right by *90* degrees.
- **Step 7**: Return to **Step 1**

While this approach assures that the maze is traversed in an orderly manner, it does not necessarily guarantee that the route out will be discovered. If the exit from a labyrinth is positioned inside the maze, and the maze construction incorporates free standing walls, a visitor may become trapped in an unending loop, preventing them from finding the answer.

## Pledge Algorithm
The pledge maze algorithm is a maze traversing algorithm that is used when the walls are disjointed; it keeps all obstacles on either the left or right hand side and uses "Counter" to keep track of all turns.
* Right turns increment the counter by one +1.
* Left turns decrement the counter by one -1.

When the counter reaches **Zero** the robot has traversed the obstacle, and it is continues on its path.
The next figure describes the scheme of the Algorithm [1]:

![Pledge Algorithm 
Scheme](https://www.researchgate.net/profile/Krzysztof-Cetnarowicz/publication/220855794/figure/fig1/AS:649286383566856@1531813345936/Pledge-algorithm-A-Left-or-Right-Hand-side_W640.jpg)

An example of the Algorithm Execution [2]:

![Example of Execution](https://www.researchgate.net/profile/Nageswara-Rao-8/publication/2377958/figure/fig3/AS:652608557555713@1532605413576/Execution-of-Pledge-algorithm.png)

***Important Note:*** This algorithm allows a person with a compass to find their way from any point inside to an outer exit of any finite two-dimensional maze, regardless of the initial position of the solver. However, this algorithm will not work in doing the reverse, namely finding the way from an entrance on the outside of a maze to some end goal within it. [3]

Some links to YouTube Videos that explain the algorithm:

1. [Link1](https://www.youtube.com/watch?v=_KxAMV-2tKU)
1. [Link2](https://www.youtube.com/watch?v=6da4ZbJlphw)
 
 
## Trémaux's algorithm
Charles Pierre Trémaux developed Trémaux's algorithm, which is an effective approach to find your way out of a maze that requires marking a path with lines on the floor. It is guaranteed to work for all mazes with well-defined passages, but it cannot always discover the shortest path.Trémaux's algorithm is also known as Depth First Search (DFS). DFS has been widely recognized as a powerful technique for solving various graph problems. The method, however, has been used to traverse mazes since the nineteenth century.
#### - How the algorithm work:
A passageway's entrance is either unmarked, marked only once, or marked twice. Because a junction may have many entrances and a passage includes entrances at both ends, it is important to note that marking an entrance is not the same as marking a junction or a passage. Dead ends are junctions with only one entrance (imagine there being a room at each dead end). The algorithm operates in accordance with the guidelines below:
- Leave a mark at the entrance each time you travel through a passageway, whether it is to enter or exit a junction.
- When you reach a crossroads, choose an exit using the first relevant rule listed below:
   - If there are any unmarked entrances, choose one at random if only the entrance you just came from is marked. If there are no indicated entrances at all and you're starting in the center of the maze, this rule still holds true.
   - If an entrance isn't marked twice, choose the one you just came from. When you come to a dead end, this rule will always apply.
   - Choose any door with the fewest marks (zero if possible, else one).
   
When you've finally found the answer, entrances that have been marked exactly once will show you how to go back to the beginning. This approach will return you to the beginning, where all entrances are marked twice, if there is no exit. Each tunnel is traversed in this instance exactly twice, once in each direction. Bidirectional double-tracing is the name of the resulting walk.

#### - Time Complexity:
The temporal complexity of DFS is O(V), where V is the number of vertices, when the full maze is traversed.

## Conclusion 
After analysing the three algorithms above we decided to go with LSRB algorithm,  In "LSRB" L stands for 'LEFT', S for 'STRAIGHT', R for RIGHT, and B for 'BACK' or BACKWARD. These LEFT, RIGHT, STRAIGHT, and BACK are the directions that the robot follows. This Algorithm is simple and straight forward. In this algorithm LEFT direction has the highest priority and the BACK (U-Turn) direction has the least priority. Let's see what this algorithm looks like:
- **Step 1**: Always follow **LEFT** whenever there is a turn possible
- **Step 2**: If **LEFT** is not possible go **STRAIGHT**.
- **Step 3**: If **LEFT** and **STRAIGHT** both are not possible take **RIGHT**.
- **Step 4**: if **LEFT**, **STRAIGHT**, and **RIGHT** are not possible go **BACK**( or it means take a **U-Turn**).

This means that whenever robot is on a turning point or an intersection it will always go **LEFT** whenever possible. if **LEFT** is not possible then **STRAIGHT** and if both are not possible then **RIGHT**. If all the three turns are not possible then and then only **BACK**. This is the only thing you need to understand the **LSRB** algorithm. Based on this there are 8 scenarios possible. Let's take a look one by one:

1. **Simple or Straight lane**: Here **LEFT** is not possible but the **STRAIGHT** path is. so robot will follow the **Straight** Path.
2. **Left Turn(left only)**: As the name suggests it's a left turn so **LEFT** is possible here. According to **LSRB** algorithm robot should follow left whenever possible. so robot will take left turn here.
3. **Right Turn(rightonly)**: Again as the name suggests it's a right turn so **LEFT** and **STRAIGHT** path are both not possible so according to **LSRB** Algorithm robot will take Right turn.
4. **T intersection(T)**: This intersection has a T like shape so it's called T intersection. Here Robot can take the left turn as you can see in the picture. So by algorithm Robot will take a Left Turn.
5. **Left T Intersection(straightor left)**: Again by image, we can see Robot can take left path so Robot will take left turn here.
6. **Right T Intersection(Straightor right)**: Here Left is not possible but the **Straight** path is. so the robot will take left turn
7. **Dead End**: Here **LEFT**, **STRAIGHT**, and **RIGHT** all three are not possible. so Robot will take U Turn here.
8. **Four lane intersection(Cross)****: Here again by the image Left turn is possible so the robot will take left turn here.
9. **End of Maze**: Here the maze ends so the robot will stop here.

## References
 1. [Link 1](https://www.researchgate.net/publication/220855794_From_Algorithm_to_Agent/figures) Accessed 19/11/2022
 1. [Link 2](https://www.researchgate.net/publication/2377958_Robot_Navigation_in_Unknown_Terrains_Introductory_Survey_of_Non-Heuristic_Algorithms/figures?lo=1) Accessed 19/11/2022
 1. [Link 3](https://en.m.wikipedia.org/wiki/Maze-solving_algorithm?fbclid=IwAR0ksshjpM7Vob-W-Kd9GWlmo4l6kpmZLtCUSvDcK_eifCKcc6PLHndjVWI) Accessed 19/11/2022