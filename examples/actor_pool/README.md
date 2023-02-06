# Example: actor-pool

This example examples multiple actor listening to the same queue to form a worker pool. The requests will be handled by all actors listening to the queue.

To compile it, please [install the PlatformIO core cli](https://docs.platformio.org/en/stable/core/index.html), then use the following commands to build or upload the program to your board.

The example is using [Seeed Xiao ESP32-C3](https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/) as example.

```bash
# Compile
pio run

# Upload
pio run --target upload
```

Once it runs, we will see the following result from the serial monitor output:

```
Schedule LED status update: Status = On
Updating LED Status: Actor = 0x3fc8c948, Pin = 10, Status = On
Updating LED Status: Actor = 0x3fc8d2bc, Pin = 11, Status = On
Updating LED Status: Actor = 0x3fc8dc30, Pin = 12, Status = On
Schedule LED status update: Status = Off
Updating LED Status: Actor = 0x3fc8c948, Pin = 10, Status = Off
Updating LED Status: Actor = 0x3fc8d2bc, Pin = 11, Status = Off
Updating LED Status: Actor = 0x3fc8dc30, Pin = 12, Status = Off
```