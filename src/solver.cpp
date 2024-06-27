/*
this is the code for solving and handling of the maze!

GAME PLAN:
  when going through maze:
    - remove wall from closed maze where gap found
    - add wall to open maze where wall found
    - flood open maze for optimistic move
    - 
  when reached the goal:
    - explore (pick other direction)
  when back to start:
    - flood closed maze to find best path back
    - run
*/

#include <map>
#include <array>
#include <queue>

typedef unsigned char uint8_t;

#define NORTH 0 // cardinal constants
#define SOUTH 1
#define WEST 2
#define EAST 3
#define mazeSize 16
#define forside(sid) for (int sid = 0; sid < mazeSize; sid++) // wonky way to iter over maze

typedef std::array<int, 2> cell_t;
cell_t goal = {8, 7};
std::array rowDelta = {-1,1,0,0}, colDelta = {0,0,-1,1}; // used to get the coords of a neighbor in NSWE
std::array<uint8_t,4> oppositeDir = {1,0,3,2};

std::map<cell_t, uint8_t> maze; // holds walls like maze[{row,col}] : 0b EWSN EWSN
typedef struct {uint8_t dist; uint8_t dir;} cellData_t; // type for a cell's flood data
std::map<cell_t, cellData_t> oFlood, cFlood; // to store most recent open and closed flood data

// direction is 0 1 2 3 [from right caus byte] for NSWE
void addWall(cell_t cell, uint8_t direction, uint8_t mazeType) {
  if (maze.find(cell)!=maze.end()) maze[cell] |= 1<<(direction+mazeType);
  cell_t neii = {cell[0]+rowDelta[direction],cell[1]+colDelta[direction]};
  if (maze.find(neii)!=maze.end()) maze[neii] |= 1<<(oppositeDir[direction]+mazeType);
}

// check for exit
bool hasExit(cell_t cell, uint8_t direction, uint8_t mazeType) {
  return ((maze[cell]&(1<<(direction+mazeType)))==0);
}

// get cell_t of neighbor in specific direction
cell_t neighbor(cell_t current, uint8_t direction) {
  return {
    current[0]+rowDelta[direction], current[1]+colDelta[direction]
  };
}

void setupMaze() {
  forside(row) forside(col) maze[{row, col}] = 0b11110000;
  forside(each) {
    addWall({0, each}, NORTH, 0);
    addWall({mazeSize-1, each}, SOUTH, 0);
    addWall({each, 0}, WEST, 0);
    addWall({each, mazeSize-1}, EAST,0);
  }
  addWall({mazeSize-1,0}, EAST, 0);
}

// floodfill a maze and store the results
void floodMaze(cell_t target, uint8_t mazeType) {
  auto &floodData = oFlood;
  if (mazeType) auto &floodData = cFlood;
  floodData[target].dist = 0; // target is at a distance of 0u from itself
  std::priority_queue<cell_t> queue;
  queue.push(target);
  while (queue.size()) {
    cell_t currentCell = queue.top(); queue.pop();
    for (uint8_t direction: {NORTH, SOUTH, WEST, EAST}) {
      if (hasExit(currentCell, direction, mazeType)) {
        cell_t nextCell = neighbor(currentCell, direction);
        uint8_t cost = floodData[currentCell].dist;
        if ((target==goal)&&(nextCell[0]==7||nextCell[0]==8)&&(nextCell[1]==7||nextCell[1]==8)) cost = 0;
        else if (direction!=floodData[currentCell].dir) cost += 2;
        else cost += 1;
        if (cost<floodData[nextCell].dist) {
          floodData[nextCell] = {cost, oppositeDir[direction]};
          queue.push(nextCell);
        }
      }
    }
  }
}