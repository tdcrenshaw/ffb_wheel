#include <Arduino.h>
#include <spi.h>

//at some point consolidate these pins to same pwm timer
const int brush_north_pin = 0; //D0
const int brush_south_pin = 1; //D1
const int coil_north_pin = 14; //C4
const int coil_south_pin = 15; //C6

const int speed_pot_pin = 38; //F0

const int foward_pin = 2; //D2
const int reverse_pin = 3; //D3

//SPI pin declaration
//everything but slave select is autodefined
//for encoder, MOSI is always 0. Probably don't need to run it
//if this encoder is the only one we use, could just tie ss to ground on chip
//SCK is 21 / B1, MOSI is 22 / B2, MISO is 23 / B3
//on chip labels: SCK is DCLK, SS is CS, MISO is DIO
//need a 10k-100k resistor on MOSI
const int slave_select = 20; //B0
SPISettings encoder_settings(

int read_wheel_position(){
  SPI.beginTransaction(encoder_settings);


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

  //SPI init here
  pinMode(slave_select, OUTPUT);
  SPI.begin();
  //needs to initalize analog refernece here

  pinMode(speed_pot_pin, INPUT);
  pinMode(foward_pin, INPUT_PULLUP);
  pinMode(reverse_pin, INPUT_PULLUP);

  //make sure motor is stopped
  stop();
  }

void loop() {
  int speed;
  int minimum_speed = 0; //slower than this and the motor won't start spinning. determine experimentally
  int max_speed = 255;

  int cur_direction; //stop is 0, foward 1, reverse 2

  int wheel_position; //might need to be a float

  //this should probably become an interrupt
  wheel_position = read_wheel_position();
  Serial.println(wheel_position)

  int new_speed = analogRead(speed_pot_pin);
  new_speed = new_speed / 4; //analog read goes to 1024, scale down x4
  if (new_speed > max_speed){
    new_speed = max_speed;
  }
  else if (new_speed < minimum_speed){
    new_speed = minimum_speed;
  }
  //check to see if speed value has been updated since last loop
  if (new_speed != speed){
    speed = new_speed;
    Serial.println(speed);
  }
  
  if(digitalRead(foward_pin) == LOW){
    if (cur_direction != 1){
      Serial.println("running foward");
    }
    cur_direction = 1;
    run_clockwise(speed);
  }
  else if (digitalRead(reverse_pin) == LOW){
    if (cur_direction != 2){
      Serial.println("running backwards");
    }
    cur_direction = 2;
    run_counterclockwise(speed);
  }
  else{
    if (cur_direction != 0){
      Serial.println("stopping");
    }
    stop();
    cur_direction = 0;
  }
}
