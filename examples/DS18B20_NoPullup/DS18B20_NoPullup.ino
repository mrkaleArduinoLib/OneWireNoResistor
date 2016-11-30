/*
  NAME:
  Usage of DS18B20 sensors without pullup resistor

  DESCRIPTION:
  The sketch measures temperature with all DS18B20 sensors on the bus.
  - Sensor is powered from GPIOs by plugging directly to the Arduino header pins 8,9, and 10.
  - If flat side of the sensor faces into the center of the board, then
    pin 8 is VDD, pin 9 is DQ, and pin 10 is GND. 
  - If round side of the sensor faces into the center of the board, then
    pin 8 is GND, pin 9 is DQ, and pin 10 is VDD.
  - The sketch initially uses the sensor in parasite power mode with pin 8 and 10
    set to LOW so that it does not matter the orientation of the sensor.  

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the MIT License (MIT).

  CREDENTIALS:
  Author: Libor Gabaj
*/
#include "OneWire.h"
#include "DS18B20.h"

#define SKETCH "DS18B20_NoPullup 1.0.0"

const unsigned int PERIOD_MEASURE = 2000;   // Time in miliseconds between measurements

// Hardware configuration
const unsigned char PIN_GND = 8;
const unsigned char PIN_DQ  = 9;
const unsigned char PIN_VDD = 10;

// Measurement
DS18B20 ds(PIN_DQ);
unsigned char deviceCount;

void setup()
{
    Serial.begin(9600);
    Serial.println(F(SKETCH));
    Serial.println(F("Libraries:"));
    Serial.println(F(DS18B20_VERSION));
    Serial.println(F(ONEWIRE_VERSION));
    Serial.println(F("---"));

    // Setup powering pins
    pinMode(PIN_GND, OUTPUT);
    digitalWrite(PIN_GND, LOW);
    pinMode(PIN_VDD, OUTPUT);
    digitalWrite(PIN_VDD, LOW);
  
    // Devices scanning
    Serial.println(F("#\tMODEL\tRES\tPOWER\t\tID\tADDRESS"));
    while(ds.selectNext())
    {
        // Print order number
        Serial.print(++deviceCount);
        Serial.print(F(".\t"));
        // Print family name
        switch(ds.getFamilyCode())
        {
            case DS18B20_MODEL_DS18S20:
                Serial.print(F("DS18S20"));
                break;
            case DS18B20_MODEL_DS1820:
                Serial.print(F("DS1820"));
                break;
            case DS18B20_MODEL_DS18B20:
                Serial.print(F("DS18B20"));
                break;
            default:
                Serial.print(F("UNKNOWN"));
        }
        // Print resolution
        Serial.print(F("\t"));
        switch (ds.getResolution())
        {
            case DS18B20_RES_9_BIT:
                Serial.print(F("9"));
                break;
            case DS18B20_RES_10_BIT:
                Serial.print(F("10"));
                break;
            case DS18B20_RES_11_BIT:
                Serial.print(F("11"));
                break;
            case DS18B20_RES_12_BIT:
                Serial.print(F("12"));
                break;
        }
        Serial.print(F("-bit"));
        // Print power mode
        Serial.print(F("\t"));
        if(ds.getPowerMode())
        {
            Serial.print(F("External"));
        }
        else
        {
            Serial.print(F("Parasite"));
        }
        // Print identifier
        char addressId[DS18B20_PRINT_ID_LEN];
        ds.printAddressId(addressId);
        Serial.print(F("\t"));
        Serial.print(addressId);
        // Print address
        char address[DS18B20_PRINT_ADDRESS_LEN];
        ds.printAddress(address);
        Serial.print(F("\t"));
        Serial.println(address);
    }
    // Scanning results
    if (deviceCount == 0) Serial.println("N/A");
    Serial.print(F("*** Found "));
    Serial.print(deviceCount);
    Serial.print(F(" device"));
    if (deviceCount > 1) Serial.print(F("s"));
    Serial.println(F(" ***"));
    Serial.println(F("---"));
}

void loop()
{
  measure();
}

void measure()
{
    static unsigned long tsMeasure;
    if (millis() - tsMeasure >= PERIOD_MEASURE || tsMeasure == 0)
    {
        tsMeasure = millis();
        ds.processConversion();
        ds.resetSearch();
        byte i = 0; 
        while(ds.selectNext())
        {
            Serial.print(++i);
            Serial.print(F(". "));
            Serial.print(ds.readTemperature(), 3);
            Serial.print(F(" 'C"));
            Serial.println();
        }
        Serial.println();
    }
}
