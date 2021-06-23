#ifndef MQTT_H
#define MQTT_H

#define MQTT_BUFFER_SIZE (100)       // This number is arbitrary
                                    // Topic can be up to 65,536 bytes
                                    // Message can be up to 268,435,456 bytes

char MQTT_outTopic[MQTT_BUFFER_SIZE];
char MQTT_inTopic[MQTT_BUFFER_SIZE];
char MQTT_teleTopic[MQTT_BUFFER_SIZE];
char MQTT_statTopic[MQTT_BUFFER_SIZE];

char MQTT_msg_out[MQTT_BUFFER_SIZE *10];

unsigned long beacon_timer = 0;
#define BEACON_INTERVAL 30000 // Timer interval for the "keep-alive" status beacon

extern const char* MQTTbroker;
extern const char* MQTTuser;
extern const char* MQTTpassword;

const char MQTT_ClientName[] = STR(DeviceName);

extern const char* ssid;

#endif  // MQTT_H

