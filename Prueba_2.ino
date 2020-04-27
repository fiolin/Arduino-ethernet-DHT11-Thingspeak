/*
  WriteMultipleFields
  
  Description: Writes values to fields 1,2,3 and 4  in a single ThingSpeak update every 20 seconds.
  
  Hardware: Arduino Ethernet
  
  !!! IMPORTANT - Modify the secrets.h file for this project with your network connection and ThingSpeak channel details. !!!
  
  Note:
 - Requires the Ethernet library
  
  ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize, and 
  analyze live data streams in the cloud. Visit https://www.thingspeak.com to sign up for a free account and create a channel.  
  
  Documentation for the ThingSpeak Communication Library for Arduino is in the README.md folder where the library was installed.
  See https://www.mathworks.com/help/thingspeak/index.html for the full ThingSpeak documentation.
  
  For licensing information, see the accompanying license file.
  
  Copyright 2018, The MathWorks, Inc.
*/


#include "ThingSpeak.h"
#include <Ethernet.h>
#include "DHT.h"
#define DHTTYPE DHT11   // DHT 11
const int DHTPin = 5;     // what digital pin we're connected to 
DHT dht(DHTPin, DHTTYPE);

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 1, 120);
IPAddress myDns(192, 168, 1, 1);

EthernetClient client;

unsigned long myChannelNumber = 1047542;
const char * myWriteAPIKey ="W1YCW660054LBOP8";



void setup() {
  Ethernet.init(10);  // Most Arduino Ethernet hardware
  Serial.begin(9600);  //Initialize serial
    
  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  dht.begin();
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
  float Humedad = dht.readHumidity();
   float Temperatura = dht.readTemperature();
   if (isnan(Humedad) || isnan(Temperatura)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
   }
   Serial.print("Humidity: ");
   Serial.print(Humedad);
   Serial.println(" %\t");
   Serial.print("Temperature: ");
   Serial.print(Temperatura);
   Serial.println(" *C ");
  
  // set the fields with the values
  ThingSpeak.setField(1, Temperatura);
  ThingSpeak.setField(2, Humedad);
  
  //ThingSpeak.setField(3, number3);
  //ThingSpeak.setField(4, number4);

  
  // write to the ThingSpeak channel 
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
 
  
  delay(20000); // Wait 20 seconds to update the channel again
}
