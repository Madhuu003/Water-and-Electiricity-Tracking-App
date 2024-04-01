
#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "E5938"
#define WIFI_PASSWORD "12345678"

// Insert Firebase project API Key
#define API_KEY "AIzaSyCOzEDNQK5Iaj64lc-SjztkYQPuk4-Ri4o"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://electricitymonitor-a6704-default-rtdb.firebaseio.com/"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;
bool flag1 = false;
bool flag2 = false;
bool flag3 = false;

String tdata = "", hdata = "", ldata = "", udata = "";
int count = 0, count1 = 0,  count2 = 0, count3 = 0;
int temp;
int heart_rate;
int level;
int a;

void setup() {
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  while (Serial.available() > 0)
  {
    char inchar = Serial.read();
    //Serial.print(inchar);
    if (inchar == '*')
    {
      while (Serial.available() > 0)
      {
        char t = Serial.read();
        //        if (count == 3)
        //        {
        //          flag1 = true;
        //          temp = tdata.toInt();
        //          Serial.println(temp);
        //          break;
        //        }
        if (t == '#') {
          a++;
          flag1 = true;
          temp = tdata.toInt();
          Serial.println(temp);
          if (Firebase.RTDB.setString(&fbdo, "dailyuse/day" + String(a) + "/day", "day"+String(a))) {
            Serial.println("PASSED");
            Serial.println("PATH: " + fbdo.dataPath());
            Serial.println("TYPE: " + fbdo.dataType());
          }
          else {
            Serial.println("FAILED");
            Serial.println("REASON: " + fbdo.errorReason());
          }
          if (Firebase.RTDB.setString(&fbdo, "dailyuse/day" + String(a) + "/usage", tdata)) {
            Serial.println("PASSED");
            Serial.println("PATH: " + fbdo.dataPath());
            Serial.println("TYPE: " + fbdo.dataType());
            tdata="";
            count=0;
          }
          else {
            Serial.println("FAILED");
            Serial.println("REASON: " + fbdo.errorReason());
          }
          if (Firebase.RTDB.setString(&fbdo, "dailyuse/day" + String(a) + "/status", "accept")) {
            Serial.println("PASSED");
            Serial.println("PATH: " + fbdo.dataPath());
            Serial.println("TYPE: " + fbdo.dataType());
          }
          else {
            Serial.println("FAILED");
            Serial.println("REASON: " + fbdo.errorReason());
          }

          break;
        }
        tdata += t;
        count++;
      }
    }
    if (inchar == '@')
    {
      while (Serial.available() > 0)
      {
        char h = Serial.read();
        //        if (count1 == 3)
        //        {
        //          heart_rate = hdata.toInt();
        //          Serial.println(heart_rate);
        //          flag2 = true;
        //          break;
        //        }
        if (h == '#') {
          flag2 = true;
          heart_rate = hdata.toInt();
          Serial.println(heart_rate);
          if (Firebase.RTDB.setString(&fbdo, "dailyuse/day" + String(a) + "/waterflow", hdata)) {
            Serial.println("PASSED");
            Serial.println("PATH: " + fbdo.dataPath());
            Serial.println("TYPE: " + fbdo.dataType());
            hdata = "";
            count1 = 0;
          }
          else {
            Serial.println("FAILED");
            Serial.println("REASON: " + fbdo.errorReason());
          }
          break;
        }
        hdata += h;
        count1++;
      }
    }
    if (inchar == '!')
    {
      while (Serial.available() > 0)
      {
        char l = Serial.read();

        //        if (count2 == 3)
        //        {
        //          flag3 = true;
        //          //          level = ldata.toInt();
        //          //          Serial.println(level);
        //
        //          break;
        //        }
        if (l == '#') {
          flag3 = true;
          level = ldata.toInt();
          Serial.println(level);
          if (Firebase.RTDB.setString(&fbdo, "monitoring/update/waterflow", ldata)) {
            Serial.println("PASSED");
            Serial.println("PATH: " + fbdo.dataPath());
            Serial.println("TYPE: " + fbdo.dataType());
            ldata = "";
            count1 = 0;
          }
          else {
            Serial.println("FAILED");
            Serial.println("REASON: " + fbdo.errorReason());
          }
          break;
        }
        ldata += l;
        count2++;
      }
    }
    if (inchar == '$')
    {
      while (Serial.available() > 0)
      {
        char u = Serial.read();

        //        if (count2 == 3)
        //        {
        //          flag3 = true;
        //          //          level = ldata.toInt();
        //          //          Serial.println(level);
        //
        //          break;
        //        }
        if (u == '#') {
          flag3 = true;
          level = udata.toInt();
          Serial.println(level);
          if (Firebase.RTDB.setString(&fbdo, "monitoring/update/useage", udata)) {
            Serial.println("PASSED");
            Serial.println("PATH: " + fbdo.dataPath());
            Serial.println("TYPE: " + fbdo.dataType());
            udata = "";
            count1 = 0;
          }
          else {
            Serial.println("FAILED");
            Serial.println("REASON: " + fbdo.errorReason());
          }
          break;
        }
        udata += u;
        count2++;
      }
    }
    Serial.println();
  }
  //Serial.print("AAA");
  //  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
  //    sendDataPrevMillis = millis();
  //    // Write an Int number on the database path test/int
  //    //Serial.println("PASSED");
  //    if (Firebase.RTDB.setString(&fbdo, "Payment/detail/data", tdata)) {
  //      Serial.println("PASSED");
  //      Serial.println("PATH: " + fbdo.dataPath());
  //      Serial.println("TYPE: " + fbdo.dataType());
  //      tdata = "";
  //      count = 0;
  //    }
  //    else {
  //      Serial.println("FAILED");
  //      Serial.println("REASON: " + fbdo.errorReason());
  //    }
  //    if (Firebase.RTDB.setInt(&fbdo, "test1/int", heart_rate)) {
  //      Serial.println("PASSED");
  //      Serial.println("PATH: " + fbdo.dataPath());
  //      Serial.println("TYPE: " + fbdo.dataType());
  //    }
  //    else {
  //      Serial.println("FAILED");
  //      Serial.println("REASON: " + fbdo.errorReason());
  //    }
  //    if (Firebase.RTDB.setInt(&fbdo, "test2/int", level)) {
  //      Serial.println("PASSED");
  //      Serial.println("PATH: " + fbdo.dataPath());
  //      Serial.println("TYPE: " + fbdo.dataType());
  //    }
  //    else {
  //      Serial.println("FAILED");
  //      Serial.println("REASON: " + fbdo.errorReason());
  //    }

  //}

  //  hdata = "";
  //  ldata = "";

  delay(3000);
}
