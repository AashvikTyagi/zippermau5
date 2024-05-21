#include <Arduino.h> // for hardware abstraction
#include <map> // bunch of stl libs
#include <array>
#include <string>
#include <vector>
#include <queue>

#define mazeSize 16
#define forside(sid) for (int sid = 0; sid < mazeSize; sid++) // wonky way for quick maze map traversal

struct cellData {float distance;  std::string walls; char direction;};
std::map<std::array<int, 2>, cellData> maze; // map that has cellData with distance and walls and direction for each {row,col}]
std::array goal = {8, 7}; // goal

// well commented function to add a wall because it was earlier unreadable lmfao
void addWall(int row, int col, std::string walls) {
  std::string wallOrder = "nsew";  // all possible directions
  std::string neighWalls = "snwe"; // corresponding to each possible direction, direction of same wall for neighbor cell
  for (const auto& currentW : walls) {
    int wallno = wallOrder.find(currentW); // which one of possible directions is th current at [0 to 3]
    int rowD[] = {-1, 1, 0, 0}; // row delta
    int colD[] = {0, 0, 1, -1}; // col delta
    std::array neigh = {row + rowD[wallno], col + colD[wallno]}; // location of neighbor to the side of the wall we're adding right now
    if (maze[{row, col}].walls.find(currentW) == std::string::npos)
      maze[{row, col}].walls += currentW; // add wall
    if (maze.find(neigh) == maze.end())
      continue; // don't add wall to neighbor on side of wall we're at if it doesnt exist
    if (maze[neigh].walls.find(neighWalls[wallno]) == std::string::npos)
      maze[neigh].walls += neighWalls[wallno]; // neighbor wall add!
  }
}

// returns all unblocked neighbors of a cell
std::vector<std::array<int, 2>> neighbors(int row, int col) {
  std::vector<std::array<int, 2>> neighborsList;
  std::string wallOrder = "nsew";
  for (int waldir = 0; waldir < 4; waldir++) { // for each wall direction
    int rowD[] = {-1, 1, 0, 0}; // row delta
    int colD[] = {0, 0, 1, -1}; // col delta
    std::array<int, 2> neighborLoc = {row + rowD[waldir], col + colD[waldir]}; // location of neighbour
    // note for future me, this skips if wall exists, don't be dumb aashvik
    if (maze[{row, col}].walls.find(wallOrder[waldir]) != std::string::npos || (maze.find(neighborLoc) == maze.end()))
      continue;
    neighborsList.push_back(neighborLoc);
  }
  return neighborsList;
}

// to sort priority queue descending
struct qCompare {
    bool operator()(std::array<int, 2> a, std::array<int, 2> b) {
      return maze[a].distance > maze[b].distance;
    }
};

// recalc
void recalc() {
  std::priority_queue<std::array<int, 2>, std::vector<std::array<int, 2>>, qCompare> queue; // queue for distances recalculation
  forside(row) forside(col) maze[{row, col}].distance = INFINITY;
  std::array<int, 2> current;
  queue.push(goal); // add center to queue
  maze[goal].distance = 0;
  while (queue.size()>0) {
    current = queue.top(); // choose cell in queue with least distance
    queue.pop(); // remove current
    auto neighs = neighbors(current[0], current[1]); // get neighbors
    for (const auto& neighC : neighs) { // neighC = current neighbor as {row,col}
      if (neighC[0] < current[0]) maze[neighC].direction = 'n'; // set direction for cell to be used in pathplanning
        else if (neighC[0] > current[0]) maze[neighC].direction = 's';
        else if (neighC[1] > current[1]) maze[neighC].direction = 'e';
        else maze[neighC].direction = 'w';
      int newDistance = maze[current].distance + 1; // neighbor dist = current dist + 1
      if (newDistance < maze[neighC].distance) {
        maze[neighC].distance = newDistance;
        queue.push(neighC);
      }
    }
  }
}





void setup() {
  delay(1000);
  forside(row) forside(col) maze[{row, col}] = {INFINITY, "", '\0'};
  forside(each) {
    addWall(0, each, "n");
    addWall(15, each, "s");
    addWall(each, 0, "w");
    addWall(each, 15, "e");
  }
  recalc(); // this takes 3/1000th of a second!
}

void loop() {} 



// turn weighting is calculated when run is planned, not when reflooded
// for SOME reason, THIS SHIT doesn't WORK

