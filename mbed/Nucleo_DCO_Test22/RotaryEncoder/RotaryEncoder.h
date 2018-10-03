/**
 * =============================================================================
 * Rotary Encoder class (Version 0.0.1)
 * =============================================================================
 * Copyright (c) 2010 Shinichiro Nakamura (CuBeatSystems)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * =============================================================================
 */

#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include "mbed.h"

/**
 */
class RotaryEncoder
{
public:
    /**
     * Create rotary encoder.
     *
     * @param pin1_name
     * @param pin2_name
     * @param min Minimum value.
     * @param max Maximum value.
     * @param val Default value.
     */
    RotaryEncoder(PinName pin1_name, PinName pin2_name, int min = 0, int max = 100, int val = 50);

    /**
     * Dispose.
     */
    ~RotaryEncoder();

    /**
     * Get the minimum value.
     *
     * @return The minimum value.
     */
    int getMin() const {
        return min;
    }

    /**
     * Get the maximum value.
     *
     * @return The maximum value.
     */
    int getMax() const {
        return max;
    }

    /**
     * Get the value.
     *
     * @return The value.
     */
    int getVal() const {
        return val;
    }

    /**
     * Set the value.
     *
     * @param The value.
     */
    void setVal(int _val) {
        if (min <= _val && _val <= max) {
            val = _val;
        }
    }
    
    /**
    * Set the ticker interval.
    *
    * @param The interval in microseconds.
    */
    void setInterval(timestamp_t t) {
        ticker.attach_us(callback(this, &RotaryEncoder::func_ticker), t);
    }

private:
    DigitalIn pin1;
    DigitalIn pin2;
    const int min;
    const int max;
    int val;
    Ticker ticker;

    uint8_t code;

    /**
     * Internal tick function.
     */
    void func_ticker();
};

#endif
