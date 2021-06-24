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
Despite setting the  MQTT_MAX_TRANSFER_SIZE and MQTT_MAX_PACKET_SIZE through the setBufferSize(size) function updates are not even acknowledged to exist through error or otherwise... Shorter bogus messages do get through and fail at various stages depending on the message and topic.

