#include "actors.h"

void setup()
{
    Serial.begin(115200);

    hello.Start();
    blinky.Start();

    vTaskStartScheduler();
}

void loop()
{
}