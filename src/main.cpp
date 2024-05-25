#include <Arduino.h> // for hardware abstraction
#include <map> // bunch of stl libs
#include <array>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>

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
      // if direction of neighbor isn't same as of current, turn costing is there
      char neighDirection = 'h';
      if (maze[neighC].distance==0) maze[neighC].direction = 'h';
        else if (neighC[0] < current[0]) neighDirection = 's';
        else if (neighC[0] > current[0]) neighDirection = 'n';
        else if (neighC[1] > current[1]) neighDirection = 'w';
        else neighDirection = 'e';
      float cost = 1;
      if (neighDirection==maze[current].direction) cost = 1.5; // turn cost
      int newDistance = maze[current].distance + cost; // neighbor dist = current dist + cost
      if ((neighC[0]==7||neighC[0]==8) && (neighC[1]==7||neighC[1]==8)) newDistance = 0;
      if (newDistance < maze[neighC].distance) {
        maze[neighC].direction = neighDirection;
        maze[neighC].distance = newDistance;
        queue.push(neighC);
      }
    }
  }
}

// on the shoulders of pathfinders
std::vector<char> pathfinder(std::array<int,2> starti, bool debugg) {
  std::vector<char> moves; // list of moves
  std::array<int, 2> mouseLoc; // virtual path-finder's current location
  std::array<int, 2> nextLoc = starti;
  int rowD[] = {-1, 1, 0, 0}; // row delta
  int colD[] = {0, 0, 1, -1}; // col delta
  while (mouseLoc!=goal) {
    mouseLoc = nextLoc;
    if (mouseLoc==goal) continue;
    // auto neighs = neighbors(mouseLoc[0], mouseLoc[1]); nextLoc = std::min_element(neighs.begin(), neighs.end(), [&](auto &a, auto &b) {return maze[a].distance < maze[b].distance;} );
    int nextLocType = std::string("nsew").find(maze[mouseLoc].direction);
    nextLoc = {mouseLoc[0] + rowD[nextLocType], mouseLoc[1] + colD[nextLocType]};
    moves.push_back(maze[mouseLoc].direction);
    if (debugg) {
      delay(20); Serial.print("best neighbor of "); delay(20); Serial.print(mouseLoc[0]);
      delay(20); Serial.print("x"); delay(20); Serial.print(mouseLoc[1]);
      delay(20); Serial.print(" is "); delay(20); Serial.print(nextLoc[0]);
      delay(20); Serial.print("x"); delay(20); Serial.print(nextLoc[1]);
      delay(20); Serial.print(", so i will add "); delay(20); Serial.println(maze[mouseLoc].direction);
    }
  }
  return moves;
}

std::vector<char> postproccessor(std::vector<char> inmoves) {
  std::vector<char> finalmoves;
  char currentState, initialDir = inmoves[0], current = inmoves[0];
  for (size_t inMoveN; inMoveN<inmoves.size(); inMoveN++) {
    currentState;
    // figure out how to maintain state, check if still in same direction, if so add straight, else add turn specific and change state
    // 90 d turn != 180 d turn for smoothness and starting/ending in the center of the cells
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
  addWall(15, 0, "e");
  recalc(); // this takes 3/1000th of a second!

  forside(row) forside(col) {
    delay(10); Serial.print(maze[{row,col}].direction);
    delay(10); if (col==15) Serial.println(); else Serial.print(", ");
  }
  for (const char &move : pathfinder({15,0},true)) Serial.println(move);
}

void loop() {}

// for a sample error message, use: for SOME reason, THIS SHIT doesn't WORK