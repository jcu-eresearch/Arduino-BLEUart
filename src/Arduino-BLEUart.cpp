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
};

void ArduinoBLEUart::end()
{
    
};

ArduinoBLEUart::~ArduinoBLEUart()
{
    callback_object = nullptr;
};

void ArduinoBLEUart::handle_rx_event(BLEDevice &device, BLECharacteristic &characteristic)
{
    
    BLEStringCharacteristic *chr = (BLEStringCharacteristic*)&characteristic;
    String v = chr->value();
    const char *buf = v.c_str();
    for(size_t i = 0; i < v.length(); i++)
    {
       rx_buffer.push(buf[i]);
    }
};

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
    return !rx_buffer.empty();
};

int ArduinoBLEUart::read()
{
   
    if(!blocking && rx_buffer.empty()){
        return -1;
    }
    int result = rx_buffer.front();
    rx_buffer.pop();
    return result;
};

int ArduinoBLEUart::peek()
{
    if(!blocking && rx_buffer.empty()){
        return -1;
    }
    int result = rx_buffer.front();
    return result;    
};

size_t ArduinoBLEUart::write(uint8_t value)
{
    Serial.println("write");
    tx_buffer.push(value);
    return 1;
};

size_t ArduinoBLEUart::write(const uint8_t *buffer, size_t size)
{
    for(size_t i = 0; i < size; i++)
    {
        tx_buffer.push(buffer[i]);
    }
    return _flush();
};    

void ArduinoBLEUart::flush() {
    _flush();
    if (tx_buffer.size() != 0)
    {
        status = ArduinoBLEUart_Buffer_Not_Empty;
    }    
};

size_t ArduinoBLEUart::_flush()
{
    size_t count = 0;
    size_t tx_size = tx_buffer.size();
    size_t block_size = tx_size > this->buffer_size?this->buffer_size:tx_size;
    char *data = (char*)malloc(block_size+1);
    memset(data, 0, block_size+1);
    while(!tx_buffer.empty()){
        size_t tmp_size = tx_buffer.size() > block_size?block_size:tx_buffer.size();
        for(int i = 0; i < tmp_size; i++)
        {
            data[i] = tx_buffer.front();
            tx_buffer.pop();
            count++;
        }
        txCharacteristic.writeValue(data, tmp_size);
    }
    free(data);
    return count;
}

void ArduinoBLEUart::setdevice(BLEDevice *central)
{
    this->device = central;
};

enum ArduinoBLEUart_Status_e ArduinoBLEUart::getStatus()
{
    return this->status;
};