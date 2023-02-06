#include "actors.h"

extern "C" void app_main() {
    ledCtrl0.Start();
    ledCtrl1.Start();
    ledCtrl2.Start();
    scheduler.Start();
}