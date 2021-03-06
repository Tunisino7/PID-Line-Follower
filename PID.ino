#include <Arduino.h>
#include <math.h>

int const sens_numb = 4; //total number of sensor used 
int Lmot[]= {9,10}; //pin numbers of left motor. 
int Rmot[]= {6,5}; // pin numbers of right motor
int sens[]= {2,3,4,7}; // pin numbers of sensor
int min_speed=100,avg_speed=120,new_speed = 120; // it can be changed by calibration

void val_read(); //this function reads the anlog values of sensors and converts those into digital values
void ser_print(); //consists of all serial print output
void bt_print(); //for taking the output using bluetooth 
void weight_gen(); //some sample weight with respect to number of sensor generated for calculation
void straight(); // straight direction move of bot
void left(); //left direction move of bot
void right(); //right direction move of bot
void stp();  // for 0stopping the bot
void pid_line_follow(); //it is the function for PID control 
void direct(); //for deciding the direction


int j,k;
int analog_val[sens_numb], digital_val[sens_numb], weight[sens_numb];
int P=0, I=0, D=0, correction, error=0, previous_error=0, kp=2, kd=0, ki=0; //value of kp,kd,ki need to be obtained by calibration
int weighted_val[sens_numb], total_weight;
void setup()
{
  for(j=0;j<sens_numb;j++)
      pinMode(sens[j],INPUT);
  
  for(j=0;j<2;j++)
     {
       pinMode(Lmot[j],OUTPUT);
      pinMode(Rmot[j],OUTPUT);
    }
   Serial.begin(9600);
   //bt.begin(9600);
   delay(10);
}

void  val_read()
{
 
  for(j=0;j<sens_numb;j++){
    digital_val[j] = digitalRead(sens[j]);  
  }
}


void ser_print()
{
    Serial.println("##################");
    Serial.println("analog values");
  for(j=0;j<sens_numb;j++)
    Serial.println("Sensor"+String(j)+"="+String(analog_val[j]));
    Serial.println(" ");
    
    Serial.println("##################");
    Serial.println("digital values");
  for(j=0;j<sens_numb;j++)
    Serial.print("  " + String(digital_val[j]));
    Serial.println(" ");
    
    Serial.println("##################");
    Serial.println("Generated weight");
  for(j=0;j<sens_numb;j++)
    Serial.print("  "+ String(weight[j]));
    Serial.println(" ");
    
    Serial.println("##################");
    Serial.println("Weighted Value");
  for(j=0;j<sens_numb;j++)
    Serial.print("  "+ String(weighted_val[j]));
    Serial.println(" ");
    Serial.println("##################");
}

/* void bt_print()
{
    bt.println("##################");
    bt.println("analog values");
  for(j=0;j<sens_numb;j++)
    bt.println("Sensor"+String(j)+"="+String(analog_val[j]));
    bt.println(" ");
    
    bt.println("##################");
    bt.println("digital values");
  for(j=0;j<sens_numb;j++)
    bt.print("  " + String(digital_val[j]));
    bt.println(" ");
    
    bt.println("##################");
    bt.println("Generated weight");
  for(j=0;j<sens_numb;j++)
    bt.print("  "+ String(weight[j]));
    bt.println(" ");
    
    bt.println("##################");
    bt.println("Weighted Value");
  for(j=0;j<sens_numb;j++)
    bt.print("  "+ String(weighted_val[j]));
    bt.println(" ");
    bt.println("##################");
}
*/
void weight_gen()
{
    total_weight=0;
     k=sens_numb/2;
   if(sens_numb%2!=0){
       k= k*(-1);
     for(j=0;j<sens_numb;j++){
       weight[j]=k;
       k=k+1;
       
     }
   }  
   else{
     k= k*(-1);
     for(j=0;j<sens_numb;j++){
       if(k==0)
         k=1;
       weight[j]=k;
       k=k+1;
     }
   }
   
   for(j=0;j<sens_numb;j++){
    weighted_val[j] =weight[j] * digital_val[j];
    total_weight = total_weight + weighted_val[j];
  }
}

void straight()
{
  analogWrite(Lmot[0],avg_speed);
  analogWrite(Lmot[1],0);
  analogWrite(Rmot[0],avg_speed);
  analogWrite(Rmot[1],0);
}

void right()
{
  analogWrite(Lmot[0],0);
  analogWrite(Lmot[1],new_speed);
  analogWrite(Rmot[0],new_speed);
  analogWrite(Rmot[1],0);
}

void left()
{
  analogWrite(Lmot[0],new_speed);
  analogWrite(Lmot[1],0);
  analogWrite(Rmot[0],0);
  analogWrite(Rmot[1],new_speed);
}

void stp()
{
  analogWrite(Lmot[0],0);
  analogWrite(Lmot[1],0);
  analogWrite(Rmot[0],0);
  analogWrite(Rmot[1],0);
}

void pid_line_follow()
{
  
  
  int current_weight=abs(total_weight);
  int target_weight=0;
  error =  current_weight - target_weight;
  P=error*kp;
  
  I=(error+previous_error)*ki;
  D=kd * (error - previous_error);
  correction= P+I+D;
  new_speed=min_speed + correction;
  previous_error=error; 
}

void direct()
{
   if(total_weight<0)
    left();
   else if(total_weight>0)
     right();
   else
     straight();
}

void loop()
{
  val_read();
  weight_gen();
  ser_print(); // use this if you want to see the output in serial monitor
  //bt_print(); // use this if you want to see output via bluetooth. I have used 'SENA Bterm' software in my android to see the output via bluetooth
  pid_line_follow();
  direct();
//  Serial.println("Total weight="+String(total_weight));
//  Serial.println("error="+String(error));
//  Serial.println("P="+String(P));
//  Serial.println("I="+String(I));
//  Serial.println("D="+String(D));
//  Serial.println("new speed="+String(new_speed));
//  delay(3000);
  
}
