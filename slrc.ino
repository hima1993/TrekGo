#include <Stepper.h>
#include <Servo.h>
#include <SoftwareSerial.h>
 
#define STEPS 2038 // the number of steps in one revolution of your motor (28BYJ-48)
#define STEPS360 5400 // the number of steps in 360 turn

//colour
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8
int frequency = 0;
int frequencyB = 0;
int frequencyG = 0;
int frequencyR = 0 ;
SoftwareSerial BTserial(10,11);
 
Stepper stepperR(STEPS,45, 49, 47, 51);
Stepper stepperL(STEPS,4,6,5,7);

Servo servo_test; 
int angle = 0;

int count=0;

//ir variables
int irL = 22;
int irLM = 24;
int irRM = 26;
int irR = 28;

int irLSignal;
int irLMSignal;
int irRMSignal;
int irRSignal;

//sornar variables
int trigR = 8;
int echoR = 9;
int trigL = 10;
int echoL = 11;


void setup() {
  pinMode(irL, INPUT);
  pinMode(irLM, INPUT);
  pinMode(irRM, INPUT);
  pinMode(irR, INPUT);

  servo_test.attach(40);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  
  // Setting frequency-scaling to 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);
  
  BTserial.begin(38400);
  
  Serial.begin(10000); 
}
 
void loop() {
  //Right 1 | Left 0
  //LOW is white | HIGH is black
  //t->turning angle

  irLSignal = digitalRead(irL);
  irLMSignal = digitalRead(irLM);
  irRMSignal = digitalRead(irRM);
  irRSignal = digitalRead(irR);

  int distanceL=distance(trigL,echoL);
  int distanceR=distance(trigR,echoR);
  
  if(irLSignal == LOW && irLMSignal == LOW && irRSignal == LOW && irRMSignal == LOW && (distanceL>20)){
    //only T -> turn left by 90
    turn(0,90);
    count++;
  }else if(irLSignal == LOW && irLMSignal == LOW && irRSignal == LOW && irRMSignal == LOW && (distanceL<20)){
    //T and wall-> go foward
    foward(5);    
  }else if(irLSignal == HIGH && irLMSignal == HIGH && irRSignal == HIGH && irRMSignal == HIGH && (distanceL>20) && (distanceR>20)){
    //black and no wall  -> turn around by 180
    colour();
    turn(0,180);
  }else if(irLSignal == HIGH && irLMSignal == HIGH && irRSignal == HIGH && irRMSignal == HIGH && ((distanceL<20) || (distanceR<20)) ){
    //black and wall  -> wall detecting
    turn(0,180);
  }else if(irLSignal == LOW && irLMSignal == LOW && irRSignal == HIGH && irRMSignal == HIGH){
    //only left -> turn left by 90
    turn(0,90);
    count++;
  }else if(irLSignal == HIGH && irLMSignal == HIGH && irRSignal == LOW && irRMSignal == LOW){
    //only right -> turn right by 90
    turn(0,90);

  //line following
  }else if(irLSignal == HIGH && irLMSignal == LOW && irRSignal == LOW && irRMSignal == HIGH){
    //if middle two on the track -> go foward
    foward(5);
  }else if(irLSignal == HIGH && irLMSignal == HIGH && irRSignal == LOW && irRMSignal == HIGH){
    //if goes to left -> turn right
    turn(1,5);
  }else if(irLSignal == HIGH && irLMSignal == LOW && irRSignal == HIGH && irRMSignal == HIGH){
    //if goes to right -> turn left
    turn(0,5);
  } 
}

void foward(int t){
  //t->foward turning angle
  int a=0;
  while(a<=t){
    stepperL.setSpeed(12);
    stepperR.setSpeed(12);
    stepperL.step(1);      
    stepperR.step(1);
    a++;
  }
}

void backward(int t){
  //t->backward turning angle
  int a=0;
  while(a<=t){
    stepperL.setSpeed(12);
    stepperR.setSpeed(12);
    stepperL.step(-1);      
    stepperR.step(-1);
    a++;  
  }
}

void turn(int side,int angle){
  //Right 1 | Left 0
  int i=0;
  int k=(STEPS360/360)*angle;
  
  if(side==1){
    //turn right
    while(i<k){
      stepperL.setSpeed(12);
      stepperR.setSpeed(12);
      stepperL.step(1);      
      stepperR.step(-1);
      i++;
    }
  }else{
    //turn left
    while(i<k){
      stepperL.setSpeed(12);
      stepperR.setSpeed(12);
      stepperL.step(-1);      
      stepperR.step(1);
      i++;
    }
  }
}

//sonar methods

long microsecondsToInches(long microseconds)
{
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds)
{
  return microseconds / 29 / 2;
}

int distance(int trig,int echo){
  long duration, cm;

  pinMode(trig, OUTPUT);  
  pinMode(echo, INPUT);
   
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  pinMode(echo, INPUT);
  duration = pulseIn(echo, HIGH);

  cm = microsecondsToCentimeters(duration); 
  
  return cm;
}

//servo code

void servoTurn(int a){
  for(angle = 0; angle < a; angle += 1){                                  
    servo_test.write(angle);                          
  }  
}

void colour(){
  // Setting red filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  // Reading the output frequency
  frequency = pulseIn(sensorOut, LOW);
  //Remaping the value of the frequency to the RGB Model of 0 to 255
  frequency = map(frequency, 95,300,255,0);
  int frequencyR = frequency ;
  // Printing the value on the serial monitor

//  Serial.print("R= ");//printing name
//  Serial.print(frequency);//printing RED color frequency
//  Serial.print("  ");
delay(100);
  // Setting Green filtered photodiodes to be read
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  frequency = pulseIn(sensorOut, LOW);
  //Remaping the value of the frequency to the RGB Model of 0 to 255
  frequency = map(frequency, 100,400,255,0);
  int frequencyG = frequency ;
  // Printing the value on the serial monitor
//  Serial.print("G= ");//printing name
//  Serial.print(frequency);//printing RED color frequency
//  Serial.print("  ");
  delay(100);
  // Setting Blue filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  frequency = pulseIn(sensorOut, LOW);
  //Remaping the value of the frequency to the RGB Model of 0 to 255
  frequency = map(frequency, 27,144,255,0);

  int frequencyB = frequency ;

  // Printing the value on the serial monitor
//  Serial.print("B= ");//printing name
//  Serial.print(frequency);//printing RED color frequency
//  Serial.println("  ");
  delay(100);



if(frequencyB > 50 || frequencyG > 50 || frequencyR > 50){



     if (frequencyB >= frequencyG)
    {
        if(frequencyB >= frequencyR){

           
 
 
           // Serial.println(state);
            BTserial.println(3);
            Serial.println("BLUE");
            //Serial.println("  ");

        }
        else{

             BTserial.println(1);
           Serial.println("RED");
           //Serial.println("  ");

          

        }
    }
    
    else
    {
        if(frequencyG >= frequencyR){

            BTserial.println(2);
            Serial.println("GREEN");
            //Serial.println("  ");

        }
        else{
            BTserial.println(1);
            Serial.println("RED");
            //Serial.println("  ");

        }
    }
}else{


  BTserial.println("Not Define"); 
  Serial.println("Not Define");
  //Serial.println("  ");

  


  // Serial.println(state);
  
  
  }



    delay(100);
}

//end
