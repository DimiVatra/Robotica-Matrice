#include "LedControl.h" //  need the library
#define SW_PIN 2
#define X_PIN A0
LedControl lc = LedControl(12, 11, 10, 1); //DIN, CLK, LOAD, No. DRIVER
int Table[8][8] =
{
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}
};
int ShowPiece();
void setup()
{pinMode(SW_PIN, INPUT);
  digitalWrite(SW_PIN, HIGH);
  randomSeed(analogRead(0));
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, 2); // sets brightness (0~05 possible values)
  lc.clearDisplay(0);
}
void Display()
{
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
      lc.setLed(0, i, j, Table[i][j]);
}
struct Piece
{
  int X, Y, Type, Shape[3][3];
  /*Types:
   * 0-Point
   * 1-Square
   * 2-Line
   * 3-T
   * 4-L
   * 5-Z
   */
  Initialize(int type)
  {
    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++)
        Shape[i][j] = 0;
    X = -2;
    Y = 3;
    Type = type;
    switch (Type)//Construct the shape dinamically
    {
    case 0:
    {
      Shape[1][1] = 1;
      return;
    }
    case 1:
    {
      Shape[0][0] = Shape[0][1] = Shape[1][0] = Shape[1][1] = 1;
      return;
    }
    case 2:
    {
      Shape[1][0] = Shape[1][1] = Shape[1][2] = 1;
      return;
    }
    case 3:
    {
      Shape[1][0] = Shape[1][1] = Shape[1][2] = Shape[0][1] = 1;
      return;
    }
    case 4:
    {
      Shape[1][0] = Shape[1][1] = Shape[1][2] = Shape[2][2] = 1;
      return;
    }
    case 5:
    {
      Shape[1][0] = Shape[2][1] = Shape[2][2] = Shape[1][1] = 1;
      return;
    }
    }
  }
  int RealX(int i)
  {
    if (i < 3)return 0;
    if (i == 7 || i == 3)return 1;
    return 2;
  }
  int RealY(int i)
  {
    if (i > 1 && i < 5)return 2;
    if (i > 5 || !i)return 0;
    return 1;
  }
  void Rotate()
  {
    int backup[3][3];
    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++)
        backup[i][j] = Shape[i][j];
    if (Type < 2)return;//point or square
    if (Type != 5)//excepting Z
    {
      int buf1 = Shape[0][0];
      int buf2 = Shape[RealX(1)][RealY(1)];
      for (int i = 0; i < 6; i++)//for each point on the edge
      {
        Shape[RealX(i)][RealY(i)] = Shape[RealX(i + 2)][RealY(i + 2)];
      }
      Shape[RealX(7)][RealY(7)] = buf2;
      Shape[RealX(6)][RealY(6)] = buf1;
    }
    else
    {
      int buf = Shape[1][0];
      Shape[1][0] = Shape[0][2];
      Shape[0][2] = buf;
      buf = Shape[1][2];
      Shape[1][2] = Shape[2][2];
      Shape[2][2] = buf;
    }
    if (!CanGoThere(X, Y))
      for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
          Shape[i][j] = backup[i][j];
  }
  bool CanGoThere(int newX, int newY)
  {
    for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        if (Shape[i][j])
        {
          if (i + newX > 0)
          {
            if (i + newX > 7 || j + newY > 7 || j + newY < 0)return 0;
            if (Table[i + newX][j + newY])return 0;
          }
          else
          {
            if (j + newY > 7 || j + newY < 0)return 0;
          }
        }
      }
    }
    return 1;
  }
  bool Paint(bool Clear = false)
  {
    for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        if (X + i > -1 && Shape[i][j])Table[X + i][Y + j] = Clear;
      }
    }
  }
  bool Step(int ChangeCol = 0)
  {
    int NewX = X + (ChangeCol==0), NewY = Y + ChangeCol;
    Paint(false);
    if (CanGoThere(NewX, NewY))
    {
      X = NewX;
      Y = NewY;
      Paint(true);
      Display();
      return true;
    }
    Paint(true);
    return false;
  }
};
void PutDigit(int digit, int coordinates)
{
  //top horizontal edge:
  if (digit != 1 && digit != 4)
  {
    for (int i = 0; i < 3; i++)
      Table[1][i + coordinates] = 1;
  }
  //middle horizontal edge
  if (digit > 1 && digit != 7)
  {
    for (int i = 0; i < 3; i++)
      Table[3][i + coordinates] = 1;
  }
  //bottom horizontal edge
  if (digit != 1 && digit != 4 && digit != 7)
  {
    for (int i = 0; i < 3; i++)
      Table[5][i + coordinates] = 1;
  }
  //top-right vertical edge
  if (digit < 5 || digit>6)
  {
    for (int j = 0; j < 3; j++)
      Table[1 + j][2 + coordinates] = 1;
  }
  //top-left vertical edge
  if (digit > 3 && digit != 7 || !digit)
  {
    for (int j = 0; j < 3; j++)
      Table[1 + j][coordinates] = 1;
  }
  //bottom-right vertical edge
  if (digit != 2)
  {
    for (int j = 0; j < 3; j++)
      Table[3 + j][2 + coordinates] = 1;
  }
  //bottom-left vertical edge
  if (!(digit % 2) && digit != 4)
  {
    for (int j = 0; j < 3; j++)
      Table[3 + j][coordinates] = 1;
  }
}
void EraseAll()
{
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
      Table[i][j] = 0;
}
void PutScore(int score)
{
  EraseAll();
  score = score % 100;
  int digit = score % 10;
  PutDigit(digit, 4);
  digit = (score / 10) % 10;
  PutDigit(digit, 0);
  Display();
  delay(4000);
}
void PutPauseStatus(bool Pause = true)
{
  EraseAll();
  if (Pause)
  {
    for (int i = 0; i < 6; i++)
      Table[i + 1][2] = Table[i + 1][5] = 1;
    Display();
  }
  else
  {
    for (int i = 0; i < 3; i++)
      Table[i][5] = Table[i][5 - i] = 1;
    for (int i = 3; i < 7; i++)
      Table[i][5] = Table[i][i - 1] = 1;
    Display();
  }
}
void BurnFullLines()
{
  int FirstFullLine = 8, LastFullLine = 8;
  for (int i = 7; i > -1; i--)
  {
    bool ThisLineIsFull = true;
    for (int j = 0; j < 8; j++)
      if (Table[i][j] == 0)ThisLineIsFull = false;
    if (ThisLineIsFull)
    {
      FirstFullLine = i;
      LastFullLine = i;
      i--;
      for (; i > -1; i--)
      {
        bool ThisLineIsNotFullAnymore = false;
        for (int j = 0; j < 8; j++)
          if (Table[i][j] == 0)ThisLineIsNotFullAnymore = true;
        if (ThisLineIsNotFullAnymore)
        {
          LastFullLine = i + 1;
          goto FoundTheInterval;
        }
        else if (i == 0)
        {
          LastFullLine = 0;
          goto FoundTheInterval;
        }
      }

    }
  }
  return;
FoundTheInterval:;
  for (int i = LastFullLine - 1; i > -1; i--)
    for (int j = 0; j < 8; j++)
    {
      Table[FirstFullLine + i - LastFullLine + 1][j] = Table[i][j];
      Table[i][j] = 0;
    }
  Display();
}

int Status = -1, Score = 0;
/*
 * -1-first iteration of the next status
 * 0-waiting to start
 * 1-playing and having an active piece
 * 2-just lost the game
 */
long long int LastIterationMoment = 0;
bool JoystickAllreadyLower = false, JoystickAllreadyHigher = false, AllreadyClicked = false;
Piece p;
void loop()
{
  long long int CurrentIterationMoment = millis();
  if (Status <= 0)
  {
    if (Status == -1)
    {
      PutPauseStatus();
      Status = 0;
    }
    int y = digitalRead(SW_PIN);
    if (y==0)
    {
      PutPauseStatus(false);
      delay(100);
      EraseAll();
      Display();
      p.Initialize(random(6));
      Status = 1;
    }
  }
  else if (Status == 1)
  {
    int x_value = analogRead(X_PIN);
    int y = digitalRead(SW_PIN);
    if (y == 0 && !AllreadyClicked)
    {
      delay(50);
      p.Paint();
      p.Rotate();
      p.Paint(true);
      Display();
      AllreadyClicked = true;
    }
    if (y==1)
    {
      AllreadyClicked = false;
    }
    if (x_value < 300 && !JoystickAllreadyLower)
    {
      p.Step(-1);
      JoystickAllreadyLower = true;
    }
    if (x_value > 350)JoystickAllreadyLower = false;
    if (x_value > 700 && !JoystickAllreadyHigher)
    {
      p.Step(1);
      JoystickAllreadyHigher = true;
    }
    if (x_value < 650)JoystickAllreadyHigher = false;
    if (CurrentIterationMoment > LastIterationMoment + 500 - 5 * Score)
    {
      LastIterationMoment = CurrentIterationMoment;
      if (!p.Step())
      {
        BurnFullLines();
        if (Table[0][4] == 1 || Table[0][3] == 1 || Table[0][2] == 1)
          Status = 2;
        else
        {
          p.Initialize(random(6));
          Score++;
        }
      }
    }
  }
  else if (Status == 2)
  {
    PutScore(Score + 1);
    Score = 0;
    Status = -1;
  }
}
