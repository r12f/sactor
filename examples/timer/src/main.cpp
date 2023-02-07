#include "actors.h"

extern "C" void app_main() {
    led_ctrl.Start();
    scheduler.Start();
}