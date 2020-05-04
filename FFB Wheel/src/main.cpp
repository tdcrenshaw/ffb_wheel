#include <Arduino.h>
#include <SPI.h>

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
//MISO = DIO yellow
//MOSI Not used
//SS = CS green
//SCK = DCLK yellow
const int encoder_select = 20; //B0 bottom green

/** volatile **/
#define NOP 0x0000
#define ERRFL 0x0001
#define PROG   0x0003
#define DIAAGC 0x3FFC
#define CORDICMAG 0x3FFD
#define ANGLEUNC  0x3FFE
#define ANGLECOM  0x3FFF

/** non-volatile **/
#define ZPOSM 0x0016
#define ZPOSL 0x0017
#define SETTINGS1 0x0018
#define SETTINGS2 0x0019

#define RD  0x40    // bit 14 "1" is Read + parity even
#define WR  0x3F    //bit 14 ="0" is Write

SPISettings settings(SPI_CLOCK_DIV4, MSBFIRST, SPI_MODE1);

//encoder parity check
int parity(unsigned int x) {
  int parity = 0;
  while (x > 0) {
    parity = (parity + (x & 1)) % 2;
    x >>= 1;
  }
  return (parity);
}

int read_wheel_position(){
  unsigned int address = ANGLECOM; //probably the value we want
  unsigned int result = 0;   // result to return

  byte res_h = 0;
  byte res_l = 0;

  // take the SS pin low to select the chip:
  SPI.beginTransaction(settings);
  digitalWrite(encoder_select, LOW);

  //  send in the address and value via SPI:
  byte v_l = address & 0x00FF;
  byte v_h = (unsigned int)(address & 0x3F00) >> 8;

  if (parity(address | (RD << 8)) == 1) v_h = v_h | 0x80; // set parity bit

  v_h = v_h | RD; // its  a read command

  // Serial.print( " parity:  ");Serial.println(parity(address | (RD <<8)));
  // Serial.print(v_h, HEX); Serial.print(" A ");  Serial.print(v_l, HEX);  Serial.print(" >> ");

  res_h = SPI.transfer(v_h);
  res_l = SPI.transfer(v_l);

  digitalWrite(encoder_select, HIGH);
  SPI.endTransaction();

  delay(2);

  SPI.beginTransaction(settings);
  digitalWrite(encoder_select, LOW);

  //if (parity(0x00 | (RD <<8))==1) res_h = res_h | 0x80;  // set parity bit
  //res_h = res_h | RD;

  res_h = (SPI.transfer(0x00));
  res_l = SPI.transfer(0x00);

  res_h = res_h & 0x3F;  // filter bits outside data

  //Serial.print(res_h, HEX);   Serial.print(" R  ");  Serial.print(res_l, HEX);   Serial.print("  ");

  digitalWrite(encoder_select, HIGH);
  SPI.endTransaction();

  return (result = (res_h << 8) | res_l);
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
  SPI.begin();
  //pretty sure next two lines are already defined in SPISettings above
  //SPI.setDataMode(SPI_MODE1); // properties chip
  //SPI.setBitOrder(MSBFIRST);  //properties chip


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

  unsigned int wheel_position;

  //this should probably become an interrupt
  wheel_position = read_wheel_position();
  Serial.println(wheel_position);

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
