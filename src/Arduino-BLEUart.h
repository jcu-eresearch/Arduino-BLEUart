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

#ifndef ARGOSTAG_ARDUINO_BLEUART_H
#define ARGOSTAG_ARDUINO_BLEUART_H

#include "ArduinoBLE.h"
#include "queue"

enum ArduinoBLEUart_Status_e
{
    ArduinoBLEUart_OK = 1,
    ArduinoBLEUart_NullPTR = -1,
    ArduinoBLEUart_Disconnected = -2,
    ArduinoBLEUart_Buffer_Not_Empty = -3,
};

class ArduinoBLEUart: public Stream
{
    BLEService uartService;
    BLEStringCharacteristic rxCharacteristic;
    BLECharacteristic txCharacteristic;
    
    std::queue<char> rx_buffer;
    std::queue<char> tx_buffer;
    int buffer_size;
    enum ArduinoBLEUart_Status_e status;
    
    private:
        void handle_rx_event(BLEDevice &device, BLECharacteristic &characteristic);
        bool blocking = false;
        BLEDevice *device = NULL;
    public:
        ArduinoBLEUart(int buffer_size):
            uartService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E"),
            rxCharacteristic("6E400002-B5A3-F393-E0A9-E50E24DCCA9E", BLEWrite | BLENotify, buffer_size),
            txCharacteristic("6E400003-B5A3-F393-E0A9-E50E24DCCA9E", BLERead | BLENotify, buffer_size)
        {
            uartService.addCharacteristic(rxCharacteristic);
            uartService.addCharacteristic(txCharacteristic);
            Serial.println("ArduinoBLEUart");
            device = NULL;
            this->buffer_size = buffer_size;
            status = ArduinoBLEUart_OK;
        }
        ~ArduinoBLEUart();
        bool begin(bool blocking = false);
        void end();

        int available();
        int read();
        int peek();
        size_t write(uint8_t value);
        size_t write(const uint8_t *buffer, size_t size);

    
        virtual void flush();        
        size_t _flush();    
        void setdevice(BLEDevice *central);
        enum ArduinoBLEUart_Status_e getStatus();

};

#endif
