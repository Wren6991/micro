#define BUFFER_LENGTH 128
uint8_t irbuffer[BUFFER_LENGTH];

void setup()
{
  Serial.begin(9600);
  DDRC = 0x02;
}

void loop()
{
  if (Serial.available() > 0)
  {
    char command = Serial.read();
    if (command == 'r')
    {
      Serial.println("Waiting for command...");
      while (PINC & 0x01);
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
      for (uint8_t i = 0; i < BUFFER_LENGTH; i++)
      {
        if (irbuffer[i] < 0x10)
          Serial.print("0");
         Serial.print((uint8_t)(irbuffer[i]), HEX);
         if (i % 16 == 15)
           Serial.println("");
      }
    }
    else if (command == 't')
    {
      Serial.print("Transmitting command... ");
      for (uint8_t i = 0; i < BUFFER_LENGTH; i++)
      {
        uint8_t c = irbuffer[i];
        for (uint8_t bitcount = 0; bitcount < 8; bitcount++)
        {
          PORTC = (PORTC & ~0x02) | ((c & 0x80) >> 6);
          c = c << 1;
          delayMicroseconds(100);
        }
      }
      PORTC = (PORTC & ~0x02);
      Serial.println("Done.");
    }
    else
    {
      Serial.print("Unrecognized: ");
      Serial.println(command);
    }
  }
}
