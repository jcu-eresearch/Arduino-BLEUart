//
// Created by nigelb on 15/11/21.
//

#ifndef ARGOSTAG_ARDUINO_BLEUART_H
#define ARGOSTAG_ARDUINO_BLEUART_H

#include "ArduinoBLE.h"
#include "queue"

class ArduinoBLEUart: public Stream
{
    BLEService uartService;
    BLEStringCharacteristic rxCharacteristic;
    BLEByteCharacteristic txCharacteristic;
    
    std::queue<char> buffer;
    private:
        void handle_rx_event(BLEDevice &device, BLECharacteristic &characteristic);
        bool blocking = false;
        BLEDevice *device = NULL;
    public:
        ArduinoBLEUart(int buffer_size):
            uartService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E"),
            rxCharacteristic("6E400002-B5A3-F393-E0A9-E50E24DCCA9E", BLEWrite | BLENotify, buffer_size),
            txCharacteristic("6E400003-B5A3-F393-E0A9-E50E24DCCA9E", BLERead | BLENotify)
        {
            uartService.addCharacteristic(rxCharacteristic);
            uartService.addCharacteristic(txCharacteristic);
            Serial.println("ArduinoBLEUart");
            device = NULL;
        }
        ~ArduinoBLEUart();
        bool begin(bool blocking = false);
        void end();

        int available();
        int read();
        int peek();
        size_t write(uint8_t value);
        void setdevice(BLEDevice *central);

};

#endif
