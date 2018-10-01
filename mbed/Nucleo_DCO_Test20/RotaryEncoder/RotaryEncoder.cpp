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

#include "RotaryEncoder.h"

/**
 * Create rotary encoder.
 *
 * @param pin1_name
 * @param pin2_name
 * @param min Minimum value.
 * @param max Maximum value.
 * @param val Default value.
 */
RotaryEncoder::RotaryEncoder(PinName pin1_name, PinName pin2_name, int min, int max, int val)
        : pin1(pin1_name), pin2(pin2_name), min(min), max(max), val(val) {
    pin1.mode(PullUp);
    pin2.mode(PullUp);
    ticker.attach_us(callback(this, &RotaryEncoder::func_ticker), 500);
}

/**
 * Dispose.
 */
RotaryEncoder::~RotaryEncoder() {
}

/**
 * Internal tick function.
 */
void RotaryEncoder::func_ticker() {
    //static uint8_t code;

    code = (code << 2) + (((pin1.read() << 1) | (pin2.read() << 0)) & 3);
    code &= 15;
    switch (code) {
        case 0x7:
            if (min < val) {
                val--;
            } else {
                val = min;
            }
            break;
        case 0xd:
            if (val < max) {
                val++;
            } else {
                val = max;
            }
            break;
    }
}
