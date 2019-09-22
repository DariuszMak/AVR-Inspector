#include "uart.h"

volatile char UART_RX_BUF[UART_READ_BUFFER_SIZE];
volatile uint16_t uart_rx_h=0;
volatile uint16_t uart_rx_t=0;

volatile char UART_TX_BUF[UART_WRITE_BUFFER_SIZE];
volatile uint16_t uart_tx_h=0;
volatile uint16_t uart_tx_t=0;

//volatile uint8_t rx_overrun=0;

void uart_init(uint16_t baud)
{
    //wyliczamy wartoœæ rejestru UBRR
    uint16_t ubrr_cal=F_CPU/16/baud-1;
    //konfiguracja baud
    UBRRH = (uint8_t)(ubrr_cal>>8);
    UBRRL = (uint8_t)ubrr_cal;
    //w³¹czenie UART
    UCSRB = (1<<RXEN)|(1<<TXEN);
    UCSRC = (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);
    UCSRB |= (1<<RXEN)|(1<<TXEN)|(1<<RXCIE);

    //DDRD &= ~(1<<PD2);
    //PORTD |= (1<<PD2);//PD2 - wejście + pull-up

    /* Tworzy strumienia danych o nazwie 'mystdout' połączony
    z funkcją 'USART_Transmit' */
    static FILE mystdout = FDEV_SETUP_STREAM(USART_Transmit, NULL, _FDEV_SETUP_WRITE);

    //static FILE mystdin = FDEV_SETUP_STREAM(NULL, uart_getc, _FDEV_SETUP_READ);

    /* Przekierowuje standardowe wyjście do  'mystdout' */
    stdout = &mystdout;
}

/* Wysyła znak do portu szeregowego */
void USART_Transmit(uint8_t c, FILE *stream)
{
    uart_putc(c);
}

void uart_putc(char data)
{
    if (data == '\n')
    {
        uart_putc('\r');
    }
    uint16_t new_h=(uart_tx_h+1)&UART_WRITE_BUFFER_MASK;
    while(new_h==uart_tx_t);
    UART_TX_BUF[new_h]=data;
    uart_tx_h=new_h;
    UCSRB |= (1<<UDRIE);
}

void uart_puts(char *string)
{
    char c;
    while((c=*string++))uart_putc(c);
}

/*void uart_putint(int number, uint8_t base)
{
    char buffer[20];
    itoa(number,buffer,base);
    uart_puts(buffer);
}*/

char uart_getc(void)
{
    if(uart_rx_t==uart_rx_h)return 0;
    uart_rx_t=(uart_rx_t+1)&UART_READ_BUFFER_MASK;
    return UART_RX_BUF[uart_rx_t];
}


/*void uart_gets(char * temporary_table, uint16_t size_of_table)
{
    //uart_putint(sizeof(temporary_table),10);
    uint16_t times = 0;
    char odebrany;

    do
    {
        odebrany = 0;
        odebrany=uart_getc();
        if(odebrany!=0)
        {
            if(odebrany!='\r') //filtrujemy znak zakoñczenia liczby
            {
                uart_putc(odebrany);
                //uart_putint(times,10);
                temporary_table[times++] = odebrany;
            }
        }

    }
    while (odebrany != '\r' && times != size_of_table - 1);

    for(; times < size_of_table; ++times)
    {
        temporary_table[times] = 0;
    }
}*/

/*uint16_t uart_getint(void)
{
    char odebrany=0;
    uint16_t liczba=0;
    do
    {
        odebrany=uart_getc();
        if(odebrany!=0)
        {
            if(odebrany!='\r') //filtrujemy znak zakoñczenia liczby
            {
                uart_putc(odebrany);
                liczba*=10;
                liczba+=odebrany-'0';
            }
        }
    }
    while(odebrany!='\r'); //jeœli odebraliœmy znak zakoñczenia liczby to wychodzimy z pêtli
    uart_putc('\n');
    return liczba;//zwracamy odebran¹ liczbê
}*/

ISR(USART_RXC_vect)
{
    uint16_t new_h=(uart_rx_h+1)&UART_READ_BUFFER_MASK;

    if(new_h!=uart_rx_t)
    {
        uart_rx_h=new_h;
        UART_RX_BUF[uart_rx_h]=UDR;
    }
    else
    {
//        rx_overrun=1;
        new_h=UDR;//odczytujemy bufor, ¿eby nie zawiesiæ systemu
    }
}

ISR(USART_UDRE_vect)
{
    if(uart_tx_t!=uart_tx_h)
    {
        uart_tx_t=(uart_tx_t+1)&UART_WRITE_BUFFER_MASK;
        UDR=UART_TX_BUF[uart_tx_t];
    }
    else
    {
        UCSRB &= ~(1<<UDRIE);
    }
}
