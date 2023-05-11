
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

SoftwareSerial HC05(12, 13);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Constants
#define NUM_JOY 2
#define MIN_VAL 0
#define MAX_VAL 1023

// Parameters
const int joyPin[2] = {A0, A1};
const int joyBtn = 2;
const int joyOffset = 0;

// Variables
int joyVal[NUM_JOY] = {0, 0};

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);

    HC05.begin(38400); // initialisation du module bluetooth

    display.clearDisplay();                   // clear
    display.drawPixel(10, 10, SSD1306_WHITE); // dessiner un point blanc
    display.display();                        // write

    for (int i = 0; i < NUM_JOY; i++)
        pinMode(joyPin[i], INPUT);
    pinMode(joyBtn, INPUT_PULLUP);
}

void loop()
{
    String message;
    while (HC05.available())
    {
        delay(3);
        char c = HC05.read();
        message += c;
    }

    // test bluetooth

    if (message.length() > 0)
    {
        int x = 0, y = 0;
        int pos = message.indexOf('/');
        String subx = message.substring(0, pos);
        String suby = message.substring(pos + 1, message.length());
        x = subx.toInt();
        y = suby.toInt();

        display.drawPixel(x, y, SSD1306_WHITE); // dessiner un point blanc
        display.display();                      // write
        // Serial.println(message);
    }

    readJoystick();
}

float joyRawToPhys(int raw)
{ /* function joyRawToPhys */
    ////Joystick conversion rule
    float phys = map(raw, MIN_VAL, MAX_VAL, -100 + joyOffset, 100 + joyOffset) - joyOffset;
    return phys;
}

void readJoystick()
{ /* function readJoystick */
    ////Test routine for Joystick
    for (int i = 0; i < NUM_JOY; i++)
    {
        joyVal[i] = analogRead(joyPin[i]);
        Serial.print(F("joy"));
        Serial.print(i);
        Serial.print(F(" : "));
        Serial.println(joyVal[i]);
    }
    for (int i = 0; i < NUM_JOY; i++)
    {
        Serial.print(F("Conv")), Serial.print(i);
        Serial.print(F(" : "));
        Serial.println(joyRawToPhys(joyVal[i]));
    }
    if (!digitalRead(joyBtn))
    {
        Serial.println(F("Joy Button pressed"));
    }
}
