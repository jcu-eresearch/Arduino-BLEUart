/*
 * Arduino-BLEUart uses the ArduinoBLE library to create a BLE UART that
 * with the same Characteristics as the Nordic UART Service (NUS).
 * 
 * Copyright (C) 2021 eResearch, James Cook University
 * Author: NigelB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "ArduinoBLE.h"

BLEStringCharacteristic rxCharacteristic("6E400002-B5A3-F393-E0A9-E50E24DCCA9E", BLEWrite | BLENotify, 20);
BLEByteCharacteristic txCharacteristic("6E400003-B5A3-F393-E0A9-E50E24DCCA9E", BLERead | BLENotify);
BLEService uartService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");

void rx_callback(BLEDevice device, BLECharacteristic characteristic)
{
    Serial.printf("RXed Characteristic: %p\r\n", &characteristic);
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting");

    uartService.addCharacteristic(rxCharacteristic);
    uartService.addCharacteristic(txCharacteristic);
    BLE.begin();
    
    Serial.printf("rx Char: %p\r\n", &rxCharacteristic);
    BLE.setDeviceName("BLE_Uart");
    BLE.setLocalName("BLE_Uart");
    BLE.setAdvertisedService(uartService);
    BLE.addService(uartService);

    rxCharacteristic.setEventHandler(BLEWritten, rx_callback);
    rxCharacteristic.setEventHandler(BLEWritten, [](BLEDevice device, BLECharacteristic characteristic)->void{
        Serial.printf("RXed Characteristic: %p\r\n", &characteristic);
    });

    BLE.advertise();
    Serial.println("BLE advertising as 'BLE_Uart'");
}


void loop()
{
   BLEDevice central = BLE.central();    
   if (central) {

    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
    long last = 0;
    while(central.connected())
    {
        if(millis() - last > 1000)
        {
            txCharacteristic.writeValue('.');
            last = millis();
        }
    }
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());


   }   
}