# Sactor

[![Platform.IO](https://img.shields.io/badge/platform.io-v0.1.0-orange)](https://registry.platformio.org/libraries/r12f/Sactor)

Sactor is a light-weighted actor model framework based on FreeRTOS, designed for small IoT devices such as MCUs.

One big challenge of developing programs for small IoT devices is resource constrains, such as CPU and RAM, and Sactor is made to help ease the pain, and in the meantime providing a more modern infra for writing the code.

## How to use

### Installation

There are a few ways to install sactor to your projects:

1. Manually add Sactor as dependent in `lib_deps` of platformio.ini.
2. Use PlatformIO Core cli to add Sactor as dependent: `pio pkg install --library "r12f/Sactor@^0.1.0"`
3. Download the project as zip folder on github, then unzip it to your `lib` folder.
4. Download the package from release page, and unzip it to your `lib` folder.
5. Use `git submodule` to add this project as submodule under your `lib` folder.

Feel free to check the examples folder to see how the end result looks like.

### Example

Let's say we like to create an app, where 1 thread switch the LED on and off every 1 second, and send the status to the another thread for reporting (printf in this case). So, we need 2 actors in this case, one blinky to switch the LED and one hello to report the status.

First, we define the contract between these 2 actors. We need to define the message id and message payload as below:

```c++
#pragma once

#include "sactor/sactor.h"

DECLARE_MESSAGE_ID_BEGIN(ActorHelloImpl)
    MESSAGE_ID(ActorHelloImpl, HelloMessage)
DECLARE_MESSAGE_ID_END()

DECLARE_MESSAGE_BEGIN(ActorHelloImpl, HelloMessage)
    bool IsOn;
DECLARE_MESSAGE_END()
```

Then we need to create these 2 actors. Because in Sactor, every actor is statically created, we put the declaration of these 2 actors in header file as extern, and implement them in the source file, like below:

Here is the header file - `actors.h`:

```c++
#pragma once

#include <stdio.h>
#include "sactor/sactor.h"
#include "actor_contacts.h"

class ActorHelloImpl : public ActorImpl
{
public:
    static constexpr const char* Name = "ActorHello";

    MESSAGE_MAP_BEGIN()
        ON_MESSAGE_NO_REPLY(ActorHelloImpl, OnHello, HelloMessage)
    MESSAGE_MAP_END()

private:
    SactorError OnHello(_In_ const HelloMessage* message);
};

class ActorBlinkyImpl : public ActorImpl
{
public:
    static constexpr const char* Name = "ActorBlinky";

    MESSAGE_MAP_BEGIN()
        ON_INIT_MESSAGE(ActorBlinkyImpl, OnInit)
    MESSAGE_MAP_END()

private:
    SactorError OnInit();
};

typedef Actor<ActorHelloImpl> ActorHello;
typedef Actor<ActorBlinkyImpl> ActorBlinky;

extern ActorHello hello;
extern ActorBlinky blinky;
```

And here is the source file: `actors.cpp`:

```c++
#include "actors.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define BLINK_GPIO GPIO_NUM_10

// All actors are instantiated here.
ActorHello hello;
ActorBlinky blinky;

SactorError ActorHelloImpl::OnHello(_In_ const HelloMessage* message)
{
    printf("Hello world! LED = %s\n", message->IsOn ? "On" : "Off");
    return SactorError_NoError;
}

SactorError ActorBlinkyImpl::OnInit()
{
    esp_rom_gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    while(1) {
        /* Blink off (output low) */
        printf("Turning off the LED\n");
        gpio_set_level(BLINK_GPIO, 0);
        hello.SendSync(HelloMessage { false });
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        /* Blink on (output high) */
        printf("Turning on the LED\n");
        gpio_set_level(BLINK_GPIO, 1);
        hello.SendSync(HelloMessage { true });
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
```

Once they are done, in our main function, we can simply include the `actors.h` file and start them all.

> Note: Sactor also supports timers, for more details, please check example here: <https://github.com/r12f/sactor/blob/main/examples/timer/src/sactor/actors.cpp>.

```c++
#include "actors.h"

extern "C" void app_main() {
    hello.Start();
    blinky.Start();
}
```

And now we can start to get it programmed and open serial monitor to check if it is working. (The log below has `SACTOR_ENABLE_TRACE_LOG` defined to enable internal logs, so we can see the message being passing around.)

```
Turning on the LED
[  Worker][     ActorHello @ 0x3fc8c250] Actor worker thread on message: Id = 1, Buffer = 0x3fc91c9c, Reply = 0x0.
Hello world! LED = On
Turning off the LED
[  Worker][     ActorHello @ 0x3fc8c250] Actor worker thread on message: Id = 1, Buffer = 0x3fc91c98, Reply = 0x0.
Hello world! LED = Off
Turning on the LED
[  Worker][     ActorHello @ 0x3fc8c250] Actor worker thread on message: Id = 1, Buffer = 0x3fc91c9c, Reply = 0x0.
Hello world! LED = On
Turning off the LED
[  Worker][     ActorHello @ 0x3fc8c250] Actor worker thread on message: Id = 1, Buffer = 0x3fc91c98, Reply = 0x0.
Hello world! LED = Off
```

For the end result, please check this example: <https://github.com/r12f/sactor/tree/main/examples/blinky_hello>.

## Design principles

If you are interested more on why Sactor is build in this way, here are more details on Sactor's design principles:

### 1. No dynamic allocation

Sactor is trying the best to make resource usage clear in compile time by leveraging `*Static` APIs in FreeRTOS to create resources, such as Queues. This helps us finding memory problem early and reduce the risk of heap fragmentation problem to get more stable memory usage in the long run.

For example, here is the memory usage before bumping the actor stack size from 2048 words (* 4 bytes) to 20480 words (* 4 bytes). In this case, we can see the RAM usage increased 18432 bytes, which matches what we changed.

```
Checking size .pio\build\seeed_xiao_esp32c3\firmware.elf
Advanced Memory Usage is available via "PlatformIO Home > Project Inspect"
RAM:   [=         ]   6.5% (used 21200 bytes from 327680 bytes)
Flash: [==        ]  15.3% (used 160008 bytes from 1048576 bytes)
Building .pio\build\seeed_xiao_esp32c3\firmware.bin
```

And here is after the bump, we can see the RAM usage increases in the post compile analysis for the firmware.

```
Checking size .pio\build\seeed_xiao_esp32c3\firmware.elf
Advanced Memory Usage is available via "PlatformIO Home > Project Inspect"
RAM:   [=         ]  12.1% (used 39632 bytes from 327680 bytes)
Flash: [==        ]  15.3% (used 160006 bytes from 1048576 bytes)
Building .pio\build\seeed_xiao_esp32c3\firmware.bin
```

Additionally, just like normal Actor model, actors talking to each other via messages, which usually requires dynamic allocations. So our messaging system is designed with this principle in mind. It works like IPC in microkernel. The message and reply buffers are allocated by the sender/client side, and all data structures used internally are either allocated on preallocated buffer, such as queue, or on the stack.

### 2. Try to be as light-weighted as possible

Due to resource constrain on the IoT devices, Sactor is trying to be a thin layer. So, it made a few trade-offs:

- **Prefer static binding over dynamic binding**: There is no virtual calls in sactor. Everything is bound statically via template or macro. This helps saves CPU usage.
- **Carefully created template usage**: Although template is used in our code, but we are trying to limit its usage as much as we can, e.g. common code will be extracted to non-templated base class and template class is only used as a wrapper layer.
- **No extra dependency other than FreeRTOS**: This helps avoid introducing unexpected resource usages.

### 3. Simple API and try to be easy to use.

The programs runs on IoT devices can be fairly simple and straight-forward (tasks runs forever instead of changing a lot overtime), so, Sactor is designed with a simple model - All actors are created as global variables. This also helps us getting a determined overall resource usage once compile is done.

Each actor can also define its own behaviors, such as stack size, queue size and etc. And the only way to talk between the actor is via the IPC-like communicate channel.

### 4. Good debuggability

To help debugging, Sactor leverages a few debug infras:

- Simplified [SAL](https://learn.microsoft.com/en-us/cpp/code-quality/understanding-sal) to annotate the usage of each function parameters.
- Design-By-Contract enabled assertions.
- Macros for tracing points for customize debugging infra when needed.


## License

MIT.