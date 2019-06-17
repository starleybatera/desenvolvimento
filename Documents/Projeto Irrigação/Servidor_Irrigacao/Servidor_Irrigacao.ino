

// ATUALIZAÇÃO 12/06/2019

/*------------------------------------DECLARAÇÃO DOS INCLUDES----------------------------------------------------------------------------*/
#include <ESP8266WiFi.h>
#include <LinkedList.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <FirebaseArduino.h>  
#include <ArduinoJson.h>

/*------------------------------------VARIÁVEIS DEFINIDAS----------------------------------------------------------------------------*/
#define BLYNK_PRINT Serial 
#define dht_dpin D4
#define DHTTYPE    DHT11
#define FIREBASE_HOST "sistema-de-irrigacao.firebaseio.com"
#define FIREBASE_AUTH "W3C5Am1QITqQoAWhBMS5A9jyyuuTKDaaO1ybJJvW"

DHT dht(dht_dpin, DHTTYPE); 

/*------------------------------------DECLARAÇÃO DOS ATRIBUTOS----------------------------------------------------------------------------*/
int pinValvula_Sensor_1 = 5;
int pinValvula_Sensor_2 = 0;
int pinValvula_Bomba;
int pinLed = 14;
int pinLDR = A0;
int pinChuva = A0;
int luminosidade = 0;
int cont = 0;
int sizeTable = 0;
String unidade = "(h)Solo: ";
String percentagem = " %";
unsigned long tempoInicial = 0;
unsigned long intervalo = 1000;



/*------------------------------------CLASSE COM OS ATRIBUTOS REFERENTE A LISTA DE SENSORES.------------------------------------------------*/
class Sensores {
  public:
    float volt;
    int hum;
    String sen;
};

LinkedList<Sensores*> listaSensores = LinkedList<Sensores*>();
Sensores *sensor = new Sensores();
Sensores *irrigando = new Sensores();


/*--------------------------------------ESCOPO DAS FUNÇÕES------------------------------------------------------------------------------------*/

void AtualizaLeituraSensor(Sensores *S);// FUNÇÃO RESPONSÁVEL EM FAZER AS ANÁLISES SE PRECISA IRRIGAR A ÁREA DO SENSOR.
void RemoverSensorFila(int index); // FUNÇÃO RESPONSÁVEL EM REMOVER UM SENSOR DA FILA.
void ImprimirLista(); // FUNÇÃO RESPONSÁVEL EM IMPRIMIR OS ELEMENTOS EXISTENTE NA FILA.
void Irrigar(); // FUNÇÃO RESPONSÁVEL EM INICIAR E FINALIZAR OS ATUADORES E BOMBA D'ÁGUA.
void InserirSensorFila(Sensores *S);// FUNÇÃO RESPONSÁVEL EM INSERIR ELEMENTO NA LISTA.
boolean  BuscarSensor(String nomeSensor);// FUNÇÃO RESPONSÁVEL EM BUSCAR ELEMENTO NA LISTA.
int BuscarIndexSensor(String nomeSensor);// FUNÇÃO RESPONSÁVEL EM BUSCAR ELEMENTO NA LISTA.
void ParardeIrrigar(String setor);
void ControleIrrigacao();
boolean EstaIrrigando();
void AtualizarBanco(String sen, int hum, float volt);


/*--------------------------------------INICIALIZANDO AS VARIÁVEIS DE CONEXÃO------------------------------------------------------------------ */

char auth[] = "06efe120dc2b424ebb560303ec6f5eeb";
char ssid[] = "MERCUSYS_AEBA";
char pass[] = " ";



/*--------------------------------------DEFININDO AS PORTAS VIRTUAIS DOS WIGGET DO BLYNK---------------------------------------------------------*/
WidgetLCD lcdVirtual(V5);



/* --------------------------------------FUNÇÃO DO BLYNK QUE RECEBE AS INFORMAÇÕES DOS SENSORES E AS ATRIBUEM A UMA LISTA AUXILIAR (SENSOR)------*/

BLYNK_WRITE(V2){

  Sensores sensor;
  sensor.sen = param[0].asString();
  sensor.volt = param[1].asFloat() + 0;
  int umidade = map( param[2].asInt(),0,1024, 100, 0); 
  sensor.hum = umidade;

//  Serial.print(sensor.sen);
//  Serial.print(" : ");
//  Serial.print(sensor.hum);
//  Serial.print(" : ");
//  Serial.print(sensor.volt);
//  Serial.println("");

  
  AtualizarBanco(sensor.sen, sensor.hum, sensor.volt);

  
  if(irrigando != NULL && sensor.sen == irrigando->sen){
     *irrigando = sensor;
  }
  else{
     boolean achouSensor = BuscarSensor(sensor.sen);
     if(!achouSensor && sensor.hum < 20 ){
       InserirSensorFila(&sensor); 
       ImprimirLista();
     }
  }   

}
/*-----------------------------------------------FUNÇÃO SETUP -------------------------------------------------------------------------------------*/
void setup()
{ 
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  dht.begin();
  pinMode(pinValvula_Sensor_1, OUTPUT);
  pinMode(pinValvula_Sensor_2, OUTPUT);
  pinMode(pinLed, OUTPUT); 
  pinMode(pinLDR, INPUT);
  pinMode(pinChuva, INPUT);
  digitalWrite(pinValvula_Sensor_1, HIGH);
  digitalWrite(pinValvula_Sensor_2, HIGH);
  digitalWrite(pinLed, LOW);
  lcdVirtual.clear();
  Blynk.virtualWrite(V6,0);
  Blynk.virtualWrite(V7,0);
  Blynk.virtualWrite(V8, "clr");
  Blynk.virtualWrite(V8, "add", 1, "Lista de Espera", "Nível(h)");
  Blynk.virtualWrite(V8, "pick", 1);
  sizeTable++; 
  
  irrigando = NULL;
}

/*-----------------------------------------------FUNÇÃO LOOP -------------------------------------------------------------------------------------*/
void loop()
{
  unsigned long tempoCorrente = millis();

  if(tempoCorrente - tempoInicial > intervalo){
    
     tempoInicial = tempoCorrente;
      
        Blynk.run();
/*------SENSOR DE TEMPERATURA E UMIDADE DO AR--------------*/
        float h = dht.readHumidity();
        float t = dht.readTemperature();         
        
        Blynk.virtualWrite(V11,h);
        
        Blynk.virtualWrite(V10,t);
        Firebase.setFloat("sensor_H", h);
        Firebase.setFloat("sensor_T", t);
        delay(800);
  
/*------SENSOR DE CHUVA--------------------------------------*/            
       int chuva = analogRead(pinChuva);
        Blynk.virtualWrite(V12,chuva);
        lcdVirtual.clear();
        Firebase.setFloat("sensor_chuva", chuva);
        if(chuva < 600){
           lcdVirtual.print(4,0, "SISTEMA");
           lcdVirtual.print(2,1, "DESATIVADO !!");
           Blynk.virtualWrite(V6,0);
           Blynk.virtualWrite(V7,0);
           ParardeIrrigar("Geral");
          }

        if(chuva > 600){
           
           ControleIrrigacao();
              
           }     
  }

}

/*FUNÇÕES DO SISTEMA */

void ControleIrrigacao(){ // FUNÇÃO RESPONSÁVEL EM CONTROLAR O SISTEMA DE IRRIGAÇÃO
  
if(!EstaIrrigando() && listaSensores.size() > 0){ // NÃO ESTÁ IRRIGANDO E TAMANHO DA LISTA É MAIOR QUE 0
   irrigando = listaSensores.get(0);
   listaSensores.remove(0);
   sizeTable--;
   Blynk.virtualWrite(V8, "clr");
   Blynk.virtualWrite(V8, "add", 1, "Lista de Espera", "Nível(h)");
   Blynk.virtualWrite(V8, "pick", 1);
   Irrigar();
   for(int i = 0 ; i < listaSensores.size(); i++){ 
        Blynk.virtualWrite(V8, "add", i+2, listaSensores.get(i)->sen, unidade + listaSensores.get(i)->hum+percentagem); 
      }
  }
 
if(EstaIrrigando() && irrigando->hum >= 20){ // ESTÁ IRRIGANDO E  SENSOR TOTALMENTE ÚMIDO
  ParardeIrrigar(irrigando->sen);
  delete(irrigando);
  irrigando = NULL;
  ImprimirLista();
  delay(2000);
  lcdVirtual.clear();
  Blynk.virtualWrite(V6,0);
  Blynk.virtualWrite(V7,0);
}
if(EstaIrrigando() && irrigando->hum < 20){ // ESTÁ IRRIGANDO E SENSOR ATUALIZANDO VALORES DE UMIDADE
  lcdVirtual.print(4,0, irrigando->sen);
  lcdVirtual.print(3,1, "IRRIGANDO ...");
  Blynk.virtualWrite(V6, irrigando->hum);
  Blynk.virtualWrite(V7, irrigando->volt);
  delay(1000);
}
if(!EstaIrrigando() && listaSensores.size() == 0 ){ // NÃO ESTÁ IRRIGANDO E  TAM LISTA É IGUAL QUE 0
  lcdVirtual.print(4,0, "PIQUETES ");
  lcdVirtual.print(3,1, "IRRIGADOS !! ");
}
    
}

/*--------------------------------------FUNÇÃO QUE BUSCA DE UM ELEMENTO NA FILA----------------------------------------------------------------------------------*/

boolean BuscarSensor(String nomeSensor){   
  int i = 0;
  if(listaSensores.size() !=0){
     while(i < listaSensores.size()){
       if(listaSensores.get(i)->sen == nomeSensor){
        return true;
        }
       i++;
     }
  }
  return false;
}
/*----------------------------------FUNÇÃO DE INSERIR UM SENSOR NA FILA --------------------------------------------------------------------------------------*/

void InserirSensorFila(Sensores *S){ 


  Sensores *novoSensor = new Sensores();
  novoSensor->sen = S->sen;
  novoSensor->volt = S->volt;
  novoSensor->hum = S->hum;
  int i;
  listaSensores.add(novoSensor);
  
  for(i = 1 ; i <=listaSensores.size(); i++){ 
  }
  Blynk.virtualWrite(V8, "add", i, novoSensor->sen, unidade + novoSensor->hum+percentagem);
  sizeTable++;

}
/*-------------------------------------IMPRIMIR TODOS OS ELEMENTOS DA FILA DE SENSORES.---------------------------------------------------------------------------*/

void ImprimirLista(){ 
  Serial.println("");
  Serial.println("FILA DE SENSORES AGUARDANDO SER IRRIGADO: ");

  if(listaSensores.size() == 0){
   Serial.println("");
   Serial.println("NENHUM PIQUETE AGUARDANDO SER IRRIGADO");
   Serial.println("");
   Blynk.virtualWrite(V8, "clr");
  }
  else{
      for(int i = 0; i < listaSensores.size(); i++){
      
      Sensores *AUX  = listaSensores.get(i);
      
        Serial.println("");
        Serial.print("Index: ");
        Serial.print(i);
        Serial.print(": ");
        Serial.print(AUX->sen);
        Serial.print(" - ");
        Serial.print(" Umidade Solo: ");
        Serial.print(AUX->hum);
        Serial.print(" - ");
        Serial.print(" Tensao: ");
        Serial.print(AUX->volt);
        Serial.println(" ");
        Serial.println(".");
        Serial.println("--------------------------------------------------------");
        
      }   
  }   
} 
/*---------------------------------------------------FUNÇÃO RESPONSÁVEL EM INICIAR A IRRIGAÇÃO DOS PIQUETES. -------------------------------------------------------*/

void Irrigar(){

  Serial.print("IRRIGANDO PIQUETE DO: ");
  Serial.print(irrigando->sen);
  Serial.println("");
  Serial.println("--------------------------------------------------------");
  if(irrigando->sen == "SENSOR 1"){
  Serial.println("VALVULA 1 ABERTA");
  digitalWrite(pinValvula_Sensor_1, LOW);
  delay(1000);
  digitalWrite(pinValvula_Bomba, LOW);
  Serial.println("");
  Serial.println("--------------------------------------------------------");
   
  }
  if(irrigando->sen == "SENSOR 2"){
   Serial.println("VALVULA 2 ABERTA");
   digitalWrite(pinValvula_Sensor_2, LOW);
   delay(1000);
   digitalWrite(pinValvula_Bomba, LOW);
   Serial.println("");
   Serial.println("--------------------------------------------------------");
  }
}
/*------------------------------------------------------FUNÇÃO RESPONSÁVEL EM FINALIZAR A IRRIGAÇÃO DE UM PIQUETE OU TOTAL DO SISTEMA.----------------------------*/

void ParardeIrrigar(String setor){
  
  Serial.print("PARANDO DE IRRIGAR: ");
  
  if(irrigando != NULL){
    if(irrigando->sen == setor){
     Serial.print(irrigando->sen);
     Serial.println("  VALVULA 1 FECHADA");
     digitalWrite(pinValvula_Sensor_1, HIGH);
     delay(3000);
     digitalWrite(pinValvula_Bomba, HIGH);
     
    }
    if(irrigando->sen == setor){
     Serial.print(irrigando->sen);
     Serial.println("  VALVULA 2 FECHADA");
     digitalWrite(pinValvula_Sensor_2, HIGH);
     delay(3000);
     digitalWrite(pinValvula_Bomba, HIGH);
    }
  }
  
  if(setor == "Geral"){
   Serial.println("SISTEMA DESATIVADO !!");
   digitalWrite(pinValvula_Sensor_1, HIGH);
   digitalWrite(pinValvula_Sensor_2, HIGH);
   delay(3000);
   digitalWrite(pinValvula_Bomba, HIGH);
  }
}
/*------------------------------------------------------------------------------------------------------------------------*/

boolean EstaIrrigando(){
  
  return irrigando != NULL;
}

void AtualizarBanco(String sen, int hum, float volt){
  
// ATUALIZANDO DADOS NO BANCO DE DADOS FIREBASE

  if(sen.equals("SENSOR 1")){
    Firebase.setFloat("sensor1_H", hum);
    Firebase.setFloat("sensor1_T", volt);
    Serial.print("ENTROU NO IF SENSOR 1: ");
    Serial.print(sen);
    Serial.print(" : ");
    Serial.print(hum);
    Serial.print(" : ");
    Serial.println(volt);
    
  }
  if(sen.equals("SENSOR 2")){
    Firebase.setFloat("sensor2_H", hum);
    Firebase.setFloat("sensor2_T", volt);
    Serial.print("ENTROU NO IF SENSOR 2: ");
    Serial.print(sen);
    Serial.print(" : ");
    Serial.print(hum);
    Serial.print(" : ");
    Serial.println(volt);
  
  }  
}
