#include "actors.h"

extern "C" void app_main() {
    led_ctrl_0.Start();
    led_ctrl_1.Start();
    led_ctrl_2.Start();
    scheduler.Start();
}