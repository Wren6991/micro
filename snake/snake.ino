#define MAX_SNAKE_LENGTH 64

char image[6] = {
  0x48,
  0x00,
  0xfc,
  0x84,
  0x44,
  0x38
};

char snakepos[MAX_SNAKE_LENGTH][2];
char snakelength = 1;
int snakestartindex = 0;
char foodposx = 0;
char foodposy = 0;

#define GET_SNAKE_POS(index) snakepos[(snakestartindex - index) % MAX_SNAKE_LENGTH]

char dirx = 0;
char diry = 1;

char safemod(char a, char n)
{
  while (a < 0)
    a += n;
  return a % n;
}

void display()
{
  for (int i = 0; i < 6; i++)
  {
    PORTD =  image[i];
    PORTB = ~(1 << i);
    delay(1);
    PORTB = 0xff;
  }
}

void updatecontrols()
{
  if (~PINC & 0x01)
  {
    dirx = 0;
    diry = -1;
  }
  else if (~PINC & 0x02)
  {
    dirx = 0;
    diry = 1;
  }
  else if (~PINC & 0x04)
  {
    dirx = -1;
    diry = 0;
  }
  else if (~PINC & 0x08)
  {
    dirx = 1;
    diry = 0;
  }
}

void step()
{
  snakestartindex++;
  GET_SNAKE_POS(0)[0] = safemod((GET_SNAKE_POS(1)[0] + dirx), 6);
  GET_SNAKE_POS(0)[1] = safemod((GET_SNAKE_POS(1)[1] + diry), 6);
  if (GET_SNAKE_POS(0)[0] == foodposx && GET_SNAKE_POS(0)[1] == foodposy)
  {
    snakelength++;
    foodposx = random(0, 6);
    foodposy = random(0, 6);
  }
  for (int i = 0; i < 6; i++)
    image[i] = 0;
  for (int i = 0; i < snakelength; i++)
  {
    image[GET_SNAKE_POS(i)[1]] |= 0x80 >> GET_SNAKE_POS(i)[0];
  }
}
  
  
void setup()
{
  DDRB = 0xff;
  PORTB = 0x00;
  DDRD = 0xff;
  PORTD = 0xff;
  DDRC = 0x00;
  PORTC = 0xff;
  snakepos[0][0] = 0;
  snakepos[0][1] = 0;
  foodposx = random(0, 6);
  foodposy = random(0, 6);
}

char count;

void loop()
{
  count++;
  updatecontrols();
  if (!(count & 0x1f))
    step();
  image[foodposy] &= ~(0x80 >> foodposx);
  if (count & 0x10)
    image[foodposy] |= 0x80 >> foodposx;
  display();
}
