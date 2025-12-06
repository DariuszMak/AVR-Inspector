/*
   Plik ds18b20.c
   (minimum kodu do odczytu temperatury z ds18b20)

   xyz.isgreat.org
*/


#include "termometer.h"


/**********************************************************/

unsigned char ds18b20_ConvertT(void)
{
    if (!OneWireReset()) return 0;

    OneWireWriteByte(0xcc); // SKIP ROM
    OneWireWriteByte(0x44); // CONVERT T

    return -1;
}

/***********************************************************/

int ds18b20_Read()
{
    unsigned char i;

    if (!OneWireReset()) return 0;

    OneWireWriteByte(0xcc); // SKIP ROM
    OneWireWriteByte(0xbe); // READ SCRATCHPAD

    for(i=0; i<2; i++) ds18b20_pad[i] = OneWireReadByte();

    return 1;
}

/**********************************************************/

void OneWireStrong(char s)
{
    if (s)
    {
        ONEWIRE_PORT  |=  ONEWIRE;
        ONEWIRE_DIR   |=  ONEWIRE;
    }
    else
    {
        ONEWIRE_DIR   &= ~ONEWIRE;
    }
}

/**********************************************************/

unsigned char OneWireReset()
{
    ONEWIRE_PORT  &= ~ONEWIRE;

    if (!(ONEWIRE_PIN   &   ONEWIRE)) return 0;

    ONEWIRE_DIR   |=  ONEWIRE;
    delay_us_var(500);
    ONEWIRE_DIR   &= ~ONEWIRE;
    delay_us_var(70);

    if(!(ONEWIRE_PIN   &   ONEWIRE))
    {
        delay_us_var(500);
        return(1);
    }

    delay_us_var(500);

    return(0);
}

/**********************************************************/

void OneWireWriteByte(unsigned char byte)
{
    unsigned char i;

    ONEWIRE_PORT  &= ~ONEWIRE;

    for (i=0; i<8; i++)
    {
        ONEWIRE_DIR   |=  ONEWIRE;

        if (byte & 0x01)
        {
            delay_us_var(7);
            ONEWIRE_DIR   &= ~ONEWIRE;
            delay_us_var(70);
        }
        else
        {
            delay_us_var(70);
            ONEWIRE_DIR   &= ~ONEWIRE;
            delay_us_var(7);
        }

        byte >>= 1;
    }
}

/***********************************************************/

unsigned char OneWireReadByte(void)
{
    unsigned char i, byte = 0;

    ONEWIRE_DIR   &= ~ONEWIRE;

    for (i=0; i<8; i++)
    {
        ONEWIRE_DIR   |=  ONEWIRE;
        delay_us_var(7);
        ONEWIRE_DIR   &= ~ONEWIRE;
        delay_us_var(7);
        byte >>= 1;

        if(ONEWIRE_PIN   &   ONEWIRE) byte |= 0x80;

        delay_us_var(70);
    }

    return byte;
}
/***********************************************************/

void ds18b20_temperature(void)
{
    //uint8_t pilot_status_temp = lockers_is_flag_bit(3);
    //if(pilot_status_temp == 1) pilot_off();
    double temp = 0;
    //cli();

    checking_pins_interrupt_off();

    if(ds18b20_ConvertT())
    {
        /* 750ms - czas konwersji */
        //delay_ms_var(750);

        /* Odczyt z układu ds18b20, dane zapisywane są w tablicy ds18b20_pad.
           Dwie pierwsze pozycje w tablicy to kolejno mniej znaczący bajt i bardziej
        znaczący bajt wartość zmierzonej temperatury */
        ds18b20_Read(ds18b20_pad);

        /* Składa dwa bajty wyniku pomiaru w całość. Cztery pierwsze bity mniej
           znaczącego bajtu to część ułamkowa wartości temperatury, więc całość
           dzielona jest przez 16 */
        temp = ((ds18b20_pad[1] << 8) + ds18b20_pad[0]) / 16.0 ;
        /* Formułuje komunikat w tablicy 'str' */
        uint8_t i = 0;
        for(; i<2; i++) ds18b20_pad[i] = 0;
    }
    checking_pins_interrupt_on();
    //sei();
    termometer_temperature = temp;
    //if(pilot_status_temp == 1) pilot_on();
}
