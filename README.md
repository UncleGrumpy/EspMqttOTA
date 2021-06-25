## EspMqttOTA
 This is an attempt to update the firmware on ESP32 and ESP8266 devices directly from an MQTT broker.

#### WARNING: NOT WORKING YET!

The idea is to have the device subscribe to a topic like:
```
DEVICE_NAME/update/+
```
Replace DEVICE_NAME with a unique identifier for each device.

The updates should be published to:
```
DEVICE_NAME/update/MD5_of_UPDATE
```
Use the same device identifier as above and replace MD5_of_UPDATE with the MD5 hash of the firmware to be upgraded.

## Current Status
Using the BufferedStreams library for Arduino has things closer to working now. The current problem is that the
stream is being truncated. But I believe this is a problem in my current implementation, not a limitation of either
the PubSubClient or BufferedStreams libraries.

