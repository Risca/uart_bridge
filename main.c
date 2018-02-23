#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#define BUFFER_SZ 64

static char usart0_buf[BUFFER_SZ];
static int usart0_rx_idx = 0;
static int usart0_tx_idx = 0;
static char usart1_buf[BUFFER_SZ];
static int usart1_rx_idx = 0;
static int usart1_tx_idx = 0;

static void setup_clk(void)
{
    cli();
    CLKPR = (1 << CLKPCE);
    CLKPR = 0;
    sei();
}

static void init_usarts(void)
{
    // USART0
    UBRR0H = 0;
    UBRR0L = 12; // 38400 @ 8 MHz F_CPU
    UCSR0A = 0;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    UCSR0C = (3 << UCSZ00);

    // USART1
    UBRR1H = 0;
    UBRR1L = 12; // 38400 @ 8 MHz F_CPU
    UCSR1A = 0;
    UCSR1B = (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1);
    UCSR1C = (3 << UCSZ10) | (2 << UPM10);
}

static void usart0_putchar(unsigned char data)
{
    /* Wait for empty transmit buffer */
    while (!(UCSR0A & (1 << UDRE0)));
    /* Put data into buffer, sends the data */
    UDR0 = data;
}

static void usart1_putchar(unsigned char data)
{
    /* Wait for empty transmit buffer */
    while (!(UCSR1A & (1 << UDRE1)));
    /* Put data into buffer, sends the data */
    UDR1 = data;
}

main()
{
    setup_clk();
    init_usarts();

    while(1) {
        if (usart0_rx_idx != usart0_tx_idx) {
            usart0_putchar(usart0_buf[usart0_tx_idx++]);
            if (usart0_tx_idx >= BUFFER_SZ) {
                usart0_tx_idx = 0;
            }
        }
        if (usart1_rx_idx != usart1_tx_idx) {
            usart1_putchar(usart1_buf[usart1_tx_idx++]);
            if (usart1_tx_idx >= BUFFER_SZ) {
                usart1_tx_idx = 0;
            }
        }
    }
}

ISR(USART0_RX_vect) {
    usart1_buf[usart1_rx_idx++] = UDR0;
    if (usart1_rx_idx >= BUFFER_SZ) {
        usart1_rx_idx = 0;
    }
}

ISR(USART1_RX_vect){
    usart0_buf[usart0_rx_idx++] = UDR1;
    if (usart0_rx_idx >= BUFFER_SZ) {
        usart0_rx_idx = 0;
    }
}

