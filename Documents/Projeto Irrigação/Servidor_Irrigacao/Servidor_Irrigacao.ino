

#define BLYNK_PRINT Serial   
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <LinkedList.h>


float voltagem;
int umidade;
String sensor;
int cont = 0;
int numSensores = 2;




/*CLASSE DE SENSORES*/
class Sensores {
  public:
    float volt;
    int hum;
    String sen;
    int posicao;
};



LinkedList<Sensores*> listaSensores = LinkedList<Sensores*>();
Sensores *ListaSensoresAux;


/*INICIALIZANDO AS VARIÁVEIS DE CONEXÃO */

char auth[] = "06efe120dc2b424ebb560303ec6f5eeb";
char ssid[] = "MERCUSYS_AEBA";
char pass[] = " ";



/* DEFININDO AS PORTAS VIRTUAIS DOS WIGGET DO BLYNK */
WidgetLCD lcd(V3);
WidgetLED ledUm(V4); 
WidgetLED ledMUm(V5); 
WidgetLED ledSec(V6);


/*FUNÇÕES DE MANIPULAÇÃO DAS LISTA */


void InserirLista(){
  
  Sensores *l_sensor = new Sensores();
    l_sensor->volt = voltagem;
    l_sensor->hum = umidade;
    l_sensor->sen = sensor;

    if(listaSensores.size() == 0){
      l_sensor->posicao = cont;
      listaSensores.add(l_sensor);
    }
    
    else{        
          int i = 0;
          bool achou_sensor = false;
          
          while( i < listaSensores.size()){
               if(listaSensores.get(i)->sen == sensor){
                          achou_sensor = true;
                          listaSensores.get(i)->hum = umidade;
                          listaSensores.get(i)->volt = voltagem;
                }   
                i++;
            }

            if(achou_sensor == false){
              cont++;
              l_sensor->posicao = cont;
              listaSensores.add(l_sensor);
            }
            
        }

  }
void ImprimirLista(){
  
         int posicao = 1;
          Serial.println("FILA DE SENSORES");
           

          for(int i = 0; i < listaSensores.size(); i++){
        
              ListaSensoresAux = listaSensores.get(i);
        
              Serial.print("Posicao: ");
              Serial.print(ListaSensoresAux->posicao);
              Serial.print(" : ");
              Serial.print(ListaSensoresAux->sen);
              Serial.print(" - ");
              Serial.print(" Hum: ");
              Serial.print(ListaSensoresAux->hum);
              Serial.print(" - ");
              Serial.print(" Volt: ");
              Serial.print(ListaSensoresAux->volt);
              Serial.println(".");
          }
  }


boolean Irrigar(){

                    Serial.println("IRRIGANDO PASTO");

  
                    ledUm.off();
                    ledMUm.off();
                    ledSec.on();
                    
                    
                    lcd.clear();
                    lcd.print(4, 0, sensor);
                    lcd.print(0, 1, "Hum: ");
                    lcd.print(4, 1, umidade);
                    lcd.print(6, 1, "% ");
                    lcd.print(9, 1, "V: ");
                    lcd.print(11, 1, voltagem);

                    

                    

                   
   }
              

/* FUNÇÕES QUE RECEBEM AS INFORMAÇÕES DOS SENSORES */

BLYNK_WRITE(V7){

  sensor = param.asString();
   
}


BLYNK_WRITE(V2)
{
  voltagem = param.asFloat();
  
}

BLYNK_WRITE(V3)
{
   umidade = param.asInt();
   
   /*------------------CRIANDO E MANIPULANDO AS LEITURAS RECEBIDAS DOS SENSORES--------------------------------------*/
    
       if(listaSensores.size() < numSensores){
                 InserirLista();
       }
        ImprimirLista();

               
         
   
   /*-----------------------COMPARANDO DADOS DOS SENSORES---------------------------------------------*/          
             int i = 0;
   
              while( listaSensores.get(i)->hum >=0 && listaSensores.get(i)->hum <= 17){
                            
                            Irrigar();
                }
             

}



void setup()
{
  
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  
 
}

void loop()
{
  
           Blynk.run();

}




















/*
 else{
                           if(umidade > 17 && umidade <= 40){
          
                            ledUm.off();
                            ledMUm.on();
                            ledSec.off();
                            lcd.clear();
                            lcd.print(4, 0, sensor);
                            lcd.print(1, 1, "SOLO MEIO ÚMIDO");
                           
                           
                       }
                       if(umidade > 40 && umidade <= 70){
                            ledUm.on();
                            ledMUm.off();
                            ledSec.off();
                            lcd.clear();
                            lcd.print(4, 0, sensor);
                            lcd.print(0, 1, "Hum: ");
                            lcd.print(4, 1, umidade);
                            lcd.print(6, 1, "% ");
                            lcd.print(9, 1, "V: ");
                            lcd.print(11, 1, voltagem);
                           
                            
                            //Serial.print("H: ");
                            //Serial.println(voltagem);
                        }
        
                        if(umidade > 70){
                            ledUm.off();
                            ledMUm.off();
                            ledSec.off();
                            lcd.clear();
                            lcd.print(4, 0, sensor);
                            lcd.print(1, 1, "SOLO ENCHARCADO ");
                           
                        }
                        
                
                }



*/
  

