/*
  Controlling a servo position using a potentiometer (variable resistor)
  by Michal Rinott <http://people.interaction-ivrea.it/m.rinott>

  modified on 8 Nov 2013
  by Scott Fitzgerald
  http://www.arduino.cc/en/Tutorial/Knob
*/

/*
 * genie mais pas drole
 *
 * si on eteint avec l'interrupteur du robot la valeur actuelle envoyer en pwm devient la valeur pour laquelle les roues s'arrete
 *
 */

#include <Servo.h>

Servo rouedroite; // create servo object to control a servo
Servo rouegauche; // create servo object to control a servo

void setup()
{
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
  Serial.println(digitalRead(2));
  rouedroite.write(90); // sets the servo position according to the scaled value
  rouegauche.write(90); // sets the servo position according to the scaled value
}