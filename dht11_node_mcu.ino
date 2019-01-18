#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

WiFiClient client;

const char *ssid =  "XXXXXXXX";     // replace with your wifi ssid and wpa2 key
const char *pass =  "XXXXXXXX";
const int R = D7;
const int V = D6;
const int B = D5;



void setup()
{
  lcd.init();
  lcd.backlight();
  Serial.begin(115200);
  dht.begin();
  Serial.println("Connecting to ");
  Serial.println(ssid);


  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  pinMode(R, OUTPUT);
  pinMode(V, OUTPUT);
  pinMode(B, OUTPUT);
}

void loop() {
  delay(5000);
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT Sensor");
    return;
  }

  Serial.print("Temperature:");
  Serial.print(t);
  Serial.print("*C/t");
  Serial.print("Humidite:");
  Serial.print(h);
  Serial.println("%");

  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(t);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Humidite:");
  lcd.print(h);
  lcd.println(" %");


  if (t < 20)
  {
    digitalWrite(R, LOW);
    digitalWrite(V, LOW);
    digitalWrite(B, HIGH);
  }

  if ((t > 19) && (t < 26))
  {
    digitalWrite(R, LOW);
    digitalWrite(V, HIGH);
    digitalWrite(B, LOW);
  }

  if (t > 25)
  {
    digitalWrite(R, HIGH);
    digitalWrite(V, LOW);
    digitalWrite(B, LOW);
  }

  HTTPClient http;
  http.begin("http://51.75.126.48/dht11/store_temp.php");
  http.addHeader("Content-Type", "application/json");
  String data = "{\"temperature\":\"" + String(t) +"\",\"humidite\":\""+ String(h) +"\"}";
  int httpCode = http.POST(data);
  http.end();

  Serial.println("Waiting...");

  // thingspeak needs minimum 15 sec delay between updates.
  delay(10000);
}
