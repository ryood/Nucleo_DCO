#ifndef _AVERAGE_ANALOG_H_
#define _AVERAGE_ANALOG_H_
 
#include "mbed.h"
 
class AverageAnalogIn {
public:
    AverageAnalogIn(PinName _pin, int _bufferSize=8) : m_AnalogIn(_pin), bufferSize(_bufferSize), index(0) {
        buffer = new unsigned short[bufferSize];
    }
    
    ~AverageAnalogIn() {
        delete[] buffer;
    }
    
    unsigned short read_u16() {
        buffer[index] = m_AnalogIn.read_u16();
        index++;
        if (index == bufferSize) {
            index = 0;
        }
        unsigned int sum = 0;
        for (int i = 0; i < bufferSize; i++) {
            sum += buffer[i];
        }
        return sum / bufferSize;
    }
    
    float read() {
        unsigned short value = read_u16();
        return (float)value / 0xFFFF;
    }
 
private:
    AnalogIn m_AnalogIn;
    int bufferSize;
    int index;
    unsigned short *buffer;
};
 
#endif //_AVERAGE_ANALOG_H_
