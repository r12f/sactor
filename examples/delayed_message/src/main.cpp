#include "actors.h"

extern "C" void app_main() {
    ledCtrl.Start();
    scheduler.Start();
}