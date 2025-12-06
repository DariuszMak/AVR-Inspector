#include "lockers.h"

void lockers_init()
{
    LOCKER_1_BUTTON_DIR  &= ~LOCKER_1_BUTTON_IN;//inicjowanie przycisku jako wejście
    LOCKER_1_BUTTON_PORT |= LOCKER_1_BUTTON_IN;//podciągnięcie przycisku tranzystorami

    LOCKER_2_BUTTON_DIR  &= ~LOCKER_2_BUTTON_IN;//inicjowanie przycisku jako wejście
    LOCKER_2_BUTTON_PORT |= LOCKER_2_BUTTON_IN;//podciągnięcie przycisku tranzystorami
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
