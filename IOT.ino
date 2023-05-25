#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Arduino.h>
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "DCS"
#define WIFI_PASSWORD "12345678"

// For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "AIzaSyCCTjKWELtwx1XzAe0dM_1F-E-wlXYR_Lk"

/* 3. Define the RTDB URL */
#define DATABASE_URL "https://ledrgb-aba5c-default-rtdb.firebaseio.com/"  //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "a@kma.com"
#define USER_PASSWORD "123456"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

const int trigPin1 = 26;
const int echoPin1 = 27;

const int trigPin2 = 14; 
const int echoPin2 = 15;  

const int trigPin3 = 16;  
const int echoPin3 = 17;  

//define sound speed in cm/uS
#define SOUND_SPEED 0.034

long duration1;
long duration2;
long duration3;
float distanceCm1;
float distanceCm2;
float distanceCm3;

unsigned long sendDataPrevMillis = 0;
unsigned long timer = 0;
unsigned long timerSR = 0;
unsigned long timerLCD = 0;


Servo servoIn;
Servo servoOut;

#define SS_PIN 5
#define RST_PIN 2

MFRC522 mfrc522(SS_PIN, RST_PIN);

byte savedCard[3][4] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
char slot[] = "000111";
int emptyPakingLotNumber = 0;
int allPakingLotNumber = 0;

void displayEmptyPakingLotNumber() {
  if (emptyPakingLotNumber > 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Cho trong: ");
    lcd.print(emptyPakingLotNumber);
    lcd.print("/");
    lcd.print(allPakingLotNumber);
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Da het cho trong");
  }
}

void setup() {
  Serial.begin(9600);
  // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();
  pinMode(trigPin1, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoPin1, INPUT);   // Sets the echoPin as an Input
  pinMode(trigPin2, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoPin2, INPUT);   // Sets the echoPin as an Input
  pinMode(trigPin3, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoPin3, INPUT);   // Sets the echoPin as an Input
  SPI.begin();
  mfrc522.PCD_Init();
  servoIn.attach(12);
  servoOut.attach(13);
  servoIn.write(90);
  servoOut.write(90);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  unsigned long ms = millis();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;  // see addons/TokenHelper.

  // Limit the size of response payload to be collected in FirebaseData
  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);

  // Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);

  config.timeout.serverResponse = 10 * 1000;

  displayEmptyPakingLotNumber();
}

void loop() {
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 2000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    Serial.printf("Set string... %s\n", Firebase.RTDB.setString(&fbdo, F("/SmartParkingLot"), F(slot)) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Get int ref... %s\n", Firebase.RTDB.getInt(&fbdo, F("/emptyPakingLotNumber"), &emptyPakingLotNumber) ? String(emptyPakingLotNumber).c_str() : fbdo.errorReason().c_str());
    Serial.printf("Get int ref... %s\n", Firebase.RTDB.getInt(&fbdo, F("/allPakingLotNumber"), &allPakingLotNumber) ? String(allPakingLotNumber).c_str() : fbdo.errorReason().c_str());
  }

  if (millis() - timerLCD > 5000 || timerLCD == 0) {
    timerLCD = millis();
    displayEmptyPakingLotNumber();
  }

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial() && (millis() - timer > 1000 || timer == 0)) {
    timer = millis();
    if (mfrc522.uid.size != 4) {
      Serial.println("");
      lcd.setCursor(0, 0);
      lcd.clear();
      lcd.print("Khong the doc ID");
      lcd.setCursor(0, 1);
      lcd.print("     The tu     ");
      delay(3000);
      return;
    }
    byte cardKey[4];
    for (byte i = 0; i < 4; i++) {
      cardKey[i] = mfrc522.uid.uidByte[i];
    }
    if (memcmp(cardKey, savedCard[0], 4) == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("   Check out!");
      for (byte i = 0; i < 4; i++) {
        savedCard[0][i] = 0x00;
      }
      servoOut.write(0);
      delay(2000);
      servoOut.write(90);
    } else if (memcmp(cardKey, savedCard[1], 4) == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("   Check out!");
      for (byte i = 0; i < 4; i++) {
        savedCard[1][i] = 0x00;
      }
      servoOut.write(0);
      delay(2000);
      servoOut.write(90);
    } else if (memcmp(cardKey, savedCard[2], 4) == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("   Check out!");
      for (byte i = 0; i < 4; i++) {
        savedCard[2][i] = 0x00;
      }
      servoOut.write(0);
      delay(2000);
      servoOut.write(90);
    } else {
      int empty = -1;
      for (int i = 0; i < 3; i++) {
        if (savedCard[i][0] == 0x00 && savedCard[i][1] == 0x00) empty = i;
      }
      if (empty != -1) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("   Check in!");
        for (byte i = 0; i < 4; i++) {
          savedCard[empty][i] = cardKey[i];
        }
        // Lưu trữ ID thẻ từ vào bộ nhớ hoặc tệp tin
        servoIn.write(0);
        delay(2000);
        servoIn.write(90);
      } else {
        // het slot giui xe
      }
    }
    mfrc522.PICC_HaltA();
  }

  if ((millis() - timerSR > 2000 || timerSR == 0)) {
    timerSR = millis();
    // Clears the trigPin
    digitalWrite(trigPin1, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin1, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin1, LOW);

    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration1 = pulseIn(echoPin1, HIGH);

    // Calculate the distance
    distanceCm1 = duration1 * SOUND_SPEED / 2;

    // Prints the distance in the Serial Monitor
    Serial.print("Distance 1 (cm): ");
    Serial.println(distanceCm1);
    if (distanceCm1 < 11) {
      slot[0] = '1';
    } else {
      slot[0] = '0';
    }
    delay(100);

    // Clears the trigPin
    digitalWrite(trigPin2, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin2, LOW);

    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration2 = pulseIn(echoPin2, HIGH);

    // Calculate the distance
    distanceCm2 = duration2 * SOUND_SPEED / 2;

    // Prints the distance in the Serial Monitor
    Serial.print("Distance 2 (cm): ");
    Serial.println(distanceCm2);
    if (distanceCm2 < 11) {
      slot[1] = '1';
    } else {
      slot[1] = '0';
    }

    delay(100);

    // Clears the trigPin
    digitalWrite(trigPin3, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin3, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin3, LOW);

    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration3 = pulseIn(echoPin3, HIGH);

    // Calculate the distance
    distanceCm3 = duration3 * SOUND_SPEED / 2;

    // Prints the distance in the Serial Monitor
    Serial.print("Distance 3 (cm): ");
    Serial.println(distanceCm3);
    if (distanceCm3 < 11) {
      slot[2] = '1';
    } else {
      slot[2] = '0';
    }
  }
  
}
