#include <LiquidCrystal_I2C.h> // Libreria para el manejo de la pantalla LCD.
#include <ESP8266WiFi.h> // Libreria para conectar el ESP8266 a WIFI

// COMPONENTES:
//____________________PANTALLA LCD____________________
LiquidCrystal_I2C lcd(0x27,16,2); // Creamos un objeto para controlar los parametros (INFO) que se le envia al LED

//____________________ESP8266____________________
// Información de la red:
String ssid = "ARRIS-7EEA"; // nombre de la red
String password = "ECA940417EEA"; // contraseña de la red
WiFiServer server(80); // puerto de comuncicacíon --> puerto 80: es para paginas no seguras

void setup() {
  // Inciamos la comunicación Serial
  Serial.begin(115200);
  Serial.println("\n");
  pinMode(D4,OUTPUT);

  // Conexión WIFI
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
    Serial.print("Conectado a la red Wifi:");
    Serial.println(WiFi.SSID());
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("macAddress: ");
    Serial.println(WiFi.macAddress());
    digitalWrite(D4, HIGH);

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
  while(!client.available()){ // hace una pausa esperando a que el cliente entre a la pagina
    delay(1);
  }

  String request = client.readStringUntil('\r'); // lee el valor digitado por el cliente
  request = request.substring(request.indexOf("=") + 1,request.indexOf("HTTP/1.1"));
  request.replace("+"," ");
  Serial.println(request);
  // client.flush(); // limpia la petición del cliente

  // Verificamos que el mensaje se envio correctamente
  if (request.indexOf("GET /") == -1){
    // Mostramos el mensaje en la pantalla
    lcd.clear();
    lcd.setCursor(10,0);
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
