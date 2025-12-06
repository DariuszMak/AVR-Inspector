#include "lockers.h"

void lockers_init()
{
    LOCKER_1_BUTTON_DIR  &= ~LOCKER_1_BUTTON_IN;//inicjowanie przycisku jako wejście
    LOCKER_1_BUTTON_PORT |= LOCKER_1_BUTTON_IN;//podciągnięcie przycisku tranzystorami

    LOCKER_2_BUTTON_DIR  &= ~LOCKER_2_BUTTON_IN;//inicjowanie przycisku jako wejście
    LOCKER_2_BUTTON_PORT |= LOCKER_2_BUTTON_IN;//podciągnięcie przycisku tranzystorami

    int i = 0;
    for(; i < AMOUNT_OF_LOCKERS; ++i)
    {
        if(i == 0) states_table[i] = locker_1_button();
        else if(i ==1) states_table[i] = locker_2_button();
    }

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
    char state = 0;//stan przycisku z danej chwili
    for(; i < AMOUNT_OF_LOCKERS; ++i)//sprawdzanie stanów przycisków i odpowiednie wypełnianie tablicy
    {
        state = (char)lockers_state_of_single_button(i);//jednorazowe złapanie stanu przycisku
        if( state != states_table[i] )//jeśli stan przycisku różni się od poprzednich wartości, należy wypełnić tabelę
        {
            action = 1;//akcja będzie podjęta
            if (state) save_info_table[i] = 2;//szafka zamknięta
            else save_info_table[i] = 1;//szafka otwarta
        }else save_info_table[i] = 0;//nie zapisuj żadnej informacji dla tej szufladki
    }

    if(action) lockers_check_events();

    for(; i < AMOUNT_OF_LOCKERS; ++i)
    {
        states_table[i] = (char)lockers_state_of_single_button(i);
    }
}

void lockers_save_events(void)
{

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
