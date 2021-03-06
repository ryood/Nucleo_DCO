// InterpolateFloatTest.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "interpolateFloat.h"

#define DIVISION	(8)

int main()
{
	printf("InterpolateFloat Test - %s %s\n", __DATE__, __TIME__);

	InterpolateFloat interpolate(3, DIVISION);

	interpolate.setNext(0);

	for (int i = 0; i < DIVISION +10; i++) {
		float v = interpolate.get();
		printf("%f\t%d\r\n", v, (int)v);
	}

	printf("\nend\n");

    return 0;
}

