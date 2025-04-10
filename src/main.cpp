#include <Arduino.h>
#include <map>
#include <array>
#include <queue>

#define NORTH 0 // cardinal dir constants
#define SOUTH 1
#define WEST 2
#define EAST 3

#define maze_size 16
#define for_side(sid) for (uint8_t sid=0; sid<maze_size; sid++) // to iter over maze side

typedef std::array<uint8_t, 2> cell_t;
cell_t goal = {8, 7};

std::array<int8_t, 4> row_delta = {-1, 1, 0, 0}, col_delta = {0, 0, -1, 1}; // for NSWE neighbour coords
std::array<uint8_t, 4> opposite_dir = {1, 0, 3, 2};

std::map<cell_t, uint8_t> maze; // maze[{row,col}] : 0b EWSN EWSN walls

typedef struct {float dist; uint8_t dir;} celldata_t; // single cell flood data
std::map<cell_t, celldata_t> o_flood, c_flood; // most recent open & maze_type flood data

// direction is 0123 r2l NSWE
void wall(cell_t cell, uint8_t direction, bool add, uint8_t maze_type=0) {
    cell_t neii = {cell[0] + row_delta[direction], cell[1] + col_delta[direction]};
    if (add) {
        if (maze.find(cell) != maze.end()) maze[cell] |= 1 << (direction + maze_type);
        if (maze.find(neii) != maze.end()) maze[neii] |= 1 << (opposite_dir[direction] + maze_type);
    } else {
        if (maze.find(cell) != maze.end()) maze[cell] &= ~(1 << (direction + maze_type));
        if (maze.find(neii) != maze.end()) maze[neii] &= ~(1 << (opposite_dir[direction] + maze_type));
    }
}

// check for exit
bool has_exit(cell_t cell, uint8_t direction, uint8_t maze_type=0) {
    return !(maze[cell] & (1 << (direction + maze_type)));
}

// get cell_t of neighbor in specific direction
cell_t neighbor(cell_t current, uint8_t direction) {
    return {current[0] + row_delta[direction], current[1] + col_delta[direction]};
}

// add initial walls
void maze_setup() {
    for_side(row) for_side(col) maze[{row, col}] = 0b11110000;
    for_side(each) {
        wall({0,each}, NORTH, true);
        wall({maze_size-1,each}, SOUTH, true);
        wall({each,0}, WEST, true);
        wall({each,maze_size-1}, EAST, true);
    }
    wall({maze_size-1,0}, EAST, true);
    wall({maze_size-1,0}, NORTH, false, 4);
}

// floodfill maze & store results
void flood_maze(cell_t target, uint8_t maze_type=0, bool full_size=true) {
    auto &flood_data = maze_type ? c_flood : o_flood;
    for_side(row) for_side(col) flood_data[{row,col}] = {INFINITY, EAST};
    flood_data[target].dist = 0; // target at dist of 0u from self
    std::priority_queue<cell_t> queue;
    queue.push(target);
    while (queue.size()) {
        cell_t current_cell = queue.top();
        queue.pop();
        for (uint8_t direction : {NORTH, SOUTH, WEST, EAST}) {
            if (has_exit(current_cell, direction, maze_type)) {
                cell_t next_cell = neighbor(current_cell, direction);
                float cost = full_size&&(next_cell[0]==7||next_cell[0]==8)&&(next_cell[1]==7||next_cell[1]==8) ? 0.0
                    : flood_data[current_cell].dist + (opposite_dir[direction]!=flood_data[current_cell].dir ? 1.5 : 1.0);
                if (cost<flood_data[next_cell].dist) {
                    flood_data[next_cell] = {cost, opposite_dir[direction]};
                    queue.push(next_cell);
                }
            }
        }
    }
}

void setup() {
    Serial.begin(115200);
    // mpu.begin();  mpu.Set_DMP_Output_Rate_Hz(10); mpu.CalibrateMPU(); mpu.load_DMP_Image(); mpu.on_FIFO(getRot);
    delay(5000);
    maze_setup();
    flood_maze(goal);
    for_side(row) for_side(col) {
        Serial.print(o_flood[{row, col}].dir);
        if (col != 15) Serial.print(" ");
        else Serial.println();
    }
}

void loop() {}

/*
when going through maze:
    - remove wall from maze_type maze where gap found
    - add wall to open maze where wall found
    - flood open maze for optimistic move
when goal reached:
    - maybe explore (pick other direction)
when back to start:
    - flood closed maze to find best path back
    - run like z wind

from flood to path:
    - make chain of cardinal directions for next cell to move to eg. NEESNN
    - turn cardinals into robot-pov ori eg. F-R90-F-F-R90-F-L180-F-F
    - group F chains together to mark long straight path
    - maybe simplify movement string
    - convert movestring into individual motor v with turns and straight path acceleration
following path:
    - proportional control loop with dampener value to keep current motor at current pathed speed
    - error corr... to be written
*/