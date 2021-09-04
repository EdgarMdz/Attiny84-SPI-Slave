#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define SS_PIN PINA0
#define DD_SS_PIN DDA0	

#define DO_PIN PA5
#define DD_DO_PIN DDA5

#define DI_PIN PA6
#define DD_DI_PIN DDA6

#define CLK_PIN PA4
#define DD_CLK_PIN DDA4

//variables used for debugging
uint8_t data=0;
uint8_t received=0;
uint8_t canReceive=0;


void SPISetup();
void PinChangeInterruptSetup();

ISR(PCINT0_vect)
{
	//indication whether the MCU can receive data or not
	if((PINA & (1<<SS_PIN)) == 0)//not data is being received
	{
		canReceive=1;
	}
	else//data is being transfered
	{
		canReceive=0;
	}
}


ISR(USI_OVF_vect)
{
	if(canReceive==1)
	{
		data=USIDR;
		USISR |= 1<<USIOIF;  // Clear Overflow bit
		received=1;
	}
}

int main(void)
{	
	uint8_t dataBuffer = 0;	
	uint8_t send=0;


	SPISetup();	//set registers to use three wired mode
	data= 0x01;
	DDRB = 0x07;
	PinChangeInterruptSetup();//setting interruption
	
	PORTB=0;
	
    while (1) 
    {
		if(received == 1)
		{
			dataBuffer = data;
			received=0;
			
			USIDR=send++;
			if(send>1)
			{
				send=0;
			}
		}
		
		PORTB=dataBuffer;
    }
}

void SPISetup()
{
	//outputs
	DDRA|= (1<<DD_DO_PIN);
	
	//inputs
	DDRA &= ~(1<<DD_DI_PIN) | ~(1<<DD_SS_PIN) | ~(1<<DD_CLK_PIN); 
	
	//spi config
	USICR |= (1<<USIWM0) | (1<<USICS1) | (1<<USIOIE);
		
}

void PinChangeInterruptSetup()
{
	cli();
	GIMSK |= (1<<PCIE0);
	PCMSK0 |=(1<<PCINT0);
	GIFR |= (1<<PCIF0);
	
	sei();
}