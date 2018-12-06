#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <MeMCore.h>

MeDCMotor motor_9(9);
MeDCMotor motor_10(10);
const int TOUT_DROIT = 1;
const int GAUCHE = 2;
const int DROITE = 3;
const int MARCHE_ARRIERE = 4;
const int STOP = 5;
const int PRESENCE_BAS = 100;
const int PRESENCE_HAUT = 101;

unsigned long startMillis;
unsigned long startMillisDir;
unsigned long currentMillis;

void move(int direction, int speed)
{
      int leftSpeed = 0;
      int rightSpeed = 0;
      if(direction == 1){
          leftSpeed = speed;
          rightSpeed = speed;
      }else if(direction == 2){
          leftSpeed = -speed;
          rightSpeed = -speed;
      }else if(direction == 3){
          leftSpeed = -speed;
          rightSpeed = speed;
      }else if(direction == 4){
          leftSpeed = speed;
          rightSpeed = -speed;
      }
      motor_9.run((9)==M1?-(leftSpeed):(leftSpeed));
      motor_10.run((10)==M1?-(rightSpeed):(rightSpeed));
}

double angle_rad = PI/180.0;
double angle_deg = 180.0/PI;
MeUltrasonicSensor ultrasonic_3(3);
MeUltrasonicSensor ultrasonic_1(1);
MeRGBLed rgbled_7(7, 7==7?2:4);
MeBuzzer buzzer;



void setup(){
   startMillis=millis();
   startMillisDir=millis();

}



 void loop(){
    currentMillis = millis();
    avancer();
    ledflash();
    _loop();
}

void _delay(float seconds){
    long endTime = millis() + seconds * 1000;
    while(millis() < endTime)_loop();
}

void _loop(){

}

boolean presence(int distance){

  boolean testPresence = false;
  if ( (ultrasonic_1.distanceCm()) <   distance || (ultrasonic_3.distanceCm()) <  distance){
    testPresence = true;
  }
  return testPresence;
  }

//Renvoie direction a prendre sous forme de distanceActivation
// Si presence quelqun a moins de distancePresence sur un seul capteur, aller du cote du capteur
// si presence sur les deux capteurs, aller tout droit
// si presence quelqun a moins de distance activation, aller en marche arriere
int testDirection(){
  int direction;
  if (currentMillis - startMillis >= 100){
    if (presence(PRESENCE_BAS)){
      if ( (ultrasonic_1.distanceCm() <   PRESENCE_BAS) && !(ultrasonic_3.distanceCm() <  PRESENCE_BAS)){
        direction = GAUCHE;
      }
      else if (!(ultrasonic_1.distanceCm() <   PRESENCE_BAS) && (ultrasonic_3.distanceCm() <  PRESENCE_BAS)) {
        direction = DROITE;
      }
      else {direction = TOUT_DROIT;}
    }
   // else if (presence(PRESENCE_HAUT)){direction = MARCHE_ARRIERE;}
    else {direction = STOP;}

    return direction;
  }
}


void avancer (){
  int vitesseBrute;
  if (currentMillis - startMillis >= 900){
    switch(testDirection()) {
      case TOUT_DROIT:
              vitesseBrute = (ultrasonic_1.distanceCm()+ultrasonic_3.distanceCm())/2 ;
              move(1,traitementVitesse(vitesseBrute, false));
              break;
      case GAUCHE:
              move(3,traitementVitesse(vitesseBrute, true));
              break;
      case DROITE:
              move(4,traitementVitesse(vitesseBrute, true));
              break;
      case MARCHE_ARRIERE:
              move(2,- traitementVitesse(vitesseBrute, true));
              break;
      case (STOP):
              move(1,0);
               break;

    }
   startMillis = millis();
   vitesseBrute = 0;
  }

}


int traitementVitesse (int vitesseBrute, boolean tourne){
  int vitesse = -2.55*vitesseBrute+255;
  if (tourne){
    if (vitesseBrute > 100) {
      vitesse = 150;
    }
    else{
      vitesse = 100;
    }
  }
 return vitesse;
}




void ledflash(){

   if (currentMillis - startMillis <= 500){
       rgbled_7.setColor(0,0,0,60);
       rgbled_7.show();

   }
   if (currentMillis - startMillis > 500){
       rgbled_7.setColor(0,60,0,0);
       rgbled_7.show();
   }
   if (currentMillis - startMillis >= 1000){
       startMillis = millis();
   }
}

