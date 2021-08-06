/*
  Basic ESP8266 MQTT example

  This sketch demonstrates the capabilities of the pubsub library in combination
  with the ESP8266 board/library.

  It connects to an MQTT server then:

  It will reconnect to the server if the connection is lost using a blocking
  reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
  achieve the same result without blocking the main loop.

  To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "credentials.h"
#include "DHT.h"                     // Include DHT library

#define DHTPIN 2            // DHT11 data pin is connected to ESP8266 pin GPIO2
#define DHTTYPE DHT22       // DHT11 sensor is used
DHT dht(DHTPIN, DHTTYPE);   // Configure DHT library
String LastKnownTemp = "19.00";
String LastKnownRH = "35.00";

float temperature = 19.0;
float humidity  = 35.0;


// deserializeJson(doc,str); can use string instead of payload
/*const char* sensor = doc["sensor"];
  long time          = doc["time"];
  float humidity  = doc["data"][0];
  float temperature   = doc["data"][1];*/



WiFiClient espClient;
PubSubClient client(espClient);

#define BAUD 57600
#define MAX_MSG_SIZE 100

char msg[MAX_MSG_SIZE + 1];
byte msg_pos;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.persistent(false);
  WiFi.setAutoReconnect(true);
  WiFi.setSleepMode(WIFI_MODEM_SLEEP); // Default is WIFI_NONE_SLEEP
  WiFi.setOutputPower(10); // 0~20.5dBm, Default max

  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet); // For Static IP
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.write(payload[i]);
  }
  Serial.write('\n');
  Serial.println();
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(MQTT_ID, MQTT_USER, MQTT_PSWD)) {
      Serial.println("connected");
      // Wait 5 seconds before retrying
      for (byte i = 0; i < 2; i++) delay(500); // delay(1000) may cause hang
      // Once connected, publish an announcement...
      // client.publish(MQTT_TOPIC_OUT, "hello world");
      // ... and resubscribe
      // client.subscribe(MQTT_TOPIC_IN);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      for (byte i = 0; i < 10; i++) delay(500); // delay(5000) may cause hang
    }
  }
}

void setup() {
  Serial.begin(BAUD, SERIAL_8N1, SERIAL_FULL);
  setup_wifi();
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
  dht.begin();                       // Initialize DHT sensor
}



float getSupplyVoltage()
{
  int rawLevel = analogRead(A0);

   //float realVoltage = (float)rawLevel / 1000 / (10000. / (47000 + 10000));
  float realVoltage = (float)rawLevel/1023*3.3;

  return realVoltage;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Loop
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
int value = 0;
float HCal = 0;
float TCal = 0;

void loop() {
  StaticJsonDocument<256> doc; //256 is the RAM allocated to this document.
  doc["Sensor"] = "DTH1";
  doc["Msg#"] = value;
  float vcc=0.0;\\getSupplyVoltage();
  doc["Bat"] = vcc;

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 60000) {
    // Read humidity
    float h = dht.readHumidity();
    float RH = h - HCal; //I noticed the readings needed calibrating so I have an offset that I can changed, the result being the value I send for use

    //String RH_str;
    float t = dht.readTemperature(); //take a Temperature reading and place in the float variable 't'
    float Temp = t - TCal; //again I noticed the readings needed calibrating so I have an offset that I can change, the result being the value I send for use

    // Check if any reads failed and exit early (to try again)
    if (isnan(RH) || isnan(Temp)) {
      doc["status "] = "error";
    }
    else
    {
      doc["Status "] = "ok";
      JsonArray data = doc.createNestedArray("data");
      data.add(Temp);
      data.add(RH);
      
      //doc["Temp"] = Temp;
      //doc["Humidity"] = RH;
    }

    int b = serializeJson(doc, msg); // Generate the minified JSON and send it to the msg char array
    // {"sensor":"DTH","msg#":1,"data":[28.0,60.1]}  ->msg

    ++value;
    //snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(MQTT_TOPIC_OUT, msg);

    // update last
    lastMsg = now;

  }

  //  msg_pos = 0;
  //
  //  while ((Serial.available() > 0) && (msg_pos < MAX_MSG_SIZE)) // Chek for availablity of data at Serial Port
  //  {
  //    msg[msg_pos++] = Serial.read(); // Reading Serial Data and saving in data variable
  //    delayMicroseconds(174U);        // 174µs/char @ 57600 Bauds
  //  }
  //
  //  msg[msg_pos++] = 0; // add Null byte at the end*/
  //
  //
  //  // msg min size 40
  //  // msg always start with "20;"
  //  if ((msg_pos > 40) && (msg[0] == '2') && (msg[1] == '0') && (msg[2] == ';'))
  //  {
  //    if (!client.connected()) reconnect();
  //    //client.loop();
  //    Serial.print("Publish message: ");
  //    Serial.println(msg);
  //    client.publish(MQTT_TOPIC_OUT, msg);
  //  }
  //  else delay(7); // Wait for a complete minimal message
  //
  //  delay(1); // To Enable some more Modem Sleep

}
