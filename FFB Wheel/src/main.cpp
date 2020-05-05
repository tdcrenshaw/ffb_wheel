#include <Arduino.h>

//at some point consolidate these pins to same pwm timer
const int brush_north_pin = 0; //D0
const int brush_south_pin = 1; //D1
const int coil_north_pin = 14; //C4
const int coil_south_pin = 15; //C6

const int speed_pot_pin = 38; //F0

const int foward_pin = 2; //D2
const int reverse_pin = 3; //D3

//SCK is 21 / B1, MOSI is 22 / B2, MISO is 23 / B3
//SPI library defines everything but slave select
//on chip labels: encoder_clock is DCLK, encoder_select is CS, encoder_data is DIO
//need a 10k-100k resistor on MOSI
//ground green
//5v red
//MISO = SO yellow 23 B3
//SS = CS green 20 B0
//SCK = CK yellow = 21 B1
const int encoder_select = 20; //B0 bottom green
const int encoder_clock = 21; // B1 bottom yellow
const int encoder_data_out = 23; //B3 top yellow


int read_wheel_position(){
  unsigned int data;
  digitalWrite(encoder_select, HIGH);

  for (int i = 0; i <=15; i++) {
    delay(1);
    digitalWrite(encoder_clock, HIGH);
    delay(1);
    digitalWrite(encoder_clock, LOW);
    byte temp_data = digitalRead(encoder_data_out);
    data = data >> temp_data;
    Serial.print(temp_data);
  }
  digitalWrite(encoder_clock, LOW);
  digitalWrite(encoder_select, LOW);
  return(data);
}



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

  //encoder init here
  pinMode(encoder_select, OUTPUT);
  pinMode(encoder_data_out, INPUT);
  pinMode(encoder_clock, OUTPUT);
  digitalWrite(encoder_select, HIGH);
  digitalWrite(encoder_clock, LOW);
  //needs to initalize analog refernece here

  pinMode(speed_pot_pin, INPUT);
  pinMode(foward_pin, INPUT_PULLUP);
  pinMode(reverse_pin, INPUT_PULLUP);

  //make sure motor is stopped
  stop();
  }

void loop() {
  int minimum_speed = 0; //slower than this and the motor won't start spinning. determine experimentally
  int max_speed = 255;

  int cur_direction; //stop is 0, foward 1, reverse 2

  unsigned int wheel_position;

  //this should probably become an interrupt
  wheel_position = read_wheel_position();
  Serial.println("angle is: ");
  Serial.println(wheel_position);

  int speed = analogRead(speed_pot_pin);
  speed = speed / 4; //analog read goes to 1024, scale down x4
  if (speed > speed){
    speed = max_speed;
  }
  else if (speed < minimum_speed){
    speed = minimum_speed;
  }
  //check to see if speed value has been updated since last loop
  //Serial.println(speed);

  if(digitalRead(foward_pin) == LOW){
    if (cur_direction != 1){
      //Serial.println("running foward");
    }
    cur_direction = 1;
    run_clockwise(speed);
  }
  else if (digitalRead(reverse_pin) == LOW){
    if (cur_direction != 2){
      //Serial.println("running backwards");
    }
    cur_direction = 2;
    run_counterclockwise(speed);
  }
  else{
    if (cur_direction != 0){
      //Serial.println("stopping");
    }
    stop();
    cur_direction = 0;
  }
}
