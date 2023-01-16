
//NIM: 5200411448 
//NAMA : Muhammad Nurjaman
//KELAS : Mobile Internet Of Things E 
//Dosen Pengampu : Ryan Ari Setyawan, S.Kom., M.Eng.


#include <WiFi.h>
#include <WiFiClient.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "ThingSpeak.h"
#define pinBuzzer 5

WiFiClient client;

String thingSpeakAddress = "api.thingspeak.com";
String writeAPIKey;
String tsfield1Name;
String request_string;

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);

Adafruit_MPU6050 mpu;

void setup()
{
  Serial.begin(115200);
  // Koneksi Wifi
  WiFi.disconnect();
  WiFi.begin("Wokwi-GUEST", "");
  while ((!(WiFi.status() == WL_CONNECTED))) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //koneksi mpu6050
  if (!mpu.begin()) {
    Serial.println("Gagal mendeteksi MPU6050 chip");
    while (1) {
      delay(1000);
    }
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("Pendeteksi Ready");
  // delay(1000);
  lcd.init();
  lcd.backlight();
 
}

//inisilisai type data nilai sensor
double x,y,z ;
double xg,yg,zg;
int t;

void loop()
{
  mpu6050();
  sendData_thingspeak(x,y,z,xg,yg,zg,t);
}

void mpu6050(){
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  //Acceleration
  x = a.acceleration.x;
  y = a.acceleration.y;
  z = a.acceleration.z;

// Gyroscope
  xg = g.gyro.x;
  yg = g.gyro.y;
  zg = g.gyro.y;

  t = temp.temperature;

  //Logic
  if ((x==0) && (y==0)){
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(" Aman Tidak Terjadi");
    lcd.setCursor(7, 2);
    lcd.print("GEMPA");
    Serial.print("Aman : ");
    Serial.print(x);
    Serial.print(",");
    Serial.print(y);
    Serial.print(",");
    Serial.print(z);
    Serial.print(", ");
    noTone(pinBuzzer);
  } 
  else{
    pinMode(pinBuzzer, OUTPUT);
    tone(5,300);
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(" TERJADI GEMPA !!!!");
    Serial.println("Perhatian Ada Gempa!!!");
    // Deklarasi Acceleration
    Serial.println("Acceleration : ");
    Serial.print(" X: ");
    Serial.print(x);
    Serial.print(", Y: ");
    Serial.print(y);
    Serial.print(", Z: ");
    Serial.print(z);
    Serial.println(" m/s^2");
    //Deklarasi Rotation
    Serial.println("Rotation : ");
    Serial.print(" X: ");
    Serial.print(xg);
    Serial.print(", Y: ");
    Serial.print(yg);
    Serial.print(", Z: ");
    Serial.print(zg);
    //Deklarasi Temperature
    Serial.println("Temperature");
    Serial.print("Temp :");
    Serial.print(t);
    Serial.println(" degC");
  }
  
}
//send data to thingspeak
void sendData_thingspeak(double x, double y,double z,double xg, double yg,double zg,double t) {
  if (client.connect("api.thingspeak.com", 80)) {
    request_string = "/update?";
    request_string += "key=";
    request_string += "X2P7EL48T55W65SO";
    request_string += "&";
    request_string += "field1";
    request_string += "=";
    request_string += x;
    request_string += "&";
    request_string += "field2";
    request_string += "=";
    request_string += y;
    request_string += "&";
    request_string += "field3";
    request_string += "=";
    request_string += z;

    request_string += "&";
    request_string += "field4";
    request_string += "=";
    request_string += xg;
    request_string += "&";
    request_string += "field5";
    request_string += "=";
    request_string += yg;
    request_string += "&";
    request_string += "field6";
    request_string += "=";
    request_string += zg;

    request_string += "&";
    request_string += "field7";
    request_string += "=";
    request_string += t;

    
  
//request get data
    Serial.println(String("GET ") + request_string + " HTTP/1.1\r\n" +
                 "Host: " + thingSpeakAddress + "\r\n" +
                 "Connection: close\r\n\r\n");
                 
    client.print(String("GET ") + request_string + " HTTP/1.1\r\n" +
                 "Host: " + thingSpeakAddress + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }

    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }

    Serial.println();
    Serial.println("Closing Connection");

  }
}