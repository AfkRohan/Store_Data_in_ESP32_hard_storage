#include <Arduino.h>
#include "FS.h"
#include "SPIFFS.h"
#include <Preferences.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);
Preferences preferences;

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "ADVANCETECH_DIR_4G";
const char* password = "12761487";


//const char* var1 = "input1";
//const char* var2 = "input2";n  
//const char* var3 = "input3";
//const char* var4 = "input4";
//const char* var5 = "input5";
//const char* var6 = "input6";
//const char* var7 = "input7";
//const char* var8 = "input8";
//const char* var9 = "input9";
//const char* var10 = "input10";

const char index_html[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title> A Simple Webform </title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" crossorigin="anonymous">
    <style> body{
        margin-left: 10rem;
        text-align: left;
        font-weight: 600;
        background-color: whitesmoke;
        font-size: 1.25rem;
        color: darkslategrey;
    }
     h1{
         text-align: center;
         text-decoration: underline;
         text-decoration-color: grey;
     }  
    </style>
</head>
<body>
    <h1> Data Webform</h1>
    <form action="/get" >
        <label for="var1">Variable 1:</label><br>
        <input type="text" id="var1" name="var1"><br>
        <label for="var2">Variable 2:</label><br>
        <input type="text" id="var2" name="var2"><br>
        <label for="var3">Variable 3:</label><br>
        <input type="text" id="var3" name="var3"><br>
        <label for="var4">Variable 4:</label><br>
        <input type="text" id="var4" name="var4"><br>
        <label for="var5">Variable 5:</label><br>
        <input type="text" id="var5" name="var5"><br>
        <label for="var6">Variable 6:</label><br>
        <input type="text" id="var6" name="var6"><br>
        <label for="var7">Variable 7:</label><br>
        <input type="text" id="var7" name="var7"><br>
        <label for="var8">Variable 8:</label><br>
        <input type="text" id="var8" name="var8"><br>
        <label for="var9">Variable 9:</label><br>
        <input type="text" id="var9" name="var9"><br>
        <label for="var10">Variable 10:</label><br>
        <input type="text" id="var10" name="var10"><br> 
        <br>
        <input type="submit" value="submit"  class="btn btn-primary mb-4">   
      </form>
    <script src="https://cdn.jsdelivr.net/npm/@popperjs/core@2.10.2/dist/umd/popper.min.js" integrity="sha384-7+zCNj/IqJ95wo16oMtfsKbZ9ccEh31eOz1HGyDuCQ6wgnyJNSYdrPa03rtR1zdB" crossorigin="anonymous"></script>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.min.js" integrity="sha384-QJHtvGhmr9XOIpI6YVutG+2QOK9T+ZnN4kzFN1RtK3zEFEIsxhlmWl5/YESvpZ13" crossorigin="anonymous"></script>
</body>
</html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void writeToPreference(String name,String value){
         const char* key = name.c_str();
         preferences.begin("Credentials",false); 
         preferences.getString(key,"Enter a value");
         preferences.putString(key,value);
         Serial.println(value);
         preferences.end();
}

void readFile(fs::FS &fs){
   const char* path = "/test.txt";
    if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
   Serial.printf("Reading file: %s\r\n", path);

   File file = fs.open(path);
   if(!file || file.isDirectory()){
       Serial.println("− failed to open file for reading");
       return;
   }

   Serial.println("− read from file:");
   while(file.available()){
      Serial.write(file.read());
   }
}

void  writeToFile(fs::FS &fs, String message ){
   const char* path = "/test.txt";
     if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
   Serial.printf("Appending to file: %s\r\n", path);
   
   File file = fs.open(path, FILE_APPEND);
   
   if(!file){
      Serial.println("− failed to open file for appending");
      return;
   }
   
   if(file.print(message)){
      Serial.println("− message appended");
   } else {
      Serial.println("− append failed");
   }
   
}


void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  // Set your Static IP address
  IPAddress local_IP(192, 168, 0, 162);
  // Set your Gateway IP address
  IPAddress gateway(192, 168, 1, 1);
  
  IPAddress subnet(255, 255, 0, 0);
  IPAddress primaryDNS(8, 8, 8, 8);   //optional
  IPAddress secondaryDNS(8, 8, 4, 4); 
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  readFile(SPIFFS);
  
  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

   // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    // GET 
    int paramsNr = request->params();
    Serial.println(paramsNr);
 
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
        
        //Write key to File
        Serial.print("Param name: ");
        Serial.println(p->name());
        writeToFile(SPIFFS,p->name());

        // Tab 
        writeToFile(SPIFFS,"\t");

        //Write Value to file
        Serial.print("Param value: ");
        Serial.println(p->value());
        writeToFile(SPIFFS,p->value());
        
        //Add to Preference
        writeToPreference(p->name(),p->value());

        //New Line
        writeToFile(SPIFFS,"\n");
        Serial.println("------");
    }
     request->send(200, "text/plain", "message received");
  });
  server.onNotFound(notFound);
  server.begin();
}
  
void loop() {
  // put your main code here, to run repeatedly:
}
