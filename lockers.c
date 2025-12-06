#include "lockers.h"

void lockers_flag_bit_on(uint8_t move)
{
    PCF8583_write(PCF8583_FLAGS_CELL,PCF8583_read(PCF8583_FLAGS_CELL) | (1 << move));
}

void lockers_flag_bit_off(uint8_t move)
{
    PCF8583_write(PCF8583_FLAGS_CELL,PCF8583_read(PCF8583_FLAGS_CELL) & ~(1 << move));
}

uint8_t lockers_is_flag_bit(uint8_t move)
{
    return (PCF8583_read(PCF8583_FLAGS_CELL) & (1 << move)) >> move;
    //return 0;//usunąć, gdy będzie PCF8563
}

void lockers_init()
{
    int16_t temp = INTERNAL_EEPROM_MAX_INDEX + 1;
    uint16_t tail_word = PCF8583_read_word(PCF8583_TAIL);
    uint16_t head_word = PCF8583_read_word(PCF8583_HEAD);
    if(head_word > temp || tail_word > temp || head_word < INTERNAL_EEPROM_MIN_INDEX || tail_word < INTERNAL_EEPROM_MIN_INDEX )
    {
        PCF8583_write_word(PCF8583_TAIL, lockers_convert_index_of_frame_to_address(0));
        lockers_queue_empty();

        show_properties(10);
        buzzer_time(1000);
        LCD_Clear();
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

    /* Przekierowuje standardowe wejście do  'mystdin' */

    //stdin = &mystdin;

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

void lockers_check_events(void)
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
            if (state == 1) save_info_table[i] = 2;//szafka otwarta
            else if(state == 0) save_info_table[i] = 1;//szafka zamknięta
        }
        else save_info_table[i] = 0; //nie zapisuj żadnej informacji dla tej szufladki
        states_table[i] = state;
    }
    if(action)
    {
        blue_colors_RGB();
        show_properties(8);
        //refresh_screen = 1;
        buzzer_time(300);
        lockers_queue_enque();
        change_color_RGB();
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

uint16_t lockers_convert_index_of_frame_to_address(uint8_t index)
{
    return SIZE_OF_FRAME * index + INTERNAL_EEPROM_MIN_INDEX;
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
    uint16_t temp_address = lockers_convert_index_of_frame_to_address(index);

    eeprom_busy_wait();

    eeprom_read_block( &frame, (const void *)temp_address, SIZE_OF_FRAME);

    temp_address += SIZE_OF_FRAME;
}

void lockers_print_entire_frame(void)
{
    buzzer_time(0.5);
    printf("%04d:%02d:%02d %02d:%02d:%02d", frame.year, frame.month, frame.day, frame.hours, frame.minutes, frame.seconds);

    uint8_t number = frame.information % 100;

    if(number != 0)
    {
        printf(" OBIEKT NR: %d ",number);
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
            printf("%03d. ", index_of_frame + 1);
            lockers_print_entire_frame();
        }
    }
}

void lockers_print_date_of_report(void)
{
    all_colors_RGB();
    PCF8583_get_wall_time();
    printf("%04d:%02d:%02d %02d:%02d:%02d\n", rok, miesiac, dzien, godz, min, sek);
}

void lockers_print_temperature(void)
{
    printf("\nTEMPERATURA");
    if(lockers_is_flag_bit(1) == 1) printf(" KRYTYCZNA");
    printf(". ");
    lockers_print_date_of_report();
    struct double_format temp_double_format = set_double_format(termometer_temperature, 2);
    printf("%d.%02d stopni Celsjusza\n", temp_double_format.integer_number, temp_double_format.decimal_number);
    refresh_screen = 1;
}

void lockers_print_all_memory(void)
{
    //uint16_t temp = 0;
    printf("\nRAPORT AWARYJNY. ");
    lockers_print_date_of_report();
    lockers_print_amount_of_first_frames(lockers_number_of_frames());
}

void lockers_print_latest_data(void)
{
    if(lockers_is_queue_empty() == 0)
    {
        printf("\nRAPORT. ");
        lockers_print_date_of_report();
        lockers_print_amount_of_first_frames(lockers_queue_number_of_records());
        lockers_queue_empty();
    }
    else buzzer_time(5);
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

uint8_t lockers_is_queue_empty(void)
{
    if(lockers_head() == lockers_tail()) return 1;
    else return 0;
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
    uint16_t temp_address = lockers_convert_index_of_frame_to_address(index);//pobranie ostatniego adresu


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
        temp_address = lockers_convert_index_of_frame_to_address(0);//jeśli następna bramka się nie zmieści, trzeba ją przesunąć
    }

    PCF8583_write_word(PCF8583_TAIL, temp_address);
}

uint8_t lockers_is_queue_full(void)
{
    if(lockers_tail() == lockers_number_of_frames() - 1)
    {
        if(lockers_head() == 0)
        {
            return 1;
        }
    }
    else
    {
        if(lockers_head() == lockers_tail() + 1)
        {
            return  1;
        }
    }
    return 0;
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
            //buzzer_time(5);
            //delay_ms_var(5);


            if( lockers_is_queue_full() == 1)
            {
                if(start_program == 3)
                {
                    lockers_queue_dequeue();
                    green_colors_RGB();
                    show_properties(9);
                    buzzer_time(2000);
                }
                else
                {
                    lockers_print_latest_data();
                    //lockers_save_frame(lockers_tail(), i);
                }
            }
            lockers_save_frame(lockers_tail(), i);
        }
    }
    backlight(2);
}

void lockers_queue_dequeue(void)
{
    if(lockers_is_queue_empty() == 0)
    {
        if(lockers_head() == lockers_number_of_frames() - 1) PCF8583_write_word(PCF8583_HEAD, lockers_convert_index_of_frame_to_address(0));
        else PCF8583_write_word(PCF8583_HEAD, lockers_convert_index_of_frame_to_address(lockers_head() + 1));
    }
}

uint8_t locker_1_button(void)//przycisk fizycznie umieszczony na płytce
{
    return (LOCKER_1_BUTTON_PIN & LOCKER_1_BUTTON_IN) >> LOCKER_1_BUTTON_PLACE;
}

uint8_t locker_2_button(void)//przycisk fizycznie umieszczony na płytce
{
    return (LOCKER_2_BUTTON_PIN & LOCKER_2_BUTTON_IN) >> LOCKER_2_BUTTON_PLACE;
}

uint8_t locker_3_button(void)//przycisk fizycznie umieszczony na płytce
{
    return (LOCKER_3_BUTTON_PIN & LOCKER_3_BUTTON_IN) >> LOCKER_3_BUTTON_PLACE;
}

uint8_t locker_4_button(void)//przycisk fizycznie umieszczony na płytce
{
    return (LOCKER_4_BUTTON_PIN & LOCKER_4_BUTTON_IN) >> LOCKER_4_BUTTON_PLACE;
}

uint8_t locker_5_button(void)//przycisk fizycznie umieszczony na płytce
{
    return (LOCKER_5_BUTTON_PIN & LOCKER_5_BUTTON_IN) >> LOCKER_5_BUTTON_PLACE;
}

uint8_t locker_6_button(void)//przycisk fizycznie umieszczony na płytce
{
    return (LOCKER_6_BUTTON_PIN & LOCKER_6_BUTTON_IN) >> LOCKER_6_BUTTON_PLACE;
}

uint8_t locker_7_button(void)//przycisk fizycznie umieszczony na płytce
{
    return (LOCKER_7_BUTTON_PIN & LOCKER_7_BUTTON_IN) >> LOCKER_7_BUTTON_PLACE;
}

uint8_t locker_8_button(void)//przycisk fizycznie umieszczony na płytce
{
    return (LOCKER_8_BUTTON_PIN & LOCKER_8_BUTTON_IN) >> LOCKER_8_BUTTON_PLACE;
}

uint8_t locker_9_button(void)//przycisk fizycznie umieszczony na płytce
{
    return (LOCKER_9_BUTTON_PIN & LOCKER_9_BUTTON_IN) >> LOCKER_9_BUTTON_PLACE;
}

uint8_t locker_10_button(void)//przycisk fizycznie umieszczony na płytce
{
    return (LOCKER_10_BUTTON_PIN & LOCKER_10_BUTTON_IN) >> LOCKER_10_BUTTON_PLACE;
}

void lockers_clear_all_memory(void)
{
    int16_t i = lockers_convert_index_of_frame_to_address(0);

    for(; i < lockers_convert_index_of_frame_to_address(lockers_number_of_frames()); ++i)
    {
        eeprom_busy_wait();
        eeprom_update_byte((uint8_t*)i, 0);
    }

    lockers_queue_empty();
}
