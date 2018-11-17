#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#define SYNC_INTERVAL 1000
#define FIREBASE_HOST "mailbell-1b6f5.firebaseio.com"
#define FIREBASE_AUTH "tXJnjHuhAQOiSfEpg9vEmvytNKrwbaHMeesQG0cc"
#define WIFI_SSID "mailbellwifi"
#define WIFI_PASSWORD "1223334444"

void setup_firebase() {
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void setup() {
  //Leave the serial monitor code.
  //Can change the baudrate
  Serial.begin(9600);
  setup_firebase();

  //Setup the other code here---
  
}

//when the light is on the mail needs checked 
//don't listen to the mailbox sensor
bool light_is_on = true;


//here put the code to turn off the indicator light
void mail_was_checked() {
  Serial.println("Mail checked");
  //TODO set some pin to turn off the light
  
  Firebase.setBool("needs_checking", false);
  if (Firebase.failed()) {
    Serial.print("setting needs_checking failed: ");
    Serial.println(Firebase.error());
  }
  light_is_on = false;  
}

//code run when sensor says mail was deposited
void mailbox_opened() {
  Serial.println("Mail deposited");
  //TODO need to set some pin to turn on the light
  
  Firebase.setBool("needs_checking", true);
  if (Firebase.failed()) {
    Serial.print("setting needs_checking failed: ");
    Serial.println(Firebase.error());
  }
  light_is_on = true;  
}

unsigned long last_sync = 0;
unsigned long last_opened = 0;
void loop() {
  if (light_is_on && (millis() > last_sync + SYNC_INTERVAL)) {
    bool needs_checking = true; //give a default value
    needs_checking = Firebase.getBool("needs_checking");
    Serial.println(needs_checking);
    last_sync = millis();
    if (!needs_checking) {
      mail_was_checked();
    }
  }
  //TODO Listen for the mailbox to tell us it was opened
  
  if (millis() > last_opened + 10000) {
    last_opened = millis();
    if (!light_is_on) {
      mailbox_opened();
    }else {
      mail_was_checked();
    }
  }
    
  
  delay(100);
}
