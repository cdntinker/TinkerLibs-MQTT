// #ifndef MQTT_H
// #define MQTT_H

// #define MQTT_BUFFER_SIZE (100)       // This number is arbitrary
//                                     // Topic can be up to 65,536 bytes
//                                     // Message can be up to 268,435,456 bytes

// char MQTT_outTopic[MQTT_BUFFER_SIZE];
// char MQTT_inTopic[MQTT_BUFFER_SIZE];
// char MQTT_teleTopic[MQTT_BUFFER_SIZE];
// char MQTT_statTopic[MQTT_BUFFER_SIZE];

// char MQTT_msg_out[MQTT_BUFFER_SIZE *10];

// unsigned long beacon_timer = 0;
// #define BEACON_INTERVAL 30000 // Timer interval for the "keep-alive" status beacon

// extern const char* MQTTbroker;
// extern const char* MQTTuser;
// extern const char* MQTTpassword;

// const char MQTT_ClientName[] = STR(DeviceName);

// extern const char* ssid;

// #endif  // MQTT_H

// MQTT ////////////////////////////////////////////////////////////////////////
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

// Debug ///////////////////////////////////////////////////////////////////////
// void debug_TitleScreen();                   // Title block for the project
// void debug_ReadyScreen();                   // Indicate setup is finished
// void debug_Separator();                     // Make a line
// void debug_SectionTitle(const char *Title); // Text at the left
// void debug_LineOut(const char *Line);       // Text 3 characters inset
// void debug_Action(const char *Line);        // Text centred
// void debug_Trouble(const char *Line);       //
// void debug_Success(const char *Line);       //
// void debug_ProgressBar0();
// void debug_ProgressBar1();
// void debug_ProgressBar2(int dotcount);
// void debug_ESP_info();

// WiFi ///////////////////////////////////////////////////////////////////////
// int WiFi_strength();
