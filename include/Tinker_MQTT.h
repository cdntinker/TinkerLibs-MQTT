
void MQTT_init();

void MQTT_callback(char *topic, byte *payload, int length);
void MQTT_reconnect();
void MQTT_beacon();

void MQTT_handler();

void MQTT_SendTELE(const char *Topic, const char *Message);
void MQTT_SendSTAT(const char *Topic, const char *Message);
void MQTT_SendCMND(const char *Topic, const char *Message);
void MQTT_SendNOTI(const char *Topic, const char *Message);

void MQTT_HandleMessages();
