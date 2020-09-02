#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

const char* ssid     = "iptime_6A";
const char* password = "kimhyunsu7291";
//const char* ssid     = "G8 ThinQ_7880";
//const char* password = "1234567890";

LiquidCrystal_I2C lcd(0x27, 16, 2);


int potPin = 0; //LM35센서로부터 입력을 받을 포트를 아날로그 A0핀으로 설정
float temperature = 0; //온도를 저장할 변수 선언
long val=0; //val 변수 선언


const char* host = "192.168.16.22";
const uint16_t port = 3000;


void setup() {

  // Setting for Wifi Connection
  Serial.begin(9600);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connecting to ");
  lcd.println(ssid);
  delay(2000);
  
  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(".");
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.println("WiFi connected");
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("IP address: ");
  lcd.println(WiFi.localIP());
  delay(1000);


  // Setting I2C LCD Screen
  // I2C LCD를 초기화
  lcd.init();
  // I2C LCD의 백라이트 on
  lcd.backlight();
 
}

void loop() {
  
  lcd.setCursor(0,0);
 

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, port)) {
    //Serial.println("connection failed");
    lcd.print("connection failed");
    delay(5000);
    lcd.clear();
    
    return;
  }

  for(int i=0; i<20; i++){
    lcd.setCursor(0,0);
    val = analogRead(potPin); //LM35센서로부터 불러온 값을 val변수에 저장
    //temperature = -40 + 0.488155 * (val - 20); //val값을 실제온도*10으로 변환
    temperature = (val*4.8828125); //val값을 실제온도*10으로 변환
    lcd.println("Temparature is : ");
    lcd.setCursor(0,1);
    lcd.print(((long)temperature / 10) - 10); //온도의 정수값표시
    lcd.print(".");    //소수점표시
    lcd.print( (long)temperature % 10); //소수점이하 온도 표시
    lcd.println("C"); //C표시 
    delay(500);
  }

//  //http://192.168.16.22:3000/gw/insert?id=abcd&temp=12 
//  
//   //This will send a string to the server
//
//   lcd.clear();
//   lcd.setCursor(0,0);
//   lcd.print("sending data to server");
//   delay(1000);
//   lcd.clear();
//  if (client.connected()) {
//    lcd.print("Requesting URL: ");
//    lcd.setCursor(0,1);
//    lcd.print("/gw/insert");
//    delay(2000);
//    lcd.clear();
//    //Serial.println("/dashboard/select");
//
//    client.print(String("GET ") + "/gw/insert?id=test&temp=" + ((temperature/10)-10) + " HTTP/1.1\r\n " + 
//                                   "Host: " + ssid + "Connection: close \r\n\r\n");
//    delay(1000);
//  }
//
//  // wait for data to be available
//  unsigned long timeout = millis();
//  while (client.available() == 0) {
//    if (millis() - timeout > 5000) {
//      lcd.setCursor(0,0);
//      lcd.print(">>> Client Timeout ! ");
//      //Serial.println(">>> Client Timeout !");
//      client.stop();
//      delay(60000);
//      lcd.clear();
//      return;
//    }
//  }
//
//  // Read all the lines of the reply from server and print them to Serial
//  //Serial.println("receiving from remote server");
//  lcd.print("receiving from");
//  lcd.setCursor(0,1);
//  lcd.print("remote server");
//  delay(1000);
//  // not testing 'client.connected()' since we do not need to send data here
////  while (client.available()) {
////    char ch = static_cast<char>(client.read());
////    lcd.clear();
////    lcd.setCursor(0,0);
////    //Serial.print(ch);
////    lcd.print(ch);
////    delay(1000);
////    lcd.clear();
////  }

  // Close the connection
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("closing connection");
    delay(1000);
    lcd.clear();
 //Serial.println("closing connection");
  client.stop();

  delay(10000); // execute once every 5 minutes, don't flood remote service
}
