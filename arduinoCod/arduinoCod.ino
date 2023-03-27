#include <ESP8266WiFi.h> // Libreria para conectar el ESP8266 a WIFI
#include <strings_en.h>
#include <WiFiManager.h> // Hace que el ESP8266 se convierta en una "access Point" para configurar la red Wifi 
#include <DNSServer.h> // Crea un servidor Web
#include <ESP8266WebServer.h>

#include <Ticker.h>
#define pinLedWifi D4
Ticker ticker;

void blinkLed(){
  // Cambia el estado del LED
  byte estado = digitalRead(pinLedWifi);
  digitalWrite(pinLedWifi, !estado);
}

#include <LiquidCrystal_I2C.h> // Libreria para el manejo de la pantalla LCD.

// COMPONENTES:
//____________________PANTALLA LCD____________________
LiquidCrystal_I2C lcd(0x27,16,2); // Creamos un objeto para controlar los parametros (INFO) que se le envia al LED

//____________________ESP8266____________________
WiFiServer server(80); // puerto de comuncicacíon --> puerto 80: es para paginas no seguras

void setup() {
  // Inciamos la comunicación Serial
  Serial.begin(115200);

  // Modo del pin
  pinMode(pinLedWifi, OUTPUT);
  // Empezamos el temporizador que hará parpadear el LED
  ticker.attach(0.2, blinkLed);

  // ESP8266
  WiFiManager wifiManager; // Creamos un objeto de tipo WiFiManager
  // BORRA LAS CREDENCIALES DE LA RED WIFI 
  //wifiManager.resetSettings();
  // Creamos AP y portal cautivo
                          // (nombre, contraseña -> 8 a 64 caracteres)
  if (!wifiManager.autoConnect("ESP8266Temp")){// Busca la RED WIFI que el ESP8266 se habia conectado la ultima vez para conectarse nuevamente
    Serial.println("Fallo en la conexión (timeout)");
    ESP.reset();
    delay(1000);
  } 
  // Nota: El modulo NodeMCU guarda los datos de la red a la cual el dispositivo se conecto la ultima vez (CREDENCIALES); Si no se conecta va a levantar un acces point en la red
  Serial.println("Ya estás conectado");
  Serial.print("Conectado a la red Wifi:");
  Serial.println(WiFi.SSID());
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("macAddress: ");
  Serial.println(WiFi.macAddress());
  // Eliminamos el temporizador
  ticker.detach();
  // Apagamos el LED
  digitalWrite(pinLedWifi, HIGH);

  // Iniciamos el servidor local
  server.begin(); 

   // PANTALLA LCD
   lcd.init();
   lcd.backlight();
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Type a message");
}

void loop() {
  WiFiClient client = server.available(); // Creamos un cliente en el servidor (Usuario)
  // avalaible() detecta si existe un cliente
  if(!client){
    return; // Reinicia el ciclo "loop()" si no hay cliente
  }
  // Nota: Reinicia el ciclo en caso de que el cliente no se haya creado

  Serial.println("New client...");
  while(!client.available()){ // hace una pausa esperando a que el cliente envie una petición
    delay(1);
  }

  String request = client.readStringUntil('\r'); // lee el valor digitado por el cliente
  request = request.substring(request.indexOf("=") + 1,request.indexOf("HTTP/1.1"));
  request.replace("+"," ");
  Serial.println(request);
  client.flush(); // limpia la petición del cliente

  // Verificamos que el mensaje se envio correctamente
  if (request.indexOf("GET /") == -1){
    // Mostramos el mensaje en la pantalla
    lcd.clear();
    lcd.setCursor(15,0);
    lcd.print(request);
    for (int i= 0; i < 5; i++){
      for (int i=0; i <= sizeof(request); i++){
      lcd.scrollDisplayLeft();
      delay(400);
      }
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Type a message");
   }

  // ------------------------------------------
  client.println("HTTP/1.1 200 OK");
  client.println("");
  client.println("");
  client.println("");
  client.println("");

  // INICIAMOS LA PAGINA

  client.println("<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><meta http-equiv='X-UA-Compatible' content='IE=edge'><meta name='viewport' content='width=device-width, initial-scale=1.0'>");
  client.println("<title>Advertisement</title>");
  client.println("<style>html,body{height: 80%;}main{width: 80%;margin: auto;margin-top: 15%;text-align: center;font-family: sans-serif;}input[type='text']{padding: .4rem; min-width: 30%;width: 300px;margin: 1rem;}input[type='submit']{display: block;margin: auto;padding: .7rem;border-radius: .5rem;border: none;min-width: 20%;width: 300px;background-color: #007bff;font-weight: bold;color: #ffffff;font-size: 1rem;}@media screen and (max-width: 810px) {input[type='text'],input[type='submit']{max-width: 80%;min-width: 50%;width: none;}}</style></head>");
  client.println("<body><main><h1>LCD Screen</h1><section><p>Type whatever thing that you want to show on the Screen</p><form action='index.html' method='get'><input type='text' name='request' required><input type='submit' value='Show' onclick='alert('message Sended...')'></form></section></main></body></html>");

  // FIN DE LA PAGINA

  delay(10);
  Serial.println("Peticíon Finalizada...");
}
