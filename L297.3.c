#define F_CPU 8000000UL
#include <util/delay.h>

#include <avr/io.h>
#include <avr/interrupt.h>

// HOME        Open collector output that indicates when the L297 is in
//             its initial state (ABCD = 0101). The transistor is open when this
//             signal is active.

// PB0      CW/!CCW     Clockwise/counterclockwise direction control input.
//                      Physical direction of motor rotation also depends on connection
//                      of windings.

// PB2      CLOCK       Step clock. An active low pulse on this input advances the motor
//                      one increment. The step occurs on the rising edge of this signal.

// PB1      HALF/!FULL  Half/full step select input. When high selects half step operation,
//                      when low selects full step operation.
//                      One-phase-on full step mode is obtained by selecting FULL when the
//                      L297's translator is at an even-numbered state.
//                      Two-phase-on full step mode is set by selecting FULL when the
//                      translator is at an odd numbered position. (The home position is
//                      designate state 1).

// The step sequence is copied from the ST L297 datasheet.
// Note that odd steps (1, 3, 5, 7) produce a 2-phase on drive
// even steps (2, 4, 6, 8) produce a wave drive (one phase on) and
// all steps results in a half-stepping mode.

#define CW_ROT    1
#define CCW_ROT   0

#define HALF      1
#define FULL_STEP 0


//const uint8_t step_sequence[] =
//{
//   /*    0101     */    0x05,
//   /*    0001     */    0x01,
//   /*    1001     */    0x09,
//   /*    1000     */    0x08,
//   /*    1010     */    0x0A,
//   /*    0010     */    0x02,
//   /*    0110     */    0x06,
//   /*    0100     */    0x04
//};

const uint8_t step_sequence[] = { 0x1, 0x3, 0x2, 0x6, 0x4, 0xC, 0x8, 0x9 };

const uint8_t step_size[2][2] = {{ -2, -1 }, { +2, +1 }};

static uint8_t current_step = 0;

uint8_t clockwise( void )
{
   return PINB & 1;
}

uint8_t half_step( void )
{
   return ( PINB & 2 ) >> 1;
}

void output_step( uint8_t s )
{
   PORTA &= 0xF0;
   s &=0x0F;
   PORTA |= s;
}

uint8_t calc_next_step( int8_t step_size )
{
   current_step += step_size;
   current_step &= sizeof( step_sequence ) - 1;
   return step_sequence[current_step];
}

ISR( INT0_vect )
{
   int8_t x, y, s;

   x = clockwise();
   y = half_step();
   s = step_size[ x ][ y ];
   output_step( calc_next_step( s));
}

int main( void )
{

   CLKPR = _BV( CLKPCE );
   CLKPR = 0;

   DDRA  = 0x0F;
   PORTA = 0;

   PORTB = 0x07;		// Enable pullups

   GIMSK = _BV( INT0 );
   MCUCR |= _BV( ISC00 ) + _BV( ISC01 );

   sei();

   while(1)
      ;
}
