#include "lockers.h"


/* Inicjuje port szeregowy AVRa */
void USART_init(unsigned int myubrr)
{
    /* Ustala prędkość transmisji */
    UBRRH = (unsigned char)(myubrr>>8);
    UBRRL = (unsigned char)myubrr;

    /* Włącza nadajnika */
    UCSRB = (1<<TXEN);

    /* Format ramki: 8 bitów danych, 1 bit stopu, brak bitu parzystości */
    UCSRC = (1<<URSEL)|(3<<UCSZ0);
}


/* Wysyła znak do portu szeregowego */
uint8_t USART_Transmit(char c, FILE *stream)
{
    while(!(UCSRA & (1<<UDRE)));
    UDR = c;

    return 0;
}



void lockers_init()
{

    LOCKER_1_BUTTON_DIR  &= ~LOCKER_1_BUTTON_IN;//inicjowanie przycisku jako wejście
    LOCKER_1_BUTTON_PORT |= LOCKER_1_BUTTON_IN;//podciągnięcie przycisku tranzystorami

    LOCKER_2_BUTTON_DIR  &= ~LOCKER_2_BUTTON_IN;//inicjowanie przycisku jako wejście
    LOCKER_2_BUTTON_PORT |= LOCKER_2_BUTTON_IN;//podciągnięcie przycisku tranzystorami

    /* Tworzy strumienia danych o nazwie 'mystdout' połączony
    z funkcją 'USART_Transmit' */
    static FILE mystdout = FDEV_SETUP_STREAM(USART_Transmit, NULL, _FDEV_SETUP_WRITE);

    /* Inicjalizuje  port szeregowy AVRa */
    USART_init(MYUBRR);

    /* Przekierowuje standardowe wyjście do  'mystdout' */
    stdout = &mystdout;

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
    if(action) lockers_save_events();
}


uint8_t lockers_number_of_frames(void)
{
    return lockers_number_of_frames_exteral_EEPROM() + lockers_number_of_frames_internal_EEPROM();
}

uint8_t lockers_number_of_frames_exteral_EEPROM(void)
{
    return ((EXTERNAL_EEPROM_MAX_INDEX + 1) / SIZE_OF_FRAME);
}

uint16_t lockers_number_of_frames_internal_EEPROM(void)
{
    return ((INTERNAL_EEPROM_MAX_INDEX + 1) / SIZE_OF_FRAME);
}

uint8_t lockers_convert_address_to_index_of_frame(uint16_t add)
{
    return (add / SIZE_OF_FRAME);
}

void lockers_read_frame(uint8_t index)
{
    uint16_t temp_address = SIZE_OF_FRAME * index;
    if(lockers_convert_address_to_index_of_frame(temp_address) < lockers_number_of_frames_exteral_EEPROM())
    {
        frame.seconds = EEPROM_read(temp_address++);

        frame.minutes = EEPROM_read(temp_address++);

        frame.hours = EEPROM_read(temp_address++);

        frame.day = EEPROM_read(temp_address++);

        frame.month = EEPROM_read(temp_address++);

        frame.year = EEPROM_read_word(temp_address++);
        temp_address++;

        frame.information = EEPROM_read(temp_address++);
    }
    else
    {
        temp_address -= SIZE_OF_FRAME * lockers_number_of_frames_exteral_EEPROM();
        eeprom_busy_wait();

        frame.seconds = eeprom_read_byte((uint8_t*)temp_address++);
        //_EEGET(frame.seconds,temp_address);

        frame.minutes = eeprom_read_byte((uint8_t*)temp_address++);

        frame.hours = eeprom_read_byte((uint8_t*)temp_address++);

        frame.day = eeprom_read_byte((uint8_t*)temp_address++);

        frame.month = eeprom_read_byte((uint8_t*)temp_address++);

//        uint8_t temp;
        //_EEGET(frame.year,temp_address);
        frame.year = eeprom_read_word((uint16_t*)temp_address++);
        temp_address++;

        frame.information = eeprom_read_byte((uint8_t*)temp_address++);
    }
}

void lockers_print_entire_frame(void)
{
    printf("%d:%d:%d %d:%d:%d %d\n\r", frame.hours, frame.minutes, frame.seconds, frame.day, frame.month, frame.year, frame.information );
}

void lockers_print_all_memory(void)
{
    //uint16_t temp = 0;
    uint8_t index_of_frame = 0;
    for(; index_of_frame < lockers_number_of_frames(); ++ index_of_frame)
    {
        lockers_read_frame(index_of_frame);
        lockers_print_entire_frame();
    }
}

void lockers_save_events(void)//funkcja zapisująca do pamięci EEPROM dane
{
    //delay_ms_var(400);

    uint8_t i = 0;
    for( ; i < AMOUNT_OF_LOCKERS; ++i)
    {
        if(save_info_table[i])
        {
            buzzer();
            delay_ms_var(50);
            PCF8583_get_wall_time();

            uint16_t temp_address = PCF8583_read_word(PCF8583_CELL);//pobranie ostatniego adresu
            uint8_t overflow_flag = 0;//jeśli == 1 - bramka zmieści się na "styk"

            // eeprom_read_word( (uint16_t*)21);

            if(lockers_convert_address_to_index_of_frame(temp_address) < lockers_number_of_frames_exteral_EEPROM())
            {
                if((EXTERNAL_EEPROM_MAX_INDEX - (int16_t)temp_address) < (SIZE_OF_FRAME - 1))//jeśli wiadomo, że się nie zmieści przy znanym adresie
                {
                    temp_address = SIZE_OF_FRAME * lockers_number_of_frames_exteral_EEPROM();//jeśli następna bramka się nie zmieści, trzeba ją przesunąć

                    overflow_flag = 1;
                }

            }
            else overflow_flag = 1;

            if(overflow_flag == 0)
            {
                EEPROM_write(temp_address++,sek);

                EEPROM_write(temp_address++,min);

                EEPROM_write(temp_address++,godz);

                EEPROM_write(temp_address++,dzien);

                EEPROM_write(temp_address++,miesiac);

                EEPROM_write_word(temp_address++, rok);

                temp_address++;

                uint8_t information = (uint8_t)save_info_table[i] * 100;
                information += i + 1;
                EEPROM_write(temp_address++,information);
            }
            else if(overflow_flag == 1)
            {
                temp_address -= SIZE_OF_FRAME * lockers_number_of_frames_exteral_EEPROM();

                eeprom_busy_wait();
                //_EEPUT(temp_address, sek);
                eeprom_write_byte((uint8_t*)temp_address++,sek);
                eeprom_busy_wait();
                //_EEPUT(temp_address, min);
                eeprom_write_byte((uint8_t*)temp_address++,min);
                //_EEPUT(temp_address, godz);
                eeprom_busy_wait();
                eeprom_write_byte((uint8_t*)temp_address++,godz);
                eeprom_busy_wait();
                //_EEPUT(temp_address, dzien);
                eeprom_write_byte((uint8_t*)temp_address++,dzien);
                eeprom_busy_wait();
                //_EEPUT(temp_address, miesiac);
                eeprom_write_byte((uint8_t*)temp_address++,miesiac);
                eeprom_busy_wait();
                //_EEPUT(temp_address, rok);
                eeprom_write_word((uint16_t*)temp_address++, rok);
                temp_address++;

                uint8_t information = (uint8_t)save_info_table[i] * 100;
                information += i + 1;
                //_EEPUT(temp_address, information);
                eeprom_busy_wait();
                eeprom_write_byte((uint8_t*)temp_address++,information);
            }

            /*if(overflow_flag == 1)
            {
                buzzer_time(200);
                //overflow_flag = 2;//przepełnienie pamięci
            }*/


            if((overflow_flag == 0) && ((EXTERNAL_EEPROM_MAX_INDEX - (int16_t)temp_address) < (SIZE_OF_FRAME - 1)))//jeśli wiadomo, że zmieści się na styk
            {
                buzzer_time(200);
                temp_address = 0;
                overflow_flag = 1;
            }

            if((overflow_flag == 1) && ((INTERNAL_EEPROM_MAX_INDEX - (int16_t)temp_address) < (SIZE_OF_FRAME - 1)))
            {
                buzzer_time(1000);
                temp_address = 0;
                overflow_flag = 0;
            }

            if(overflow_flag == 0)
            {
                PCF8583_write_word(PCF8583_CELL, temp_address);
            }
            else PCF8583_write_word(PCF8583_CELL, temp_address + SIZE_OF_FRAME * lockers_number_of_frames_exteral_EEPROM());
        }
    }


}

uint8_t locker_1_button(void)//przycisk fizycznie umieszczony na płytce
{
    int temp = 0;
    if(!( LOCKER_1_BUTTON_PIN & LOCKER_1_BUTTON_IN ))
    {
        temp = 1;
    }
    return temp;
}

uint8_t locker_2_button(void)//przycisk fizycznie umieszczony na płytce
{
    int temp = 0;
    if(!( LOCKER_2_BUTTON_PIN & LOCKER_2_BUTTON_IN ))
    {
        temp = 1;
    }
    return temp;
}


void lockers_clear_all_memory(void)
{
    int16_t i = 0;
    for(; i <= EXTERNAL_EEPROM_MAX_INDEX; ++i)
    {
        EEPROM_write((uint8_t) i, 0);
    }

    i = 0;

    for(; i <= INTERNAL_EEPROM_MAX_INDEX; ++i)
    {
        eeprom_busy_wait();
        eeprom_write_byte((uint8_t*)i, 0);
    }

    PCF8583_write_word(PCF8583_CELL, 0);
}
