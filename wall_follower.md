## Wall Follower
The most frequent maze solution method is the wall follower, whose fundamental principle is to follow walls in the maze region. The solver robot looks at the right or left wall and goes around the maze till it finds its way out. The wall follower has two alternative rules: left-hand rule and right-hand rule. Depending on the regulation, the turning precedence will be to the left or to the right.
The corresponding algorithm involves the following steps:
- **Step 1**: Sense the left wall.
- **Step 2**: If left wall present, then set _flag1_ to 1, else set
_flag1_ to 0.
- **Step 3**: If _flag1_ is 1, then go to **Step 4**, else turn left by
*90* degrees.
- **Step 4**: Sense the front wall.
- **Step 5**: If front wall present, then set _flagf_ to 1, else set
_flagf_ to 0.
- **Step 6**: If _flagf_ is 0, then move straight, else turn right by
*90* degrees.
- **Step 7**: Return to **Step 1**

While this approach assures that the maze is traversed in an orderly manner, it does not necessarily guarantee that the route out will be discovered. If the exit from a labyrinth is positioned inside the maze, and the maze construction incorporates free standing walls, a visitor may become trapped in an unending loop, preventing them from finding the answer.
