#include "timer_2.h"

void timer_0_init( void )
{
// set up timer with prescaler = 256
    // initialize counter
    TCNT0 = 0;

    // enable overflow interrupt
    TIMSK |= (1 << TOIE0);
}

void checking_pins_interrupt_on()
{
    TCCR0 |= (1 << CS02) | (1 << CS00);
    TCCR0 &= ~(1 << CS01) /*| (1 < CS00)*/;
}

void checking_pins_interrupt_off()
{
    TCCR0 &= ~(( 1 << CS00 ) | ( 1 << CS01 ) | ( 1 << CS02 )); // wyłączenie timera preskaler 1024, timer do odświeżania
}

// TIMER0 overflow interrupt service routine
// called whenever TCNT0 overflows
ISR(TIMER0_OVF_vect)
{
    //buzzer_time(0.5);
    int i = 0;//zmienna pmocnicza w pętlach
    //int action = 0;//jeśli ta zmienna będzie inna od zera, to wykona się zapis
    uint8_t state;//stan przycisku z danej chwili
    for(; i < AMOUNT_OF_LOCKERS; ++i)//sprawdzanie stanów przycisków i odpowiednie wypełnianie tablicy
    {
        state = lockers_state_of_single_button(i);//jednorazowe złapanie stanu przycisku
        if( state != states_table[i] && save_info_table[i] == 0)//jeśli stan przycisku różni się od poprzednich wartości i wartość jeszcze nie jest zapisana, należy wypełnić tabelę
        {
            if (state == 1) save_info_table[i] = 2;//szafka otwarta
            else if(state == 0) save_info_table[i] = 1;//szafka zamknięta
            states_table[i] = state;
        }
        //else save_info_table[i] = 0; //nie zapisuj żadnej informacji dla tej szufladki
    }

    if (pilot_button_pressed == 0 && stop_button())
    {
        pilot_button_pressed = 1;//zmienna pamiętająca naciśnięcie przycisku
    }
}
