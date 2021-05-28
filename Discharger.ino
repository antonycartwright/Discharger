27/04/2017

#include <SPI.h>
#include <Adafruit_SSD1306.h>

#define BTN_1   2
#define BTN_2   3
#define PRE_LOAD_V   A0
#define POST_LOAD_V   A1
#define MOSFET  4
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13

Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

int mode;
int cellMah;
unsigned long lastButtonOnePress = 0;
unsigned long millisNow = 0;

float dischargeRate = 0;

float vcc = 0.0;

float startVoltage = 0;
float currentVoltage = 0;
float endVoltage = 0;

unsigned long lastADCSample = 0;

bool loadOn = false;

int buttonPressDelay = 125;

float preLoadVoltage = 0;
float postLoadVoltage = 0;
float loadVoltageDrop = 0;

void setup()   {
  Serial.begin(115200);

  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP);

  //Initiate
  display.begin(SSD1306_SWITCHCAPVCC);

  //Clears the Adafruit logo.
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(6, 9);
  display.println("Welcome.");
  display.display();
  delay(500);

  display.clearDisplay();
  display.setCursor(6, 9);
  display.println("Welcome..");
  display.display();
  delay(500);

  display.clearDisplay();
  display.setCursor(6, 9);
  display.println("Welcome...");
  display.display();
  delay(500);

  mode = 0;
  Serial.println("Mode: " + String(mode));
  display.setTextSize(1);
}

void loop() {
  millisNow = millis();

  switch (mode) {
    case 0:

      if (millisNow > lastButtonOnePress + buttonPressDelay) {
        if (!digitalRead(BTN_1)) {
          cellMah += 100;
          Serial.println("Mah: " + String(cellMah));
          if (cellMah >= 5000) {
            cellMah = 0;
          }
          delay(buttonPressDelay);
        }
        lastButtonOnePress = millis();
      }

      display.clearDisplay();
      display.setCursor(9, 9);
      display.println("Choose Cell MAh");
      display.setCursor(9, 18);
      display.println(MahToString(cellMah));
      display.display();

      if (!digitalRead(BTN_2)) {
        dischargeRate = (float)cellMah / 1000;
        Serial.println("Discharge Rate: " + String(dischargeRate));

        display.clearDisplay();
        display.setCursor(9, 9);
        display.println("Drain Max: " + String((float)4.2 / 3.4) + "a");
        display.setCursor(9, 18);
        display.println("Drain Min: " + String((float)3 / 3.4) + "a");
        display.display();

        mode = 1;
        Serial.println("Mode: " + String(mode));

        delay(buttonPressDelay);
      }

      break;

    case 1:
      if (!digitalRead(BTN_2)) {

        mode = 2;
        Serial.println("Mode: " + String(mode));

        //loadOn = true;
        delay(50);
        startVoltage = ReadVoltage(PRE_LOAD_V);
        delay(50);

        Serial.println("Start Voltage: " + String(startVoltage));

        display.clearDisplay();
        display.setCursor(9, 9);
        display.println("Start V: " + String(startVoltage) + "v");
        display.display();

        digitalWrite(MOSFET, HIGH);
        Serial.print("MOSFET: " + digitalRead(MOSFET));
        Serial.println(digitalRead(MOSFET));

        //Serial.println(loadOn);
        delay(buttonPressDelay);
      }
      break;

    case 2:

      if (!digitalRead(BTN_2)) {
        digitalWrite(MOSFET, !digitalRead(MOSFET));
        delay(buttonPressDelay);
        Serial.print("MOSFET: " + digitalRead(MOSFET));
        Serial.println(digitalRead(MOSFET));
      }

      if (millisNow > lastADCSample + 1000) {
        preLoadVoltage = ReadVoltage(PRE_LOAD_V);
        Serial.println("PreLV: " + String(preLoadVoltage));
        postLoadVoltage = ReadVoltage(POST_LOAD_V);
        Serial.println("PostLV: " + String(postLoadVoltage));
        loadVoltageDrop = preLoadVoltage - postLoadVoltage;
        Serial.println("VD: " + String(loadVoltageDrop));

        display.clearDisplay();
        display.setCursor(9, 2);
        display.println("Start V: " + String(startVoltage) + "v");
        display.setCursor(9, 12);
        display.println("Drain: " + String((loadVoltageDrop / 3.4) * 1000) + "ma");
        display.setCursor(9, 22);
        display.println("Current V: " + String(preLoadVoltage) + "v");

        display.display();

        lastADCSample = millisNow;
        break;
      }
  }
}

String MahToString(int mah) {
  String value;

  if (cellMah < 1) {
    value = "0000";
  } else if (cellMah < 10) {
    value = "00" + String(mah);
  }  else if (cellMah < 100) {
    value = "000" + String(mah);
  } else if (cellMah < 1000) {
    value = "0" + String(mah);
  } else {
    value = String(mah);
  }
  return value;
}

float ReadVoltage(int pin) {
  vcc = ReadVcc() / 1000.0;
  return analogRead(pin) * (vcc / 1024);
}

long ReadVcc() {
  long result;
  result = 5000;
  //  // Read 1.1V reference against AVcc
  //  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  //  delay(2); // Wait for Vref to settle
  //  ADCSRA |= _BV(ADSC); // Convert
  //  while (bit_is_set(ADCSRA, ADSC));
  //  result = ADCL;
  //  result |= ADCH << 8;
  //  result = 1125300L / result; // Back-calculate AVcc in mV
  return result;
}
