#include <boarddefs.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <ir_Lego_PF_BitStreamEncoder.h>
#include <IRRemote.h> 
#include <Servo.h>


// Récepteur IR
int RECV_PIN = 11;  //  PB3
IRrecv irrecv(RECV_PIN);
decode_results results;

// Servo
Servo servoGD;
int angleGD=90;
int pinServo=13;  // PB5

// Moteurs 1=gauche et 2=droite 
int pin1Moteur1=2;  // cmd 1 du moteur 1 PD2
int pin2Moteur1=3;  // cmd 2 du moteur => 1 PD3
int pinPMoteur1=5;  // PMM du moteur 1 => PD5

int pin1Moteur2=4;  //  cmd 1 du moteur 2 =>PD4
int pin2Moteur2=7;  //  cmd 2 du moteur 2 => PD7
int pinPMoteur2=6;  //  PWM du moteur 2  => PD6

int puissanceMax=100;
int puissanceMoy=25;

// Leds
const int ledverte = 17;  // PC3
const byte PIN_LED_R = 16; // PC2
const byte PIN_LED_G = 15; // PC1
const byte PIN_LED_B = 14; // PC0

const byte COLOR_BLACK = 0b000;
const byte COLOR_RED = 0b100;
const byte COLOR_GREEN = 0b010;
const byte COLOR_BLUE = 0b001;
const byte COLOR_MAGENTA = 0b101;
const byte COLOR_CYAN = 0b011;
const byte COLOR_YELLOW = 0b110;
const byte COLOR_WHITE = 0b111;

// Pompe
int pinTransistorBase=9; //PMM du transistor => PB1


void setup()
{  
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver

  // Règle les servos  
  servoGD.attach(pinServo);
  servoGD.write(angleGD);

 // Règle les pins des moteurs
  pinMode(pin1Moteur1,OUTPUT);
  pinMode(pin2Moteur1,OUTPUT);
  pinMode(pinPMoteur1,OUTPUT);
  pinMode(pin1Moteur2,OUTPUT);
  pinMode(pin2Moteur2,OUTPUT);
  pinMode(pinPMoteur2,OUTPUT);
  int vitesseM1=0; //variable de pourcentage de vitesse pour moteur 1
  int sensM1=1; //variable de sens pour moteur 1
  int vitesseM2=0; //variable de pourcentage de vitesse pour moteur 2
  int sensM2=1; //variable de sens pour moteur 2

  // LEDS
  pinMode(ledverte, OUTPUT);
  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);
}

void loop()
{
  digitalWrite(ledverte, HIGH);  // La LED verte s'allume en permanence pour s'assurer que le canon est bien alimenté
  DisplayColor(COLOR_BLACK);  // La LED RGB est éteinte
  
  if (irrecv.decode(&results))  // Dès qu'on reçoit un signal IR, on fait un switch
  {
     Serial.println(results.value, HEX);
     switch(results.value)
     {
        case 0xFFE01F:
        case 0x41C2393C: 
        case 0xF076C13B: Serial.println("tourner tourelle -> gauche");
                         DisplayColor(COLOR_BLUE);
                         TournerGaucheTourelle();
                         delay(500);
                         break;
        case 0x872CD4C:
        case 0xFFA857: 
        case 0x4B05DA56: Serial.println("tourner tourelle -> droite");
                         DisplayColor(COLOR_BLUE);
                         TournerDroiteTourelle();
                         delay(500);
                         break;
        case 0xFF906F:
        case 0xE5CFBD7F: Serial.println("TIR");
                         DisplayColor(COLOR_WHITE);
                         analogWrite(pinTransistorBase, 255);
                         delay(3000);
                         analogWrite(pinTransistorBase, 0);
                         break;
        case 0xFFA25D:
        case 0xE318261B: Serial.println("avancer");
                         DisplayColor(COLOR_GREEN);
                         VoitureAvancer();
                         delay(500);
                         break;
        case 0x511DBB:
        case 0xFF629D:   Serial.println("reculer");
                         DisplayColor(COLOR_GREEN);
                         VoitureReculer();
                         delay(500);
                         break;
        case 0x20FE4DBB:
        case 0xFFC23D:   Serial.println("arret");
                         DisplayColor(COLOR_GREEN);
                         VoitureArreter();
                         delay(500);
                         break;
        case 0x52A3D41F:
        case 0xFF22DD:   Serial.println("Voiture tourne gauche");
                         DisplayColor(COLOR_CYAN);
                         TournerGaucheVoiture();
                         delay(500);
                         break;
        case 0xFF02FD:
        case 0xD7E84B1B:
        case 0x15CB03C4:
        case 0x62B984D4: Serial.println("Voiture tourne droite");
                         DisplayColor(COLOR_CYAN);
                         TournerDroiteVoiture();
                         delay(500);
                         break;
        default: Serial.println("ERREUR");
                 DisplayColor(COLOR_RED);
                 delay(300);
                 DisplayColor(COLOR_BLACK);
                 delay(300);
                 DisplayColor(COLOR_RED);
                 delay(300);
                 break;
     }
     
     irrecv.resume(); // Receive the next value
  }
}

void TournerGaucheTourelle()
{
  if(angleGD > 0)
      angleGD -=10;
  servoGD.write(angleGD);
  delay(500);
}

void TournerDroiteTourelle()
{
  Serial.println(angleGD);
  if(angleGD < 180)
      angleGD +=10;
  servoGD.write(angleGD);
  Serial.println(angleGD);
  delay(500);
}

void VoitureAvancer()
{
  actionMoteur(1,1,puissanceMax); //commande pour moteur gauche
  actionMoteur(2,1,puissanceMax); //commande pour moteur droite
}

void VoitureReculer()
{
  actionMoteur(1,-1,puissanceMax); //commande pour moteur gauche
  actionMoteur(2,-1,puissanceMax); //commande pour moteur droite
}

void VoitureArreter()
{
  actionMoteur(1,0,puissanceMax); //commande pour moteur gauche
  actionMoteur(2,0,puissanceMax); //commande pour moteur droite
}

void TournerGaucheVoiture()
{
  actionMoteur(1,1,puissanceMax); //commande pour moteur gauche
  actionMoteur(2,0,puissanceMoy); //commande pour moteur droite
}

void TournerDroiteVoiture()
{
  actionMoteur(1,0,puissanceMoy); //commande pour moteur gauche
  actionMoteur(2,1,puissanceMax); //commande pour moteur droite
}

void actionMoteur(int moteur,int sens,int pourcentage){
  int pin1,etat1,pin2,etat2,pinP,puissance; //variable de la fonction
  //test numéro du moteur
  if (moteur==1){
    pin1=pin1Moteur1;
    pin2=pin2Moteur1;
    pinP=pinPMoteur1;
  }
  else {
    pin1=pin1Moteur2;
    pin2=pin2Moteur2;
    pinP=pinPMoteur2;
  }
  //test sens du moteur 1,-1 (sens contrainre) ou tout autre valeur (stoppe le moteur)
  if (sens==1){
    etat1=1;
    etat2=0;
  }
  else if (sens==-1){
    etat1=0;
    etat2=1;
  }
  else {
    etat1=0;
    etat2=0;
  }
  puissance= pourcentage; //map(pourcentage,0,100,0,255);
  analogWrite(pinP,puissance);
  digitalWrite(pin1,etat1);
  digitalWrite(pin2,etat2);
}

void DisplayColor(byte color)
{
  digitalWrite(PIN_LED_R, bitRead(color, 2));
  digitalWrite(PIN_LED_G, bitRead(color, 1));
  digitalWrite(PIN_LED_B, bitRead(color, 0));
}
