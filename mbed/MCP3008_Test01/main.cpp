#include "mbed.h"
#include "mcp3008.h"

#define SPI_CLOCK   (2000000)

//SPI (PinName mosi, PinName miso, PinName sclk, PinName ssel=NC)
SPI spiM(D11, D12, D13);
MCP3008 mcp3008_0(&spiM, D10);
MCP3008 mcp3008_1(&spiM, D9);

DigitalOut CheckPin1	(D4);
DigitalOut CheckPin2	(D5);

int main()
{
    printf("\r\n MCP3008 Test01\r\n");
    
    spiM.frequency(SPI_CLOCK);
    
    uint16_t v0[8];
    uint16_t v1[8];

    for (;;) {
		CheckPin1.write(1);
        for (int i = 0; i < 8; i++) {
			CheckPin2.write(1);
            v0[i] = mcp3008_0.read_input_u16(i);
			CheckPin2.write(0);
			CheckPin2.write(1);
            v1[i] = mcp3008_1.read_input_u16(i);
			CheckPin2.write(0);
        }
		CheckPin1.write(0);
        
        for (int i = 0; i < 8; i++) {
            printf("%4d\t", v0[i]);
        }
        
        printf(": ");
        for (int i = 0; i < 8; i++) {
            printf("%4d\t", v1[i]);
        }
        
		printf("\r\n");
        
        //wait(0.2);
    }
}
