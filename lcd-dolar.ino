#include <ArduinoJson.h>
#include <SPI.h>
#include <Ethernet.h>
#include <LiquidCrystal.h>
#define Luz_fundo 7
LiquidCrystal lcd(9, 8, 6, 5, 3, 2); // GND - RW, 12 - RS, 11 - E, 5 - D4, 4 - D5, 3 - D6, 2 - D7

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char server[] = "www.developers.agenciaideias.com.br";
boolean startRead = false;
String jsonString = "";
String dolar, euro;

IPAddress ip(192,168,0,166);

EthernetClient client;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(Luz_fundo, OUTPUT);
  digitalWrite(Luz_fundo, HIGH);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  atualizar_lcd("Conectando...", 0);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Network Error!");
    lcd.setCursor(0, 1);
    lcd.print("Restart 5 seg.");
    delay(5000);
    setup();
  }
  delay(1000);
}

void loop(){
  DynamicJsonBuffer jsonBuffer;
  
  jsonString = server_query();
  char jsonChar[jsonString.length()];
  jsonString.toCharArray(jsonChar, jsonString.length()-2);
  
  //Serial.print("String: ");
  //Serial.println(jsonChar);
  
  JsonObject& root = jsonBuffer.parseObject(jsonChar);
  if (root.success()) {
    String dolar_cotacao = root["dolar"]["cotacao"];
    dolar = "Dolar: ";
    dolar += dolar_cotacao;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(dolar);

    String euro_cotacao = root["euro"]["cotacao"];
    euro = "Euro: ";
    euro += euro_cotacao;
    lcd.setCursor(0,1);
    lcd.print(euro);
  }
  else{
    Serial.println("Erro!");
  }
  delay(60000);   
}

String server_query(){
  String jsonString;
  if (client.connect(server, 80)) {
    // Make a HTTP request:
    client.println("GET /cotacoes/json HTTP/1.1");
    client.println("Host: www.developers.agenciaideias.com.br");
    client.println("Connection: close");
    client.println();

    //Aguardando conexao
    while(!client.available()){
      delay(1);
    }
    
    //Percorre os caracteres do envelope HTTP do servidor e armazena na String apenas o conteudo JSON
    while (client.available()) {
    char c = client.read();
    if( c == '{' ) { 
      startRead = true; 
    }
    if ( startRead && c != '\n') { 
      jsonString += c; 
    }
  }
  
  //Reseta a flag de leitura de conteudo JSON
  startRead = false;
   
  } 
 else {
    // Caso nao ocorra conexao
    Serial.println("Connection fail!");
    atualizar_lcd("Connection fail!", 0);
  }
  
  //Aguarda a desconexao com o servidor
  while(client.connected()){
    delay(1);
  }
  
  // Finaliza o socket de conexao
  if (!client.connected()) {
    client.stop();
    
  }
  return jsonString; 
}

void atualizar_lcd(String value, int linha){
  lcd.clear();
  lcd.setCursor(0 ,linha);
  lcd.print(value);
}

