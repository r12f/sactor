#include "actors.h"

void setup()
{
    Serial.begin(115200);

    hello.start();
    blinky.start();

    vTaskStartScheduler();
}

void loop()
{
}