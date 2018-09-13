/*
 * InterpolateFloat.h
 * Interplate float
 *
 * 2018.09.13
 *
 */

#ifndef _INTERPOLATE_FLOAT_H_
#define _INTERPOLATE_FLOAT_H_
 
#include <stdint.h> 

class InterpolateFloat {
public:
	InterpolateFloat(float _initial, uint16_t _division) : data(_initial), prev(_initial), next(_initial), delta(0.0f) {
		setDivision(_division);
	}
	
	~InterpolateFloat() {}

	void setDivision(uint16_t _division) {
		if (division > 0) {
			division = _division;
		}
	}
	
	void setNext(float _next) {
		next = _next;
		prev = data;
		delta = (next - prev) / division;
	}
	
	float get() {
		data += delta;
		if (delta >= 0.0f && data >= next) {
			data = next;
		}
		if (delta < 0.0f && data <= next) {
			data = next;
		}
		return data;
	}
	
private:
	float data;
	float prev;
	float next;
	float delta;
	uint16_t division;
};
 
#endif //_INTERPOLATE_FLOAT_H_
