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
 Czyta bajt z układu
 \param address adres komórki w układzie
 \return odczytany bajt
*/
uint8_t PCF8583_read(uint8_t address)
{
    uint8_t a;
    a = (PCF8583_A0 << 1) | PCF8583_ADDRESS;
    i2cStart();
    i2cWrite(a);
    i2cWrite(address);
    i2cStart();
    i2cWrite(a | 1);
    a = i2cRead(NOACK);
    i2cStop();
    return a;
}


/**
 Zapisuje bajt do układu
 \param address adres komórki w układzie
 \param data bajt do wpisania
*/
void PCF8583_write(uint8_t address,uint8_t data)
{
    i2cStart();
    i2cWrite((PCF8583_A0 << 1) | PCF8583_ADDRESS);
    i2cWrite(address);
    i2cWrite(data);
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
    PCF8583_write(0, PCF8583_read(0) & ~0b11111101);//bez zerowania flagi alarmu
    PCF8583_write(0, PCF8583_read(0) | 0x04);//komórki do alarmu dozwolone
    PCF8583_hold_off();//normalne zliczanie, bez zatrzasków
    PCF8583_mask_off();//maskowanie dni i roku wyłączone
    PCF8583_write(4, PCF8583_read(4) & ~0xC0);//1100 0000 (wskaźnik am, 24 godzinny format)
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
void PCF8583_alarm_flag_off(void)
{
    PCF8583_write(0, PCF8583_read(0) & ~0b00000010);//alarm codzienny
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
 Ustawia datę w układzie
 \param address adres komórki w układzie
 \param day dzień
 \param year rok
*/
void PCF8583_write_date(uint8_t address,uint8_t day,uint16_t year)
{
    PCF8583_write(address, bin2bcd(day) | ( ( (uint8_t)year & 0x03) << 6 ) );
}

/**
 Ustawia miesiąc i dzień tykgodnia
 \param address adres komórki w układzie
 \param day dzień
 \param month miesiąc
*/
void PCF8583_write_month_dayOfWeek(uint8_t address,uint8_t month,uint8_t day_of_week)
{
    PCF8583_write(address, bin2bcd(month) | ( ( (uint8_t)day_of_week & 0x07) << 5 ) );
}

/**
 Czyta czas z układu
 \param hour godzina
 \param min minuta
 \param sec sekunda
 \param hsec setne części sekundy
*/
void PCF8583_get_time(int8_t *hour,int8_t *min,int8_t *sec,int8_t *hsec)
{
    PCF8583_hold_on();
    *hsec=PCF8583_read_bcd(1);
    *sec=PCF8583_read_bcd(2);
    *min=PCF8583_read_bcd(3);
    *hour=PCF8583_read_bcd(4);
    PCF8583_hold_off();
}

/**
 Ustawia czas w układzie
 \param hour godzina
 \param min minuta
 \param sec sekunda
 \param hsec setne części sekundy
*/
void PCF8583_set_time(uint8_t hour,uint8_t min,uint8_t sec,uint8_t hsec)
{
    PCF8583_stop();
    PCF8583_write_bcd(1,hsec);
    PCF8583_write_bcd(2,sec);
    PCF8583_write_bcd(3,min);
    PCF8583_write_bcd(4,hour);
    PCF8583_start();
}

/**
 Czyta datę z układu
 \param day dzień
 \param month miesiąc
 \param year rok
*/
void PCF8583_get_date(int8_t *day, int8_t *day_of_week, int8_t *month, int16_t *year)
{
    uint16_t y1;
    uint8_t dy;
    PCF8583_mask_on();
    PCF8583_hold_on();
    *day = bcd2bin(PCF8583_read(5));
    *month = bcd2bin(PCF8583_read(6));
    PCF8583_mask_off();
    *day_of_week = (PCF8583_read(6) & 0b11100000) >> 5;

    dy = (PCF8583_read(5) & 0b11000000) >> 6;
    y1 = PCF8583_read(16) | ( (uint16_t)PCF8583_read(17) << 8);
    if ( ( (uint8_t) y1 & 3 ) != dy )
        PCF8583_write_word(16, ++y1);
    *year = y1;
    PCF8583_hold_off();
}

/**
 Ustawia datę w układzie
 \param day dzień
 \param month miesiąc
 \param year rok
*/
void PCF8583_set_date(uint8_t day, uint8_t day_of_week, uint8_t month,uint16_t year)
{
    PCF8583_write_word(16, year);
    PCF8583_stop();
    PCF8583_write_date(5, day, year);
    PCF8583_write_month_dayOfWeek(6, month, day_of_week);
    PCF8583_start();
}

/**
 Czyta czas alarmu z układu
 \param hour godzina
 \param min minuta
 \param sec sekunda
 \param hsec setne części sekundy
*/
void PCF8583_get_alarm_time(int8_t *hour, int8_t *min, int8_t *sec, int8_t *hsec)
{
    *hsec=PCF8583_read_bcd(0x9);
    *sec=PCF8583_read_bcd(0xA);
    *min=PCF8583_read_bcd(0xB);
    *hour=PCF8583_read_bcd(0xC);
}

/**
 Czyta datê alarmu z układu
 \param day dzień
 \param month miesiąc
*/
void PCF8583_get_alarm_date(int8_t *day, int8_t *month)
{
    *day = PCF8583_read_bcd(0xD);
    if(PCF8583_recognise_type_of_alarm() == 2)
    {
        *month = PCF8583_read(0xe) & 0b01111111;
    }
    else *month = PCF8583_read_bcd(0xE);
}

/**
 Ustawia datę alarmu w układzie
 \param day dzień
 \param month miesiąc
*/
void PCF8583_set_alarm_date (uint8_t day, uint8_t month )
{
    PCF8583_write_bcd( 0xD, day );
    PCF8583_write_bcd( 0xE, month );
}



/*****************************PRZYDATNE FUNKCJE ZEWNĘTRZNE********************************/

uint8_t PCF8583_recognise_type_of_alarm(void)
{
    return ((PCF8583_read(8) & 0x30) >> 4);
}

/**
 Wyłącza alarm
*/
void PCF8583_alarm_off(void)
{
    PCF8583_write(8, PCF8583_read(8) & ~0b00110000);//wyłączenie alarmu
}

/**
 Załącza alarm codzienny
*/
void PCF8583_alarm_every_day(void)
{
    PCF8583_write(8, PCF8583_read(8) | 0b00010000);//alarm codzienny
    PCF8583_write(8, PCF8583_read(8) & ~0b00100000);//alarm codzienny
}

/**
 Załącza alarm dla dni w tygodniu - niewygodna opcja, ponieważ inaczej porównuje bity (zajrzeć do dokumantacji)
*/
void PCF8583_alarm_weekly(void)
{
    PCF8583_write(8, PCF8583_read(8) & ~0b00010000);//alarm codzienny
    PCF8583_write(8, PCF8583_read(8) | 0b00100000);//alarm codzienny
}

/**
 Załącza alarm dla dni w miesiącu
*/
void PCF8583_alarm_monthly(void)
{
    PCF8583_write(8, PCF8583_read(8) | 0b00110000);//alarm codzienny
}

/**
 Ustawia czas alarmu w układzie
 \param hour godzina
 \param min minuta
 \param sec sekunda
 \param hsec setne części sekundy
*/
void PCF8583_set_alarm_time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t hsec)
{
    PCF8583_write_bcd(0x9, hsec);
    PCF8583_write_bcd(0xA, sec);
    PCF8583_write_bcd(0xB, min);
    PCF8583_write_bcd(0xC, hour);
}

void PCF8583_set_weekly_alarm(uint8_t days_of_week, uint8_t hour, uint8_t min, uint8_t sec, uint8_t hsec)
{
    PCF8583_set_alarm_time( hour,  min,  sec,  hsec);
    PCF8583_write(0xE, days_of_week & 0b01111111);
}

void PCF8583_set_monthly_alarm(uint8_t day, uint8_t month, uint8_t hour, uint8_t min, uint8_t sec, uint8_t hsec)
{
    PCF8583_set_alarm_time( hour,  min,  sec,  hsec);
    PCF8583_set_alarm_date(day, month);
}

void PCF8583_get_wall_alarm(void)//pobiera jedynie te zmienne, które należą do alarmu
{
    PCF8583_get_alarm_time( &godz, &min, &sek, &hsek );//należy pamiętać, że w trybie alarmu dziennego w zmiennej miesac przechowywane są dni tygodnia, w których będzie aktywny alarm
    PCF8583_get_alarm_date( &dzien, &miesiac );
}

void PCF8583_get_wall_time(void)
{
    PCF8583_get_time( &godz, &min, &sek, &hsek );
    PCF8583_get_date( &dzien,&dzien_tygodnia, &miesiac, &rok );
}


/*@}*/

