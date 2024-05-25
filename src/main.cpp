#include <Arduino.h> // for hardware abstraction
#include <map> // bunch of stl libs
#include <array>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>

// direction constants
#define NORTH 0
#define SOUTH 1
#define WEST 2
#define EAST 3

#define mazeSize 16
#define forside(sid) for (int sid = 0; sid < mazeSize; sid++) // wonky way

// maze[{row,col}] : 0b EWSN EWSN
std::map<std::array<int, 2>, uint8_t> maze;
std::array goal = {8, 7};

// direction is 0 1 2 3 from right for NSWE
void addWall(std::array<int, 2> cell, int direction, int mazeType) {
  maze[cell] |= 1<<(direction+mazeType);
}

// check for exit
bool hasExit(std::array<int, 2> cell, int direction, int mazeType) {
  return (maze[cell]&(1<<(direction+mazeType)))==0;
}

// sets definite walls
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

void setup() {
  delay(1000);
  setupMaze();
}

void loop() {}


/*
GAME PLAN:
  when going through maze:
    - remove wall from closed maze where gap found
    - add wall to open maze where wall found
    - flood open maze for optimistic move
  when reached the goal:
    - explore (pick other direction)
  when back to start:
    - flood closed maze to find best path back
    - run
TO NOTE:
  - borrow flooding from oldie
*/