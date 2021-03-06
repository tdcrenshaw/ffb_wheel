#include <Arduino.h>

//at some point consolidate these pins to same pwm timer
const int brush_north_pin = 0; //D0
const int brush_south_pin = 1; //D1
const int coil_north_pin = 14; //C4
const int coil_south_pin = 15; //C6

const int speed_pot_pin = 38; //F0

const int foward_pin = 2; //D2
const int reverse_pin = 3; //D3


//chip uses 3 wire serial like interface. also kinda like spi?
//on chip labels: encoder_clock is DCLK, encoder_select is CS, encoder_data is DIO
//need a 10k-100k resistor on DIO
//ground green
//5v red
//MISO = SO yellow 23 B3
//MOSI = SI
//SS = CS green 20 B0
//SCK = CK yellow = 21 B1
const int encoder_select = 20; //B0 bottom green
<<<<<<< HEAD
=======
const int encoder_clock = 21; // B1 bottom yellow
const int encoder_data_out = 23; //B3 top yellow

int read_wheel_position(){
  int data;
  byte temp_data;
  int n = 9;
  digitalWrite(encoder_select, LOW);
  digitalWrite(encoder_clock, HIGH);
  delayMicroseconds(1);
  digitalWrite(encoder_select, HIGH);
  //data is 21 bits long
  //first 5 are writeable if we want to. Five zeros reads angle
  //garbage in the middle, last 9 are angle register
  for (int i = 0; i <=20; i++) {
    delayMicroseconds(1);
    digitalWrite(encoder_clock, LOW);
    delayMicroseconds(1);
    digitalWrite(encoder_clock, HIGH);
    temp_data = digitalRead(encoder_data_out);
    //looking at just last 9 bits
    if (i > 11){
      bitWrite(data, n, temp_data);
      n--;
    }
  }
  digitalWrite(encoder_clock, LOW);
  digitalWrite(encoder_select, LOW);
  return(data);
}

>>>>>>> simplified_SPI


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
<<<<<<< HEAD
  digitalWrite(encoder_select, LOW);

=======
  pinMode(encoder_clock, OUTPUT);
  pinMode(encoder_data_out, INPUT);

  digitalWrite(encoder_select, LOW);
  digitalWrite(encoder_clock, LOW);
>>>>>>> simplified_SPI

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

  int wheel_position;

  //this should probably become an interrupt
  wheel_position = read_wheel_position();
<<<<<<< HEAD
  Serial.print("angle is: ");
  Serial.println(wheel_position);
=======
  //Serial.print("angle is: ");
  //Serial.println(wheel_position);
>>>>>>> simplified_SPI

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
