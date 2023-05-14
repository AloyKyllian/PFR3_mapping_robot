/*
  Controlling a servo position using a potentiometer (variable resistor)
  by Michal Rinott http://people.interaction-ivrea.it/m.rinott

  modified on 8 Nov 2013
  by Scott Fitzgerald
  http://www.arduino.cc/en/Tutorial/Knob
  /

  /
   genie mais pas drole

   si on eteint avec l'interrupteur du robot la valeur actuelle envoyer en pwm devient la valeur pour laquelle les roues s'arrete

*/

#include <Servo.h>

const int DIST_MAX_D = 15;
const int DIST_MAX_A = 20;

const int PIN_TRIG_SON_D = 12;
const int PIN_ECHO_SON_D = 11;
const int PIN_TRIG_SON_A = 7;
const int PIN_ECHO_SON_A = 6;
const int VITESSE_NOMINALE_AVANT = 90 - 15;
const int VITESSE_NOMINALE_ARRIERE = 90 + 15;

int dist_mur_D = 99;
int dist_mur_A = 99;

typedef enum Etat
{
  INIT,
  CORECTION_G,
  CORECTION_D,
  SCAN,
  FIN
};

int vitesse = 0;
int diff = 0;

Servo rouedroite; // create servo object to control a servo
Servo rouegauche; // create servo object to control a servo

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

  if (tmpD >= 0)
  {
    dist_mur_D = tmpD;
  }
  if (tmpA >= 0)
  {
    dist_mur_A = tmpA;
  }
}

void MAE_Deplacement()
{
  static Etat etat_p = INIT;
  static Etat etat_s;

  switch (etat_p)
  {
  case INIT:
  {
    if (dist_mur_D <= DIST_MAX_D)
    {
      etat_s = SCAN;
    }
    Deplacement(90, 90);
  }
  break;
  case SCAN:
  {
    if ((dist_mur_D < DIST_MAX_D) || (dist_mur_A < DIST_MAX_A))
    { // Si rob se rapproche du mur
      etat_s = CORECTION_G;
    }
    else if (dist_mur_D > DIST_MAX_D)
    { // Si rob s'eloigne du mur
      etat_s = CORECTION_D;
    }
    Deplacement(VITESSE_NOMINALE_AVANT, VITESSE_NOMINALE_AVANT);
  }
  break;
  case CORECTION_D:
  {
    if (dist_mur_D <= DIST_MAX_D)
    {
      etat_s = SCAN;
    }
    if (dist_mur_A < DIST_MAX_A)
    {
      etat_s = CORECTION_G;
    }
    Deplacement(VITESSE_NOMINALE_AVANT - 5, VITESSE_NOMINALE_AVANT);
  }
  break;
  case CORECTION_G:
  {
    if (dist_mur_D >= DIST_MAX_D && dist_mur_A > DIST_MAX_A)
    {
      etat_s = SCAN;
    }
    Deplacement(VITESSE_NOMINALE_AVANT, VITESSE_NOMINALE_AVANT - 5);
  }
  break;
  case FIN:
  {
    Deplacement(90, 90);
  }
  break;

  default:
  {
    etat_s = INIT;
  }
  break;
  }

  etat_p = etat_s;
  Serial.print("Etat = ");
  Serial.println(etat_p);
  Serial.print("Distance en cm DROITE: ");
  Serial.println(dist_mur_D);
  Serial.print("Distance en cm AVANT: ");
  Serial.println(dist_mur_A);
  delay(1000);
}

void setup()
{
  Serial1.begin(9600);
  pinMode(PIN_TRIG_SON_D, OUTPUT);
  pinMode(PIN_TRIG_SON_A, OUTPUT);

  digitalWrite(PIN_TRIG_SON_D, LOW);
  digitalWrite(PIN_TRIG_SON_A, LOW);

  pinMode(PIN_ECHO_SON_D, INPUT);
  pinMode(PIN_ECHO_SON_A, INPUT);

  Serial.begin(9600);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  rouedroite.attach(9);  // attaches the servo on pin 9 to the servo object
  rouegauche.attach(10); // attaches the servo on pin 9 to the servo object
}

void loop()
{

  Capt_son();
  MAE_Deplacement();
  // lecture bluetooth
  if (Serial1.available())
  {
    int data = Serial1.read();
    // Serial.println(data);
  }
  // 60
  // 190

  // mode manuel
  if (data <= 130)
  {
    vitesse = data;
  }
  else
  {
    diff = data - 200; //-10
  }

  if (vitesse && diff)
  {
    rouedroite.write(vitesse + diff); // 60-10=50
    rouegauche.write(vitesse);        // 60
  }

  // Deplacement(VITESSE_NOMINALE_AVANT,VITESSE_NOMINALE_AVANT); // Si arret d'urgence
}