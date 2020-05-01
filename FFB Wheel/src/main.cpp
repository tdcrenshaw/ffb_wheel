#include <Arduino.h>

//at some point consolidate these pins to same pwm timer
const int brush_north_pin = 0; //D0
const int brush_south_pin = 1; //D1
const int coil_north_pin = 14; //C4
const int coil_south_pin = 15; //C6


void run_clockwise(int speed){
  //this should have coils and brushes set opposing fields
  digitalWrite(brush_south_pin, LOW);
  digitalWrite(coil_north_pin, LOW);

  for (int cur_speed = 0; cur_speed <= speed; cur_speed++){
    analogWrite(brush_north_pin, cur_speed);
    analogWrite(coil_south_pin, cur_speed);
  }
}
void run_counterclockwise(int speed){
  //this should have coils and brushes set to equal fields
  digitalWrite(brush_south_pin, LOW);
  digitalWrite(coil_south_pin, LOW);

  for (int cur_speed = 0; cur_speed <= speed; cur_speed++){
    analogWrite(brush_north_pin, cur_speed);
    analogWrite(coil_north_pin, cur_speed);
  }
}

void stop(){
  digitalWrite(brush_north_pin, LOW);
  digitalWrite(brush_south_pin, LOW);
  digitalWrite(coil_south_pin, LOW);
  digitalWrite(coil_south_pin, LOW);
}

void setup() {
  Serial.begin(9600);

  pinMode(brush_north_pin, OUTPUT);
  pinMode(brush_south_pin, OUTPUT);
  pinMode(coil_north_pin, OUTPUT);
  pinMode(coil_south_pin, OUTPUT);

  digitalWrite(brush_north_pin, LOW);
  digitalWrite(brush_south_pin, LOW);
  digitalWrite(coil_south_pin, LOW);
  digitalWrite(coil_south_pin, LOW);
  }

void loop() {
  //speed is out of 255 max
  int speed = 50;
  //run_clockwise(speed);
  //digitalWrite(brush_north_pin, HIGH);
  analogWrite(brush_north_pin, speed);
  digitalWrite(coil_south_pin, HIGH);
  delay(10000);

  stop();

  delay(2000);
  //run_counterclockwise(speed);
  //digitalWrite(brush_north_pin, HIGH);
  analogWrite(brush_north_pin, speed);
  digitalWrite(coil_south_pin, LOW);
  digitalWrite(coil_north_pin, HIGH);

  delay(10000);
  stop();

  delay(2000);
}
