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

#include "Arduino-BLEUart.h"

static ArduinoBLEUart *callback_object = NULL;

bool ArduinoBLEUart::begin(bool blocking)
{
    this->blocking = blocking;
    BLE.addService(uartService);
    BLE.setAdvertisedService(uartService);
    
    if(callback_object != NULL)
    {
        return false;
    }

    callback_object = this;

    rxCharacteristic.setEventHandler(BLEWritten, [](BLEDevice device, BLECharacteristic characteristic)->void{
        if(callback_object != NULL)
        {
            callback_object->handle_rx_event(device, characteristic);
        }else{
            Serial.println("ERROR! Callback Object is NULL!");
        }
    });    
    return true;
}

void ArduinoBLEUart::end()
{
    
}

ArduinoBLEUart::~ArduinoBLEUart()
{

}

void ArduinoBLEUart::handle_rx_event(BLEDevice &device, BLECharacteristic &characteristic)
{
    
    BLEStringCharacteristic *chr = (BLEStringCharacteristic*)&characteristic;
    String v = chr->value();
    const char *buf = v.c_str();
    for(size_t i = 0; i < v.length(); i++)
    {
       buffer.push(buf[i]);
    }
}

int ArduinoBLEUart::available()
{
    if(device != NULL)
    {
        if(!device->connected())
        {
            return ArduinoBLEUart_Disconnected;
        }
    }else{
        return ArduinoBLEUart_NullPTR;
    }
    return !buffer.empty();
}

int ArduinoBLEUart::read()
{
   
    if(!blocking && buffer.empty()){
        return -1;
    }
    int result = buffer.front();
    buffer.pop();
    return result;
}

int ArduinoBLEUart::peek()
{
    if(!blocking && buffer.empty()){
        return -1;
    }
    int result = buffer.front();
    return result;    
}

size_t ArduinoBLEUart::write(uint8_t value)
{
    return txCharacteristic.writeValue(value);
}


void ArduinoBLEUart::setdevice(BLEDevice *central)
{
    this->device = central;
}