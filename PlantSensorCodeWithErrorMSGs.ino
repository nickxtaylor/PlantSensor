#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <dht.h> 

LiquidCrystal_I2C lcd(0x27, 20, 4);
dht DHT;
float dt = 3000;

// RE and DE Pins set the RS485 module
// to Receiver or Transmitter mode
#define RE 8 // RS485
#define DE 7 // RS485
#define DHT_pinOut 12 // DHT11
#define moisture_sense_pin A0 // moisture sensor

// Modbus RTU requests for reading NPK values
const byte nitro[] = {0x01,0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte phos[] = {0x01,0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte pota[] = {0x01,0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};

// A variable used to store NPK values
byte values[11];

// Sets up a new SoftwareSerial object
// Digital pins 10 and 11 should be used with a Mega or Mega 2560
// SoftwareSerial mod(2, 3);
SoftwareSerial mod(10, 11);
 
void setup() {
  // Set the baud rate for the Serial port
  Serial.begin(9600);

  // Set the baud rate for the SerialSoftware object
  mod.begin(9600);

  // Define pin modes for RE and DE
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);

  // LCD setup
  lcd.init();
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on
  lcd_boot(dt);

}
 
void loop() {

  // Read the Analog Input
  // int value = analogRead(moisture_sense_pin);
  lcd_print_moisture(analogRead(moisture_sense_pin), dt);

  // do DHT stuff
  int readData = DHT.read22(DHT_pinOut);
  lcd_print_temp(DHT.temperature, DHT.humidity, dt);

  // do NPK stuff
  byte val1,val2,val3;
  val1 = nitrogen();
  delay(500);
  val2 = phosphorous();
  delay(500);
  val3 = potassium();
  delay(500);
  lcd_print_NPK(val1, val2, val3, dt);

}

// -- FUNCTIONS -------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void lcd_print_moisture(int value, float dt) {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Moisture LVL/600:");
  lcd.setCursor(0, 1);
  lcd.print(value);

  if (value < 280 && value > 0) {

    // soil is too wet
    lcd.setCursor(0, 2);
    lcd.print("soil so wet. ");

  } else if (value >= 280 && value < 400) {

    // soil is in good condition
    lcd.setCursor(0, 2);
    lcd.print("soil moisture is so good");

  } else if (value >= 400 && value < 600) {

    // soil is too dry
    lcd.setCursor(0, 2);
    lcd.print("soil too dry");

  } else {

    // sensor is broken
    lcd.setCursor(0, 2);
    lcd.print("moisture sensor failure");

  }

  // Print the value to the serial monitor
  Serial.print("moisture sense stuff");
  delay(dt);

}

void lcd_print_temp(float t, float h, float dt) {

  lcd.clear();
  lcd.setCursor(0,0);
	lcd.print("Temperature: ");
  lcd.setCursor(0, 1);  
	lcd.print(t);
	lcd.print((char)223);	//shows degrees character
	lcd.print(" C || ");
  lcd.print((t*9.0)/5.0+32.0);
  lcd.print((char)223);
  lcd.print(" F");
	lcd.setCursor(0,2);
	lcd.print("Humidity: ");
  lcd.setCursor(0, 3); 
	lcd.print(h);
	lcd.print(" %");
	
	delay(dt);

  //
  // Serial.print("");
  // Serial.print("DHT sense stuff");
	//Serial.print("Temperature = ");
	//Serial.print(t);
	//Serial.print("°C | ");
	//Serial.print((t*9.0)/5.0+32.0);        // Convert celsius to fahrenheit
	//Serial.println("°F ");
	//Serial.print("Humidity = ");
	//Serial.print(h);
	//Serial.println("% ");
	//Serial.println("");

}

void lcd_print_NPK(byte val1, byte val2, byte val3, float dt) {

  //
  lcd.clear();

  // nitrogen
  lcd.setCursor(0,0);   
  if (val1 < 50) {

    lcd.print("Nitrogen (LOW): ");
    lcd.print(val1);
    lcd.print(" mg/kg");

  } else if (val1 >= 50 && val1 <= 150) {

    lcd.print("Nitrogen (good): ");
    lcd.print(val1);
    lcd.print(" mg/kg");

  } else if (val1 > 150) {

    lcd.print("Nitrogen (HIGH): ");
    lcd.print(val1);
    lcd.print(" mg/kg");

  }

  // phosphorus
  lcd.setCursor(0,1);  
  if (val2 < 20) {

    lcd.print("Phosphorus (LOW): ");
    lcd.print(val2);
    lcd.print(" mg/kg");

  } else if (val2 >= 20 && val2 <= 60) {

    lcd.print("Phosphorus (good): ");
    lcd.print(val2);
    lcd.print(" mg/kg");

  } else if (val2 > 60) {

    lcd.print("Phosphorus (HIGH): ");
    lcd.print(val2);
    lcd.print(" mg/kg");

  } 

  // postassium
  lcd.setCursor(0,2);

  if (val3 < 100) {

    lcd.print("Potassium (LOW): ");
    lcd.print(val3);
    lcd.print(" mg/kg");

  } else if (val3 >= 100 && val3 <= 250) {

    lcd.print("Potassium (good): ");
    lcd.print(val3);
    lcd.print(" mg/kg");

  } else if (val3 > 250) {

    lcd.print("Potassium (HIGH): ");
    lcd.print(val3);
    lcd.print(" mg/kg");

  }    

  delay(dt);

  // Print values to the serial monitor
  // Serial.print("");
  // Serial.print("NPK Sense stuff");
  //Serial.print("Nitrogen: ");
  //Serial.print(val1);
  //Serial.println(" mg/kg");
  //Serial.print("Phosphorous: ");
  //Serial.print(val2);
  //Serial.println(" mg/kg");
  //Serial.print("Potassium: ");
  //Serial.print(val3);
  //Serial.println(" mg/kg");

}

void lcd_boot(float dt) {

  lcd.setCursor(3, 1);   //Set cursor to character 2 on line 0
  lcd.print("A BIGGEST BELLY");
  lcd.setCursor(2, 2);
  lcd.print("SOLUTIONS PRODUCT");

  delay(dt);

  lcd.clear();
  lcd.setCursor(2, 1);
  lcd.print("BOOTING BIG BELLY");
  lcd.setCursor(5, 2);
  lcd.print("SERVICES...");

  delay(dt);

  lcd.clear();
  lcd.setCursor(5, 1);   //Set cursor to character 2 on line 0
  lcd.print("a gift for");
  lcd.setCursor(3, 2);
  lcd.print("all my friends,");

  delay(dt);

  lcd.clear();
  lcd.setCursor(2, 0);   //Set cursor to character 2 on line 0
  lcd.print("happy birthday to"); 
  lcd.setCursor(5, 1);
  lcd.print("al & amanda,");
  lcd.setCursor(5, 3);   //Set cursor to character 2 on line 0
  lcd.print("from nick t");

  delay(dt);

  /*
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("dont kill the plant");
  lcd.setCursor(8, 1);
  lcd.print("...");
  lcd.setCursor(8, 2);
  lcd.print("");
  lcd.setCursor(0, 3);
  lcd.print("call me if it breaks");

  delay(dt);
  */

}

// -- BYTE UART CONVERSIONS -------------------------------------------------------------------------------------------------------------------------------------------------------------
 
byte nitrogen(){
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(mod.write(nitro,sizeof(nitro))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
    //Serial.print(mod.read(),HEX);
    values[i] = mod.read();
    // Serial.print(values[i],HEX);
    }
  }
  return values[4];
}
 
byte phosphorous(){
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(mod.write(phos,sizeof(phos))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
    //Serial.print(mod.read(),HEX);
    values[i] = mod.read();
    // Serial.print(values[i],HEX);
    }
  }
  return values[4];
}
 
byte potassium(){
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(mod.write(pota,sizeof(pota))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
    //Serial.print(mod.read(),HEX);
    values[i] = mod.read();
    // Serial.print(values[i],HEX);
    }
  }
  return values[4];
}