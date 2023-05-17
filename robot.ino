#include <Servo.h>

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

volatile long int encodeur_1 = 0;
float distance_a;
float distance_p = 0;

Servo rouedroite;  // create servo object to control a servo
Servo rouegauche;  // create servo object to control a servo

typedef enum Etat {
  INIT,
  CORECTION_G,
  CORECTION_D,
  MUR_DV,
  SCAN,
  TELECOMANDE
};

void Deplacement(int mot_G, int mot_D) {
  rouedroite.write(mot_D);
  rouegauche.write(mot_G);
}

void Capt_son() {
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

  if (tmpD > 0) {
    dist_mur_D = tmpD;
  }
  if (tmpA > 0) {
    dist_mur_A = tmpA;
  }
}

void MAE() {
  static Etat etat_p = INIT;
  static Etat etat_s;

  int data;
  static int val = 0;

  // Calcule x et y du robot
  if (val == 0) {
    y = y + distance_a - distance_p;
  } else if (val == 1) {
    x = x - (distance_a - distance_p);
  } else if (val == 2) {
    y = y - (distance_a - distance_p);
  } else {
    x = x + distance_a - distance_p;
  }

  // envoie x et y par bluetuth
  int valx = map(x, -10000, 10000, 0, 180);
  int valy = map(y, -10000, 10000, 180, 260);
  Serial1.write(valx);
  Serial1.write(valy);

  // lecture val bluethue
  if (Serial1.available()) {
    data = Serial1.read();
  }

  // force le changement de mode (entre telecomande et auto)
  if (data == 250) {
    if (etat_p == TELECOMANDE) {
      etat_p = INIT;
    } else {
      etat_p = TELECOMANDE;
    }
  }

  // force le passage en mode evitement du mur en face
  if (dist_mur_A <= DIST_MAX_A && etat_p != INIT) {
    etat_p = MUR_DV;
    etat_s = MUR_DV;
    distance_a = 0;
    encodeur_1 = 0;
    distance_p = distance_a;
  }


// debut MAE
  switch (etat_p) {
    case INIT:
      {
        if (dist_mur_D <= DIST_MAX_D) {
          etat_s = SCAN;
          distance_a = 0;
          distance_p = 0;
          x = 0;
          y = 0;
          encodeur_1 = 0;
        }
        Deplacement(90, 90);
        Serial.println("ETAT init");
        distance_p = distance_a;
      }
      break;
    case SCAN:
      {
        if (dist_mur_D <= DIST_MAX_D - 3) {  // Si rob se rapproche du mur
          etat_s = CORECTION_D;
        } else if (dist_mur_D >= DIST_MAX_D + 3) {  // Si rob s'eloigne du mur
          etat_s = CORECTION_G;
        }
        Deplacement(102, 101);
        Serial.println("ETAT SCAN");
        distance_p = distance_a;
      }
      break;
    case CORECTION_D:
      {
        if (dist_mur_D >= DIST_MAX_D) {
          etat_s = SCAN;
        }
        Deplacement(99, 103);
        Serial.println("ETAT CORRECTION D");
        distance_p = distance_a;
      }
      break;
    case CORECTION_G:
      {
        if (dist_mur_D <= DIST_MAX_D) {
          etat_s = SCAN;
        }
        Deplacement(104, 100);
        Serial.println("ETAT CORRECTION G");
        distance_p = distance_a;
      }
      break;
    case MUR_DV:
      {
        if (distance_a >= PERIMETRE_ROUE / 5) {
          Deplacement(90, 90);
          delay(500);
          distance_a = 0;
          encodeur_1 = 0;
          distance_p = distance_a;

          etat_s = SCAN;
          if (val >= 3) {
            val = 0;
          } else {
            val++;
          }
        }
        Deplacement(80, 103);
        Serial.println("ETAT 4");
      }
      break;
    case TELECOMANDE:
      {
        if (data <= 180) {
          vitesse = map(data, 180, 0, 120, 60);
        } else {
          diff = data - 200;  //-10
        }
        if (vitesse && diff) {
          Deplacement(vitesse,vitesse + diff);
        }
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

void setup() {

  Serial.begin(9600);

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

  rouedroite.attach(PIN_ROUE_D);  // attaches the servo on pin 9 to the servo object
  rouegauche.attach(PIN_ROUE_G);  // attaches the servo on pin 10 to the servo object

  attachInterrupt(digitalPinToInterrupt(PIN_ENCODEUR_VERT), readEncoder, RISING);
}
void readEncoder() {

  if (digitalRead(PIN_ENCODEUR_JAUNE) == digitalRead(PIN_ENCODEUR_VERT)) {
    encodeur_1++;
  } else {
    encodeur_1--;
  }
}

void loop() {
  distance_a = abs((encodeur_1 / TOUR_MOTEUR) * PERIMETRE_ROUE);
  Capt_son();
  MAE();
}
