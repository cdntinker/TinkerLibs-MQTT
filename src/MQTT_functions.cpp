// #include "libraries.h"
#include <string.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include "WiFi.h"
#endif
#include <PubSubClient.h>

// #include "functions.h"
#include <Tinker_DEBUG.h>
int WiFi_strength();

#if defined(SiniLink)
// #include "Tinker_SiniLink.h"
void SiniLink_MQTT(char* Topic, char* Message);
extern bool SiniLink_PWR_STATE;
#endif

// #include "MACROS.h"
#define ST(A) #A
#define STR(A) ST(A)

#include "MQTT.h"

WiFiClient espClient;
PubSubClient MQTT_client(espClient);

void MQTT_handler()
{
    if (!MQTT_client.connected())
    {
        MQTT_reconnect();
    }
    MQTT_client.loop();
    // MQTT_beacon();
}

////////////////////////////////////////////////////////////////
// MQTT Functions
////////////////////////////////////////////////////////////////

void MQTT_init()
{
    DEBUG_Init("MQTT");

    MQTT_client.setServer(MQTTbroker, 1883);
    MQTT_client.setCallback(MQTT_callback);

    //  Build the topic names
    strcpy(MQTT_inTopic, "cmnd/"); //  in - Commands
    strcat(MQTT_inTopic, MQTT_ClientName);
    strcat(MQTT_inTopic, "/#");

    strcpy(MQTT_teleTopic, "tele/"); // out - Telemetry
    strcat(MQTT_teleTopic, MQTT_ClientName);

    strcpy(MQTT_statTopic, "stat/"); // out - Status
    strcat(MQTT_statTopic, MQTT_ClientName);

    strcpy(MQTT_outTopic, "noti/"); // out - Notifications
    strcat(MQTT_outTopic, MQTT_ClientName);

    MQTT_reconnect();
}

void MQTT_callback(char *topic, byte payload[100], int length)
{
    char debugTEXT[46];

    // char MQTT_msg_in[MQTT_BUFFER_SIZE];
    char MQTT_msg_in[28];                   // Limiting size for DEBUG functions...
    
    char *MQTT_command = strrchr(topic, '/');
    char CNasT[MQTT_BUFFER_SIZE];
    strcpy(CNasT, "/");
    strcat(CNasT, MQTT_ClientName); // "ClientName as Topic"

    DEBUG_SectionTitle("Message arrived");
    sprintf(debugTEXT, "Topic: %30s", MQTT_command);
    DEBUG_LineOut(debugTEXT);

    // if (length < MQTT_BUFFER_SIZE)
    // if (length < 63)
    // Messages 63 characters long or bigger make BOOM
    // with a "Soft WDT reset"
    // MQTT_BUFFER_SIZE is 100
    // I R cornfoozed...
    if (length < 59) // & now 59 is bad...  :|
    {

        MQTT_msg_in[0] = '\0'; // start with an empty string!
        for (int i = 0; i < length; i++)
        {
            MQTT_msg_in[i] = (char)payload[i];
            MQTT_msg_in[i + 1] = '\0';
        }

Serial.printf("sizeof(MQTT_msg_in) = %d\n", sizeof(MQTT_msg_in));
Serial.printf("strlen(MQTT_msg_in) = %d\n", strlen(MQTT_msg_in));
        sprintf(debugTEXT, "Message: %28s", MQTT_msg_in);
        DEBUG_LineOut(debugTEXT);
        sprintf(debugTEXT, "Message Size: %d", length);
        DEBUG_LineOut(debugTEXT);

        /////////////////////////////////////////////////////
        // Message handling goes here...
        /////////////////////////////////////////////////////

        if (strcmp(MQTT_command, CNasT) == 0) // MQTT_ClientName as Topic
        {
            DEBUG_Trouble("Missing topic...");
        }

        else if (strcmp(MQTT_command, "/Test") == 0)
        {
            MQTT_SendNOTI(MQTT_command, MQTT_msg_in);
        }

#if defined(SiniLink)
        else if ((strcmp(MQTT_command, "/Power") == 0) |
                 (strcmp(MQTT_command, "/LED01") == 0) |
                 (strcmp(MQTT_command, "/LED02") == 0))
        {
            // Rip the leading slash off MQTT_command like a bandaid
            char Command[MQTT_BUFFER_SIZE];
            strcpy(Command, MQTT_command);
            memmove(Command, MQTT_command + 1, strlen(MQTT_command + 1) + 1);
            // Then send it along with MQTT_msg_in off to SiniLink...
            SiniLink_MQTT(Command, MQTT_msg_in);
        }
#endif

        else
        {
            DEBUG_Trouble("Dunno Whatcha want...");
        }
    }
    else
    {
        DEBUG_Trouble("But, it's TOO Bloody Big!");
    }
}

void MQTT_reconnect()
{
    char debugTEXT[46];

    // Loop until we're reconnected
    while (!MQTT_client.connected())
    {

        sprintf(debugTEXT, "WiFi:%d dBm", WiFi_strength());
        DEBUG_LineOut(debugTEXT);

        DEBUG_SectionTitle("Attempting MQTT connection...");

        // Create a random client ID
        String clientId = MQTT_ClientName;
        clientId += String(random(0xffff), HEX);

        // Attempt to connect
        if (MQTT_client.connect(clientId.c_str()))
        {

            sprintf(debugTEXT, "WiFi:%d dBm", WiFi_strength());
            DEBUG_LineOut(debugTEXT);

            // SSD1306_Static(" MQTT good ", 3);
            // delay(500);
            // SSD1306_Static("           ", 3);
            sprintf(debugTEXT, "connected to %-24s", MQTTbroker);
            DEBUG_LineOut(debugTEXT);
            sprintf(debugTEXT, "My Name:  %-27s", MQTT_ClientName);
            DEBUG_LineOut(debugTEXT);
            // Once connected, publish an announcement...
            char MQTT_statTopic_Device[100];
            strcpy(MQTT_statTopic_Device, MQTT_statTopic);
            strcat(MQTT_statTopic_Device, "/HELLO");
            // MQTT_client.publish(MQTT_statTopic_Device, "world");
            MQTT_client.publish(MQTT_statTopic_Device, ssid);
            // ... and resubscribe
            MQTT_client.subscribe(MQTT_inTopic);
        }
        else
        {
            /////////////////////////////////////////////////////////////////////////////////////////////////
            sprintf(debugTEXT, "WiFi:%d dBm", WiFi_strength());
            DEBUG_LineOut(debugTEXT);
            /////////////////////////////////////////////////////////////////////////////////////////////////

            if (WiFi.status() != WL_CONNECTED)
            {
                // WiFi_Test2();
                DEBUG_Trouble("UH OH!!!  No WiFi!!!");
            }
            else
            {
                // SSD1306_Static("  no MQTT  ", 3);
                // WiFi_Test3();
                DEBUG_Trouble("UH OH!!!  No MQTT!!!");
            }

            sprintf(debugTEXT, "failed, rc=%d", MQTT_client.state());
            DEBUG_Trouble(debugTEXT);
            DEBUG_Trouble("trying again in 2 seconds");
            // Wait 5 seconds before retrying
            delay(2000);
        }
    }
}

void MQTT_beacon()
{
    /* Beacon signal published at set interval to indicate the device
   * is still powered up and actively connected to MQTT...
   * A keepalive of sorts
   * also updates state within MQTT so it can be captured for
   * indicator light elsewhere
   * &, as a bonus, it's sending the WiFi strength.
   */
    char MQTT_statTopic_Device[100];
    char WiFiSignal[10];
    strcpy(MQTT_statTopic_Device, MQTT_statTopic);
    strcat(MQTT_statTopic_Device, "/WiFi_strength");
    sprintf(WiFiSignal, "%d dBm", WiFi_strength());
    DEBUG_SectionTitle("WiFi:");
    DEBUG_LineOut(WiFiSignal);

    MQTT_client.publish(MQTT_statTopic_Device, WiFiSignal);
    // blinkLED(5);

    DEBUG_LineOut("Beacon sent");
}

void MQTT_Status(char const *Device, char const *Status) // Send status messages
{
    char MQTT_statTopic_Device[100];
    strcpy(MQTT_statTopic_Device, MQTT_statTopic);
    strcat(MQTT_statTopic_Device, "/");
    strcat(MQTT_statTopic_Device, Device);

    char debugTEXT[46];

    sprintf(debugTEXT, "}- %16s = %-16s -{", Device, Status);
    DEBUG_Trouble(debugTEXT);
    MQTT_client.publish(MQTT_statTopic_Device, Status);
}

void MQTT_SendTELE(const char *Topic, const char *Message)
{
    char MQTT_teleTopic_Device[100];
    strcpy(MQTT_teleTopic_Device, MQTT_teleTopic);
    strcat(MQTT_teleTopic_Device, "/");
    strcat(MQTT_teleTopic_Device, Topic);
    MQTT_client.publish(MQTT_teleTopic_Device, Message);
}

void MQTT_SendSTAT(const char *Topic, const char *Message)
{
    char MQTT_statTopic_Device[100];
    strcpy(MQTT_statTopic_Device, MQTT_statTopic);
    strcat(MQTT_statTopic_Device, "/");
    strcat(MQTT_statTopic_Device, Topic);
    MQTT_client.publish(MQTT_statTopic_Device, Message);
}

void MQTT_SendCMND(const char *Topic, const char *Message)
{
    char MQTT_statTopic_Device[100];
    strcpy(MQTT_statTopic_Device, Topic);
    // strcat(MQTT_statTopic_Device, "/");
    // strcat(MQTT_statTopic_Device, Topic);
    MQTT_client.publish(MQTT_statTopic_Device, Message);
}

void MQTT_SendNOTI(const char *Topic, const char *Message)
{
    char MQTT_notiTopic_Device[100];
    strcpy(MQTT_notiTopic_Device, MQTT_outTopic);
    strcat(MQTT_notiTopic_Device, "/");
    strcat(MQTT_notiTopic_Device, Topic);
    MQTT_client.publish(MQTT_notiTopic_Device, Message);
}

void MQTT_HandleMessages()
{
}
