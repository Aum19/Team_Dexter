/*Pin 0 on Arduino must be disconnected when uploading the program

BMP280 Sensor:
Vin (Voltage In) -> D2 3.3V, Gnd (Ground) -> Gnd, SDA (Serial Data) -> A4, SCL (Serial Clock) -> A5
SD card reader:
MOSI -> pin 11 (blue), MISO -> pin 12 (white), CLK -> pin 13  (grey), CS -> pin 4 (purple)
Monitor baud rate 9600, connect vcc to 5v
*/
#include <BME280I2C.h>      //BMP
#include <Wire.h>           //BMP
#include <SPI.h>            //SD
#include <SD.h>             //SD
#define SERIAL_BAUD 9600    //BMP
BME280I2C bme;
File myFile;
bool W = false;
unsigned long previousMillis = 0;
long INTERVAL = 1000;                    //Set interval to write to file here

int VCC1 = 2;                            //VCC for sensor 1 is on digital pin 2 
int VCC2 = 8;                            //VCC for sensor 2 is on digital pin 8
int VCC3 = 7;                            //VCC for sensor 3 is on digital pin 7
int temp_alert_pin = 10;
int COUNTER = 0;
float current_status[6] = {0,0,0,1000,26.0};
const char *components[6]; 
String CARD_FILE = "try.txt";    //Name of file to be created or used on SD
String BT_INPUT = "";
String last_timestamp = "--";

void setup() {
  Serial.begin(SERIAL_BAUD);
  while(!Serial) {}                      // Wait
  Wire.begin();
  assign_components();
//  display_bme_type();
}
void loop(){
  BT_INPUT = Serial.readString();
  sensor_control(BT_INPUT);  
  if(W == true){
      getBME280Data(&Serial);
  }
}
// Get temperature and pressure data and calls function to write on SD
void getBME280Data(Stream* client)
{
   float temp(NAN), hum(NAN), pres(NAN);
   BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
   BME280::PresUnit presUnit(BME280::PresUnit_Pa);
   bme.read(pres, temp, hum, tempUnit, presUnit);
   temp_alert(temp);
   String bme_string = String(temp)+ "_" +String(pres);
   //Serial.println(bme_string);
   sample_write(bme_string);
}

void sample_write(String sensor_data)
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= INTERVAL) {
    previousMillis = currentMillis;
    Serial.println(sensor_data);
    sd_write(sensor_data);
  }
}

void sd_write(String bme_data)
{
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  myFile = SD.open(CARD_FILE, FILE_WRITE);  
  
    if (myFile) {
      myFile.println(bme_data);
    } 
    else {
      Serial.println("error opening "+CARD_FILE);
    }
  myFile.close();
}


// The format of command should be sensorno(1)operation(0)seperator(;)value(1) example--->  10;1
bool sensor_control(String BT_INPUT){
  String comment = "";
  char input_chars[15] = "";
//converting input string to char array  
  BT_INPUT.toCharArray(input_chars,20);
//Extracting part 1 of the command
  char part1[5] = "";
  String p1 = strtok(input_chars,";");
  p1.toCharArray(part1,5);
//Extracting part 2 of the command
  char part2[20] = "";
  String p2 = strtok(NULL,";");
  p2.toCharArray(part2,20);
//Converting part 1 to int for using in switch case
  int part1_value = atoi(part1);
//Method call based on part1 of the command
  switch(part1_value){
    case 99:
      get_status();
      break;
    case 10:
      strcmp("0",part2) == 0 ? (digitalWrite(VCC1, LOW), current_status[0] = 0,W = false) : (digitalWrite(VCC1, HIGH),current_status[0] = 1,bme.begin());
      get_status();
      break;
    case 11:
      strcmp("0",part2) == 0 ? Serial.println("Sample rate cannot be zero") : INTERVAL = atoi(part2),current_status[3] = INTERVAL;
      get_status();
      break;
    case 12:
      current_status[4] = atof(part2);
      get_status();
    case 20:
      strcmp("0",part2) == 0 ? (digitalWrite(VCC2,LOW),current_status[1] = 0,W = false) : (digitalWrite(VCC2,HIGH),current_status[1] = 1);
      get_status();
      break;
    case 21:
      sd_write(p2);
      bme.begin();
      delay(200);
      W = true;
      last_timestamp = p2;
      get_status();
      break;
    case 22:
      W = false;
      sd_write(p2);
      last_timestamp = p2;
      Serial.println("Now reading sd");
      read_sd(CARD_FILE);
      get_status();
      break;
    case 23:
      comment = "//"+ p2;
      sd_write(comment);
      Serial.println(comment);
      break;
    case 24:
     strcmp("0",part2) == 0 ?  : CARD_FILE = part2 ;
      break;
    case 30:
      strcmp("0",part2) == 0 ? (digitalWrite(VCC3,LOW),current_status[2] = 0) : (digitalWrite(VCC3,HIGH),current_status[2] = 1);
      get_status();
      break;   
    default:
      break;
  }                                                                                                                                                                    
}


// Get BME type and display model name
void display_bme_type(){
  switch(bme.chipModel())
  {
     case BME280::ChipModel_BME280:
       Serial.println("Found BME280 sensor! Success.");
       break;
     case BME280::ChipModel_BMP280:
       Serial.println("Found BMP280 sensor!");
       break;
     default:
       Serial.println("Found UNKNOWN sensor! Error!");
  }
}
void read_sd(String filename){
  myFile = SD.open(CARD_FILE);
  if (myFile) {
    Serial.println("$$$$$ File START $$$$$");
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
    Serial.println("$$$$$ File END $$$$$");
  } 
  else {
    Serial.println("error opening file");
  }
}

void temp_alert(float t){
  current_status[4] <= t ? digitalWrite(temp_alert_pin,HIGH) : digitalWrite(temp_alert_pin,LOW);
}

void get_status(){
  int i = 0;
  for(i=0;i<5;i++){
    Serial.println(String(components[i]) + " " + String(current_status[i]));
  }
  Serial.println(String(components[5]) + " " + last_timestamp);
}
void assign_components(){
  pinMode(VCC1,OUTPUT);
  pinMode(VCC2,OUTPUT);
  pinMode(VCC3,OUTPUT);
  pinMode(temp_alert_pin,OUTPUT);
  components[0] = "BME_power: ";
  components[1] = "SD_power:  ";
  components[2] = "GPS_power: ";
  components[3] = "Rate:      ";
  components[4] = "Temp_alert:";
  components[5] = "Last_write:";
  digitalWrite(VCC1, LOW);
  digitalWrite(VCC2, LOW);
  digitalWrite(VCC3, LOW);
  digitalWrite(temp_alert_pin,LOW);
}
