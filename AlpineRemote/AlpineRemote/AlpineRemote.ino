/*
 Name:		AlpineRemote.ino
 Created:	30.01.2020 21:54:53
 Author:	BraciaB
*/

#define SRC 13
#define MUTE 12
#define VOL_UP 11
#define VOL_DN 10
#define BAND 9
#define SCROLL A5
#define SIGNAL 8

int analogCheck;
int analogValue;

int startSignal[] = { 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1 };
int endSignal[] = { 1, 0, 1, 0, 1, 0, 1 };

int volUp[] = { 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1 };
int volDn[] = { 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1 };
int muteBtn[] = { 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1 };
int stUp[] = { 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0 };
int stDn[] = { 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
int sourceBtn[] = { 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0 };
int trkUp[] = { 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1 };
int trkDn[] = { 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1 };
int powerBtn[] = { 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0 };
int entPlay[] = { 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0 };
int bandProg[] = { 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0 };

void setup() 
{
    pinMode(SRC, INPUT_PULLUP);
    pinMode(MUTE, INPUT_PULLUP);
    pinMode(VOL_UP, INPUT_PULLUP);
    pinMode(VOL_DN, INPUT_PULLUP);
    pinMode(BAND, INPUT_PULLUP);
    pinMode(SIGNAL, OUTPUT);
    digitalWrite(SIGNAL, LOW);
    delay(5000);
    Serial.begin(9600);
}

void loop() 
{
    analogCheck = analogRead(SCROLL);
    Serial.println(analogCheck);
    delay(100);
    analogValue = analogRead(SCROLL);
    Serial.println(analogValue);

    /* if (analogValue > analogCheck)
     {
       if ((analogValue - analogCheck) > 3)
       {
         Serial.println("HIGHER");
         delay(100);
         StartSignal();
         StatUp();
         EndSignal();
         delay(100);
       }

     }
     else if (analogValue < analogCheck)
     {
       if ((analogCheck - analogValue) > 3)
       {
         Serial.println("LOWER");
         delay(100);
         StartSignal();
         StatDn();
         EndSignal();
         delay(100);
       }

     }*/

    if (digitalRead(BAND) == LOW)
    {
        StartSignal();
        AlpineSignal(bandProg, "BAND");
        EndSignal();
        delay(100);
    }

    if (digitalRead(VOL_UP) == LOW)
    {
        StartSignal();
        AlpineSignal(volUp, "VOL_UP");
        EndSignal();
        delay(100);
    }
    if (digitalRead(VOL_DN) == LOW)
    {
        StartSignal();
        AlpineSignal(volDn, "VOL_DN");
        EndSignal();
        delay(100);
    }

    if (digitalRead(MUTE) == LOW)
    {
        StartSignal();
        AlpineSignal(muteBtn, "MUTE");
        EndSignal();
        delay(100);
    }

    if (digitalRead(SRC) == LOW)
    {
        StartSignal();
        AlpineSignal(sourceBtn, "SOURCE");
        EndSignal();
        delay(100);
    }
    /*if ((digitalRead(SRC_UP) == LOW) || (digitalRead(SRC_DN) == LOW))
    {
      int held = 0;
      while ((digitalRead(SRC_UP) == LOW) || (digitalRead(SRC_DN) == LOW) && held < 50)
      {
        delay(100);
        held++;
      }
      if (held < 10)
      {
        Serial.println("SOURCE");
        delay(5000);
        StartSignal();
        Source();
        EndSignal();
        delay(500);
      }
      if (held >= 10 && held < 30)
      {
        StartSignal();
        Mute();
        EndSignal();
        delay(500);
      }
      else if (held >= 30)
      {
        StartSignal();
        Power();
        EndSignal();
        delay(500);
      }
    }*/
}

void StartSignal()
{
    digitalWrite(SIGNAL, HIGH);
    delayMicroseconds(8000);
    digitalWrite(SIGNAL, LOW);
    delayMicroseconds(4500);

    for (int i = 0; i < 24; i = i + 1)
    {
        if (startSignal[i] == 1)
        {
            digitalWrite(SIGNAL, HIGH);
            delayMicroseconds(500);
            digitalWrite(SIGNAL, LOW);
            delayMicroseconds(500);
        }
        else if (startSignal[i] == 0)
        {
            digitalWrite(SIGNAL, LOW);
            delayMicroseconds(1000);
        }
    }
}
void EndSignal()
{
    for (int i = 0; i < 7; i = i + 1)
    {
        if (endSignal[i] == 1)
        {
            digitalWrite(SIGNAL, HIGH);
            delayMicroseconds(500);
            digitalWrite(SIGNAL, LOW);
            delayMicroseconds(500);
        }
        else if (endSignal[i] == 0)
        {
            digitalWrite(SIGNAL, LOW);
            delayMicroseconds(1000);
        }
    }
    delay(40);
    delay(100);
}

void AlpineSignal(int signal[], String signalInfo)
{
    Serial.println();
    for (int i = 0; i < 17; i = i + 1)
    {
        if (signal[i] == 1)
        {
            digitalWrite(SIGNAL, HIGH);
            delayMicroseconds(500);
            digitalWrite(SIGNAL, LOW);
            delayMicroseconds(500);
        }
        else if (signal[i] == 0)
        {
            digitalWrite(SIGNAL, LOW);
            delayMicroseconds(1000);
        }
        Serial.print(signal[i]);
    }
    Serial.println(signalInfo);
}