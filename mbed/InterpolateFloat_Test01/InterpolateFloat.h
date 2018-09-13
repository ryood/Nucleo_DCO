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
	InterpolateFloat(float _initial, uint16_t _division) 
			: data(_initial), prev(_initial), next(_initial), delta(0.0f), division(_division) 
	{
		if (division == 0) {
			division = 1;
		}
		//pc.printf("%f\t%f\t%f\t%f\t%d\r\n", data, prev, next, delta, division);
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
		//pc.printf("%f\t%f\t%f\t%f\t%d\r\n", data, prev, next, delta, division);

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
