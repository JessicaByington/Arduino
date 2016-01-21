volatile unsigned char LEDs[128];
volatile bool sel;
volatile bool spid;
#include <SPI.h>

void setup() 
{   
  DDRD = 0xFF;
  DDRC = 0x1F;

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
  SPCR |= _BV(SPE);

  SPI.attachInterrupt();
  
  //Delay output to LEDs for debugging. This gives a ten second window to add new code
  randomSeed(200);
}

// SPI interrupt routine
ISR (SPI_STC_vect)
{
  byte c = SPDR;  // grab byte from SPI Data Register
  
  if (sel != 0xFF)
  {
    LEDs[sel] = c;
    sel = 0xFF;
  }
  else
  {
    sel = c;
  }
  spid = true;
}


void loop() 
{
  unsigned static char count = 0;
  unsigned static char pass_count = 0;
  unsigned static char led_signal = 0;

  unsigned char t = 0;
  
  for (unsigned char i = 1; i <= sizeof(LEDs); i++)
  {
    t = i - 1;

    //Only comparing the first 4 bits of color for brightness
    if (LEDs[t] > (count & 0x0F))
      led_signal |= 1 << (t & 0b0111);
    
    if ((i & 0b0111) == 0)
    {
      PORTD = led_signal;
      led_signal = 0;  
      PORTC = (t >> 3);
      PORTC |= 0x10;
      PORTC &= 0x0F;
    }
  }
  count++;
  
}
