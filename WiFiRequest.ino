// NTPClient - Version: Latest 
#include <NTPClient.h>

// ArduinoHttpClient - Version: Latest 
#include <ArduinoHttpClient.h>
#include <WiFi101.h>
#include <WiFiUdp.h>

const int light_pin = A1;

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

char serverAddress[] = "slippery-dog-42.loca.lt";  // server address
int port = 80;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);
int status = WL_IDLE_STATUS;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

long unsigned int formattedDate;

void setup() {
  Serial.begin(9600);
  
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);             

    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
  }

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  
  timeClient.begin();
  timeClient.setTimeOffset(3600);
}

void loop() {
  int light_val = analogRead(light_pin);
  Serial.println(light_val);
  
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  
  formattedDate = timeClient.getEpochTime() - 3600;
  String stringFormattedDate = String(formattedDate) + "000";

  
  Serial.print("Current Timestamp");
  Serial.println(formattedDate);
  
  Serial.print("Light Value");
  Serial.println(light_val);
  
  Serial.println("making POST request");
  String contentType = "application/json";
  String postData = "{\"@timestamp\": " + String(stringFormattedDate) + ", \"light_value\": " + String(light_val) + ", \"name\": \"Raymond Wu\"}";

  client.post("/my_index/_doc", contentType, postData);

  // read the status code and body of the response
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
  

  Serial.println("Wait five seconds");
  delay(5000);
}