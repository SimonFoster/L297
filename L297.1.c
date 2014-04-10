#define F_CPU 8000000UL
#include <util/delay.h>

#include <avr/io.h>

const unsigned char step_sequence[] = { 0x1, 0x3, 0x2, 0x6, 0x4, 0xC, 0x8, 0x9 };
unsigned char current_step = 0;

void setup()
{
   CLKPR = _BV( CLKPCE );
   CLKPR = 0;

   DDRA = 0x0F;
   PORTA = 0;
}

inline void output_step( uint8_t s )
{
   PORTA &= 0xF0;
   PORTA |= s;
}

inline uint8_t calc_next_step( uint8_t step_size )
{
   current_step += step_size;
   current_step &= sizeof( step_sequence ) - 1;
   return step_sequence[current_step];
}

void loop()
{
   output_step( calc_next_step( +1 ));
   _delay_ms( 100 );
}

int main( void )
{
   setup();
   while(1) loop();

   return 0;
}
