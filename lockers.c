#include "lockers.h"

#if SAFETY_BIT == 1

void lockers_safety_bit_on(void)
{
    PCF8583_write(PCF8583_SAFETY_CELL,1);
}

void lockers_safety_bit_off(void)
{
    PCF8583_write(PCF8583_SAFETY_CELL,0);
}

uint8_t lockers_is_safety_bit(void)
{
    //return 0;//usunąć, gdy będzie PCF8563
    if (PCF8583_read(PCF8583_SAFETY_CELL) == 0) return 0;
    else return 1;
}

#endif

/* Inicjuje port szeregowy AVRa */
void USART_init(uint16_t myubrr)
{
    /* Ustala prędkość transmisji */
    UBRRH = (uint8_t)(myubrr>>8);
    UBRRL = (uint8_t)myubrr;

    /* Włącza nadajnika */
    UCSRB = (1<<RXEN) | (1<<TXEN);

    /* Format ramki: 8 bitów danych, 1 bit stopu, brak bitu parzystości */
    UCSRC = (1<<URSEL)|(3<<UCSZ0);
}

/* Wysyła znak do portu szeregowego */
void USART_Transmit(uint8_t c, FILE *stream)
{
    if (c == '\n')
    {
        USART_Transmit('\r', stream);
    }
    while(!(UCSRA & (1<<UDRE)));
    UDR = c;

}

/* Odbiera znak z portu szeregowego */
uint8_t USART_Recieve(FILE *stream)
{
    while(!(UCSRA & (1<<RXC)));
    return UDR;
}

uint8_t USART_Recieve_without_waiting(void)
{
    if(UCSRA & (1<<RXC))
    {
        return UDR;
    }
    else return 0;
}

void lockers_init()
{
    int16_t temp = INTERNAL_EEPROM_MAX_INDEX + 1;
    if(PCF8583_read_word(PCF8583_HEAD) > temp || PCF8583_read_word(PCF8583_TAIL) > temp || PCF8583_read_word(PCF8583_HEAD) < INTERNAL_EEPROM_MIN_INDEX || PCF8583_read_word(PCF8583_TAIL) < INTERNAL_EEPROM_MIN_INDEX )
    {
        PCF8583_write_word(PCF8583_TAIL, INTERNAL_EEPROM_MIN_INDEX);
        lockers_queue_empty();
        buzzer_time(1000);
    }

    //lockers_queue_head = 0;

    //lockers_queue_tail = 0;

    LOCKER_1_BUTTON_DIR  &= ~LOCKER_1_BUTTON_IN;//inicjowanie przycisku jako wejście
    LOCKER_1_BUTTON_PORT |= LOCKER_1_BUTTON_IN;//podciągnięcie przycisku tranzystorami

    LOCKER_2_BUTTON_DIR  &= ~LOCKER_2_BUTTON_IN;//inicjowanie przycisku jako wejście
    LOCKER_2_BUTTON_PORT |= LOCKER_2_BUTTON_IN;//podciągnięcie przycisku tranzystorami

    LOCKER_3_BUTTON_DIR  &= ~LOCKER_3_BUTTON_IN;//inicjowanie przycisku jako wejście
    LOCKER_3_BUTTON_PORT |= LOCKER_3_BUTTON_IN;//podciągnięcie przycisku tranzystorami

    LOCKER_4_BUTTON_DIR  &= ~LOCKER_4_BUTTON_IN;//inicjowanie przycisku jako wejście
    LOCKER_4_BUTTON_PORT |= LOCKER_4_BUTTON_IN;//podciągnięcie przycisku tranzystorami

    LOCKER_5_BUTTON_DIR  &= ~LOCKER_5_BUTTON_IN;//inicjowanie przycisku jako wejście
    LOCKER_5_BUTTON_PORT |= LOCKER_5_BUTTON_IN;//podciągnięcie przycisku tranzystorami

    LOCKER_6_BUTTON_DIR  &= ~LOCKER_6_BUTTON_IN;//inicjowanie przycisku jako wejście
    LOCKER_6_BUTTON_PORT |= LOCKER_6_BUTTON_IN;//podciągnięcie przycisku tranzystorami

    LOCKER_7_BUTTON_DIR  &= ~LOCKER_7_BUTTON_IN;//inicjowanie przycisku jako wejście
    LOCKER_7_BUTTON_PORT |= LOCKER_7_BUTTON_IN;//podciągnięcie przycisku tranzystorami

    LOCKER_8_BUTTON_DIR  &= ~LOCKER_8_BUTTON_IN;//inicjowanie przycisku jako wejście
    LOCKER_8_BUTTON_PORT |= LOCKER_8_BUTTON_IN;//podciągnięcie przycisku tranzystorami

    LOCKER_9_BUTTON_DIR  &= ~LOCKER_9_BUTTON_IN;//inicjowanie przycisku jako wejście
    LOCKER_9_BUTTON_PORT |= LOCKER_9_BUTTON_IN;//podciągnięcie przycisku tranzystorami

    LOCKER_10_BUTTON_DIR  &= ~LOCKER_10_BUTTON_IN;//inicjowanie przycisku jako wejście
    LOCKER_10_BUTTON_PORT |= LOCKER_10_BUTTON_IN;//podciągnięcie przycisku tranzystorami

    /* Tworzy strumienia danych o nazwie 'mystdout' połączony
    z funkcją 'USART_Transmit' */
    static FILE mystdout = FDEV_SETUP_STREAM(USART_Transmit, NULL, _FDEV_SETUP_WRITE);

    static FILE mystdin = FDEV_SETUP_STREAM(NULL, USART_Recieve, _FDEV_SETUP_READ);

    /* Inicjalizuje  port szeregowy AVRa */
    USART_init(MYUBRR);

    /* Przekierowuje standardowe wyjście do  'mystdout' */
    stdout = &mystdout;

    /* Przekierowuje standardowe wejście do  'mystdin' */

    stdin = &mystdin;


    //lockers_find_latest_data();
}

void lockers_beginning_actions(void)
{
    int i = 0;
    for(; i < AMOUNT_OF_LOCKERS; ++i)
    {
        states_table[i] = (uint8_t) lockers_state_of_single_button(i);//przypisanie wartości początkowych
    }
}


uint8_t lockers_state_of_single_button( uint8_t index )//zwraca stan danego przycisku względem numeru indeksu
{
    if(index == 0) return locker_1_button();
    else if(index == 1) return locker_2_button();
    else if(index == 2) return locker_3_button();
    else if(index == 3) return locker_4_button();
    else if(index == 4) return locker_5_button();
    else if(index == 5) return locker_6_button();
    else if(index == 6) return locker_7_button();
    else if(index == 7) return locker_8_button();
    else if(index == 8) return locker_9_button();
    else if(index == 9) return locker_10_button();
    return -1;//bląd
}

void lockers_check_events()
{
    int i = 0;//zmienna pmocnicza w pętlach
    int action = 0;//jeśli ta zmienna będzie inna od zera, to wykona się zapis
    uint8_t state = 0;//stan przycisku z danej chwili
    for(; i < AMOUNT_OF_LOCKERS; ++i)//sprawdzanie stanów przycisków i odpowiednie wypełnianie tablicy
    {
        state = (uint8_t)lockers_state_of_single_button(i);//jednorazowe złapanie stanu przycisku
        if( state != states_table[i] )//jeśli stan przycisku różni się od poprzednich wartości, należy wypełnić tabelę
        {
            action = 1;//akcja będzie podjęta
            if (state == 1) save_info_table[i] = 2;//szafka zamknięta
            else if(state == 0) save_info_table[i] = 1;//szafka otwarta
        }
        else save_info_table[i] = 0; //nie zapisuj żadnej informacji dla tej szufladki
        states_table[i] = state;
    }
    if(action)
    {
        lockers_queue_enque();
        refresh_screen = 1;
    }
}


uint8_t lockers_number_of_frames(void)
{
    return ((INTERNAL_EEPROM_MAX_INDEX + 1 - INTERNAL_EEPROM_MIN_INDEX) / SIZE_OF_FRAME);
}

uint8_t lockers_convert_address_to_index_of_frame(uint16_t add)
{
    return ((add - INTERNAL_EEPROM_MIN_INDEX) / SIZE_OF_FRAME);
}

void lockers_queue_read(uint8_t index)
{
    uint16_t temp_index = index + lockers_head();
    if(temp_index > lockers_number_of_frames() - 1)
    {
        temp_index -= lockers_number_of_frames();
    }
    lockers_read_frame(temp_index);
}

void lockers_read_frame(uint8_t index)
{
    uint16_t temp_address = SIZE_OF_FRAME * index + INTERNAL_EEPROM_MIN_INDEX;


    eeprom_busy_wait();

    eeprom_read_block( &frame, (const void *)temp_address, SIZE_OF_FRAME);

    temp_address += SIZE_OF_FRAME;

}

void lockers_print_entire_frame(void)
{
    buzzer_time(0.5);
    printf("%02d:%02d:%02d %02d:%02d:%d", frame.hours, frame.minutes, frame.seconds, frame.day, frame.month, frame.year);

    uint8_t number = frame.information % 100;

    if(number != 0)
    {
        printf(" SZAFKA NR: %d ",number);
        uint8_t t = frame.information / 100;
        if(t == 1) printf("OTWARCIE");
        else if(t == 2) printf("ZAMKNIECIE");
    }
    printf("\n");
}

void lockers_print_amount_of_first_frames(uint8_t numbers_of_frames)
{
    uint8_t index_of_frame = 0;
    if(numbers_of_frames == 0)
    {
        printf("Brak danych\n");
    }
    else
    {
        refresh_screen = 1;
        for(; index_of_frame < numbers_of_frames; ++ index_of_frame)
        {
            lockers_queue_read(index_of_frame);
            printf("%d. ", index_of_frame + 1);
            lockers_print_entire_frame();
        }
    }
}

void lockers_print_date_of_report()
{
    PCF8583_get_wall_time();
    printf("%02d:%02d:%02d %02d:%02d:%d\n", godz, min, sek, dzien, miesiac, rok);
}

void lockers_print_all_memory(void)
{
    //uint16_t temp = 0;
    printf("Raport awaryjny. ");
    lockers_print_date_of_report();
    lockers_print_amount_of_first_frames(lockers_number_of_frames());
}

void lockers_print_latest_data(void)
{
    printf("Raport. ");
    lockers_print_date_of_report();
    lockers_print_amount_of_first_frames(lockers_queue_number_of_records());
    lockers_queue_empty();
}

uint8_t lockers_tail(void)
{
    return lockers_convert_address_to_index_of_frame(PCF8583_read_word(PCF8583_TAIL));
}

uint8_t lockers_head(void)
{
    return lockers_convert_address_to_index_of_frame(PCF8583_read_word(PCF8583_HEAD));
}

void lockers_queue_empty(void)
{
    PCF8583_write_word(PCF8583_HEAD, PCF8583_read_word(PCF8583_TAIL));
}

uint8_t lockers_queue_number_of_records(void)
{
    if( lockers_head() > lockers_tail() ) return lockers_number_of_frames() - ( lockers_head() - lockers_tail() );
    else return lockers_tail() - lockers_head();
}

void lockers_save_frame(uint8_t index, uint8_t i)
{
    frame.seconds = sek;
    frame.minutes = min;
    frame.hours = godz;
    frame.day = dzien;
    frame.month = miesiac;
    frame.year = rok;
    frame.information = (uint8_t)save_info_table[i] * 100;
    frame.information += i + 1;
    uint16_t temp_address = SIZE_OF_FRAME * index + INTERNAL_EEPROM_MIN_INDEX;//pobranie ostatniego adresu


    /*if((INTERNAL_EEPROM_MAX_INDEX - (int16_t)temp_address) < (SIZE_OF_FRAME - 1))//jeśli wiadomo, że się nie zmieści przy znanym adresie
    {
        buzzer_time(500);
        temp_address = INTERNAL_EEPROM_MIN_INDEX;//jeśli następna bramka się nie zmieści, trzeba ją przesunąć
    }*/

    eeprom_busy_wait();

    eeprom_update_block( &frame, (void*)temp_address, SIZE_OF_FRAME);

    temp_address += SIZE_OF_FRAME;

    if((INTERNAL_EEPROM_MAX_INDEX - (int16_t)temp_address) < (SIZE_OF_FRAME - 1))//jeśli wiadomo, że się nie zmieści przy znanym adresie
    {
        buzzer_time(500);
        temp_address = INTERNAL_EEPROM_MIN_INDEX;//jeśli następna bramka się nie zmieści, trzeba ją przesunąć
    }

    PCF8583_write_word(PCF8583_TAIL, temp_address);
}

void lockers_queue_enque(void)//funkcja zapisująca do pamięci EEPROM dane
{
    //delay_ms_var(400);

    PCF8583_get_wall_time();
    uint8_t i = 0;
    for( ; i < AMOUNT_OF_LOCKERS; ++i)
    {
        if(save_info_table[i])
        {
            //printf("%d %d %d \n",lockers_tail(), lockers_head(), lockers_number_of_frames());
            uint8_t end_of_mem = 0;
            buzzer();
            delay_ms_var(5);
            if(lockers_tail() == lockers_number_of_frames() - 1)
            {
                if(lockers_head() == 0)
                {
                    end_of_mem = 1;
                }
            }
            else
            {
                if(lockers_head() == lockers_tail() + 1)
                {
                    end_of_mem = 1;
                }
            }

            if( end_of_mem == 1)
            {
                if(start_program == 3)
                {
                    buzzer_time(3000);
#if SAFETY_BIT == 1
                    lockers_safety_bit_on();
#endif
                }
                else
                {
                    lockers_print_latest_data();
                    lockers_save_frame(lockers_tail(), i);
                }
            }
            else lockers_save_frame(lockers_tail(), i);
        }
    }
    backlight(2);
}

uint8_t locker_1_button(void)//przycisk fizycznie umieszczony na płytce
{
    state_temp = 0;
    if(!( LOCKER_1_BUTTON_PIN & LOCKER_1_BUTTON_IN ))
    {
        state_temp = 1;
    }
    return state_temp;
}

uint8_t locker_2_button(void)//przycisk fizycznie umieszczony na płytce
{
    state_temp = 0;
    if(!( LOCKER_2_BUTTON_PIN & LOCKER_2_BUTTON_IN ))
    {
        state_temp = 1;
    }
    return state_temp;
}

uint8_t locker_3_button(void)//przycisk fizycznie umieszczony na płytce
{
    state_temp = 0;
    if(!( LOCKER_3_BUTTON_PIN & LOCKER_3_BUTTON_IN ))
    {
        state_temp = 1;
    }
    return state_temp;
}

uint8_t locker_4_button(void)//przycisk fizycznie umieszczony na płytce
{
    state_temp = 0;
    if(!( LOCKER_4_BUTTON_PIN & LOCKER_4_BUTTON_IN ))
    {
        state_temp = 1;
    }
    return state_temp;
}

uint8_t locker_5_button(void)//przycisk fizycznie umieszczony na płytce
{
    state_temp = 0;
    if(!( LOCKER_5_BUTTON_PIN & LOCKER_5_BUTTON_IN ))
    {
        state_temp = 1;
    }
    return state_temp;
}

uint8_t locker_6_button(void)//przycisk fizycznie umieszczony na płytce
{
    state_temp = 0;
    if(!( LOCKER_6_BUTTON_PIN & LOCKER_6_BUTTON_IN ))
    {
        state_temp = 1;
    }
    return state_temp;
}

uint8_t locker_7_button(void)//przycisk fizycznie umieszczony na płytce
{
    state_temp = 0;
    if(!( LOCKER_7_BUTTON_PIN & LOCKER_7_BUTTON_IN ))
    {
        state_temp = 1;
    }
    return state_temp;
}

uint8_t locker_8_button(void)//przycisk fizycznie umieszczony na płytce
{
    state_temp = 0;
    if(!( LOCKER_8_BUTTON_PIN & LOCKER_8_BUTTON_IN ))
    {
        state_temp = 1;
    }
    return state_temp;
}

uint8_t locker_9_button(void)//przycisk fizycznie umieszczony na płytce
{
    state_temp = 0;
    if(!( LOCKER_9_BUTTON_PIN & LOCKER_9_BUTTON_IN ))
    {
        state_temp = 1;
    }
    return state_temp;
}

uint8_t locker_10_button(void)//przycisk fizycznie umieszczony na płytce
{
    state_temp = 0;
    if(!( LOCKER_10_BUTTON_PIN & LOCKER_10_BUTTON_IN ))
    {
        state_temp = 1;
    }
    return state_temp;
}

void lockers_clear_all_memory(void)
{
    int16_t i = INTERNAL_EEPROM_MIN_INDEX;

    for(; i <= INTERNAL_EEPROM_MAX_INDEX; ++i)
    {
        eeprom_busy_wait();
        eeprom_update_byte((uint8_t*)i, 0);
    }

    lockers_queue_empty();
}
