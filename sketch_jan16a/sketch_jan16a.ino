unsigned char LEDs[128];

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
  
  //Delay output to LEDs for debugging. This gives a ten second window to add new code
  randomSeed(200);
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
  
  if (count == 0)
  {
    pass_count++;
    if ((pass_count & 0x1F) == 0x10)
    {
      for (unsigned char i = 1; i <= sizeof(LEDs); i++)
      {
        LEDs[i] = random(16);
      } 
    } 
  }
}
