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

#include "Arduino.h"
#include "Arduino-BLEUart.h"

ArduinoBLEUart *uart;

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting");
    BLE.begin();
    BLE.setDeviceName("BLE_Uart");
    BLE.setLocalName("BLE_Uart");
    uart = new ArduinoBLEUart(100);
    uart->begin();
    BLE.advertise();
    Serial.println("BLE advertising as 'BLE_Uart'");

}


void loop()
{
   BLEDevice central = BLE.central();    
   if (central) {

    Serial.print("Connected to central: ");
    Serial.println(central.address());
    long last = 0;
    while(central.connected())
    {
        if(millis() - last > 1000)
        {
            uart->write('.');
            last = millis();
        }
        if(uart->available()){
            Serial.print("< ");
            while(uart->available())
            {
                char c = (char)uart->read();
                Serial.print(c);
                if(c == 10)
                {
                    c = 13;
                    Serial.print(c);
                }
            }
            
        }
    }
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());


   }   
}