unsigned char LEDs[128] = {0};
unsigned char sel;

#include <SPI.h>

void setup()
{
  DDRD = 0xFF;
  DDRC = 0x3F;

  for (unsigned char x = 0; x < 3; x++)
  {
    unsigned char led_signal = 0;
    for (unsigned char i = 0; i < sizeof(LEDs); i++)
    {
      unsigned char t = i + x;

      if ((t % 3) == 0)
        led_signal |= 1 << (i & 0b0111);

      if (((i + 1) & 0b0111) == 0)
      {
        PORTD = led_signal;
        led_signal = 0;
        PORTC = (i >> 3);
        PORTC |= 0x10;
        PORTC &= 0x0F;
      }
    }
    delay(1000);
  }

  sel = 0xFF;

  pinMode(MISO, OUTPUT);

  // Enable SPI as slave.
  SPCR |= (1 << SPE);
  SPI.attachInterrupt();
}

// SPI interrupt routine
ISR (SPI_STC_vect)
{
  byte c = 0;

  c = SPSR; //Clean status register
  c = SPDR; //Read data register

  if (c == 0xFD) //Reset sel
  {
    sel = 0xFF;
    return;
  }
  if (sel == 0xFF)
    sel = c; //Load new C val
  else 
  {
    LEDs[sel] = c;
    sel = 0xFF; //Reset sel
  }
}

void loop()
{
  unsigned static char count = 0;
  unsigned static char led_signal = 0;
  unsigned char t = 0;

  for (unsigned char i = 1; i <= sizeof(LEDs); i++)
  {
    t = i - (unsigned char)1;

    //Only comparing the first 4 bits of color for brightness
    if (LEDs[t] > (count & (unsigned char)(0x0F)))
      led_signal |= (unsigned char)(1) << (t & (unsigned char)(0b0111));

    if ((i & 0b0111) == 0)
    {
      PORTD = led_signal;
      led_signal = 0;
      PORTC = (unsigned char)(t >> 3);
      PORTC |= (unsigned char)0x10;
      PORTC &= (unsigned char)0x0F;
    }
  }
  count++;
}
