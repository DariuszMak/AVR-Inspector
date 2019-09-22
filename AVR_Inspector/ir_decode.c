#include "ir_decode.h"

volatile uint8_t address;
volatile uint8_t command;
volatile uint8_t toggle_bit;

volatile uint8_t Ir_key_press_flag;

volatile uint8_t rc5cnt;

void pilot_off(void)
{
    TCCR1B &= ~( ( 1 << CS12 ) | ( 1 << CS11 ) | ( 1 << CS10 ) ); //wyłączenie Timera1 (prescaler na zero)
    //Ir_key_press_flag = 0;
}

void pilot_on(void)
{
#if TIMER1_PRESCALER == 1
    TCCR1B |= ( 1 << CS10 );
#endif // TIMER1_PRESCALER

#if TIMER1_PRESCALER == 8
    TCCR1B |= ( 1 << CS11 );
#endif // TIMER1_PRESCALER

#if TIMER1_PRESCALER == 64
    TCCR1B |= ( 1 << CS11 ) | ( 1 << CS10 );
#endif // TIMER1_PRESCALER

#if TIMER1_PRESCALER == 256
    TCCR1B |= ( 1 << CS12 );
#endif // TIMER1_PRESCALER

#if TIMER1_PRESCALER == 1024
    TCCR1B |= ( 1 << CS12 ) | ( 1 << CS10 );
#endif // TIMER1_PRESCALER
}

void ir_init()
{
    IR_DIR &= ~IR_IN;
    IR_PORT |= IR_IN;

    TCCR1B &= ~( 1 << ICES1 ); //zbocze opadaj¹ce na ICP
    rc5cnt = 0; // zerowanie licznika wystêpuj¹cych zboczy

    TIMSK |= ( 1 << TICIE1 ); //przerwanie
    Ir_key_press_flag = 0;

    STOP_BUTTON_DIR  &= ~STOP_BUTTON_IN;//inicjowanie przycisku stopu jako wejście
    STOP_BUTTON_PORT |= STOP_BUTTON_IN;//podciągnięcie przycisku stopu do tranzystora
}

ISR( TIMER1_CAPT_vect )
{
    static uint16_t LastCapture;
    uint16_t PulseWidth;
    static uint8_t IrPulseCount;
    static uint16_t IrData;
    static uint8_t frame_status;

    PulseWidth = ICR1 - LastCapture;
    LastCapture = ICR1;

    TCCR1B ^= ( 1 << ICES1 );

    if( PulseWidth > MAX_BIT ) rc5cnt = 0;

    if( rc5cnt > 0 ) frame_status = FRAME_OK;

    if ( rc5cnt == 0 )
    {
        IrData = 0;
        IrPulseCount = 0;
        TCCR1B |= ( 1 << ICES1 );
        ++rc5cnt;
        frame_status = FRAME_END;
    }

    if ( frame_status == FRAME_OK )
    {
        if( PulseWidth < MIN_HALF_BIT ) frame_status = FRAME_RESTART;
        if( PulseWidth > MAX_BIT ) frame_status = FRAME_RESTART;
        if ( frame_status == FRAME_OK )
        {
            if ( PulseWidth > MAX_HALF_BIT ) rc5cnt++;
            if ( rc5cnt > 1 )
                if ( ( rc5cnt % 2 ) == 0 )
                {
                    IrData = IrData << 1;
                    if( ( TCCR1B & ( 1 << ICES1 ) ) ) IrData |= 0x0001;
                    IrPulseCount++;

                    if( IrPulseCount > 12 )
                    {
                        if ( Ir_key_press_flag == 0 )
                        {
                            command = IrData & 0b0000000000111111;
                            address = ( IrData & 0b0000011111000000 ) >> 6;
                            toggle_bit = ( IrData & 0b0000100000000000 ) >> 11;
                        }
                        frame_status = FRAME_RESTART;
                        Ir_key_press_flag = 1;
                    }
                }
            rc5cnt++;
        }
    }

    if ( frame_status == FRAME_RESTART )
    {
        rc5cnt = 0;
        TCCR1B &= ~( 1 << ICES1 );
    }
}

void pilot_reset(void)
{
    Ir_key_press_flag = 0;
    //uint8_t command_temp = command;
    //command = 0xff;
    address = 0xff;
}

uint8_t stop_button()//przycisk fizycznie umieszczony na płytce
{
    return ((~STOP_BUTTON_PIN | ~STOP_BUTTON_IN) & STOP_BUTTON_IN) >> STOP_BUTTON_PLACE;
}

