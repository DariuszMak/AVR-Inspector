/**
    Biblioteka obsługi zegara czasu rzeczywistego z magistralą I2C typu PCF8583.
*/

#include "PCF8583.h"//sprzętowa obsługa magistrali TWI (I2C)

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
    PCF8583_write_buf(address, 1, &data );
}

/**
 Czyta bajt z układu
 \param address adres komórki w układzie
 \return odczytany bajt
*/
uint8_t PCF8583_read(uint8_t address)
{
    uint8_t temp;
    PCF8583_read_buf( address, 1, &temp );
    return temp;
}

void PCF8583_write_buf(uint8_t adr, uint8_t len, uint8_t *buf )
{
    i2cStart();
    i2cWrite((PCF8583_A0 << 1) | PCF8583_ADDRESS);
    i2cWrite(adr);
    while (len--) i2cWrite(*buf++);
    i2cStop();
}

void PCF8583_read_buf(uint8_t adr, uint8_t len, uint8_t *buf)
{
    uint8_t a;
    a = (PCF8583_A0 << 1) | PCF8583_ADDRESS;
    i2cStart();
    i2cWrite(a);
    i2cWrite(adr);
    i2cStart();
    i2cWrite(a + 1);
    while (len--) *buf++ = i2cRead( len ? ACK : NOACK );
    i2cStop();
}

/**
 Czyta bajt z układu w formacie BCD
 \param address adres komórki w układzie
 \return odczytany bajt
*/
uint8_t PCF8583_read_bcd(uint8_t address)
{
    return bcd2bin(PCF8583_read(address));
}

/**
 Zapisuje bajt do układu w formacie BCD
 \param address adres komórki w układzie
 \param data bajt do wpisania
*/
void PCF8583_write_bcd(uint8_t address,uint8_t data)
{
    PCF8583_write(address,bin2bcd(data));
}


/**
 Inicjalizuje układ
*/
void PCF8583_init(void)
{
//    PCF8583_alarm=0;
    PCF8583_write(0x00, PCF8583_read(0x00) & ~0b11111101);//bez zerowania flagi alarmu
    PCF8583_write(0x00, PCF8583_read(0x00) | 0b00000100);//komórki do alarmu dozwolone
    PCF8583_hold_off();//normalne zliczanie, bez zatrzasków
    PCF8583_mask_off();//maskowanie dni i roku wyłączone
    PCF8583_write(0x04, PCF8583_read(0x04) & ~0xC0);//1100 0000 (wskaźnik am, 24 godzinny format)

    PCF8583_write(0x08, PCF8583_read(0x08) & ~0b00000101);//przerwania alarmu dozwolone i timera też
    PCF8583_write(0x08, PCF8583_read(0x08) | 0b10001010);//przerwania alarmu dozwolone i timera też

    PCF8583_write(0x0F, 0b11111100);

    PCF8583_timer_flag_off();

    PCF8583_timer_on();


    //PCF8583_write(8, 0x80);//1000 0000 alarm wyłączony
}

/**
 Zatrzymuje układ
*/
void PCF8583_stop(void)
{
    PCF8583_write(0, PCF8583_read(0) | 0x80);
}

/**
 Startuje układ
*/
void PCF8583_start(void)
{
    PCF8583_write(0, PCF8583_read(0) & ~0x80);
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
    PCF8583_write(0, PCF8583_read(0) & ~0b00000001);//alarm codzienny
}

/**
 Włącza wskaźnik alarmu
*/
void PCF8583_timer_flag_on(void)
{
    PCF8583_write(0, PCF8583_read(0) | 0b00000001);//alarm codzienny
}

/**
 Wyłacza wskaźnik alarmu
*/
void PCF8583_alarm_flag_off(void)
{
    PCF8583_write(0, PCF8583_read(0) & ~0b00000010);//alarm codzienny
}

/**
 Włącza wskaźnik alarmu
*/
void PCF8583_alarm_flag_on(void)
{
    PCF8583_write(0, PCF8583_read(0) | 0b00000010);//alarm codzienny
}

/**
 Zapisuje słowo do układu
 \param address adres komórki w układzie
 \param data słowo do wpisania
*/
void PCF8583_write_word(uint8_t address,uint16_t data)
{
    PCF8583_write(address, (uint8_t)(data & 0xFF));
    PCF8583_write(++address, (uint8_t)(data >> 8));
}

uint16_t PCF8583_read_word(uint8_t address)
{
    uint16_t temp;
    temp = PCF8583_read(address) & 0xFF;
    temp |= PCF8583_read(++address) << 8;
    return temp;
}

/**
 Czyta czas z układu
 \param hour godzina
 \param min minuta
 \param sec sekunda
 \param hsec setne części sekundy
*/
void PCF8583_get_time(uint8_t *hour, uint8_t *min, uint8_t *sec, uint8_t *hsec, uint8_t *day, uint8_t *day_of_week, uint8_t *month, int16_t *year)
{
    struct time_frame time_f;
    uint8_t year_table[2];
    PCF8583_mask_off();
    PCF8583_hold_on();
    PCF8583_read_buf(0x01, 6, (uint8_t*)&time_f);
    PCF8583_hold_off();
    PCF8583_mask_on();
    PCF8583_read_buf(0x10, 2, year_table);

    *hsec=bcd2bin(time_f.hseconds);
    *sec=bcd2bin(time_f.seconds);
    *min=bcd2bin(time_f.minutes);
    *hour=bcd2bin(time_f.hours);
    *day=bcd2bin(time_f.days & 0b00111111);
    *month=bcd2bin(time_f.months & 0b00011111);
    *day_of_week = (time_f.months & 0b11100000) >> 5;

    int16_t y1;
    uint8_t dy;

    dy = (time_f.days & 0b11000000) >> 6;
    y1 = year_table[0] | ( (int16_t)year_table[1] << 8);
    if ( ( (uint8_t) y1 & 3 ) != dy )
        PCF8583_write_word(0x10, ++y1);
    *year = y1;
}

/**
 Ustawia czas w układzie
 \param hour godzina
 \param min minuta
 \param sec sekunda
 \param hsec setne części sekundy
*/
void PCF8583_set_time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t hsec, uint8_t day, uint8_t day_of_week, uint8_t month, int16_t year)
{
    struct time_frame time_f;
    uint8_t year_table[2];
    time_f.hseconds=bin2bcd(hsec);
    time_f.seconds=bin2bcd(sec);
    time_f.minutes=bin2bcd(min);
    time_f.hours=bin2bcd(hour);
    time_f.days = bin2bcd(day) | ( ( (uint8_t)year & 0x03) << 6 );
    time_f.months = bin2bcd(month) | ( ( (uint8_t)day_of_week & 0x07) << 5 );

    year_table[0] = year;
    year_table[1] = year >> 8;

    PCF8583_stop();
    PCF8583_write_buf(0x01, 6, (uint8_t*)&time_f);
    PCF8583_start();
    PCF8583_write_buf(0x10, 2, year_table);
}

/**
 Czyta czas alarmu z układu
 \param hour godzina
 \param min minuta
 \param sec sekunda
 \param hsec setne części sekundy
*/
void PCF8583_get_alarm_time(int8_t *hour, int8_t *min, int8_t *sec, int8_t *hsec, int8_t *day, int8_t *month)
{
    struct time_frame time_f;
    PCF8583_read_buf(0x09, 6, (uint8_t*)&time_f);

    *hsec=bcd2bin(time_f.hseconds);
    *sec=bcd2bin(time_f.seconds);
    *min=bcd2bin(time_f.minutes);
    *hour=bcd2bin(time_f.hours);
    *day = bcd2bin(time_f.days);

    if(PCF8583_recognise_type_of_alarm() == 2)
    {
        *month = time_f.months & 0b01111111;
    }
    else *month = bcd2bin(time_f.months);
}


/**
 Ustawia czas alarmu w układzie
 \param hour godzina
 \param min minuta
 \param sec sekunda
 \param hsec setne części sekundy
*/
void PCF8583_set_alarm_time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t hsec, uint8_t day, uint8_t month, uint8_t type_of_alarm)
{
    struct time_frame time_f;
    time_f.hseconds=bin2bcd(hsec);
    time_f.seconds=bin2bcd(sec);
    time_f.minutes=bin2bcd(min);
    time_f.hours=bin2bcd(hour);

    if(type_of_alarm == 0)
    {
        PCF8583_alarm_off();
    }
    else if(type_of_alarm == 1)
    {
        PCF8583_alarm_every_day();
    }
    else if(type_of_alarm == 2)
    {
        PCF8583_alarm_weekly();
        time_f.months = month & 0b01111111;
    }
    else if(type_of_alarm == 3)
    {
        PCF8583_alarm_monthly();
        time_f.days = bin2bcd(day);
        time_f.months = bin2bcd(month);
    }

    PCF8583_write_buf(0x09, 6, (uint8_t*)&time_f);
}

void PCF8583_timer_off()
{
    PCF8583_write(0x08, PCF8583_read(0x08) & ~0b01000000);
}

void PCF8583_timer_on()
{
    PCF8583_write(0x08, PCF8583_read(0x08) | 0b01000000);
}

/**
 Wyłącza alarm
*/
void PCF8583_alarm_off(void)
{
    PCF8583_write(0x08, PCF8583_read(0x08) & ~0b00110000);//wyłączenie alarmu
}

/**
 Załącza alarm codzienny
*/
void PCF8583_alarm_every_day(void)
{
    PCF8583_write(0x08, PCF8583_read(0x08) | 0b00010000);//alarm codzienny
    PCF8583_write(0x08, PCF8583_read(0x08) & ~0b00100000);//alarm codzienny
}

/**
 Załącza alarm dla dni w tygodniu - niewygodna opcja, ponieważ inaczej porównuje bity (zajrzeć do dokumantacji)
*/
void PCF8583_alarm_weekly(void)
{
    PCF8583_write(0x08, PCF8583_read(0x08) & ~0b00010000);//alarm codzienny
    PCF8583_write(0x08, PCF8583_read(0x08) | 0b00100000);//alarm codzienny
}

/**
 Załącza alarm dla dni w miesiącu
*/
void PCF8583_alarm_monthly(void)
{
    PCF8583_write(0x08, PCF8583_read(0x08) | 0b00110000);//alarm codzienny
}


/*****************************PRZYDATNE FUNKCJE ZEWNĘTRZNE********************************/


uint8_t PCF8583_recognise_type_of_alarm(void)
{
    return ((PCF8583_read(0x08) & 0x30) >> 4);
}

uint8_t PCF8583_is_timer_set(void)
{
    if(PCF8583_read(0x00) & 0b00000001) return 1;
    else return 0;
}

/**
 Sprawdza, czy alarm jest włączony
*/

uint8_t PCF8583_is_alarm_set(void)
{
    if(PCF8583_read(0x00) & 0b00000010) return 1;
    else return 0;
}

void PCF8583_get_wall_alarm(void)//pobiera jedynie te zmienne, które należą do alarmu
{
    PCF8583_get_alarm_time( &godz, &min, &sek, &hsek, &dzien, &miesiac);//należy pamiętać, że w trybie alarmu dziennego w zmiennej miesac przechowywane są dni tygodnia, w których będzie aktywny alarm
}

void PCF8583_get_wall_time(void)
{
    PCF8583_get_time( (uint8_t*)&godz, (uint8_t*)&min, (uint8_t*)&sek, (uint8_t*)&hsek, (uint8_t*)&dzien, (uint8_t*)&dzien_tygodnia, (uint8_t*)&miesiac, (int16_t*)&rok );
}

/*@}*/

