#include "lockers.h"



void lockers_init()
{

    LOCKER_1_BUTTON_DIR  &= ~LOCKER_1_BUTTON_IN;//inicjowanie przycisku jako wejście
    LOCKER_1_BUTTON_PORT |= LOCKER_1_BUTTON_IN;//podciągnięcie przycisku tranzystorami

    LOCKER_2_BUTTON_DIR  &= ~LOCKER_2_BUTTON_IN;//inicjowanie przycisku jako wejście
    LOCKER_2_BUTTON_PORT |= LOCKER_2_BUTTON_IN;//podciągnięcie przycisku tranzystorami


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
    return ((EEPROM_MAX_ADDRESS + 1) / SIZE_OF_FRAME);
}


uint8_t lockers_convert_address_to_index_of_frame(uint8_t add)
{
    return (add / SIZE_OF_FRAME);
}

void lockers_read_frame(uint8_t index)
{
    uint8_t temp_address = SIZE_OF_FRAME * index;
    frame.seconds = EEPROM_read(temp_address);
    ++temp_address;
    frame.minutes = EEPROM_read(temp_address);
    ++temp_address;
    frame.hours = EEPROM_read(temp_address);
    ++temp_address;
    frame.day = EEPROM_read(temp_address);
    ++temp_address;
    frame.month = EEPROM_read(temp_address);
    ++temp_address;
    frame.year = EEPROM_read_word(temp_address);
    ++temp_address;
    ++temp_address;
    frame.information = EEPROM_read(temp_address);
    ++temp_address;
}

void lockers_save_events(void)//funkcja zapisująca do pamięci EEPROM dane
{
    //delay_ms_var(400);
    uint8_t temp_address = PCF8583_read_word(PCF8583_CELL);;//pobranie ostatniego adresu
    uint8_t overflow_flag = 0;

    if((EEPROM_MAX_ADDRESS - temp_address) < (SIZE_OF_FRAME - 1))
    {
        temp_address = 0;//jeśli następna bramka się nie zmieści, trzeba ją przesunąć
        overflow_flag = 1;
    }

    uint8_t i = 0;
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
            EEPROM_write_word(temp_address, rok);
            ++temp_address;
            ++temp_address;
            uint8_t information = (uint8_t)save_info_table[i] * 100;
            information += i + 1;
            EEPROM_write(temp_address,information);
            ++temp_address;
        }
    }
    PCF8583_write(PCF8583_CELL, temp_address);
    if(temp_address == 0) overflow_flag = 1;
    if(overflow_flag == 1) buzzer_time(1000);//przepełnienie pamięci
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
