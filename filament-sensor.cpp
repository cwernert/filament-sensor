#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Tone.cpp>

/* ADJUSTABLE CONSTANTS */
String jam = "{\"message\":\"3D printer has jammed, or run out of filament.\"}";
String dun = "{\"message\":\"3D printer has finished.\"}";
String ssid = "YOUR_WIFI_SSID_HERE";
String pass = "YOUR_WIFI_PASSWORD_HERE";
String url = "YOUR_ZAPIER_WEBHOOK_URL_HERE";

void setup(){
  int wled = 0;
  pinMode(5,INPUT_PULLUP);      //D1: Sensor
  pinMode(4, OUTPUT);           //D2: Speaker
  pinMode(0, OUTPUT);           //D3: Rotation LED
  pinMode(2, OUTPUT);           //D4: WiFi LED
  pinMode(14, INPUT_PULLUP);    //D5: Completion magnet Pwr Sup
  pinMode(12, OUTPUT);          //D6: Completion magnet Ground
  Serial.begin(115200);
  Serial.println();

  digitalWrite(12,LOW);         //prep 12 as a pseudo ground pin
  digitalWrite(2,0);            //make sure the wifi led is off until connection established

  WiFi.begin(ssid,pass);
  Serial.print("Connecting");
  while (WiFi.status()!=WL_CONNECTED){
    if(wled==0){
      wled=1;
    }else{
      wled=0;
    }
    digitalWrite(2,wled);
    delay(500);
    Serial.print(".");
  }
  digitalWrite(2,1);
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

unsigned long tc = 0;        //time current
unsigned long tp = 0;        //time previous
unsigned long lp = 0;        //last ping
unsigned long lb = 0;        //last beep
unsigned long st = 0;        //start time - set at first loop iteration when completion magnet is disconnected
int beeps = 0;               //beep counter
bool previousState = 0;      //sensor position on the last iteration
bool currentState = 0;       //sensor position on this iteration
bool alarmState = false;     //whether or not the alarm is on

void loop(){
  tc = millis();                              //get the timestamp for this iteration
  if(digitalRead(14)==LOW){                   //if the magnet is connected
    //Serial.println("Magnet is connected");
    if((st>0)&&(st+300000<tc)){               //if the magnet was not connected at boot, and if 5mins (300000 ms) has passed since disconnection
      pingZapier(dun);
    }
    st=0;                                     //reset st to zero
  }else{                                      //do the stuff
    //Serial.println("Magnet is NOT connected");
    if(st==0){ st = tc; }                     //set the timestamp of the magnet disconnection
    currentState = digitalRead(5);            //take a reading from the rotation sensor
    digitalWrite(0,currentState);             //reflect the sensor status on the sensor LED
    if(currentState == previousState){
      if((tp+4000<tc)&&(st+300000<tc)){       //if the status has been the same for more than 4s & its been >5m since magnet disconnect
        alarmState = true;                    //turn on the alarm
        if(lp+60000<tc){                      //and if the last ping was >1m ago
          pingZapier(jam);
          lp = tc;                            //set the "last pinged" timestamp to now
        }
      }else{
        alarmState = false;                   //otherwise turn the alarm off
      }
    }else{                                    //if the status is different to the last check
      alarmState = false;                     //turn the alarm off
      tp = tc;                                //reset the threshold to zero (logically, not literally)
    }
    previousState = currentState;             //update the comparitor ready for the next iteration
  
    if( (alarmState) && (tc - lb >= 1000) ){  //if the alarm should be active & it has been at least 1s since last beep
      tone(4,600,500);                        //beep
      lb = tc;                                //set the last beep timestamp to now
    }else{
      noTone(1);                              //otherwise we should not be beeping now, silence the speaker
    }
  } //end of magnetcheck
}

void pingZapier(String json){
  if (WiFi.status() == WL_CONNECTED){       //if the wifi is connected
    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(json);       //ping Zapier the json payload
    String payload = http.getString();
    Serial.println(httpCode);             //report the http status
    Serial.println(payload);              //log Zapier's response
    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }
}