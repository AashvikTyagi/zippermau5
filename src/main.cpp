/*
this is the main file!
*/

#include <Arduino.h>
#include "solver.cpp"
// #include "hware.cpp"

void setup() {
  delay(1000);
  setupMaze();
  floodMaze({8,7},0);
  forside(row) forside(col) { Serial.print(oFlood[{row, col}].dir); if (col!=15) Serial.print(", "); else Serial.println(); }
}

void loop() {}