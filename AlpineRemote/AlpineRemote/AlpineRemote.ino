/*
 Name:		AlpineRemote.ino
 Created:	30.01.2020 21:54:53
 Author:	BraciaB
*/

#pragma region Po³¹czenia

//  Wtyczka         Pin     Funkcja             Atmega
#define RED 2       //  Br¹zowy         24      Masa 1              GND(8,22)
#define BLACK 3     //  Pomarañczowy    26      Masa 2              GND(8,22)

#define BLUE 5      //  Bia³y           29      Mute/VolUp          11
#define YELLOW 6    //  Czarny          27      VolDn/SrcDn         12
#define GREEN 7     //  Kremowy         25      Band/SrcUp          13
#define BROWN A0    //  Fioletowy       28      Rolka               23

#define SIGNAL 8    //  Radio Remote    REM     Sygna³ do radia     14

#pragma endregion

#pragma region Przyciski

                        // Masa     Odpowiednik     Pin     Uwagi
int Mute = BLUE;        // Black-   Pomarañczowy    26      U nas nie ma osobnego przycisku wiêc wywo³uje zwieraj¹c VolUp i VolDn w tym samym czasie do czerwonej masy
int VolDn = YELLOW;     // Red-     Br¹zowy         24
int VolUp = BLUE;       // Red-     Br¹zowy         24
int Band = GREEN;       // Red-     Br¹zowy         24
int SrcDn = YELLOW;     // Black-   Pomarañczowy    26      Dodatkowo zaprogramuje tu w³¹czanie i wy³¹czanie radia podobnie jak Mute
int SrcUp = GREEN;      // Black-   Pomarañczowy    26

#pragma endregion

#pragma region Sygna³y

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

#pragma endregion

#pragma region Zmienne

int analogCheck;
int analogValue;

unsigned long lastCheck;

bool redFlag = false;
bool blackFlag = false;

#pragma endregion

void setup()
{
    pinMode(RED, INPUT);
    pinMode(BLACK, INPUT);
    pinMode(BLUE, INPUT_PULLUP);
    pinMode(YELLOW, INPUT_PULLUP);
    pinMode(GREEN, INPUT_PULLUP);

    pinMode(SIGNAL, OUTPUT);
    digitalWrite(SIGNAL, LOW);

    Serial.begin(9600);
    Serial.println("STARTING UP");
    delay(500);
    lastCheck = millis();
    attachInterrupt(digitalPinToInterrupt(RED), redISR, RISING);
    attachInterrupt(digitalPinToInterrupt(BLACK), blackISR, RISING);
    Serial.println("READY");
}

void loop()
{
    /*CheckAnalog();*/
    CheckRed();
    CheckBlack();
}

#pragma region Funkcje sygna³ów

void StartSignal() // Sygna³ startowy transmisji
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
void EndSignal() // Sygna³ koñcowy transmisji
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

void AlpineSignal(int signal[], String signalInfo) // Sygna³ w³aœciwy transmisji. Polecenie odczytywane jest ze stanów niskich i wysokich podanych w przekazanej tablicy. Do celów testowych nazwa sygna³u podawana jest na monitor portu szeregowego.
{
    Serial.println();

    StartSignal();

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
    }

    EndSignal();

    Serial.println(signalInfo);
}

#pragma endregion


#pragma region Funkcje przerwañ i przycisków

void redISR()
{
    redFlag = true;
}

void CheckRed() // Przerwanie na czerwonej masie
{
    if (redFlag == true)
    {
        RedFlag();
    }
}

void RedFlag()
{
    if (digitalRead(Band) == LOW) // Zmiana pasma
    {
        AlpineSignal(bandProg, "BAND");
        delay(100);
        redFlag = false;
    }

    if (digitalRead(VolDn) == LOW && digitalRead(VolUp) == LOW) // Wyciszanie poprzez przytrzymanie obu klawiszy g³oœnoœci - gdy brak dodatkowego przycisku
    {
        delay(500);

        if (digitalRead(VolDn) == LOW && digitalRead(VolUp) == LOW)
        {
            AlpineSignal(muteBtn, "MUTE");
            delay(100);
            redFlag = false;
        }
        redFlag = false;        
    }

    if (digitalRead(VolUp) == LOW && digitalRead(VolDn) == HIGH) // Podg³aszanie
    {
        delay(500);

        if (digitalRead(VolDn) == LOW && digitalRead(VolUp) == LOW)
        {
            AlpineSignal(muteBtn, "MUTE");
            delay(100);
            redFlag = false;
        }
        else
        {
            AlpineSignal(volUp, "VOL_UP");
            delay(100);
            redFlag = false;
        }
        redFlag = false;
    }

    if (digitalRead(VolDn) == LOW && digitalRead(VolUp) == HIGH) // Przyciszanie
    {
        delay(500);

        if (digitalRead(VolDn) == LOW && digitalRead(VolUp) == LOW)
        {
            AlpineSignal(muteBtn, "MUTE");
            delay(100);
            redFlag = false;
        }
        else
        {
            AlpineSignal(volDn, "VOL_DN");
            delay(100);
            redFlag = false;
        }
        redFlag = false;
    }
}

void blackISR()
{
    blackFlag = true;
}

void CheckBlack() // Przerwanie na czarnej masie
{
    if (blackFlag == true)
    {
        BlackFlag();
    }
}
void BlackFlag()
{
    if (digitalRead(SrcUp) != digitalRead(SrcDn))
    {
        delay(1000);

        if (digitalRead(SrcUp) != digitalRead(SrcDn)) // Naciœnij jeden d³u¿ej aby zapauzowaæ
        {
            AlpineSignal(entPlay, "PLAY/PAUSE");
            delay(100);
            blackFlag = false;
        }

        else if (digitalRead(SrcUp) == HIGH && digitalRead(SrcDn) == HIGH) // Naciœnij jeden krótko aby zmieniæ Ÿród³o
        {
            AlpineSignal(sourceBtn, "SOURCE");
            delay(100);
            blackFlag = false;
        }
    }

    if (digitalRead(SrcUp) == LOW && digitalRead(SrcDn) == LOW) // Przytrzymaj oba aby w³¹czyæ/wy³¹czyæ
    {
        delay(5000);

        if (digitalRead(SrcUp) == LOW && digitalRead(SrcDn) == LOW)
        {
            AlpineSignal(powerBtn, "POWER");
            delay(100);
            blackFlag = false;
        }
        blackFlag = false;
    }

    //if (digitalRead(Mute) == LOW) // Jeœli jest osobny przycisk do wyciszania
    //{
    //    AlpineSignal(muteBtn, "MUTE");
    //    delay(100);
    //}
}

#pragma endregion

#pragma region Rolka

void CheckAnalog() // Obs³uga rolki
{
    analogCheck = analogRead(BROWN);
    Serial.print("AnalogCheck: ");
    Serial.println(analogCheck);

    if (millis() - lastCheck >= 150)
    {
        analogValue = analogRead(BROWN);
        Serial.println("AnalogValue: ");
        Serial.println(analogValue);

        if (analogValue > analogCheck)
        {
            if ((analogValue - analogCheck) > 3) // Zmiana stacji lub utworu zale¿nie od trybu
            {
                AlpineSignal(stUp, "ST_UP");
                delay(100);

                AlpineSignal(trkUp, "TRK_UP");
                delay(100);
            }
        }

        if (analogValue < analogCheck)
        {
            if ((analogValue - analogCheck) > 3) // Zmiana stacji lub utworu zale¿nie od trybu
            {
                AlpineSignal(stDn, "ST_DN");
                delay(100);

                AlpineSignal(trkDn, "TRK_DN");
                delay(100);
            }
        }
    }
    lastCheck = millis();
}

#pragma endregion

// Przerwania zdaj¹ siê reagowaæ. Przytrzymanie przycisku przez pewien czas te¿. Nale¿y ustawiæ czasy przytrzymania (w szczególnoœci mute) i przetestowaæ na radiu, a potem przenieœæ projekt na atmegê. Wczeœniej potestowaæ z rolk¹.