#include "mbed.h"

#define TEST_TWORD_ONLY  (0)

#define LOOP_N  (100000)
float buffer[LOOP_N];

Serial pc(USBTX, USBRX);
Timer t;

void floatTest()
{
	int elapse = -1;
	
	// addf
    t.start();
    for (int i = 0; i < LOOP_N; i++) {
        buffer[i] = (float)i + LOOP_N;
    }
    t.stop();
    elapse = t.read_us();
    pc.printf("addf\t%d\t%f\r\n", elapse, (float)elapse / LOOP_N);
	
	// subf
    t.start();
    for (int i = 0; i < LOOP_N; i++) {
        buffer[i] = (float)i - LOOP_N;
    }
    t.stop();
    elapse = t.read_us();
    pc.printf("subf\t%d\t%f\r\n", elapse, (float)elapse / LOOP_N);

	// mulf
    t.start();
    for (int i = 0; i < LOOP_N; i++) {
        buffer[i] = (float)i * LOOP_N;
    }
    t.stop();
    elapse = t.read_us();
    pc.printf("mulf\t%d\t%f\r\n", elapse, (float)elapse / LOOP_N);
	
#if !(TEST_TWORD_ONLY)
    // divf
    t.start();
    for (int i = 0; i < LOOP_N; i++) {
        buffer[i] = (float)i / LOOP_N;
    }
    t.stop();
    elapse = t.read_us();
    pc.printf("divf\t%d\t%f\r\n", elapse, (float)elapse / LOOP_N);
    
    // sinf
    t.reset();
    t.start();
    for (int i = 0; i < LOOP_N; i++) {
        buffer[i] = sinf((float)i / LOOP_N);
    }
    t.stop();
    elapse = t.read_us();
    pc.printf("sinf\t%d\t%f\r\n", elapse, (float)elapse / LOOP_N);
 
    // cosf
    t.reset();
    t.start();
    for (int i = 0; i < LOOP_N; i++) {
        buffer[i] = cosf((float)i / LOOP_N);
    }
    t.stop();
    elapse = t.read_us();
    pc.printf("cosf\t%d\t%f\r\n", elapse, (float)elapse / LOOP_N);
 
    // expf
    t.reset();
    t.start();
    for (int i = 0; i < LOOP_N; i++) {
        buffer[i] = expf((float)i / LOOP_N);
    }
    t.stop();
    elapse = t.read_us();
    pc.printf("expf\t%d\t%f\r\n", elapse, (float)elapse / LOOP_N);
    
    // logf
    t.reset();
    t.start();
    for (int i = 0; i < LOOP_N; i++) {
        buffer[i] = logf((float)i / LOOP_N);
    }
    t.stop();
    elapse = t.read_us();
    pc.printf("logf\t%d\t%f\r\n", elapse, (float)elapse / LOOP_N);
    
    // sqrtf
    t.reset();
    t.start();
    for (int i = 0; i < LOOP_N; i++) {
        buffer[i] = sqrtf((float)i / LOOP_N);
    }
    t.stop();
    elapse = t.read_us();
    pc.printf("sqrtf\t%d\t%f\r\n", elapse, (float)elapse / LOOP_N);

#endif // !(TEST_TWORD_ONLY)
	
	// powf
    t.reset();
    t.start();
    for (int i = 0; i < LOOP_N; i++) {
        buffer[i] = powf((float)i / LOOP_N, 32);
    }
    t.stop();
    elapse = t.read_us();
    pc.printf("powf32\t%d\t%f\r\n", elapse, (float)elapse / LOOP_N);
	
	// calc tuning word
    t.reset();
    t.start();
    for (int i = 0; i < LOOP_N; i++) {
        buffer[i] = powf(2.0f, 32) * (float)i / 100000.0f;
    }
    t.stop();
    elapse = t.read_us();
    pc.printf("tword_f\t%d\t%f\r\n", elapse, (float)elapse / LOOP_N);
}
 
void doubleTest()
{
	int elapse = -1;
	
	// add
    t.start();
    for (int i = 0; i < LOOP_N; i++) {
        buffer[i] = (double)i + LOOP_N;
    }
    t.stop();
    elapse = t.read_us();
    pc.printf("add\t%d\t%f\r\n", elapse, (float)elapse / LOOP_N);
	
	// sub
    t.start();
    for (int i = 0; i < LOOP_N; i++) {
        buffer[i] = (double)i - LOOP_N;
    }
    t.stop();
    elapse = t.read_us();
    pc.printf("sub\t%d\t%f\r\n", elapse, (float)elapse / LOOP_N);

	// mul
    t.start();
    for (int i = 0; i < LOOP_N; i++) {
        buffer[i] = (double)i * LOOP_N;
    }
    t.stop();
    elapse = t.read_us();
    pc.printf("mul\t%d\t%f\r\n", elapse, (float)elapse / LOOP_N);
	
#if !(TEST_TWORD_ONLY)
    // div
    t.start();
    for (int i = 0; i < LOOP_N; i++) {
        buffer[i] = (double)i / LOOP_N;
    }
    t.stop();
    elapse = t.read_us();
    pc.printf("div\t%d\t%f\r\n", elapse, (float)elapse / LOOP_N);
    
    // sin
    t.reset();
    t.start();
    for (int i = 0; i < LOOP_N; i++) {
        buffer[i] = sin((double)i / LOOP_N);
    }
    t.stop();
    elapse = t.read_us();
    pc.printf("sin\t%d\t%f\r\n", elapse, (float)elapse / LOOP_N);
 
    // cos
    t.reset();
    t.start();
    for (int i = 0; i < LOOP_N; i++) {
        buffer[i] = cos((double)i / LOOP_N);
    }
    t.stop();
    elapse = t.read_us();
    pc.printf("cos\t%d\t%f\r\n", elapse, (float)elapse / LOOP_N);
 
    // exp
    t.reset();
    t.start();
    for (int i = 0; i < LOOP_N; i++) {
        buffer[i] = exp((double)i / LOOP_N);
    }
    t.stop();
    elapse = t.read_us();
    pc.printf("exp\t%d\t%f\r\n", elapse, (float)elapse / LOOP_N);
    
    // log
    t.reset();
    t.start();
    for (int i = 0; i < LOOP_N; i++) {
        buffer[i] = log((double)i / LOOP_N);
    }
    t.stop();
    elapse = t.read_us();
    pc.printf("log\t%d\t%f\r\n", elapse, (float)elapse / LOOP_N);
    
    // sqrt
    t.reset();
    t.start();
    for (int i = 0; i < LOOP_N; i++) {
        buffer[i] = sqrt((double)i / LOOP_N);
    }
    t.stop();
    elapse = t.read_us();
    pc.printf("sqrt\t%d\t%f\r\n", elapse, (float)elapse / LOOP_N);

#endif //!(TEST_TWORD_ONLY)

	// pow
    t.reset();
    t.start();
    for (int i = 0; i < LOOP_N; i++) {
        buffer[i] = pow((double)i / LOOP_N, 32);
    }
    t.stop();
    elapse = t.read_us();
    pc.printf("pow32\t%d\t%f\r\n", elapse, (float)elapse / LOOP_N);
	
	// calc tuning word
    t.reset();
    t.start();
    for (int i = 0; i < LOOP_N; i++) {
        buffer[i] = pow(2.0, 32) * (double)i / 100000.0;
    }
    t.stop();
    elapse = t.read_us();
    pc.printf("tword\t%d\t%f\r\n", elapse, (float)elapse / LOOP_N);
}
 
int main()
{
	pc.baud(115200);
    pc.printf("\r\nFloating Point Test\r\n");
	pc.printf("%s %s\r\n\n", __DATE__, __TIME__);
	
	pc.printf("LOOP_N: %d\r\n\n", LOOP_N);
	
    pc.printf("float\r\n");
    pc.printf("op\ttotal\t1-op(us)\r\n");
    pc.printf("-------------------------------\r\n");
	
    floatTest();
    
    pc.printf("\r\ndouble\r\n");
    pc.printf("op\ttotal\t1-op(us)\r\n");
    pc.printf("-------------------------------\r\n");
    
	doubleTest();
        
    pc.printf("\r\nEnd.\r\n");    
}