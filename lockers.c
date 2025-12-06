#include "lockers.h"

struct frame
{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;

    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint8_t information;
} frame; //8

void lockers_init()
{

    LOCKER_1_BUTTON_DIR  &= ~LOCKER_1_BUTTON_IN;//inicjowanie przycisku jako wejście
    LOCKER_1_BUTTON_PORT |= LOCKER_1_BUTTON_IN;//podciągnięcie przycisku tranzystorami

    LOCKER_2_BUTTON_DIR  &= ~LOCKER_2_BUTTON_IN;//inicjowanie przycisku jako wejście
    LOCKER_2_BUTTON_PORT |= LOCKER_2_BUTTON_IN;//podciągnięcie przycisku tranzystorami

    int i = 0;
    for(; i < AMOUNT_OF_LOCKERS; ++i)
    {
        states_table[i] = (uint8_t) lockers_state_of_single_button(i);//przypisanie wartości początkowych
    }
    lockers_find_latest_data();
}

int lockers_state_of_single_button( int index )//zwraca stan danego przycisku względem numeru indeksu
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
            if (state) save_info_table[i] = 2;//szafka zamknięta
            else save_info_table[i] = 1;//szafka otwarta
        }
        else save_info_table[i] = 0; //nie zapisuj żadnej informacji dla tej szufladki
        states_table[i] = state;
    }
    if(action) lockers_save_events();
}

void buzzer()//funkcja odpowiedzialna za sygnał dźwiękowy (trwa jedną milisekundę)
{
    PORTD |= ( 1 << PD7 );
    _delay_ms( 1 );
    PORTD &= ~( 1 << PD7 );
}

void lockers_find_latest_data(void)
{
    PCF8583_write(PCF8583_SAVED_ADDRESS_CELL, 0);
}

void lockers_save_events(void)//funkcja zapisująca do pamięci EEPROM dane
{
    delay_ms_var(1000);
    uint8_t temp_address = PCF8583_read(PCF8583_SAVED_ADDRESS_CELL);//pobranie adresu z zegara RTC

    if((EEPROM_ADDRESS - temp_address) < SIZE_OF_FRAME) temp_address = 0;//jeśli następna bramka się nie zmieści, trzeba ją przesunąć

    int i = 0;
    for( ; i < AMOUNT_OF_LOCKERS; ++i)
    {

        if(save_info_table[i])
        {
              buzzer();
                    delay_ms_var(50);
            PCF8583_get_wall_time();
            EEPROM_write(temp_address,sek);
            ++temp_address;
            EEPROM_write(temp_address,min);
            ++temp_address;
            EEPROM_write(temp_address,godz);
            ++temp_address;
            EEPROM_write(temp_address,dzien);
            ++temp_address;
            EEPROM_write(temp_address,miesiac);
            ++temp_address;
            EEPROM_write_word(temp_address,sek);
            ++temp_address;
            ++temp_address;
            uint8_t information = (uint8_t)save_info_table[i] * 100;
            information += i;
            EEPROM_write(temp_address,information);
            ++temp_address;
        }
    }
    PCF8583_write(PCF8583_SAVED_ADDRESS_CELL, temp_address);
}


int locker_1_button(void)//przycisk fizycznie umieszczony na płytce
{
    int temp = 0;
    if(!( LOCKER_1_BUTTON_PIN & LOCKER_1_BUTTON_IN ))
    {
        temp = 1;
    }
    return temp;
}

int locker_2_button(void)//przycisk fizycznie umieszczony na płytce
{
    int temp = 0;
    if(!( LOCKER_2_BUTTON_PIN & LOCKER_2_BUTTON_IN ))
    {
        temp = 1;
    }
    return temp;
}
