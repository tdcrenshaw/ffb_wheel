#include <Arduino.h>

//at some point consolidate these pins to same pwm timer
const int brush_north_pin = 0; //D0
const int brush_south_pin = 1; //D1
const int coil_north_pin = 14; //C4
const int coil_south_pin = 15; //C6

const int speed_pot_pin = 38; //F0

const int foward_pin = 2; //D2
const int reverse_pin = 3; //D3

void run_clockwise(int speed){
  //this should have coils and brushes set opposing fields
  digitalWrite(brush_south_pin, LOW);
  digitalWrite(coil_north_pin, LOW);
  
  digitalWrite(coil_south_pin, HIGH);
  analogWrite(brush_north_pin, speed);
}
void run_counterclockwise(int speed){
  //this should have coils and brushes set to equal fields
  digitalWrite(brush_south_pin, LOW);
  digitalWrite(coil_south_pin, LOW);

  digitalWrite(coil_north_pin, HIGH);
  analogWrite(brush_north_pin, speed);
}

void stop(){
  digitalWrite(brush_north_pin, LOW);
  digitalWrite(brush_south_pin, LOW);
  digitalWrite(coil_south_pin, LOW);
  digitalWrite(coil_south_pin, LOW);
}

void setup() {
  Serial.begin(9600);

  //hbridge pins
  pinMode(brush_north_pin, OUTPUT);
  pinMode(brush_south_pin, OUTPUT);
  pinMode(coil_north_pin, OUTPUT);
  pinMode(coil_south_pin, OUTPUT);

  //needs to initalize analog refernece here

  pinMode(speed_pot_pin, INPUT);
  pinMode(foward_pin, INPUT_PULLUP);
  pinMode(reverse_pin, INPUT_PULLUP);

  //make sure motor is stopped
  stop();
  }

void loop() {
  int speed;
  int minimum_speed = 75; //slower than this and the motor won't start spinning. determine experimentally


  int new_speed = analogRead(speed_pot_pin);
  //check to see if speed value has been updated since last loop
  if (new_speed != speed){
    speed = new_speed;
    Serial.println(speed);
  }

  if(digitalRead(foward_pin)){
    run_clockwise(speed)
  }
  else if (digitalRead(reverse_pin)){
    run_counterclockwise(speed)
  }
  else{
    stop()
  }




}

