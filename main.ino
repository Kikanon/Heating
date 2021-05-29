  #include <LiquidCrystal.h>
  #include <DallasTemperature.h>
  #include <LiquidCrystal.h>
  #include <LiquidCrystal_I2C.h>
  #include <Wire.h>
  #include <OneWire.h>

  #define pinOil 1
  #define pinSensors 10 //pin po katerem komunicirajo senzorji za temparaturo
  #define pinBoi 11
  #define pinPec 12
  #define debugMode 1
  

  LiquidCrystal_I2C lcd(0x27, 16, 2);// lcd display 16*2 pikslov //0x27 je naslov kr so A0 A1 in A2 povezani
  //lcd A4 (SDA), A5 (SCL)
  /*-----( Declare objects )-----*/
  // Setup a oneWire instance to communicate with any OneWire devices
  // (not just Maxim/Dallas temperature ICs)
  OneWire oneWire(pinSensors);
  
  // Pass address of our oneWire instance to Dallas Temperature.
  DallasTemperature sensors(&oneWire);

  //naslovi da vsakic dobimo prave senzorje
  //https://www.hacktronics.com/Tutorials/arduino-1-wire-address-finder.html
  //rabim se tretjega
  DeviceAddress addrPec = { 0x28, 0xC2, 0x9B, 0x3E, 0x07, 0x00, 0x00, 0x0F }; // pec
  DeviceAddress addrBoi = { 0x28, 0x77, 0x56, 0x3F, 0x07, 0x00, 0x00, 0xA2 }; // bojler
  DeviceAddress addrRet = { 0x28, 0x16, 0xEA, 0x69, 0x0A, 0x00, 0x00, 0xF9 }; // cev povratna topla voda
  
  
  void setup() {
	if(debugMode){
		Serial.begin(9600);
	}
    
    sensors.begin();
    lcd.begin();
    
    lcd.backlight(); //lcd.noBacklight() izklop
    pinMode(pinPec, OUTPUT);
    digitalWrite(pinPec, HIGH);
    pinMode(pinBoi, OUTPUT);
    digitalWrite(pinBoi, HIGH);
    pinMode(pinOil, INPUT_PULLUP);
    
  
    // nastavimo resulucijo od 9 do 12 bit (nižnja resolucija hitrejša meritev)
    sensors.setResolution(addrPec, 12);
    sensors.setResolution(addrBoi, 12);
  
  }

    float tempPec = 0, tempBoi = 0; //temparatura v peci in bojlerju
    bool statePec = 0, stateBoi = 0, stateOil = 1; //stanje relejev a crpalke

  void loop(){
	sensors.requestTemperatures(); //dobi temparature
	tempPec = getTemp(addrPec);
	tempBoi = getTemp(addrBoi);
	stateOil = !digitalRead(pinOil); // stanje olja
	  
    controlPec(); // logika
    controlBoi();
	
	if(debugMode)debug(); //just for debug purposes
    displayLCD();
  }
  
//short support functions
float getTemp(DeviceAddress deviceAddress){
	return sensors.getTempC(deviceAddress);
}
void setPec(bool state){
    digitalWrite(pinPec, state);
}	
void setBoiler(bool state){
    digitalWrite(pinBoi, state);
}
  
void displayLCD(){
    //spremenljivka lcd, velikost ekrana 16x2
	if ((tempPec <= 35) && (stateOil == 1)){
      lcd.noBacklight();
	  return;
    }
    else lcd.backlight();

    lcd.clear();
    
    lcd.setCursor(0, 0);// prva vrsta (pec)
    lcd.print("Pec:");
    lcd.print(tempPec);
    lcd.print("C  ");
    lcd.print(statePec==1?"ON":"OFF");

    lcd.setCursor(0, 1);// druga vrsta (bojler)
    lcd.print("Boi:");
    lcd.print(tempBoi);
    lcd.print("C  ");
    lcd.print(stateBoi==1?"ON":"OFF");
    }
	
void controlPec(){
    if (tempPec < 40)
    {
      statePec = 0;
    }
    if (tempPec >= 45)
    {
      statePec = 1;
    }
	setPec(statePec);
}
void controlBoi(){
    if (tempPec >= 50 && tempBoi < 70 && tempPec >= tempBoi + 5 || tempPec >= 100 || stateOil){
      stateBoi = 1;
    }
    else if((tempPec < 45 || tempBoi >= tempPec  || tempBoi >= 73) && !stateOil){
      stateBoi = 0;
    }
    setBoiler(stateBoi);
	  delay(2000); //wait for temperature wiggle
  }
  
void debug(){
	if (tempPec == -127.00 || tempBoi == -127.00) // Meritev nepravilna ali ni signala
    {
      Serial.println("Napaka na senzorju za temparaturo!!");
    }
	float tempPec = 0, tempBoi = 0; //temparatura v peci in bojlerju
    bool statePec = 0, stateBoi = 0, stateOil = 1; //stanje relejev a crpalke
	
	Serial.println("Temparature(Pec Boi): ");
	Serial.print(tempPec);
	Serial.print(" ");
	Serial.print(tempBoi);
	
	Serial.println("Stanja(Pec Boi Oil): ");
	Serial.print(statePec);
	Serial.print(" ");
	Serial.print(stateBoi);
	Serial.print(" ");
	Serial.print(stateOil);
}
