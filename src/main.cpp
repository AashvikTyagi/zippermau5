#include <Arduino.h>
#include <solver.cpp>

void setup() {
    delay(1000);
    Serial.begin(115200);
    // mpu.begin();
    // mpu.Set_DMP_Output_Rate_Hz(10);
    // mpu.CalibrateMPU();
    // mpu.load_DMP_Image();
    // mpu.on_FIFO(getRot);
    setupMaze();
    floodMaze({8, 7}, 0);
    forside(row) forside(col)
    {
        Serial.print(oFlood[{row, col}].dir);
        if (col != 15)
            Serial.print(", ");
        else
            Serial.println();
    }
}

void loop() {
    // Serial.print(y); Serial.print('-'); Serial.print(p); Serial.print('-'); Serial.println(r); delay(100);
}