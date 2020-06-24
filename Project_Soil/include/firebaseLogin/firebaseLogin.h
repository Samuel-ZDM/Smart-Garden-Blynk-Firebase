#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>

#ifndef FIRELOGIN_UTIL_H_
#define FIRELOGIN_UTIL_H_



#define FIREBASE_HOST   "https://esp32-b6beb.firebaseio.com" //YOUR-FIREBASE_HOST //Without http:// or https:// 
#define FIREBASE_AUTH  "RKZVtG5dys5v6pMNmiTF5cDFMzJxZWoiapXDwzlJ" //YOUR-FIREBASE_AUTH 
#define WIFI_SSID    "Dal Magro(ZeP)" //SSID-YOUR-WIFI
#define WIFI_PASSWORD "a3b8c4d9"//PASSWORD-YOUR-WIFI

/*
 *  Connect on Wifi
*/
void connectWifi();

/*
 *  Connect on Firebase
*/
void connectFire();

/*
 *  Get bool variable value on Firebase 
*/
bool getBool(String value);

/*
 *  Get int variable value on Firebase 
*/
int getInt(String value);

/*
 *  Get float variable value on Firebase 
*/
float getFloat(String value);

/*
 *  Set float variable value on Firebase 
*/
void setFloat(String value, float sensor);

/*
 *  Set int variable value on Firebase 
*/
void setInt(String value, int sensor);

/*
 *  Set bool variable value on Firebase 
*/
void setBool(String value, bool sensor);

#endif /* FIRELOGIN_UTIL_H_ */