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

#pragma endregion

void setup() 
{
    pinMode(RED, INPUT_PULLUP);
    pinMode(BLACK, INPUT_PULLUP);
    pinMode(BLUE, INPUT_PULLUP);
    pinMode(YELLOW, INPUT_PULLUP);
    pinMode(GREEN, INPUT_PULLUP);

    pinMode(SIGNAL, OUTPUT);
    digitalWrite(SIGNAL, LOW);

    attachInterrupt(digitalPinToInterrupt(RED), RedInterrupt, FALLING);
    attachInterrupt(digitalPinToInterrupt(BLACK), BlackInterrupt, FALLING);

    delay(5000);
    lastCheck = millis();
    Serial.begin(9600);
}

void loop() 
{
    CheckAnalog();    
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

#pragma endregion


#pragma region Funkcje przerwañ

void RedInterrupt() // Przerwanie na czerwonej masie
{
    delay(150);

    if (digitalRead(Band) == LOW) // Zmiana pasma
    {
        StartSignal();
        AlpineSignal(bandProg, "BAND");
        EndSignal();
        delay(100);
    }

    if (digitalRead(VolUp) == LOW && digitalRead(VolDn) == HIGH) // Podg³aszanie
    {
        StartSignal();
        AlpineSignal(volUp, "VOL_UP");
        EndSignal();
        delay(100);
    }
    if (digitalRead(VolDn) == LOW && digitalRead(VolUp) == HIGH) // Przyciszanie
    {
        StartSignal();
        AlpineSignal(volDn, "VOL_DN");
        EndSignal();
        delay(100);
    }

    if (digitalRead(VolDn) == LOW && digitalRead(VolUp) == LOW) // Wyciszanie poprzez przytrzymanie obu klawiszy g³oœnoœci - gdy brak dodatkowego przycisku
    {
        StartSignal();
        AlpineSignal(muteBtn, "MUTE");
        EndSignal();
        delay(100);
    }
}

void BlackInterrupt() // Przerwanie na czarnej masie
{
    delay(150);

    if (digitalRead(Mute) == LOW) // Jeœli jest osobny przycisk do wyciszania
    {
        StartSignal();
        AlpineSignal(muteBtn, "MUTE");
        EndSignal();
        delay(100);
    }

    if (digitalRead(SrcUp) == LOW && digitalRead(SrcDn) == LOW) // Przytrzymaj oba aby w³¹czyæ/wy³¹czyæ
    {
        StartSignal();
        AlpineSignal(powerBtn, "POWER");
        EndSignal();
        delay(100);
    }

    if (digitalRead(SrcUp) != digitalRead(SrcDn))
    {
        unsigned long hold = millis();

        if (millis() - hold >= 150)
        {
            if (digitalRead(SrcUp) == HIGH && digitalRead(SrcDn) == HIGH) // Naciœnij jeden krótko aby zmieniæ Ÿród³o
            {
                StartSignal();
                AlpineSignal(sourceBtn, "SOURCE");
                EndSignal();
                delay(100);
            }
            else if (digitalRead(SrcUp) != digitalRead(SrcDn)) // Przytrzymaj jeden aby zapauzowaæ/odtworzyæ
            {
                StartSignal();
                AlpineSignal(entPlay, "PLAY/PAUSE");
                EndSignal();
                delay(100);
            }
        }
    }
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
                StartSignal();
                AlpineSignal(stUp, "ST_UP");
                EndSignal();
                delay(100);

                StartSignal();
                AlpineSignal(trkUp, "TRK_UP");
                EndSignal();
                delay(100);
            }
        }

        if (analogValue < analogCheck)
        {
            if ((analogValue - analogCheck) > 3) // Zmiana stacji lub utworu zale¿nie od trybu
            {
                StartSignal();
                AlpineSignal(stDn, "ST_DN");
                EndSignal();
                delay(100);

                StartSignal();
                AlpineSignal(trkDn, "TRK_DN");
                EndSignal();
                delay(100);
            }
        }
    }
    lastCheck = millis();
}

#pragma endregion