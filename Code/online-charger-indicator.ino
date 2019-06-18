#include<SoftwareSerial.h>
#include <LiquidCrystal.h>

#define DEBUG true

int role=11;
int butonDurum=0;
int roleDurum=0;
int x=0;
int seviye=0;
int buton=0;

SoftwareSerial esp(3,2);
int status=0;

LiquidCrystal lcd(8,9,4,5,6,7);

void setup() {
  lcd.begin(16,2);
  pinMode(role,OUTPUT);
  
  Serial.begin(9600);
  
  esp.begin(9600);
  sendData("AT+RST\r\n",1000,DEBUG);
  sendData("AT+CWMODE=1\r\n",1000,DEBUG);
  sendData("AT+CWJAP=\"your-wifi-ssid\",\"your-wifi-password\"\r\n",5000,DEBUG);
  delay(3000);
  sendData("AT+CIFSR\r\n",3000,DEBUG);
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG);
  sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG);
}

void loop() {
  int sensorValue = analogRead(A3); 
  float voltage = sensorValue * (5.00 / 1090.00); 
  lcd.setCursor(0,0);
  lcd.print("Pil: %");
  seviye = (voltage*100)/3.70;
  lcd.print(seviye);
  
  if(esp.available()){
    if(esp.find("+IPD,")){
      delay(1000);
      
      int connectionId = esp.read()-48;
    
      String webpage = "<HEAD>";
      webpage+="<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'/>";
      webpage+="</HEAD>";
      webpage+="<h1>LiIon Seviye Gösterici</h1><br><h1>Pil Seviyesi: %";
      webpage.concat(seviye);
      webpage+="</h1>";
      webpage+="<br><td><a href=\"?pin=on\"><button><h1>Şarj Et</h1></button></a></td>";
      webpage+="<td><a href=\"?pin=off\"><button><h1>Şarjı Durdur</h1></button></a></td>";
      
      String cipSend = "AT+CIPSEND=";
      cipSend+=connectionId;
      cipSend+=",";
      cipSend+=webpage.length();
      cipSend+="\r\n";
      sendData(cipSend,1000,DEBUG);
      sendData(webpage,1000,DEBUG);

      String closeCommand = "AT+CIPCLOSE=";
      closeCommand+=connectionId;
      closeCommand+="\r\n";
      sendData(closeCommand,3000,DEBUG);
      delay(1000);
      }
   }
  
  if(analogRead(0) < 50)
    buton=1;
  else
    buton=0;
  
  butonDurum=buton;  
  if(butonDurum==HIGH && x==0){
    x=1;
    if(roleDurum==0)
      roleDurum=1;
    else if (roleDurum==1)
      roleDurum=0;
    }
  else if (butonDurum==LOW && x==1)
    x=0;
  if(roleDurum==1){
    digitalWrite(role,LOW);
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("Sarj Ediliyor.");
  }
  else{
    digitalWrite(role,HIGH);
    lcd.setCursor(0,1);
    lcd.print("Sarj Edilmiyor.");
  }
}

String sendData(String command, const int timeout, boolean debug){
  String response = "";
  esp.print(command); 
  long int time = millis();
  while( (time+timeout) > millis()){
    while(esp.available()){
      char c = esp.read();
      response+=c;
      }
    }  
  if(debug){
    Serial.print(response);
    int a=response.indexOf(":GET /?pin=on");
    Serial.print("-0-0-0-0-0-----a=");
    Serial.println(a);
    if(a>3){
      pinMode(role, OUTPUT);
      if(status==0){
        digitalWrite(role,LOW);
        roleDurum=1;
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("Sarj Ediliyor.");
      }
    }
    int b=response.indexOf(":GET /?pin=off");
    Serial.print("-0-0-0-0-0-----b=");
    Serial.println(b);
    if(b>3){
      pinMode(role, OUTPUT);
      if(status==0){
        digitalWrite(role,HIGH);
        roleDurum=0;
        lcd.setCursor(0,1);
        lcd.print("Sarj Edilmiyor.");
      } 
    }
  }
        
  return response;
 }
