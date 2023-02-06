# Example: blinky-hello

This example demos p2p communication between 2 actors. This is also the demo shown in the [project README.md](https://github.com/r12f/sactor).

To compile it, please [install the PlatformIO core cli](https://docs.platformio.org/en/stable/core/index.html), then use the following commands to build or upload the program to your board.

The demo is using [Seeed Xiao ESP32-C3](https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/) as example.

```bash
# Compile
pio run

# Upload
pio run --target upload
```

Once it runs, we will see the following result from the serial monitor output:

```
Turning on the LED
Hello world! LED = On
Turning off the LED
Hello world! LED = Off
```