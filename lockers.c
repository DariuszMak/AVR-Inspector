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

int lockers_state_of_single_button( int index )
{
    if(index == 0) return locker_1_button();
    else if(index == 1) return locker_2_button();
    return -1;//bląd
}

void lockers_check_events()
{
    char * save_info_table = (char*) malloc(AMOUNT_OF_LOCKERS * sizeof (char));
    int i = 0;
    int action = 0;
    char state = 0;
    for(; i < AMOUNT_OF_LOCKERS; ++i)
    {
        save_info_table[i] = 0;
        state = (char)lockers_state_of_single_button(i);
        if( state != states_table[i])
        {
            action = 1;
            if (state) save_info_table[i] = 2;//szafka zamknięta
            else save_info_table[i] = 1;//szafka otwarta
        }

    }
    if(action);
    free(save_info_table);

    for(; i < AMOUNT_OF_LOCKERS; ++i)
    {
        states_table[i] = (char)lockers_state_of_single_button(i);
    }

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
