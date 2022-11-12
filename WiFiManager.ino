#include "WiFiManager.h"
#include "settings.h"
#include <ArduinoJson.h>
#define FORMAT_SPIFFS_IF_FAILED true

//name of the log file
const char * CONFIG_FILE = "/config.json";

//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback() {
  //Serial.println("Should save config");
  shouldSaveConfig = true;
}

//default custom static IP
char static_ip[16] = "";
char static_gw[16] = "";
char static_sn[16] = "";
char static_dns[16] = "";

static unsigned long lastWiFiBeginTimeStamp = 0;
#define WIFI_DELAY_INTERVAL 1500//milliseconds

void attempt_connect_wifi_with_animation(const char * ssid = "", const char * password = "") {
  xx = -48; //begin off the screen and "walk" into view
  int i = 0;
  int numFrames = 28;
  const unsigned char* frames[numFrames] = {walker_frame0, walker_frame1, walker_frame2, walker_frame3, walker_frame4, walker_frame5, walker_frame6, walker_frame7, walker_frame8, walker_frame9, walker_frame10, walker_frame11, walker_frame12, walker_frame13, walker_frame14, walker_frame15, walker_frame16, walker_frame17, walker_frame18, walker_frame19, walker_frame20, walker_frame21, walker_frame22, walker_frame23, walker_frame24, walker_frame25, walker_frame26, walker_frame27};
  for (i; i < 6; ++i) {
    for (int j = 0; j < numFrames; ++j) {
      //this is the hacky trick to fix WiFi only connects every other reboot issue. call WiFi.begin() every 1.5 seconds (want it to be called at least 3 times for this to work)
      if (millis() - lastWiFiBeginTimeStamp > WIFI_DELAY_INTERVAL) {
        if (ssid != "" && password != "") {          
          WiFi.begin(ssid, password);
        } else {
          WiFi.begin();
        }
        lastWiFiBeginTimeStamp = millis();
      }
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 50);
      display.println("connecting wifi...");
      display.drawBitmap(xx += 1, yy, frames[j], 48, 48, 1);
      display.display();
    }
  }
}


void initWifi() {
  initWiFiConfig(); //for saving wifi info for WiFiManager

  attempt_connect_wifi_with_animation(); //show a cool graphic while you wait for the wifi to connect!

  if (WiFi.status() != WL_CONNECTED) {
    int result = WiFi.scanNetworks(true); //scan for wifi networks async
    Serial.print("Scanning wifi networks");
    while (WiFi.scanComplete() == WIFI_SCAN_RUNNING) {
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 50);
      display.println("scanning for networks...");
      delay(100);
    }
    result = WiFi.scanComplete();
    Serial.println(String(result) + " wifi networks found: ");
    
    //iterate through networks
    int networks[result];
    for (int k = 0; k < result; ++k) {      
      Serial.println(WiFi.SSID(networks[k]));      
    }

    if (SPIFFS.exists(CONFIG_FILE)) {
      Serial.println("initWiFiConfig() method: config.json file exists, reading...");
      File configFile = SPIFFS.open(CONFIG_FILE, "r");
      if (configFile) {
        //Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);
        configFile.readBytes(buf.get(), size);
        DynamicJsonDocument json(2048);
        deserializeJson(json, buf.get());

        if (!json.isNull()) {
          const char * ssid;
          const char * password;
          byte saved_networks = json["elements"];
          for (int i = 0; i < saved_networks; ++i) {
            JsonObject root = json["wificonfiguration"][i].as<JsonObject>();

            // using C++11 syntax (preferred):
            for (JsonPair key_value : root) {
              if (key_value.key() == "ssid") ssid = key_value.value().as<char*>();
              if (key_value.key() == "password") password = key_value.value().as<char*>();
             
              //iterate through networks
              int indices[result];
              for (byte j = 0; j < result; ++j) {
                //get the ssid from the scan and check if it is a match to one in our config file
                String cssid = WiFi.SSID(indices[j]);
                if (cssid == String(ssid)) attempt_connect_wifi_with_animation(ssid, password);
              }
            }
          }
        }
      }
    }
  }

  //set config save notify callback
  wm.setSaveConfigCallback(saveConfigCallback);

  // set static ip
  IPAddress _ip, _gw, _sn, _dns;
  _ip.fromString(static_ip);
  _gw.fromString(static_gw);
  _sn.fromString(static_sn);
  _dns.fromString(static_dns);
  //if(_ip) wm.setSTAStaticIPConfig(_ip, _gw, _sn, _dns);


  wm.setConnectTimeout(15); //amount of time it will look for a known wifi network
  //wm.setTimeout(120); //config portal closes after this many seconds

  if (WiFi.status() != WL_CONNECTED) {
    showVomiterAnimation();
    display.setTextSize(1);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("WiFi connect Fail");
    display.println();
    display.println("Connect phone");
    display.println("to WiFi network:");
    display.println("AutoConnectAP");
    display.println("\n");
    display.println("192.168.4.1");
    display.display();
    display.setTextSize(2);
  }

  //automatically connect using saved credentials if they exist
  //If connection fails it starts an access point with the specified name
  //here  "AutoConnectAP" if empty will auto generate based on chipid, if password is blank it will be anonymous
  //and goes into a blocking loop awaiting configuration
  if (!wm.autoConnect("AutoConnectAP")) {
    //Serial.println("failed to connect and hit timeout");
  }

  //if here, WiFi should be connected
  //save the custom parameters to FS

  if (shouldSaveConfig) {
    save_configfile();

    shouldSaveConfig = false;

    display.setTextSize(1);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("WiFi successful");
    display.println("rebooting...");
    display.println("3...");
    display.display();
    delay(1000);
    display.println("2...");
    display.display();
    delay(1000);
    display.println("1...");
    display.display();
    delay(1000);
    esp_restart(); //reboot because there is a bug somewhere in the tcp/ip or wifi library and wifi is stuck in ap mode
  }
}

//open file and store contents to buffer
//deserialize json
//get number of elements in the json file
//get the elements of the file

/*
bool read_config_file( output) {
  bool result= false;
  if (SPIFFS.exists(CONFIG_FILE)) {
    Serial.println("initWiFiConfig() method: config.json file exists, reading...");
    File configFile = SPIFFS.open(CONFIG_FILE, "r");
    if (configFile) {
      //Serial.println("opened config file");
      size_t size = configFile.size();
      // Allocate a buffer to store contents of the file.
      std::unique_ptr<char[]> buf(new char[size]);
      configFile.readBytes(buf.get(), size);
      deserializeJson(output, buf.get());
      DynamicJsonDocument json(2048);
      deserializeJson(json, buf.get());

      if (!json.isNull()) {
        int s = json["elements"];
        for (int i = 0; i < s; ++i) {
          JsonObject root = json["wificonfiguration"][i].as<JsonObject>();

          // using C++11 syntax (preferred):
          for (JsonPair key_value : root) {
            key_value.key().c_str();
            key_value.value().as<char*>();
          }
        }
      result = true;
    }
  }
  return result;
}
*/

void initWiFiConfig() {
  DynamicJsonDocument json(2048);
  //bool result = read_config_file(json);
  //if(result) Serial.println("Read config file");
  
  if (SPIFFS.exists(CONFIG_FILE)) {
    Serial.println("initWiFiConfig() method: config.json file exists, reading...");
    File configFile = SPIFFS.open(CONFIG_FILE, "r");
    if (configFile) {
      //Serial.println("opened config file");
      size_t size = configFile.size();
      // Allocate a buffer to store contents of the file.
      std::unique_ptr<char[]> buf(new char[size]);
      configFile.readBytes(buf.get(), size);
      DynamicJsonDocument json(2048);
      deserializeJson(json, buf.get());

      if (!json.isNull()) {
        int s = json["elements"];
        for (int i = 0; i < s; ++i) {
          JsonObject root = json["wificonfiguration"][i].as<JsonObject>();

          // using C++11 syntax (preferred):
          for (JsonPair key_value : root) {
            Serial.println(key_value.key().c_str());
            Serial.println(key_value.value().as<char*>());
          }
        }
        //Serial.println("wifimanager json config file contains a previously saved IP:");
        //serializeJson(json, Serial); //print to Serial port
        /*
        strcpy(static_ip, json["wificonfiguration"][0]["ip"]);
        strcpy(static_gw, json["wificonfiguration"][0]["gateway"]);
        strcpy(static_sn, json["wificonfiguration"][0]["subnet"]);
        strcpy(static_dns, json["wificonfiguration"][0]["dns"]);
        */
        //Serial.println(static_ip);
      } else {
        Serial.println("initWiFiConfig() method: config.json file not found...");
      }
    }
  }
}

void save_configfile() {
  int numElements = 0;
  File configfile;
  DynamicJsonDocument jsonconfig(2048);

  StaticJsonDocument<256> newjson; //Use a StaticJsonDocument to store in the stack (recommended for documents smaller than 1KB)
  newjson["ssid"]       = wm.getWiFiSSID();
  newjson["password"]   = wm.getWiFiPass();
  newjson["ip"]         = WiFi.localIP().toString();
  newjson["gateway"]    = WiFi.gatewayIP().toString();
  newjson["subnet"]     = WiFi.subnetMask().toString();
  newjson["dns"]        = WiFi.dnsIP().toString();

  //if configfile already exists, we just add to it
  if (SPIFFS.exists(CONFIG_FILE)) {
    configfile = SPIFFS.open(CONFIG_FILE, "r");
    size_t filesize = configfile.size();
    // Allocate a buffer to store contents of the file.
    std::unique_ptr<char[]> bufr(new char[filesize]);
    configfile.readBytes(bufr.get(), filesize);
    deserializeJson(jsonconfig, bufr.get());
    configfile.close();
    //increment the counter
    numElements = jsonconfig["elements"];
    ++numElements;
    jsonconfig["elements"] = numElements;
    //add the new line
    jsonconfig["wificonfiguration"].add(newjson);
  } else {
    //configfile doesnt exist yet, so we have to build the json before writing the config file
    numElements = 1;
    jsonconfig["elements"] = numElements;
    JsonArray array = jsonconfig.createNestedArray("wificonfiguration");
    array.add(newjson);
  }

  //write to the config file
  File writeFile = SPIFFS.open(CONFIG_FILE, "w");
  if (writeFile) {
    serializeJson(jsonconfig, writeFile);
    writeFile.close();
  }
}
