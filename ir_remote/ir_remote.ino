// PORTD: bit 7 -> |Bt7|Bt6|Bt5|Bt4|Bt3|Bt2|Bt1|Bt0| -> bit 0
// PORTC: bit 5 ->         | - | - |Lrn|LED|IRo|IRi|

#include <EEPROM.h>

#define BUFFER_LENGTH 64
uint8_t irbuffer[BUFFER_LENGTH];
bool irout_enable;
uint8_t current_file;

void loadfile(uint8_t filenum)
{
  if (current_file == filenum)
    return;
  uint16_t address = BUFFER_LENGTH * (uint16_t)filenum;
  for (uint8_t i = 0; i < BUFFER_LENGTH; i++, address++)
    irbuffer[i] = EEPROM.read(address);
  current_file = filenum;
}

void savefile(uint8_t filenum)
{
  uint16_t address = BUFFER_LENGTH * (uint16_t)filenum;
  for (uint8_t i = 0; i < BUFFER_LENGTH; i++, address++)
    EEPROM.write(address, irbuffer[i]);
  current_file = filenum;
}

void setup()
{
  DDRC = 0x06;    // led and ir out are configured for output
  PORTC = 0x08;   // pullup enabled for learn button
  DDRD = 0x00;    // all buttons are inputs
  PORTD = 0xff;   // all pullups enabled
  for (int i = 0; i < 6; i++)
  {
    PORTC ^= 0x04;
    delay(100);
  }
  irout_enable = false;
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  
  OCR1A = 210;          // compare match register 16MHz/210 = 76kHz
  TCCR1B |= 1 << WGM12;  // CTC mode
  TCCR1B |= 1 << CS10;   // no prescale
  TIMSK1 |= 1 << OCIE1A; // enable timer output compare interrupt
  interrupts();
  loadfile(0);
}

ISR(TIMER1_COMPA_vect)
{
  if (irout_enable)
    PORTC ^= 0x02;
  else
    PORTC &= ~0x02;
}

void loop()
{
  if (PIND == 0xff) // no function buttons pressed
    return;
  uint8_t function_number = 0;
  while ((PIND >> function_number) & 0x01)
    function_number++;
  if (PINC & 0x08) // learn button not pressed
  {
    loadfile(function_number);
    PINC |= 0x04; // switch LED on
    for (uint8_t i = 0; i < BUFFER_LENGTH; i++)
    {
      uint8_t c = irbuffer[i];
      for (uint8_t bitcount = 0; bitcount < 8; bitcount++)
      {
        irout_enable = c & 0x80;
        c = c << 1;
        delayMicroseconds(100);
      }
    }
    PORTC = PORTC & ~0x04;  // switch off LED and IRout
    irout_enable = false;
  }
  else // learn button is pressed
  {
    while (PINC & 0x01)
      PORTC ^= 0x04;
    PORTC |= 0x04;
    for (uint8_t i = 0; i < BUFFER_LENGTH; i++)
    {
      uint8_t c = 0;
      for (uint8_t count = 0; count < 8; count++)
      {
        c = (c << 1) | (PINC & 0x01);
        delayMicroseconds(100);
      }
      irbuffer[i] = ~c;
    }
    savefile(function_number);
    PORTC &= ~0x04;
  }
  delay(20);
}
