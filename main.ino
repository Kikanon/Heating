  #include <LiquidCrystal.h>
  #include <DallasTemperature.h>
  #include <LiquidCrystal.h>
  #include <LiquidCrystal_I2C.h>
  #include <Wire.h>
  #include <OneWire.h>
  
  #define ONE_WIRE_BUS 10 //bus senzorji
  float pec, boi;
  bool stanje, stanjeb;
  LiquidCrystal_I2C lcd(0x27, 16, 2);
  //lcd A4 (SDA), A5 (SCL)
  /*-----( Declare objects )-----*/
  // Setup a oneWire instance to communicate with any OneWire devices
  // (not just Maxim/Dallas temperature ICs)
  OneWire oneWire(ONE_WIRE_BUS);
  int inPin = 1, olje = 1;
  // Pass address of our oneWire instance to Dallas Temperature.
  DallasTemperature sensors(&oneWire);
  
  DeviceAddress Probe01 = { 0x28, 0xC2, 0x9B, 0x3E, 0x07, 0x00, 0x00, 0x0F }; // pec
  DeviceAddress Probe02 = { 0x28, 0x77, 0x56, 0x3F, 0x07, 0x00, 0x00, 0xA2 }; //bojler
  
  
  void setup() {
    Serial.begin(9600);
    lcd.begin();
    lcd.backlight();
    //lcd.noBacklight() izklop
    pinMode(12, OUTPUT);
    digitalWrite(12, HIGH);
    pinMode(11, OUTPUT);
    digitalWrite(11, HIGH);
    pinMode(inPin, INPUT_PULLUP);
    sensors.begin();
  
    // nastavimo resulucijo od 9 do 12 bit (nižnja resolucija hitrejša meritev)
    sensors.setResolution(Probe01, 12);
    sensors.setResolution(Probe02, 12);
  
  }
  
  void loop()
  {
    merjenje_temperature();
    krmilje();
    krmiljeb();
    ledlcd();
    olje = digitalRead(inPin);
  }
  
  /* podzanke*/
  void merjenje_temperature()
  {
    sensors.requestTemperatures();
  
    lcd.setCursor(0, 0); //začne pisati  0 v linji 0
    lcd.clear();
    lcd.print("1:");
    displayTemperature(Probe01);  //gre v nasledni podprogram
  
    lcd.setCursor(0, 1); //začne pisati 0 v linji 1
    lcd.print("2:");
    displayTemperature(Probe02);
  
    //delay(1500);
  }
  
  void displayTemperature(DeviceAddress deviceAddress)
  {
    float tempC = sensors.getTempC(deviceAddress);
  
    if (tempC == -127.00) // Meritev nepravilna ali ni signala
    {
      lcd.print("Temperature Error");
      Serial.println("error");
    }
    else
    {
      // Serial.print("temperatura: ");
      // Serial.println(tempC);
      lcd.print("C=");
      lcd.print(tempC);
      lcd.print("C");
    }
    if ((deviceAddress) == Probe01)
    {
      Serial.print("pec:");
      pec = tempC;
      Serial.print(pec);
    }
  
    if ((deviceAddress) == Probe02)
    {
      Serial.print(" ,bojler:");
      boi = tempC;
      Serial.println(boi);
    }
  
  }
  

  void krmilje()
  {
    lcd.setCursor(10, 0);
    lcd.print("pec");
    if (pec < 40)
    {
      stanje = 0;
    }
    if (pec >= 45)
    {
      stanje = 1;
    }
    stan();
  }
  void krmiljeb()
  {
    lcd.setCursor(10, 1);
    lcd.print("boi");
    if ((((pec >= 50) && (boi < 70) && (pec - boi >= 5 )) || (pec >= 100)) || (olje == 0)){
      //nea gre zadno cez 100 kr je spoder nastavleno da pr 73 izklop ali ce je vkloplen saltar za olno pec
      stanjeb = 1;
    }
    else if ((((boi >= 73) && (pec < 85)) || (pec < 45) || (boi >= pec )) || (boi >= 73) || (olje == 1)){
      //izklop pri temperaturah ali ce je temperatura in olje izklopljeno
      stanjeb = 0;
    }
    stanb();
    delay (1500);
  }
  void ledlcd()
  {
    if ((pec > 38) || (olje == 0))
    {
      lcd.backlight();
    }
    else if ((pec <= 35) && (olje == 1))
    {
      lcd.noBacklight();
    }
  
  }
  void stan()
  {
    if (stanje == 1 || (olje == 0)){
      lcd.print("on");
      digitalWrite(12, 0);
    }
    else if (stanje == 0 && (olje == 1)){
      lcd.print("off");
      digitalWrite(12, 1);
    }
  }
  void stanb()
  {
    if (stanjeb == 1){
      lcd.print("on");
      digitalWrite(11, 0);
    }
    else if (stanjeb == 0){
      lcd.print("off");
      digitalWrite(11, 1);
    }
  }
