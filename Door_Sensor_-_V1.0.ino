/*
 * This Example is for Door sensor, Connected to  GPIO 4  and One More Status LED is connected to GPIO 5
 * MQTT details you will get after your login and add a device in the dreamzIoT.com IoT protal
 * Change your Wifi name and Password Name as per your requirement. 
 * This code is compiled for ESP32 , you can easily compile it for ESP8266 with just changing header file. 
 * When door is open we are publishing one Json value {"doorStatus":"open", "deviceID":"mqtt_311231"}, You can change doorStatus as per your key value, But you have to send deviceID, which 
 * mqtt client ID. 
 * If you have any doubt or concern you can ask on our slack channel or fourm.dreamzIoT.com
 */
#include <WiFi.h>
#include <PubSubClient.h>

//-------------------------------------------------
// Enter All Configuration Details Here below 

#define MQTT_BROKER     "io.dreamziot.com"
#define MQTT_USERNAME   "user_dadf6dd2"
#define MQTT_PASSWORD   "password_7faedb78"
#define MQTT_TOPIC      "mqtt_0cdc869c_door-sensor"
#define MQTT_CLIENT_ID  "mqtt_0cdc869c"   //Enter the deviceID within double quote. 

#define WIFI_SSID       "DreamzIOT"
#define WIFI_PASSWORD   "admin@123"

//--------------------------------------------------
/* create an instance of PubSubClient client */
WiFiClient espClient;
PubSubClient client(espClient);

/*Define Door and LED GPIO*/
int doorGPIO = 4;
int ledGPIO = 5;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: [");
  Serial.print(topic);
  Serial.println("] ");
  
  Serial.print("payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

}

void mqttconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(MQTT_CLIENT_ID, MQTT_USERNAME , MQTT_PASSWORD)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup_wifi() {
  delay(10);
  //We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}



void setup() {
  Serial.begin(115200);     //Initialize serial
  setup_wifi();
  pinMode(doorGPIO, INPUT);
  pinMode(ledGPIO, OUTPUT);
  client.setServer(MQTT_BROKER, 1883);
  client.setCallback(callback);
}

//Publish temprature and humidity on mqtt broker
void mqttPublish(int doorStatus)
{
    char msg[75];
    if(doorStatus==1)
    {
      sprintf(msg, "{\n\"doorStatus\":\"open\", \"deviceID\":\"%s\"}", MQTT_CLIENT_ID); // You can cheange JSON key from dootStatus to Anyother, depending upon your requirement
      Serial.println("doorStatus:open");
      client.publish(MQTT_TOPIC, msg);
     
    }
    else if(doorStatus==0){
      sprintf(msg, "{\"doorStatus\":\"close\", \"deviceID\":\"%s\"}", MQTT_CLIENT_ID); //You can cheange JSON key from dootStatus to Anyother, depending upon your requirement
      Serial.println("doorStatus:close");
      client.publish(MQTT_TOPIC, msg);
 
    }
}

int doorPreviousStatus = 2;
void loop() {
  /* if client was disconnected then try to reconnect again */
  if (!client.connected()) {
    mqttconnect();
  }
   client.loop();
   int doorStatus = digitalRead(doorGPIO);
   Serial.println(doorStatus);
   if(doorStatus == 1 && doorPreviousStatus == 3 ){
    mqttPublish(doorStatus);
    digitalWrite(ledGPIO, LOW);  
    doorPreviousStatus = 2;
   }
   if(doorStatus == 0 && doorPreviousStatus == 2 ){
    mqttPublish(doorStatus);
    digitalWrite(ledGPIO, HIGH);  
    doorPreviousStatus = 3;
   }
   /*
   2- opened state
   3 - close state
   */   
}

