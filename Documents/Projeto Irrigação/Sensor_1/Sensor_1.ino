// ATUALIZAÇÃO 28/04/2019


#define BLYNK_PRINT Serial    
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>

SimpleTimer timer;

#define pinUmiA A0
#define pinUmiD 5

unsigned long tempoInicial = 0;
unsigned long intervalo = 30 * 1000; 
int umidade = 0;
int umidade_media = 0;
int contador = 0;


uint32_t tempo_sono = 900 * 1000000; // Definindo 15 min de descanso para o sensor.

WidgetBridge bridge1(V1); // Definindo o objeto que criar o link com o servidor.

char auth[] = "1b8995893c0c4ee7851d33cd873771a9"; 
char ssid[] = "MERCUSYS_AEBA";
char pass[] = " ";


void Sensor(); // FUNÇÃO RESPONSÁVEL EM ENVIAR AS INFORMAÇÕES DE LEITURA DO SENSOR AO SERVIDOR.
void Sleep(); // FUNÇÃO RESPONSÁVEL EM EXECUTAR O TEMPO DE DESCANSO DO MICROCONTROLADOR.

BLYNK_CONNECTED() {
  bridge1.setAuthToken("06efe120dc2b424ebb560303ec6f5eeb"); 
}
 
void setup(){
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  pinMode(pinUmiA, INPUT);
  pinMode(pinUmiD, OUTPUT);
  digitalWrite(pinUmiD, LOW);
}

void loop(){
  Blynk.run();
  timer.run();
  unsigned long tempoCorrente = millis();
      
  if(tempoCorrente - tempoInicial > intervalo){
     tempoInicial = tempoCorrente;
     contador = 0;
     Sleep();
  }
  else{
     umidade = 0;
     umidade_media =0;
    for(int i = 0; i < 500 ; i++){
       umidade += analogRead(A0); // Recebedo os valores da porta analógica do ESP atribuindo-as a variável umidade.
      }
      umidade_media = (umidade/500);// Gerando a média das leituras.
      contador++;
      Sensor();
      delay(2000);
     
  }
}
void Sensor(){ 
  
  float tensao = (umidade_media * (5 / 1024.0));       
  Serial.println("");
  Serial.print("TENSAO: ");
  Serial.print(tensao);
  Serial.print("  UMIDADE DO SOLO: ");
  Serial.println(umidade_media);            
  bridge1.virtualWrite(V2,"SENSOR 1", tensao, umidade_media); // Enviando as leituras para o BLYNK_WRITE (V2) do Servidor para que sejam analisadas.
  Blynk.virtualWrite(V6,umidade_media);
  //Blynk.virtualWrite(V7,tensao);
  
}

void Sleep(){ 
  Serial.println("");
  Serial.println("SENSOR-1 ENTRANDO EM MODO DE DESCANSO");
  ESP.deepSleep(tempo_sono);
  
}
