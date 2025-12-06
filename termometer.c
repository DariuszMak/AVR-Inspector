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
        SET_ONEWIRE_PORT;
        SET_OUT_ONEWIRE_DDR;
    }
    else
    {
        SET_IN_ONEWIRE_DDR;
    }
}

/**********************************************************/

unsigned char OneWireReset()
{
    CLR_ONEWIRE_PORT;

    if (!(IS_SET_ONEWIRE_PIN)) return 0;

    SET_OUT_ONEWIRE_DDR;
    _delay_us(500);
    SET_IN_ONEWIRE_DDR;
    _delay_us(70);

    if(!(IS_SET_ONEWIRE_PIN))
    {
        _delay_us(500);
        return(1);
    }

    _delay_us(500);

    return(0);
}

/**********************************************************/

void OneWireWriteByte(unsigned char byte)
{
    unsigned char i;

    CLR_ONEWIRE_PORT;

    for (i=0; i<8; i++)
    {
        SET_OUT_ONEWIRE_DDR;

        if (byte & 0x01)
        {
            _delay_us(7);
            SET_IN_ONEWIRE_DDR;
            _delay_us(70);
        }
        else
        {
            _delay_us(70);
            SET_IN_ONEWIRE_DDR;
            _delay_us(7);
        }

        byte >>= 1;
    }
}

/***********************************************************/

unsigned char OneWireReadByte(void)
{
    unsigned char i, byte = 0;

    SET_IN_ONEWIRE_DDR;

    for (i=0; i<8; i++)
    {
        SET_OUT_ONEWIRE_DDR;
        _delay_us(7);
        SET_IN_ONEWIRE_DDR;
        _delay_us(7);
        byte >>= 1;

        if(IS_SET_ONEWIRE_PIN) byte |= 0x80;

        _delay_us(70);
    }

    return byte;
}
/***********************************************************/

double ds18b20_temperature(void)
{
    double temp = 0;
    if(ds18b20_ConvertT())
    {

        /* 750ms - czas konwersji */
        //_delay_ms(750);

        /* Odczyt z układu ds18b20, dane zapisywane są w tablicy ds18b20_pad.
           Dwie pierwsze pozycje w tablicy to kolejno mniej znaczący bajt i bardziej
        znaczący bajt wartość zmierzonej temperatury */
        ds18b20_Read(ds18b20_pad);

        /* Składa dwa bajty wyniku pomiaru w całość. Cztery pierwsze bity mniej
           znaczącego bajtu to część ułamkowa wartości temperatury, więc całość
           dzielona jest przez 16 */
        temp = ((ds18b20_pad[1] << 8) + ds18b20_pad[0]) / 16.0 ;
        /* Formułuje komunikat w tablicy 'str' */
        int i = 0;
        for(; i<2; i++) ds18b20_pad[i] = 0;

    }
    return temp;

}
