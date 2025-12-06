/**
    Biblioteka obsługi zegara czasu rzeczywistego z magistralą I2C typu PCF8583.
*/

#include "PCF8583.h"//sprzętowa obsługa magistrali TWI (I2C)


/**
 Inicjalizuje układ
*/
void PCF8583_init(void)
{
//    PCF8583_alarm=0;
    PCF8583_write(0x00, PCF8583_read(0x00) & ~0b00110000);
    PCF8583_write(0x00, PCF8583_read(0x00) | 0b00000100);
}

/**-------------------------------------------------------------------------------------------------

  Name         :  uint8_t bcd2bin(uint8_t bcd)

  Description  :  Zamiana kodu BCD na binarny, bity BCD: 7654 - dziesiatki, 3210 -jednosci

  Argument(s)  :  bcd - wartość w kodzie BCD

  Return value :  kod binarny z kodu BCD

--------------------------------------------------------------------------------------------------*/
uint8_t bcd2bin(uint8_t bcd)
{
    return ( 10 * ( bcd >> 4 ) + ( bcd & 0x0f ) );
}
/**-------------------------------------------------------------------------------------------------

  Name         :  uint8_t bcd2bin(uint8_t bcd);

  Description  :  Zamiana kodu binarnego na BCD

  Argument(s)  :  bin - wartość binarna z zakresu 0-99

  Return value :  kod BCD, bity: 7654 - dziesiatki, 3210 -jednosci

--------------------------------------------------------------------------------------------------*/
uint8_t bin2bcd(uint8_t bin)
{
    return ( ( ( bin / 10 ) << 4) | ( bin % 10 ) );
}

/**
 Zapisuje bajt do układu
 \param address adres komórki w układzie
 \param data bajt do wpisania
*/
void PCF8583_write(uint8_t address,uint8_t data)
{
    i2c_write_buf(PCF8583_address(), address, 1, &data );
}

/**
 Czyta bajt z układu
 \param address adres komórki w układzie
 \return odczytany bajt
*/
uint8_t PCF8583_read(uint8_t address)
{
    uint8_t temp;
    i2c_read_buf( PCF8583_address(), address, 1, &temp );
    return temp;
}

uint8_t PCF8583_address(void)
{
    return (PCF8583_A0 << 1) | PCF8583_ADDRESS;
}

/**
 Zatrzymuje układ
*/
void PCF8583_stop(void)
{
    PCF8583_write(0x00, PCF8583_read(0x00) | (1 << 7));
}

/**
 Startuje układ
*/
void PCF8583_start(void)
{
    PCF8583_write(0x00, PCF8583_read(0x00) & ~(1 << 7));
}

uint8_t PCF8583_is_clock_counting(void)
{
    return ((~PCF8583_read(0x00) | ~(1 << 7)) & (1 << 7)) >> 7;
}

uint8_t PCF8583_timer_mode(void)
{
    uint8_t temp = PCF8583_read(0x08) & 0b00000111;
    if(temp == 0b00000111) temp = 0b00000110;
    return temp;
}

void PCF8583_set_timer_mode(uint8_t mode)
{
    if(mode == 0b00000110) mode = 0b00000111;
    PCF8583_write(0x08, (PCF8583_read(0x08) & 0b11111000) | (mode & 0b00000111));
}


/**
 Odwiesza układ
*/
void PCF8583_hold_off(void)
{
    PCF8583_write(0, PCF8583_read(0) & ~0x40);//pozwala dalej zliczać układowi
}

/**
 Zawiesza układu
*/
void PCF8583_hold_on(void)//przechowanie w zatrzaskach wyniku ostatniego zliczania
{
    PCF8583_write(0, PCF8583_read(0) | 0x40);
}

/**
 Odwiesza układ
*/
void PCF8583_mask_off(void)
{
    PCF8583_write(0, PCF8583_read(0) & ~0x08);//wyłącza maskę - dostępne są wszystkie rejestry
}

/**
 Zawiesza układu
*/
void PCF8583_mask_on(void)//maskuje dane - można bezpośrednio odczytywać
{
    PCF8583_write(0, PCF8583_read(0) | 0x08);
}

/**
 Wyłacza wskaźnik alarmu
*/
void PCF8583_timer_flag_off(void)
{
    PCF8583_write(0x00, PCF8583_read(0x00) & ~(1 << 0));//alarm codzienny
}

/**
 Włącza wskaźnik alarmu
*/
void PCF8583_timer_flag_on(void)
{
    PCF8583_write(0x00, PCF8583_read(0x00) | (1 << 0));//alarm codzienny
}

/**
 Wyłacza wskaźnik alarmu
*/
void PCF8583_alarm_flag_off(void)
{
    PCF8583_write(0x00, PCF8583_read(0x00) & ~(1 << 1));//alarm codzienny
}

/**
 Włącza wskaźnik alarmu
*/
void PCF8583_alarm_flag_on(void)
{
    PCF8583_write(0x00, PCF8583_read(0x00) | (1 << 1));//alarm codzienny
}

/**
 Zapisuje słowo do układu
 \param address adres komórki w układzie
 \param data słowo do wpisania
*/
void PCF8583_write_word(uint8_t address,uint16_t data)
{
    uint8_t table_temp[2];
    table_temp[0] = (uint8_t)(data & 0xFF);
    table_temp[1] = (uint8_t)(data >> 8);
    i2c_write_buf(PCF8583_address(), address, 2, table_temp);
}

uint16_t PCF8583_read_word(uint8_t address)
{
    uint16_t temp;
    uint8_t table_temp[2];
    i2c_read_buf(PCF8583_address(), address, 2, table_temp);
    temp = table_temp[0] & 0xFF;
    temp |= table_temp[1] << 8;
    return temp;
}

/**
 Czyta czas z układu
 \param hour godzina
 \param min minuta
 \param sec sekunda
 \param hsec setne części sekundy
*/
void PCF8583_get_time(uint8_t *hour, uint8_t *min, uint8_t *sec, uint8_t *hsec, uint8_t *day, uint8_t *day_of_week, uint8_t *month, int16_t *year, uint8_t *timer, uint8_t *AM_PM)
{
    struct time_frame time_f;
    uint8_t year_table[2];
    PCF8583_mask_off();
    PCF8583_hold_on();
    i2c_read_buf(PCF8583_address(), 0x01, 7, (uint8_t*)&time_f);
    PCF8583_hold_off();
    PCF8583_mask_on();
    i2c_read_buf(PCF8583_address(), 0x10, 2, year_table);

    *hsec=bcd2bin(time_f.hseconds);
    *sec=bcd2bin(time_f.seconds);
    *min=bcd2bin(time_f.minutes);
    *hour=bcd2bin(time_f.hours & 0b00111111);
    *AM_PM = (time_f.hours & 0b01000000) >> 6;
    *day=bcd2bin(time_f.days & 0b00111111);
    *month=bcd2bin(time_f.months & 0b00011111);
    *day_of_week = (time_f.months & 0b11100000) >> 5;

    int16_t y1;
    uint8_t dy;

    dy = (time_f.days & 0b11000000) >> 6;
    y1 = year_table[0] | ( (int16_t)year_table[1] << 8);
    if ( ( (uint8_t) y1 & 0b00000011 ) != dy )
        PCF8583_write_word(0x10, ++y1);
    *year = y1;
    *timer= bcd2bin(time_f.timer);
}

/**
 Ustawia czas w układzie
 \param hour godzina
 \param min minuta
 \param sec sekunda
 \param hsec setne części sekundy
*/
void PCF8583_set_time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t hsec, uint8_t day, uint8_t day_of_week, uint8_t month, int16_t year, uint8_t timer, uint8_t AM_PM)
{
    struct time_frame time_f;

    uint8_t year_table[2];
    time_f.hseconds=bin2bcd(hsec);
    time_f.seconds=bin2bcd(sec);
    time_f.minutes=bin2bcd(min);
    time_f.hours=(bin2bcd(hour) & 0b00111111) | ((AM_PM << 6) & 0b01000000) | (PCF8583_read(0x04) & 0b10000000);
    time_f.days = (bin2bcd(day) & 0b00111111) | ( ( (uint8_t)year & 0x03) << 6 );
    time_f.months = (bin2bcd(month) & 0b00011111) | ( ( day_of_week & 0x07) << 5 );

    year_table[0] = year;
    year_table[1] = year >> 8;

    time_f.timer = bin2bcd(timer);

    uint8_t temp = PCF8583_is_clock_counting();
    PCF8583_stop();
    i2c_write_buf(PCF8583_address(), 0x01, 7, (uint8_t*)&time_f);
    i2c_write_buf(PCF8583_address(), 0x10, 2, year_table);
    if(temp == 1) PCF8583_start();
}

/**
 Czyta czas alarmu z układu
 \param hour godzina
 \param min minuta
 \param sec sekunda
 \param hsec setne części sekundy
*/
void PCF8583_get_alarm_time(uint8_t *hour, uint8_t *min, uint8_t *sec, uint8_t *hsec, uint8_t *day, uint8_t *month, uint8_t *timer, uint8_t *AM_PM)
{
    struct time_frame time_f;
    i2c_read_buf(PCF8583_address(), 0x09, 7, (uint8_t*)&time_f);

    *hsec=bcd2bin(time_f.hseconds);
    *sec=bcd2bin(time_f.seconds);
    *min=bcd2bin(time_f.minutes);
    *hour=bcd2bin(time_f.hours & 0b00111111);
    *AM_PM = (time_f.hours & 0b01000000) >> 6;
    *day = bcd2bin(time_f.days);

    if(PCF8583_recognise_type_of_alarm() == 2)
    {
        *month = time_f.months & 0b01111111;
    }
    else *month = bcd2bin(time_f.months);
    *timer=bcd2bin(time_f.timer);
}


/**
 Ustawia czas alarmu w układzie
 \param hour godzina
 \param min minuta
 \param sec sekunda
 \param hsec setne części sekundy
*/
void PCF8583_set_alarm_time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t hsec, uint8_t day, uint8_t month, uint8_t timer, uint8_t AM_PM)
{
    uint8_t type_of_alarm = PCF8583_recognise_type_of_alarm();
    struct time_frame time_f;
    time_f.hseconds=bin2bcd(hsec);
    time_f.seconds=bin2bcd(sec);
    time_f.minutes=bin2bcd(min);
    time_f.hours=(bin2bcd(hour) & 0b00111111) | ((AM_PM << 6) & 0b01000000) | (PCF8583_read(0x0C) & 0b10000000);

    if(type_of_alarm == 2)
    {
        time_f.months = month & 0b01111111;
    }
    else if(type_of_alarm == 3)
    {
        time_f.days = bin2bcd(day);
        time_f.months = bin2bcd(month);
    }
    time_f.timer=bin2bcd(timer);

    i2c_write_buf(PCF8583_address(), 0x09, 7, (uint8_t*)&time_f);
}


void PCF8583_timer_alarm_off(void)
{
    PCF8583_write(0x08, PCF8583_read(0x08) & ~(1 << 6));
}

void PCF8583_timer_alarm_on(void)
{
    PCF8583_write(0x08, PCF8583_read(0x08) | (1 << 6));
}


void PCF8583_alarm_interrupt_off(void)
{
    PCF8583_write(0x08, PCF8583_read(0x08) & ~(1 << 7));
}

void PCF8583_alarm_interrupt_on(void)
{
    PCF8583_write(0x08, PCF8583_read(0x08) | (1 << 7));
}

uint8_t PCF8583_is_alarm_interrupt(void)
{
    return (PCF8583_read(0x08) & (1 << 7)) >> 7;
}

void PCF8583_timer_interrupt_off(void)
{
    PCF8583_write(0x08, PCF8583_read(0x08) & ~(1 << 3));
}

void PCF8583_timer_interrupt_on(void)
{
    PCF8583_write(0x08, PCF8583_read(0x08) | (1 << 3));
}

uint8_t PCF8583_is_timer_interrupt(void)
{
    return (PCF8583_read(0x08) & (1 << 3)) >> 3;
}

//ustawianie formatu godzin 24h/12h

void PCF8583_24h_format(void)
{
    PCF8583_write(0x04, PCF8583_read(0x04) & ~(1 << 7));
    PCF8583_write(0x0C, PCF8583_read(0x0C) & ~(1 << 7));
}

void PCF8583_12h_format(void)
{
    PCF8583_write(0x04, PCF8583_read(0x04) | (1 << 7));
    PCF8583_write(0x0C, PCF8583_read(0x0C) | (1 << 7));
}

uint8_t PCF8583_is_12h_24h_format(void)
{
    if((PCF8583_read(0x04) & (1 << 7)) && (PCF8583_read(0x0C) & (1 << 7))) return 1;
    else if(!(PCF8583_read(0x04) & (1 << 7)) && !(PCF8583_read(0x0C) & (1 << 7))) return 0;
    else return 255;
}


/*****************************PRZYDATNE FUNKCJE ZEWNĘTRZNE********************************/


void PCF8583_set_type_of_alarm(uint8_t type_of_alarm)
{
    PCF8583_write(0x08, (PCF8583_read(0x08) & 0b11001111) | (0b00110000 & (type_of_alarm << 4)));
}

uint8_t PCF8583_recognise_type_of_alarm(void)
{
    return (PCF8583_read(0x08) & 0b00110000) >> 4;
}

uint8_t PCF8583_recognise_type_of_timer_alarm(void)
{
    return (PCF8583_read(0x08) & (1 << 6)) >> 6;
}

uint8_t PCF8583_is_timer_flag_set(void)
{
    return (PCF8583_read(0x00) & (1 << 0)) >> 0;
}

/**
 Sprawdza, czy alarm jest włączony
*/

uint8_t PCF8583_is_alarm_flag_set(void)
{
    return (PCF8583_read(0x00) & (1 << 1)) >> 1;
}

void PCF8583_get_wall_alarm(void)//pobiera jedynie te zmienne, które należą do alarmu
{
    PCF8583_get_alarm_time( (uint8_t*)&godz, (uint8_t*)&min, (uint8_t*)&sek, (uint8_t*)&hsek, (uint8_t*)&dzien, (uint8_t*)&miesiac, (uint8_t*)&timer, (uint8_t*)&rano_wieczor);//należy pamiętać, że w trybie alarmu dziennego w zmiennej miesac przechowywane są dni tygodnia, w których będzie aktywny alarm
}

void PCF8583_get_wall_time(void)
{
    PCF8583_get_time( (uint8_t*)&godz, (uint8_t*)&min, (uint8_t*)&sek, (uint8_t*)&hsek, (uint8_t*)&dzien, (uint8_t*)&dzien_tygodnia, (uint8_t*)&miesiac, (int16_t*)&rok, (uint8_t*)&timer, (uint8_t*)&rano_wieczor);
}

/*@}*/

