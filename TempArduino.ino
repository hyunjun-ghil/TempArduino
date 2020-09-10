#include "WiFiEsp.h"
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(2, 3); // RX, TX
#endif

#define SS_PIN 10
#define RST_PIN 9

char ssid[] = "iptime_6A";
char pass[] = "kimhyunsu7291";
char server[] = "192.168.16.17";
int status = WL_IDLE_STATUS;     // the Wifi radio's status
String TagID = "";

float temperature = 0; //온도를 저장할 변수 선언
float temperatureC = 0;
int val = 0; //val 변수 선언


LiquidCrystal_I2C lcd(0x27, 16, 2);

MFRC522 mfrc522(SS_PIN, RST_PIN); // Instance of the class

WiFiEspClient client;

void setup()
{
  pinMode(7, OUTPUT);
  Serial.begin(9600);
  Serial1.begin(9600);

  WiFi.init(&Serial1);
  SPI.begin();       // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("You're connected to the network");


  printWifiStatus();



  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("Ip Address");
  lcd.setCursor(0, 1);
  lcd.println(WiFi.localIP());
  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Show your card:)");

}

void loop()
{
  digitalWrite(7, LOW);
  TagID = "";

  if ( mfrc522.PICC_IsNewCardPresent())
  {
    if ( mfrc522.PICC_ReadCardSerial())
    {
      lcd.clear();
      Serial.print("Tag UID:");
      lcd.setCursor(0, 0);
      lcd.print("Tag UID:");
      lcd.setCursor(0, 1);

      for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        lcd.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        lcd.print(mfrc522.uid.uidByte[i], HEX);
        TagID.concat(mfrc522.uid.uidByte[i]);
      }

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("your id is");
      lcd.setCursor(0, 1);
      lcd.println(TagID);
      delay(2000);

      Serial.println();
      delay(3000);

      lcd.clear();
      lcd.setCursor(0, 0);
      Serial.println("============================");
      mfrc522.PICC_HaltA();

      delay(500);

      for (int i = 0; i < 10; i++) {
        lcd.setCursor(0, 0);
        val = analogRead(0); //LM35센서로부터 불러온 값을 val변수에 저장
        //temperature = -40 + 0.488155 * (val - 20); //val값을 실제온도*10으로 변환
        //temperature = (val * 5.0); //val값을 실제온도*10으로 변환
        //temperature /= 1024.0;
        lcd.println("Temparature is : ");
        lcd.setCursor(0, 1);
        temperatureC = val - 50;
        if (temperatureC > 30) {
          digitalWrite(7, HIGH);
        }
        else {
          digitalWrite(7, LOW);
        }
        lcd.print(temperatureC); //온도의 정수값표시
        lcd.println("C"); //C표시
        delay(1000);
      }

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.println("Your temp is:");
      lcd.setCursor(0, 1);
      lcd.println(temperatureC);
      delay(2000);

      // if you get a connection, report back via serial
      if (client.connect(server, 3000)) {
        Serial.println("Connected to server");
        client.println("GET /test/insert?id=" + TagID + "&temp=" + temperatureC + " HTTP/1.1");
        client.println("Host: 192.168.16.17");
        client.println("Connection: close");
        client.println();
      }

      String response = "";
      bool begin = false;
      while (client.available() || !begin) {

        char in = client.read();

        if (in == '{') {
          begin = true;
        }

        if (begin) response += (in);
      
        if (in == '}') {
          break;
        }

        delay(1);
      }
      Serial.println(response);
      String response1 = response.substring(0, 10);
      String response2 = response.substring(11, 25);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.println(response1);
      lcd.setCursor(0,1);
      lcd.println(response2);
      delay(2000);
      
      reset_state();
    }

  }
  //  while (client.available()) {
  //    char c = client.read();
  //    Serial.write(c);
  //  }
  //
  //  // if the server's disconnected, stop the client
  //  if (!client.connected()) {
  //    Serial.println();
  //    Serial.println("Disconnecting from server...");
  //    client.stop();
  //
  //    // do nothing forevermore
  //    while (true);
  //  }
}


void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


void reset_state()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Show your card:)");
}
