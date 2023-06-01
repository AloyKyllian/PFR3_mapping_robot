#include <Servo.h>
#include <RPLidar.h>

const int DIST_MAX_D = 20;
const int DIST_MAX_A = 25;

const float TOUR_MOTEUR = 2970.0;
const float PERIMETRE_ROUE = 36.13;

const int PIN_TRIG_SON_D = 46;
const int PIN_ECHO_SON_D = 48;
const int PIN_TRIG_SON_A = 32;
const int PIN_ECHO_SON_A = 34;
const int PIN_ROUE_D = 9;
const int PIN_ROUE_G = 10;
const int PIN_ENCODEUR_VERT = 18;
const int PIN_ENCODEUR_JAUNE = 19;

int dist_mur_D = 99;
int dist_mur_A = 99;
int vitesse = 90;
int diff = 0;

float x = 0;
float y = 0;
int murs = 150;
int maxx;
int maxy;
int maxy2;

volatile long int encodeur_1 = 0;
float distance_a;
float distance_p = 0;

Servo rouedroite; // create servo object to control a servo
Servo rouegauche; // create servo object to control a servo

// You need to create an driver instance
RPLidar lidar;
#define RPLIDAR_MOTOR 2 // The PWM pin for control the speed of RPLIDAR's motor. \
                         // This pin should connected with the RPLIDAR's MOTOCTRL signal \
                         //
int ytemp = 10000;
int xtemp = 10000;
int count = 0;

typedef enum Etat
{
  INIT,
  CORECTION_G,
  CORECTION_D,
  MUR_DV,
  SCAN,
  TELECOMANDE
};

void Deplacement(int mot_G, int mot_D)
{
  rouedroite.write(mot_D);
  rouegauche.write(mot_G);
}

void Capt_son()
{
  int tmpD;
  int tmpA;

  digitalWrite(PIN_TRIG_SON_D, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG_SON_D, LOW);
  tmpD = pulseIn(PIN_ECHO_SON_D, HIGH);

  digitalWrite(PIN_TRIG_SON_A, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG_SON_A, LOW);
  tmpA = pulseIn(PIN_ECHO_SON_A, HIGH);

  tmpD = tmpD / 58;
  tmpA = tmpA / 58;

  if (tmpD > 0)
  {
    dist_mur_D = tmpD;
  }
  if (tmpA > 0)
  {
    dist_mur_A = tmpA;
  }
}

void MAE()
{
  static Etat etat_p = INIT;
  static Etat etat_s;

  int data = 0;
  static int val = 0;

  // Calcule x et y du robot

  if (etat_p != MUR_DV)
  {
    if (val == 0)
    {
      y = y + (distance_a - distance_p);
    }
    else if (val == 1)
    {
      x = x - (distance_a - distance_p);
    }
    else if (val == 2)
    {
      y = y - (distance_a - distance_p);
    }
    else
    {
      x = x + distance_a - distance_p;
    }
  }
  envoie_donnee(val);

  // lecture val bluethue
  if (Serial3.available())
  {
    data = Serial3.read();
  }
  // force le changement de mode (entre telecomande et auto)
  if (data == 250)
  {
    if (etat_p == TELECOMANDE)
    {
      etat_p = INIT;
      etat_s = etat_p;
    }
    else
    {
      etat_p = TELECOMANDE;
      etat_s = etat_p;
    }
  }

  // force le passage en mode evitement du mur en face
  if (dist_mur_A <= DIST_MAX_A && etat_p != INIT && etat_p != TELECOMANDE)
  {
    etat_p = MUR_DV;
    etat_s = MUR_DV;
    distance_a = 0;
    encodeur_1 = 0;
    distance_p = distance_a;
  }

  // debut MAE
  switch (etat_p)
  {
  case INIT:
  {
    if (dist_mur_D <= DIST_MAX_D)
    {
      etat_s = SCAN;
      distance_a = 0;
      distance_p = 0;
      x = 0;
      y = 0;
      encodeur_1 = 0;
    }
    Deplacement(90, 90);
    // Serial.println("ETAT init");
    distance_p = distance_a;
  }
  break;
  case SCAN:
  {
    if (dist_mur_D <= DIST_MAX_D - 3)
    { // Si rob se rapproche du mur
      etat_s = CORECTION_D;
    }
    else if (dist_mur_D >= DIST_MAX_D + 3)
    { // Si rob s'eloigne du mur
      etat_s = CORECTION_G;
    }
    Deplacement(102, 101);
    // Serial.println("ETAT SCAN");
    distance_p = distance_a;
  }
  break;
  case CORECTION_D:
  {
    if (dist_mur_D >= DIST_MAX_D)
    {
      etat_s = SCAN;
    }
    Deplacement(99, 103);
    // Serial.println("ETAT CORRECTION D");
    distance_p = distance_a;
  }
  break;
  case CORECTION_G:
  {
    if (dist_mur_D <= DIST_MAX_D)
    {
      etat_s = SCAN;
    }
    Deplacement(104, 100);
    // Serial.println("ETAT CORRECTION G");
    distance_p = distance_a;
  }
  break;
  case MUR_DV:
  {
    // Serial.println(distance_a);
    if (distance_a >= PERIMETRE_ROUE / 5)
    {
      Deplacement(90, 90);
      delay(500);
      distance_a = 0;
      encodeur_1 = 0;
      distance_p = distance_a;

      etat_s = SCAN;
      if (val >= 3)
      {
        val = 0;
      }
      else
      {
        val++;
      }
    }
    Deplacement(80, 103);
    // Serial.println("ETAT 4");
  }
  break;
  case TELECOMANDE:
  {
    if (data <= 130)
    {
      vitesse = data;
    }
    else
    {
      diff = data - 200; //-10
    }
    if (vitesse > 90)
    {
      if (diff != 0)
      {
        Deplacement(vitesse + diff, vitesse - diff);
      }
      else
      {
        Deplacement(vitesse, vitesse);
      }
    }
    else if (vitesse < 90)
    {
      if (diff != 0)
      {
        Deplacement(vitesse - diff, vitesse + diff);
      }
      else
      {
        Deplacement(vitesse, vitesse);
      }
    }

    Serial.println("ETAT TELECOMANDE");
  }
  break;

  default:
  {
    etat_s = INIT;
  }
  break;
  }

  etat_p = etat_s;
}

void envoie_donnee(int val)
{

  int valmapX;
  int valmapY;

  if (val == 0 || val == 2)
  {
    valmapY = map(y, -180, 180, 21, 61);
    if (val == 0)
    {

      Serial.println("val 0");
      valmapX = map((int)(murs / 10), 0, 40, 41, 61);
      maxy = valmapY;
    }
    else if (val == 2)
    {
      Serial.println("val 2");
      valmapX = map((int)(murs / 10), 40, 0, 1, maxx);
      maxy2 = valmapY;
    }

    Serial.print("murs : ");
    Serial.print(murs);
    Serial.print("\t valmapX0 : ");
    Serial.print(valmapX);
    Serial.print("\t valmapY0 : ");
    Serial.println(valmapY);
    Serial3.write(valmapX);
    Serial3.write(valmapY + 70);
  }

  else if (val == 1 || val == 3)
  {
    valmapX = map(x, -180, 0, 21, 41);
    if (val == 1)
    {
      maxx = valmapX;
      Serial.println("val 1");
      valmapY = map((int)(murs / 10), 0, 40, maxy, 81);
    }
    else if (val == 3)
    {
      Serial.println("val 3");
      valmapY = map((int)(murs / 10), 40, 0, 1, maxy2);
    }
    Serial.print("murs : ");
    Serial.print(murs);
    Serial.print("\t valmapX0 : ");
    Serial.print(valmapX);
    Serial.print("\t valmapY0 : ");
    Serial.println(valmapY);
    Serial3.write(valmapX);
    Serial3.write(valmapY + 70);
  }
}

void RPLidar()
{
  if (IS_OK(lidar.waitPoint()))
  {
    float distance = lidar.getCurrentPoint().distance; // distance value in mm unit
    float angle = lidar.getCurrentPoint().angle;       // anglue value in degree
    if ((int)angle == 90)
    {
      int mur = distance * sin(radians(angle));
      if (mur < 1000)
        murs = mur;
      Serial.println("______________________________________________________");
    }
  }
  else
  {
    analogWrite(RPLIDAR_MOTOR, 0); // stop the rplidar motor
    // try to detect RPLIDAR...
    rplidar_response_device_info_t info;
    if (IS_OK(lidar.getDeviceInfo(info, 100)))
    {

      // detected...
      lidar.startScan();
      // start motor rotating at max allowed speed
      analogWrite(RPLIDAR_MOTOR, 255);
      delay(1000);
    }
  }
}

void setup()
{
  Serial.begin(9600);
  Serial3.begin(9600);
  Serial2.begin(115200);
  // Serial.println("HELLOOOOO");

  // capt ultrason
  pinMode(PIN_TRIG_SON_D, OUTPUT);
  pinMode(PIN_ECHO_SON_D, INPUT);
  pinMode(PIN_TRIG_SON_A, OUTPUT);
  pinMode(PIN_ECHO_SON_A, INPUT);

  // encodeur
  pinMode(PIN_ENCODEUR_JAUNE, INPUT);
  pinMode(PIN_ENCODEUR_VERT, INPUT);

  digitalWrite(PIN_TRIG_SON_D, LOW);
  digitalWrite(PIN_TRIG_SON_A, LOW);

  rouedroite.attach(PIN_ROUE_D); // attaches the servo on pin 9 to the servo object
  rouegauche.attach(PIN_ROUE_G); // attaches the servo on pin 10 to the servo object

  attachInterrupt(digitalPinToInterrupt(PIN_ENCODEUR_VERT), readEncoder, RISING);
  // bind the RPLIDAR driver to the arduino hardware serial
  lidar.begin(Serial2);
  // set pin modes
  pinMode(RPLIDAR_MOTOR, OUTPUT);
}
void readEncoder()
{

  if (digitalRead(PIN_ENCODEUR_JAUNE) == digitalRead(PIN_ENCODEUR_VERT))
  {
    encodeur_1++;
  }
  else
  {
    encodeur_1--;
  }
}

void loop()
{
  // Serial.println(encodeur_1);
  distance_a = abs((encodeur_1 / TOUR_MOTEUR) * PERIMETRE_ROUE);
  Capt_son();
  MAE();
  RPLidar();
}