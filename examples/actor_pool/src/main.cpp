#include "actors.h"

extern "C" void app_main() {
    led_ctrl_0.start();
    led_ctrl_1.start();
    led_ctrl_2.start();
    scheduler.start();
}