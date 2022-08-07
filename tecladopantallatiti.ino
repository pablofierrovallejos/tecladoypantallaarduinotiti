#include "GFButton.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <Wire.h>
#include "RTClib.h"
//#include <TimerOne.h>

RTC_DS3231 rtc;
String daysOfTheWeek[7] = {"Domingo", "Lunes",   "Martes", "Miercoles", "Jueves", "Viernes", "Sabado" };
String monthsNames[12]  = {"EN",   "FB", "MR",  "AB",     "MY",   "JN",   "JL",  "AG", "SP","OC","NV","DI" };


// Create button instances 
GFButton button1(49);   GFButton button2(48);  
GFButton button3(47);   GFButton button4(46);  
GFButton button5(45);   GFButton button6(44);  
GFButton button7(43);   GFButton button8(42);
GFButton button9(41);   GFButton button10(40); 
GFButton button11(39);  GFButton button12(38); 
GFButton button13(37);  GFButton button14(36); 
GFButton button15(35);  GFButton button16(34); 
GFButton button17(33);  GFButton button18(32); 
GFButton button19(31);  GFButton button20(30); 
GFButton button21(29);  GFButton button22(28);
GFButton button23(27);  GFButton button24(26);
GFButton button25(25);  GFButton button26(24);
GFButton button27(23);  GFButton button28(22);
GFButton button29(13);  GFButton button30(12);
GFButton button31(9);   GFButton button32(8);
GFButton button33(7);   GFButton button34(6);
GFButton button35(5);   GFButton button36(4);
GFButton button37(3);   GFButton button38(2);
GFButton button39(A11); GFButton button40(A12);
GFButton button41(A13); GFButton button49(A4);  
GFButton button50(A3);  GFButton button51(A2);  
GFButton button52(A1);



SoftwareSerial mySoftwareSerial(10, 11); // RX, TX para comunicacion con dfplayer
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
void escribeLienzo(char cletra, int idelay);
void calculaSuma();
void getInfoPlayer();
int getUltimaCancion();
void cargaOperando(int op);
void playNumerosConvertido(int numero);
void escribePalabraRandom();


//Vcc - Vcc
//Gnd - Gnd
//Din - Mosi (Pin 11)
//Cs  - SS (Pin 10)
//Clk - Sck (Pin 13) 
//       CS       DIN                    CLK     
//UNO    SS:10    MOSI:11    MISO:12     SCK:13
//MEGA   SS:53    MOSI:51    MISO:50     SCK:52

const int pinCS = 53;       //Pin para seleccionar pantalla
const int numberOfHorizontalDisplays = 16;
const int numberOfVerticalDisplays = 1;

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

const int wait = 230;            // In milliseconds velocidad para avanzar el texto
const int spacer = 5;            // Espacio entre letras
const int width = 5 + spacer;    // The font width is 5 pixels

//*******************************************************************************
//Bloque variables globale de control del programa
int espaciadoPF = 6; 
int PosxPF = 0;     //Posicion actual de la letra a escribir en pantalla
int totletras = 0;  //Cuando hay mas de 20 letras en pantalla la limpia
int funcionTeclasAvance   = 0;      //Cambia al presionar tecla de sonrisa
//                         0 = Funcion Avanza canciones español
//                         1 = Funcion Avanza canciones ingles
//                         2 = Funcion Avanza frases en español
//                         3 = Funcion Avanza frases en ingles   
int esadoActualAvance    = 0;
//                         0 = Indica que no ha presionado avance ni retroceso
//                         1 = Indica que ya esta avanzando o retrocediendo  
int lenguajeFrases       = 0;
//                         0 = Español
//                         1 = Ingles 
//*******************************************************************************
//Bloque sonidos  ABECEDARIO EN ESPAÑOL
int primeraLetraESP   = 1;
int ultimaLetraESP    = 27;
//Bloque sonidos  ABECEDARIO EN INGLES
int primeraLetraENG   = 28;
int ultimaLetraENG    = 53;
//*******************************************************************************
//Bloque sonidos CANCIONES ESPAÑOL
int primeraCancionESP    = 412;
int ultimaCancionESP     = 439;
int actualCancionESP     = 0;
//*******************************************************************************
//Bloque sonidos CANCIONES INGLES
int primeraCancionENG    = 440;
int ultimaCancionENG     = 441;
int actualCancionENG     = 0;
//*******************************************************************************
//Bloque sonidos FRASES ESPAÑOL 
int primeraFraseESP = 254;
int ultimaFraseESP  = 304;
int actualFraseESP    = 0;
//*******************************************************************************
//Bloque sonidos FRASES EN INGLES
int primeraFraseENG = 305;
int ultimaFraseENG  = 411;
int actualFraseENG    = 0;
//*******************************************************************************
//Bloque sonidos NUMEROS ESPAÑOL del 1 al 100 
int primerNumeroESP = 54;
int ultimoNumeroESP = 153;
//*******************************************************************************
//Bloque sonidos NUMEROS INGLES  del 1 al 100
int primerNumeroENG = 154;
int ultimoNumeroENG  = 253;
//*******************************************************************************
//Bloque de variables para controlar la suma
int sumando1= 0;
int sumando2= 0;
char operador=' ';
int countmas= 0;
int ivolumen = 0;
//*******************************************************************************
String frases[110];//frases a mostrar dinamicamente
boolean primerCambioFuncion = true; 
boolean necesitaResetScreen = false;
boolean esEspaniol = true;
//playFolder(15, 4);
boolean primerCiclo = true;
boolean muestraHora = true;
boolean detienePlayer = false;
boolean Alarma1 = true;
boolean Alarma2 = true;
boolean Alarma3 = true;
boolean Alarma4 = true;

void setup() {
  //Timer1.initialize(5000000);         // Dispara cada 1 segundo reloj
  //Timer1.attachInterrupt(mainTime);   // Activa la interrupcion y la asocia a ISR_Blink
  //Timer1.stop();
  attachInterrupt(digitalPinToInterrupt(2), blink, RISING);  // Interrupcion tecla STOP para romper el ciclo de la hora

  //Esto es para que la pantalla se vaya a blanco y no quede encendida mientras se inicializan las bibliotecas lo que produce exceso de corriente.  
  pinMode(53, OUTPUT);    
  digitalWrite(53, LOW); 
  pinMode(51, OUTPUT);    
  digitalWrite(51, LOW); 
  pinMode(50, OUTPUT);    
  digitalWrite(50, LOW); 
  pinMode(52, OUTPUT);    
  digitalWrite(52, LOW); 
  
  //       CS       DIN                    CLK     
  //MEGA   SS:53    MOSI:51    MISO:50     SCK:52

   totletras = 0;

   setearPantalla();

   delay(6000);
   // Prepare serial interface
   Serial.begin(9600);
   mySoftwareSerial.begin(9600);                  //Inicia puerto serial para dfplayer
   if (!myDFPlayer.begin(mySoftwareSerial)) {     //Use softwareSerial to communicate with mp3.
      Serial.println(F("Unable to begin:"));     
      while(true);
   }
   myDFPlayer.volume(10);  //Set volume value. From 0 to 30
   ivolumen = 10;
   
   myDFPlayer.play(457);


   
   if (!rtc.begin()) { // INICIA MODULO RTC
      Serial.println(F("Couldn't find RTC"));
   }else{
      Serial.println(F("RTC inicializado"));
   }
   if (rtc.lostPower()) {   // Si se ha perdido la corriente, fijar fecha y hora
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Fijar a fecha y hora de compilacion
      Serial.println(F("RTC hora fijada"));
      // Fijar a fecha y hora específica. En el ejemplo, 21 de Enero de 2016 a las 03:00:00        // rtc.adjust(DateTime(2016, 1, 21, 3, 0, 0));
   }

   dibujaBienvenida();
   InicializaArregloPalabras(2); //Inicializa vector con lenguaje español
}

void loop() {
        if( primerCiclo == true  ){
          
          primerCiclo = false;
        }
        // Check if  button is pressed
        if (button1.wasPressed())  {    Serial.println(F("9 was pressed"));  resetScreenCondicional(); escribeLienzo('9',wait);  if(esEspaniol){  myDFPlayer.play(451) ; }else{  myDFPlayer.play(162);}  cargaOperando(9);} 
        if (button2.wasPressed())  {    Serial.println(F("8 was pressed"));  resetScreenCondicional(); escribeLienzo('8',wait);  if(esEspaniol){  myDFPlayer.play(450) ; }else{  myDFPlayer.play(161);}  cargaOperando(8);} 
        if (button3.wasPressed())  {    Serial.println(F("7 was pressed"));  resetScreenCondicional(); escribeLienzo('7',wait);  if(esEspaniol){  myDFPlayer.play(449) ; }else{  myDFPlayer.play(160);}  cargaOperando(7);}  
        if (button4.wasPressed())  {    Serial.println(F("6 was pressed"));  resetScreenCondicional(); escribeLienzo('6',wait);  if(esEspaniol){  myDFPlayer.play(448) ; }else{  myDFPlayer.play(159);}  cargaOperando(6);} 
        if (button5.wasPressed())  {    Serial.println(F("5 was pressed"));  resetScreenCondicional(); escribeLienzo('5',wait);  if(esEspaniol){  myDFPlayer.play(447) ; }else{  myDFPlayer.play(158);}  cargaOperando(5);} 
        if (button9.wasPressed())  {    Serial.println(F("4 was pressed"));  resetScreenCondicional(); escribeLienzo('4',wait);  if(esEspaniol){  myDFPlayer.play(446) ; }else{  myDFPlayer.play(157);}  cargaOperando(4);} 
        if (button10.wasPressed()) {    Serial.println(F("3 was pressed"));  resetScreenCondicional(); escribeLienzo('3',wait);  if(esEspaniol){  myDFPlayer.play(445) ; }else{  myDFPlayer.play(156);}  cargaOperando(3);} 
        if (button11.wasPressed()) {    Serial.println(F("2 was pressed"));  resetScreenCondicional(); escribeLienzo('2',wait);  if(esEspaniol){  myDFPlayer.play(444) ; }else{  myDFPlayer.play(155);}  cargaOperando(2);} 
        if (button12.wasPressed()) {    Serial.println(F("1 was pressed"));  resetScreenCondicional(); escribeLienzo('1',wait);  if(esEspaniol){  myDFPlayer.play(443) ; }else{  myDFPlayer.play(154);}  cargaOperando(1);} 
        if (button13.wasPressed()) {    Serial.println(F("0 was pressed"));  resetScreenCondicional(); escribeLienzo('0',wait);  if(esEspaniol){  myDFPlayer.play(442) ; }else{  myDFPlayer.play(456);}  cargaOperando(0);} 
        if (button50.wasPressed()) {    Serial.println(F("+ was pressed"));  resetScreenCondicional(); 
                                               if(sumando1==0){
                                                      int tmp = 0;
                                                      countmas++;
                                                      if(countmas < 0){
                                                           escribeLienzo('-',wait);
                                                           myDFPlayer.play(454); 
                                                           delay(1000);
                                                           tmp =  countmas * -1;
                                                           playNumerosConvertido(tmp);
                                                      }else{
                                                          playNumerosConvertido(countmas);
                                                      }  
                                               }else{
                                                      escribeLienzo('+',wait);  myDFPlayer.play(453) ; operador = '+'; 
                                               }   
        }
        if (button51.wasPressed()) {    Serial.println(F("- was pressed"));  resetScreenCondicional(); 
                                               if(sumando1==0){
                                                      int tmp = 0;
                                                      countmas--;
                                                      if(countmas < 0){
                                                           escribeLienzo('-',wait);
                                                           myDFPlayer.play(454); 
                                                           delay(1000);
                                                           tmp =  countmas * -1;
                                                           playNumerosConvertido(tmp);
                                                      }else{
                                                          playNumerosConvertido(countmas);
                                                      }  
                                               }else{
                                                      escribeLienzo('-',wait);  myDFPlayer.play(454) ; operador = '-';
                                               }
        }                                        
        if (button52.wasPressed()) {    Serial.println(F("= was pressed"));  resetScreenCondicional(); escribeLienzo('=',wait);  myDFPlayer.play(455) ; delay(1000); calculaSuma();}

        int indxAbc = 0;
        if (esEspaniol==true){
            indxAbc =  primeraLetraESP;
        }else {
            indxAbc =  primeraLetraENG;
        }
        if (button14.wasPressed()) {    Serial.println(F("Z was pressed"));  resetScreenCondicional(); escribeLienzo('Z',wait); if(esEspaniol){ myDFPlayer.play(indxAbc+26);}else{  myDFPlayer.play(indxAbc+25);} } 
        if (button15.wasPressed()) {    Serial.println(F("Y was pressed"));  resetScreenCondicional(); escribeLienzo('Y',wait); if(esEspaniol){ myDFPlayer.play(indxAbc+25);}else{  myDFPlayer.play(indxAbc+24);} } 
        if (button16.wasPressed()) {    Serial.println(F("X was pressed"));  resetScreenCondicional(); escribeLienzo('X',wait); if(esEspaniol){ myDFPlayer.play(indxAbc+24);}else{  myDFPlayer.play(indxAbc+23);} } 
        if (button17.wasPressed()) {    Serial.println(F("W was pressed"));  resetScreenCondicional(); escribeLienzo('W',wait); if(esEspaniol){ myDFPlayer.play(indxAbc+23);}else{  myDFPlayer.play(indxAbc+22);} } 
        if (button18.wasPressed()) {    Serial.println(F("V was pressed"));  resetScreenCondicional(); escribeLienzo('V',wait); if(esEspaniol){ myDFPlayer.play(indxAbc+22);}else{  myDFPlayer.play(indxAbc+21);} } 
        if (button19.wasPressed()) {    Serial.println(F("U was pressed"));  resetScreenCondicional(); escribeLienzo('U',wait); if(esEspaniol){ myDFPlayer.play(indxAbc+21);}else{  myDFPlayer.play(indxAbc+20);} } 
        if (button20.wasPressed()) {    Serial.println(F("T was pressed"));  resetScreenCondicional(); escribeLienzo('T',wait); if(esEspaniol){ myDFPlayer.play(indxAbc+20);}else{  myDFPlayer.play(indxAbc+19);} } 
        if (button21.wasPressed()) {    Serial.println(F("S was pressed"));  resetScreenCondicional(); escribeLienzo('S',wait); if(esEspaniol){ myDFPlayer.play(indxAbc+19);}else{  myDFPlayer.play(indxAbc+18);} } 
        if (button22.wasPressed()) {    Serial.println(F("R was pressed"));  resetScreenCondicional(); escribeLienzo('R',wait); if(esEspaniol){ myDFPlayer.play(indxAbc+18);}else{  myDFPlayer.play(indxAbc+17);} } 
        if (button23.wasPressed()) {    Serial.println(F("Q was pressed"));  resetScreenCondicional(); escribeLienzo('Q',wait); if(esEspaniol){ myDFPlayer.play(indxAbc+17);}else{  myDFPlayer.play(indxAbc+16);} } 
        if (button24.wasPressed()) {    Serial.println(F("P was pressed"));  resetScreenCondicional(); escribeLienzo('P',wait); if(esEspaniol){ myDFPlayer.play(indxAbc+16);}else{  myDFPlayer.play(indxAbc+15);} } 
        if (button25.wasPressed()) {    Serial.println(F("O was pressed"));  resetScreenCondicional(); escribeLienzo('O',wait); if(esEspaniol){ myDFPlayer.play(indxAbc+15);}else{  myDFPlayer.play(indxAbc+14);} } 
        if (button26.wasPressed()) {    Serial.println(F("Ñ was pressed"));  resetScreenCondicional(); escribeLienzo(165,wait); if(esEspaniol){ myDFPlayer.play(15) ;  }  }  
        if (button27.wasPressed()) {    Serial.println(F("N was pressed"));  resetScreenCondicional(); escribeLienzo('N',wait); myDFPlayer.play(indxAbc+13); } 
        if (button28.wasPressed()) {    Serial.println(F("M was pressed"));  resetScreenCondicional(); escribeLienzo('M',wait); myDFPlayer.play(indxAbc+12); } 
        if (button7.wasPressed()) {    Serial.println(F("L was pressed"));  resetScreenCondicional(); escribeLienzo('L',wait); myDFPlayer.play(indxAbc+11); } 
        if (button37.wasPressed()) {    Serial.println(F("K was pressed"));  resetScreenCondicional(); escribeLienzo('K',wait); myDFPlayer.play(indxAbc+10); } 
        if (button36.wasPressed()) {    Serial.println(F("J was pressed"));  resetScreenCondicional(); escribeLienzo('J',wait); myDFPlayer.play(indxAbc+9); } 
        if (button35.wasPressed()) {    Serial.println(F("I was pressed"));  resetScreenCondicional(); escribeLienzo('I',wait); myDFPlayer.play(indxAbc+8); } 
        if (button39.wasPressed()) {    Serial.println(F("H was pressed"));  resetScreenCondicional(); escribeLienzo('H',wait); myDFPlayer.play(indxAbc+7); } 
        if (button41.wasPressed()) {    Serial.println(F("G was pressed"));  resetScreenCondicional(); escribeLienzo('G',wait); myDFPlayer.play(indxAbc+6); } 
        if (button40.wasPressed()) {    Serial.println(F("F was pressed"));  resetScreenCondicional(); escribeLienzo('F',wait); myDFPlayer.play(indxAbc+5); } 
        if (button34.wasPressed()) {    Serial.println(F("E was pressed"));  resetScreenCondicional(); escribeLienzo('E',wait); myDFPlayer.play(indxAbc+4); } 
        if (button33.wasPressed()) {    Serial.println(F("D was pressed"));  resetScreenCondicional(); escribeLienzo('D',wait); myDFPlayer.play(indxAbc+3); } 
        if (button32.wasPressed()) {    Serial.println(F("C was pressed"));  resetScreenCondicional(); escribeLienzo('C',wait); myDFPlayer.play(indxAbc+2); } 
        if (button31.wasPressed()) {    Serial.println(F("B was pressed"));  resetScreenCondicional(); escribeLienzo('B',wait); myDFPlayer.play(indxAbc+1); } 
        //if (button30.wasPressed()) {    Serial.println(F("A was pressed"));  resetScreenCondicional(); escribeLienzo('A',wait); myDFPlayer.play(indxAbc); Timer1.start(); } 
        if (button30.wasPressed()) {    Serial.println(F("A was pressed"));  resetScreenCondicional(); escribeLienzo('A',wait); myDFPlayer.play(indxAbc); } 
        if (button49.wasPressed()) {    Serial.println(F("Tecla Funciones carita"));   cambiaFuncionTeclado();} //                     escribePalabraRandom(lenguajeFrases);} //Tecla carita feliz de funciones
        
        if (button8.wasPressed()) {     // ---------------  RETROCEDER  musica y limpiar pantalla ------------------------------
             escribeLienzo('<',wait);  
             necesitaResetScreen = true;
             reproducirAnteriorSonido(); 
        }
             
        if (button6.wasPressed()) {     // --------------  ADELANTAR  musica y limpiar pantalla -----------------------------------
             escribeLienzo('>',wait);  
             necesitaResetScreen = true;
             reproducirSiguienteSonido();
        }   

        if (button38.wasPressed()) {     // --------------  DETENER    musica y limpiar pantalla -----------------YA NO FUNCIONA SE VA POR INTERRUPCION------------------
             Serial.println(F(".. was pressed"));      
             PosxPF = 0; matrix.fillScreen(LOW);  matrix.write();
             myDFPlayer.pause();
      
             sumando1= 0;
             sumando2= 0;
             operador=' ';
             countmas= 0;
        }
     
        //if (myDFPlayer.available()) {
        //  printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
        //}


        if ( detienePlayer == true ) {
              detienePlayer = false;
              myDFPlayer.pause();
        }
}//Fin Loop


void  reproducirAnteriorSonido(){
      PosxPF = 0;
       switch (funcionTeclasAvance) {
                  case 0:  actualFraseESP--;             //CANCIONES EN ESPAÑOL
                          if( (primeraCancionESP +  actualCancionESP)  <  primeraCancionESP ) { 
                               actualCancionESP = ultimaCancionESP - primeraCancionESP ;
                          }
                          myDFPlayer.play( primeraCancionESP + actualCancionESP );  //textoFuncion ="2 = Frases en Español";    funcionTeclasAvance = 2; lenguajeFrases = 0;  break;
                          muestrFuncionActualenPantalla(frases[actualCancionESP],5);
                  case 1: 
                         actualCancionENG--;              //CANCIONES EN INGLES
                          if( (primeraCancionENG +  actualCancionENG)  <  primeraCancionENG ) { 
                               actualCancionENG = ultimaCancionENG - primeraCancionENG ;
                          }
                          myDFPlayer.play( primeraCancionENG + actualCancionENG );  //textoFuncion ="2 = Frases en Español";    funcionTeclasAvance = 2; lenguajeFrases = 0;  break;
                          muestrFuncionActualenPantalla(frases[actualCancionENG],5);
                  case 2: 
                          actualFraseESP--;              //FRASE EN ESPAÑOL 
                          if( (primeraFraseESP +  actualFraseESP)  <  primeraFraseESP ) { 
                               actualFraseESP = ultimaFraseESP - primeraFraseESP ;
                          }
                          myDFPlayer.play( primeraFraseESP + actualFraseESP );  //textoFuncion ="2 = Frases en Español";    funcionTeclasAvance = 2; lenguajeFrases = 0;  break;
                          muestrFuncionActualenPantalla(frases[actualFraseESP],5);
                          myDFPlayer.play( primeraFraseESP + actualFraseESP );
 
                        break;
                  case 3:                               //FRASE EN INGLES
                          actualFraseENG--;
                          if( (primeraFraseENG +  actualFraseENG)  <  primeraFraseENG ) { 
                               actualFraseENG = ultimaFraseENG - primeraFraseENG ;
                          }
                          myDFPlayer.play( primeraFraseENG + actualFraseENG );  
                          muestrFuncionActualenPantalla(frases[actualFraseENG],5);

                        break;
                  case 4: 
                         
                         if(ivolumen >= 0 ){
                            ivolumen = ivolumen - 1;
                            myDFPlayer.volume(ivolumen);  //Set volume value. From 0 to 30
                            myDFPlayer.play(457);
                         }  
                        break;      
                  default:  break;
       }
}
void  reproducirSiguienteSonido(){
     PosxPF = 0;
     switch (funcionTeclasAvance) {
                  case 0: myDFPlayer.play( primeraCancionESP + actualCancionESP ) ;  //CANCION EN ESPAÑOL
                          muestrFuncionActualenPantalla(frases[actualCancionESP],5);
                          actualCancionESP++;
                          if( (primeraCancionESP + actualCancionESP)     > ultimaCancionESP ) {       actualCancionESP = 0;       }
                        break;
                  case 1: myDFPlayer.play( primeraCancionENG + actualCancionENG ) ;  //CANCION EN INGLES
                          muestrFuncionActualenPantalla(frases[actualCancionENG],5);
                          actualCancionENG++;
                          if( (primeraCancionENG + actualCancionENG)     > ultimaCancionENG ) {       actualCancionENG = 0;       }
                        break;
                  case 2: myDFPlayer.play( primeraFraseESP + actualFraseESP ) ;  //FRASES EN ESPAÑOL
                          muestrFuncionActualenPantalla(frases[actualFraseESP],5);
                          myDFPlayer.play( primeraFraseESP + actualFraseESP ) ;
                          actualFraseESP++;
                          if( (primeraFraseESP + actualFraseESP)     > ultimaFraseESP ) {       actualFraseESP = 0;       }
                        break;
                  case 3: myDFPlayer.play( primeraFraseENG + actualFraseENG ) ;  //FRASES EN INGLES
                          muestrFuncionActualenPantalla(frases[actualFraseENG],5);
                          actualFraseENG++;
                          if( (primeraFraseENG + actualFraseENG)     > ultimaFraseENG ) {       actualFraseENG = 0;       }
                        break;
                  case 4: 
                         if(ivolumen <= 30 ){
                            ivolumen = ivolumen + 1;
                            myDFPlayer.volume(ivolumen);  //Set volume value. From 0 to 30
                            myDFPlayer.play(457);
                         }   
                        break;
                  case 5: 
                        //Muestra hora
                        //Timer1.attachInterrupt(mainTime); // Activa la interrupcion y la asocia a ISR_Blink
                         muestraHora = true;
                         mainTime();
                         InicializaArregloPalabras(2); //Inicializa a lenguaje español
                  default:  break;
       }
}


void cambiaFuncionTeclado(){
     String textoFuncion ="";
     if(primerCambioFuncion == true){
            primerCambioFuncion = false; 
     }else{
            funcionTeclasAvance++; 
     }

     if(funcionTeclasAvance >= 6){
            funcionTeclasAvance = 0;
     }
     
     switch (funcionTeclasAvance) {
                case 0:  textoFuncion =" 1.CANCION en Espa*ol";   funcionTeclasAvance = 0; lenguajeFrases = 2;  necesitaResetScreen = true;  esEspaniol = true; break; 
                case 1:  textoFuncion ="2.CANCION en Ingles";     funcionTeclasAvance = 1; lenguajeFrases = 3;   necesitaResetScreen = true;  esEspaniol = false;break;
                case 2:  textoFuncion =" 3.FRASES en Espa*ol";    funcionTeclasAvance = 2; lenguajeFrases = 0;  necesitaResetScreen = true;  esEspaniol = true;break;
                case 3:  textoFuncion =" 4.FRASES en Ingles";     funcionTeclasAvance = 3; lenguajeFrases = 1;  necesitaResetScreen = true;  esEspaniol = false;break;
                case 4:  textoFuncion ="5.VOLUMEN          ";     funcionTeclasAvance = 4;   necesitaResetScreen = true;  ;break;
                case 5:  textoFuncion ="6.LA HORA          ";     funcionTeclasAvance = 5;   necesitaResetScreen = true;  ;break;
                default: break;
     }
     Serial.println(textoFuncion); 
     InicializaArregloPalabras(lenguajeFrases);  //Carga las frases segun el lenguaje seleccionado por la tecla sonrisa
     muestrFuncionActualenPantalla(textoFuncion,2);//Muestra funcion seleccionada en pantalla
}

void muestrFuncionActualenPantalla(String tape, int demora){
      //int demora = 25;
      int spacer2 = 1;            // Espacio entre letras
      int width = 1 + spacer;     // The font width is 5 pixels
    
      for (int i = 0; i < width * tape.length() + matrix.width() - 1 - spacer2; i++) {
          matrix.fillScreen(LOW);
     
          int letter = i / width;
          int x = (tape.length()*5 +20  ) + (matrix.width() - 1) - i % width;
          int y = (matrix.height() - 8) / 2; // center the text vertically
     
          while (  (x + width - spacer2) >= 0 && letter >= 0) {
                 if (letter  < tape.length()) {
                        if( tape[letter]=='*' ) {           //Reemplaza por una enie dado que el caracter alfanumerico no existe
                            matrix.drawChar(x, y, 164, HIGH, LOW, 1);
                        }else{
                            matrix.drawChar(x, y, tape[letter], HIGH, LOW, 1);
                        } 
                 }
                 letter--;
                 x -= width;
          }
          matrix.write(); // Send bitmap to display
          delay(demora);
       }
}


//void escribePalabraRandom(int lenguajeFrases){
//      InicializaArregloPalabras(lenguajeFrases); //2 Inicializa frases en español   3 Inicializa frases en ingles
//      if(indxpalabra > ultimaPalabra ){
//        indxpalabra = 0;
//      }
//      myDFPlayer.play(indxpalabra + primeraPalabra);
//      indxpalabra++; 
//}//escribePalabraRandom


void printDetail(uint8_t type, int value){  //Imprime errores del Dfplayer por puerto serial
      switch (type) {
        case TimeOut:                Serial.println(F("Time Out!"));      break;
        case WrongStack:             Serial.println(F("Stack Wrong!"));   break;
        case DFPlayerCardInserted:   Serial.println(F("Card Inserted!")); break;
        case DFPlayerCardRemoved:    Serial.println(F("Card Removed!"));  break;
        case DFPlayerCardOnline:     Serial.println(F("Card Online!"));   break;
        case DFPlayerPlayFinished:   Serial.print(F("Number:"));      Serial.print(value);      Serial.println(F(" Play Finished!"));           break;
        case DFPlayerError:          Serial.print(F("DFPlayerError:"));
          switch (value) {
                case Busy:              Serial.println(F("Card not found"));          break;
                case Sleeping:          Serial.println(F("Sleeping"));                break;
                case SerialWrongStack:  Serial.println(F("Get Wrong Stack"));         break;
                case CheckSumNotMatch:  Serial.println(F("Check Sum Not Match"));     break;
                case FileIndexOut:      Serial.println(F("File Index Out of Bound")); break;
                case FileMismatch:      Serial.println(F("Cannot Find File"));        break;
                case Advertise:         Serial.println(F("In Advertise"));            break;
                default:
                  break;
          }
          break;
        default:
          break;
      }
}





void calculaSuma(){
    int resultado  = 0;
    if( operador == '+'){ 
          resultado = sumando1 + sumando2;
          playNumerosConvertido(resultado);
    }
    if( operador == '-'){ 
          resultado = sumando1 - sumando2;
          playNumerosConvertido(resultado);
    }
}

void cargaOperando(int op){
     if( sumando1 == 0){ 
         sumando1 =op;
     }else{
         sumando2 =op;
     }
}


void getInfoPlayer(){
  Serial.println("Mp3 state:"       + String(myDFPlayer.readState()) ); //read mp3 state
  Serial.println("Volumne:"         +  String(myDFPlayer.readVolume())  ); //read current volume
  Serial.println("Equializador:"    + String(myDFPlayer.readEQ())); //read EQ setting
  Serial.println("Tot files in sd:" + String(myDFPlayer.readFileCounts())); //read all file counts in SD card
  Serial.println("Nro file actual:" + String(myDFPlayer.readCurrentFileNumber())); //read current play file number
  Serial.println("Tot files in folder 03:" + String(myDFPlayer.readFileCountsInFolder(3))); //read fill counts in folder SD:/03
}  


int getUltimaCancion(){
  return myDFPlayer.readFileCounts(); //read all file counts in SD card
}   


void playNumerosConvertido(int numero){
    int indxNum;

    if (lenguajeFrases == 0 || lenguajeFrases == 2 ){
        indxNum = primerNumeroESP;
    }else {
        indxNum =  primerNumeroENG;
    }
    
    switch (numero) {
          case 0:       if (lenguajeFrases == 0 || lenguajeFrases == 2 ){    
                                  myDFPlayer.play(442);  
                        }else {
                                  myDFPlayer.play(456);  
                        }
                        escribeLienzo('0',wait);  break;
          case 1:       myDFPlayer.play(indxNum);  escribeLienzo('1',wait);  break;
          case 2:       myDFPlayer.play(indxNum+1);  escribeLienzo('2',wait);  break;   
          case 3:       myDFPlayer.play(indxNum+2);  escribeLienzo('3',wait);  break;   
          case 4:       myDFPlayer.play(indxNum+3);  escribeLienzo('4',wait);  break;   
          case 5:       myDFPlayer.play(indxNum+4);  escribeLienzo('5',wait);  break;   
          case 6:       myDFPlayer.play(indxNum+5);  escribeLienzo('6',wait);  break;   
          case 7:       myDFPlayer.play(indxNum+6);  escribeLienzo('7',wait);  break;   
          case 8:       myDFPlayer.play(indxNum+7);  escribeLienzo('8',wait);  break;   
          case 9:       myDFPlayer.play(indxNum+8);  escribeLienzo('9',wait);  break;   
          case 10:      myDFPlayer.play(indxNum+9);  escribeLienzo('1',wait);escribeLienzo('0',wait); break;   
          case 11:      myDFPlayer.play(indxNum+10);  escribeLienzo('1',wait);escribeLienzo('1',wait);  break;   
          case 12:      myDFPlayer.play(indxNum+11);  escribeLienzo('1',wait);escribeLienzo('2',wait);  break;   
          case 13:      myDFPlayer.play(indxNum+12);  escribeLienzo('1',wait);escribeLienzo('3',wait);  break;   
          case 14:      myDFPlayer.play(indxNum+13);  escribeLienzo('1',wait);escribeLienzo('4',wait);  break;   
          case 15:      myDFPlayer.play(indxNum+14);  escribeLienzo('1',wait);escribeLienzo('5',wait);  break;   
          case 16:      myDFPlayer.play(indxNum+15);  escribeLienzo('1',wait);escribeLienzo('6',wait);  break;   
          case 17:      myDFPlayer.play(indxNum+16);  escribeLienzo('1',wait);escribeLienzo('7',wait);  break;   
          case 18:      myDFPlayer.play(indxNum+17);  escribeLienzo('1',wait);escribeLienzo('8',wait);  break;   
          case 19:      myDFPlayer.play(indxNum+18);  escribeLienzo('1',wait);escribeLienzo('9',wait);  break;   
          case 20:      myDFPlayer.play(indxNum+19);  escribeLienzo('2',wait);escribeLienzo('0',wait);  break;   
          case 21:      myDFPlayer.play(indxNum+20);  escribeLienzo('2',wait);escribeLienzo('1',wait);  break;   
          case 22:      myDFPlayer.play(indxNum+21);  escribeLienzo('2',wait);escribeLienzo('2',wait);  break;   
          case 23:      myDFPlayer.play(indxNum+22);  escribeLienzo('2',wait);escribeLienzo('3',wait);  break;   
          case 24:      myDFPlayer.play(indxNum+23);  escribeLienzo('2',wait);escribeLienzo('4',wait);  break;   
          case 25:      myDFPlayer.play(indxNum+24);  escribeLienzo('2',wait);escribeLienzo('5',wait);  break;   
          case 26:      myDFPlayer.play(indxNum+25);  escribeLienzo('2',wait);escribeLienzo('6',wait);  break;   
          case 27:      myDFPlayer.play(indxNum+26);  escribeLienzo('2',wait);escribeLienzo('7',wait);  break;   
          case 28:      myDFPlayer.play(indxNum+27);  escribeLienzo('2',wait);escribeLienzo('8',wait);  break;   
          case 29:      myDFPlayer.play(indxNum+28);  escribeLienzo('2',wait);escribeLienzo('9',wait);  break;   
          case 30:      myDFPlayer.play(indxNum+29);  escribeLienzo('3',wait);escribeLienzo('0',wait);  break;   
          case 31:      myDFPlayer.play(indxNum+30);  escribeLienzo('3',wait);escribeLienzo('1',wait);  break;   
          case 32:      myDFPlayer.play(indxNum+31);  escribeLienzo('3',wait);escribeLienzo('2',wait);  break;   
          case 33:      myDFPlayer.play(indxNum+32);  escribeLienzo('3',wait);escribeLienzo('3',wait);  break;   
          case 34:      myDFPlayer.play(indxNum+33);  escribeLienzo('3',wait);escribeLienzo('4',wait);  break;   
          case 35:      myDFPlayer.play(indxNum+34);  escribeLienzo('3',wait);escribeLienzo('5',wait);  break;   
          case 36:      myDFPlayer.play(indxNum+35);  escribeLienzo('3',wait);escribeLienzo('6',wait);  break;   
          case 37:      myDFPlayer.play(indxNum+36);  escribeLienzo('3',wait);escribeLienzo('7',wait);  break;   
          case 38:      myDFPlayer.play(indxNum+37);  escribeLienzo('3',wait);escribeLienzo('8',wait);  break;   
          case 39:      myDFPlayer.play(indxNum+38);  escribeLienzo('3',wait);escribeLienzo('9',wait);  break;   
          case 40:      myDFPlayer.play(indxNum+39);  escribeLienzo('4',wait);escribeLienzo('0',wait);  break;   
          case 41:      myDFPlayer.play(indxNum+40);  escribeLienzo('4',wait);escribeLienzo('1',wait);  break;   
          case 42:      myDFPlayer.play(indxNum+41);  escribeLienzo('4',wait);escribeLienzo('2',wait);  break;   
          case 43:      myDFPlayer.play(indxNum+42);  escribeLienzo('4',wait);escribeLienzo('3',wait);  break;   
          case 44:      myDFPlayer.play(indxNum+43);  escribeLienzo('4',wait);escribeLienzo('4',wait);  break;   
          case 45:      myDFPlayer.play(indxNum+44);  escribeLienzo('4',wait);escribeLienzo('5',wait);  break;   
          case 46:      myDFPlayer.play(indxNum+45);  escribeLienzo('4',wait);escribeLienzo('6',wait);  break;   
          case 47:      myDFPlayer.play(indxNum+46);  escribeLienzo('4',wait);escribeLienzo('7',wait);  break;   
          case 48:      myDFPlayer.play(indxNum+47);  escribeLienzo('4',wait);escribeLienzo('8',wait);  break;   
          case 49:      myDFPlayer.play(indxNum+48);  escribeLienzo('4',wait);escribeLienzo('9',wait);  break;   
          case 50:      myDFPlayer.play(indxNum+49);  escribeLienzo('5',wait);escribeLienzo('0',wait);  break;   
          case 51:      myDFPlayer.play(indxNum+50);  escribeLienzo('5',wait);escribeLienzo('1',wait);  break;   
          case 52:      myDFPlayer.play(indxNum+51);  escribeLienzo('5',wait);escribeLienzo('2',wait);  break;   
          case 53:      myDFPlayer.play(indxNum+52);  escribeLienzo('5',wait);escribeLienzo('3',wait);  break;   
          case 54:      myDFPlayer.play(indxNum+53);  escribeLienzo('5',wait);escribeLienzo('4',wait);  break;   
          case 55:      myDFPlayer.play(indxNum+54);  escribeLienzo('5',wait);escribeLienzo('5',wait);  break;   
          case 56:      myDFPlayer.play(indxNum+55);  escribeLienzo('5',wait);escribeLienzo('6',wait);  break;   
          case 57:      myDFPlayer.play(indxNum+56);  escribeLienzo('5',wait);escribeLienzo('7',wait);  break;   
          case 58:      myDFPlayer.play(indxNum+57);  escribeLienzo('5',wait);escribeLienzo('8',wait);  break;   
          case 59:      myDFPlayer.play(indxNum+58);  escribeLienzo('5',wait);escribeLienzo('9',wait);  break;   
          case 60:      myDFPlayer.play(indxNum+59);  escribeLienzo('6',wait);escribeLienzo('0',wait);  break;   
          case 61:      myDFPlayer.play(indxNum+60);  escribeLienzo('6',wait);escribeLienzo('1',wait);  break;   
          case 62:      myDFPlayer.play(indxNum+61);  escribeLienzo('6',wait);escribeLienzo('2',wait);  break;   
          case 63:      myDFPlayer.play(indxNum+62);  escribeLienzo('6',wait);escribeLienzo('3',wait);  break;   
          case 64:      myDFPlayer.play(indxNum+63);  escribeLienzo('6',wait);escribeLienzo('4',wait);  break;   
          case 65:      myDFPlayer.play(indxNum+64);  escribeLienzo('6',wait);escribeLienzo('5',wait);  break;   
          case 66:      myDFPlayer.play(indxNum+65);  escribeLienzo('6',wait);escribeLienzo('6',wait);  break;   
          case 67:      myDFPlayer.play(indxNum+66);  escribeLienzo('6',wait);escribeLienzo('7',wait);  break;   
          case 68:      myDFPlayer.play(indxNum+67);  escribeLienzo('6',wait);escribeLienzo('8',wait);  break;   
          case 69:      myDFPlayer.play(indxNum+68);  escribeLienzo('6',wait);escribeLienzo('9',wait);  break;   
          case 70:      myDFPlayer.play(indxNum+69);  escribeLienzo('7',wait);escribeLienzo('0',wait);  break;   
          case 71:      myDFPlayer.play(indxNum+70);  escribeLienzo('7',wait);escribeLienzo('1',wait);  break;   
          case 72:      myDFPlayer.play(indxNum+71);  escribeLienzo('7',wait);escribeLienzo('2',wait);  break;   
          case 73:      myDFPlayer.play(indxNum+72);  escribeLienzo('7',wait);escribeLienzo('3',wait);  break;   
          case 74:      myDFPlayer.play(indxNum+73);  escribeLienzo('7',wait);escribeLienzo('4',wait);  break;   
          case 75:      myDFPlayer.play(indxNum+74);  escribeLienzo('7',wait);escribeLienzo('5',wait);  break;   
          case 76:      myDFPlayer.play(indxNum+75);  escribeLienzo('7',wait);escribeLienzo('6',wait);  break;   
          case 77:      myDFPlayer.play(indxNum+76);  escribeLienzo('7',wait);escribeLienzo('7',wait);  break;   
          case 78:      myDFPlayer.play(indxNum+77);  escribeLienzo('7',wait);escribeLienzo('8',wait);  break;   
          case 79:      myDFPlayer.play(indxNum+78);  escribeLienzo('7',wait);escribeLienzo('9',wait);  break;   
          case 80:      myDFPlayer.play(indxNum+79);  escribeLienzo('8',wait);escribeLienzo('0',wait);  break;   
          case 81:      myDFPlayer.play(indxNum+80);  escribeLienzo('8',wait);escribeLienzo('1',wait);  break;   
          case 82:      myDFPlayer.play(indxNum+81);  escribeLienzo('8',wait);escribeLienzo('2',wait);  break;   
          case 83:      myDFPlayer.play(indxNum+82);  escribeLienzo('8',wait);escribeLienzo('3',wait);  break;   
          case 84:      myDFPlayer.play(indxNum+83);  escribeLienzo('8',wait);escribeLienzo('4',wait);  break;   
          case 85:      myDFPlayer.play(indxNum+84);  escribeLienzo('8',wait);escribeLienzo('5',wait);  break;   
          case 86:      myDFPlayer.play(indxNum+85);  escribeLienzo('8',wait);escribeLienzo('6',wait);  break;   
          case 87:      myDFPlayer.play(indxNum+86);  escribeLienzo('8',wait);escribeLienzo('7',wait);  break;   
          case 88:      myDFPlayer.play(indxNum+87);  escribeLienzo('8',wait);escribeLienzo('8',wait);  break;   
          case 89:      myDFPlayer.play(indxNum+88);  escribeLienzo('8',wait);escribeLienzo('9',wait);  break;   
          case 90:      myDFPlayer.play(indxNum+89);  escribeLienzo('9',wait);escribeLienzo('0',wait);  break;   
          case 91:      myDFPlayer.play(indxNum+90);  escribeLienzo('9',wait);escribeLienzo('1',wait);  break;   
          case 92:      myDFPlayer.play(indxNum+91);  escribeLienzo('9',wait);escribeLienzo('2',wait);  break;   
          case 93:      myDFPlayer.play(indxNum+92);  escribeLienzo('9',wait);escribeLienzo('3',wait);  break;   
          case 94:      myDFPlayer.play(indxNum+93);  escribeLienzo('9',wait);escribeLienzo('4',wait);  break;   
          case 95:      myDFPlayer.play(indxNum+94);  escribeLienzo('9',wait);escribeLienzo('5',wait);  break;   
          case 96:      myDFPlayer.play(indxNum+95);  escribeLienzo('9',wait);escribeLienzo('6',wait);  break;   
          case 97:      myDFPlayer.play(indxNum+96);  escribeLienzo('9',wait);escribeLienzo('7',wait);  break;   
          case 98:      myDFPlayer.play(indxNum+97);  escribeLienzo('9',wait);escribeLienzo('8',wait);  break;   
          case 99:      myDFPlayer.play(indxNum+98);  escribeLienzo('9',wait);escribeLienzo('9',wait);  break;   
          case 100:     myDFPlayer.play(indxNum+99);  escribeLienzo('1',wait);escribeLienzo('0',wait);escribeLienzo('0',wait);  break;   
    }
    escribeLienzo(' ',wait);
}



void InicializaArregloPalabras(int lenguajeFrasest){
  if (lenguajeFrasest == 0){   //Frases en Español
         frases[0] = "Hola",
         frases[1] = "Adios",
         frases[2] = "Hasta luego",
         frases[3] = "Hasta ma*ana",
         frases[4] = "Buenos dias",
         frases[5] = "Buenas tardes",
         frases[6] = "Buenas noches",
         frases[7] = "Que hora es?",
         frases[8] = "Son las 2 en punto",
         frases[9] = "A que hora empieza?",
         frases[10] = "Este es?",
         frases[11] = "Esta es?",
         frases[12] = "Encantado de conocerte",
         frases[13] = "Gracias",
         frases[14] = "Muchas gracias",
         frases[15] = "De nada",
         frases[16] = "Como te llamas?",
         frases[17] = "Me llamo",
         frases[18] = "De donde eres?",
         frases[19] = "Soy de",
         frases[20] = "Donde vives?",
         frases[21] = "Vivo en",
         frases[22] = "Que interesante",
         frases[23] = "Que dia es hoy?",
         frases[24] = "Hoy es",
         frases[25] = "Ma*ana es",
         frases[26] = "Como se dice?",
         frases[27] = "Se dice",
         frases[28] = "Como se deletrea?",
         frases[29] = "Se deletrea",
         frases[30] = "Se escribe",
         frases[31] = "Cuantos a*os tienes?",
         frases[32] = "Como estas?",
         frases[33] = "Estoy bien",
         frases[34] = "Estoy muy contento",
         frases[35] = "Estoy cansado",
         frases[36] = "Estoy triste",
         frases[37] = "Estoy enfadado",
         frases[38] = "Estoy enfermo",
         frases[39] = "Estoy asustado",
         frases[40] = "Estoy pensativo",
         frases[41] = "Tengo frio",
         frases[42] = "Tengo hambre",
         frases[43] = "Tengo sueno",
         frases[44] = "Tengo calor",
         frases[45] = "Tengo fiebre",
         frases[46] = "De que color es tu coche?",
         frases[47] = "Mi coche es azul",
         frases[48] = "No entiendo",
         frases[49] = "Como?",
         frases[50] = "Que?";
  }
  if (lenguajeFrasest == 1){   //Frases en Ingles
         frases[0]  = "Hello"; 
         frases[1]  = "Hello every body";
         frases[2]  = "Nice to meet you";
         frases[3]  = "How are you?";
         frases[4]  = "Toothpaste";
         frases[5]  = "Monday";
         frases[6]  = "Tuesday";
         frases[7]  = "Wensday";
         frases[8]  = "Thursday";
         frases[9]  = "Friday";
         frases[10]  = "Saturday";
         frases[11]  = "Sunday";
         frases[12]  = "Wall";
         frases[13]  = "Door";
         frases[14]  = "Window";
         frases[15]  = "Floor";
         frases[16]  = "Ceiling";
         frases[17]  = "Courtins";
         frases[18]  = "Picture";
         frases[19]  = "Carpet";
         frases[20]  = "Sofa";
         frases[21]  = "Cushions";
         frases[22]  = "Coffee table";
         frases[23]  = "Fireplace";
         frases[24]  = "Smoke";
         frases[25]  = "Dining room";
         frases[26]  = "Table";
         frases[27]  = "Chair";
         frases[28]  = "Table cloth";
         frases[29]  = "Glasses";
         frases[30]  = "Flower vase";
         frases[31]  = "Dish";
         frases[32]  = "Spoon";
         frases[33]  = "Fork";
         frases[34]  = "Knife";
         frases[35]  = "The kitchen";
         frases[36]  = "Refrigerator";
         frases[37]  = "Mixer";
         frases[38]  = "Toaster";
         frases[39]  = "Clock";
         frases[40]  = "Blender";
         frases[41]  = "Bathroom";
         frases[42]  = "Bathtub";
         frases[43]  = "Shower";
         frases[44]  = "Mirror";
         frases[45]  = "Lavatory";
         frases[46]  = "Tootbrush";
         frases[47]  = "Toothpaste";
         frases[48]  = "Towell";
         frases[49]  = "Soap";
         frases[50]  = "Shampoo";
         frases[51]  = "Comb";
         frases[52]  = "Bedroom";
         frases[53]  = "Bed";
         frases[54]  = "Sheet";
         frases[55]  = "Blanket";
         frases[56]  = "Pillow";
         frases[57]  = "Lamp";
         frases[58]  = "Telephone";
         frases[59]  = "Wardrobe";
         frases[60]  = "Garden";
         frases[61]  = "Yard";
         frases[62]  = "Car";
         frases[63]  = "Bus";
         frases[64]  = "Motorcycle";
         frases[65]  = "Subway";
         frases[66]  = "Train";
         frases[67]  = "Boat";
         frases[68]  = "Plane";
         frases[69]  = "Helicopter";
         frases[70]  = "Dog";
         frases[71]  = "Cat";
         frases[72]  = "Ferret";
         frases[73]  = "Bird";
         frases[74]  = "Fish";
         frases[75]  = "Hamster";
         frases[76]  = "Turtle";
         frases[77]  = "Police officer";
         frases[78]  = "Firefighter";
         frases[79]  = "Doctor";
         frases[80]  = "Teacher";
         frases[81]  = "Lawyer";
         frases[82]  = "Architect";
         frases[83]  = "Veterinary";
         frases[84]  = "Astronaut";
         frases[85]  = "Apple";
         frases[86]  = "Banana";
         frases[87]  = "Strawberry";
         frases[88]  = "Orange";
         frases[89]  = "Lemon";
         frases[90]  = "Pineapple";
         frases[91]  = "Pear";
         frases[92]  = "Grapes";
         frases[93]  = "Mother";
         frases[94]  = "Father";
         frases[95]  = "Grandmother";
         frases[96]  = "Grandfather";
         frases[97]  = "Aunt";
         frases[98]  = "Uncle";
         frases[99]  = "Sister";
         frases[100]  = "Brother";
         frases[101]  = "Yes";
         frases[102]  = "No";
         frases[103]  = "Please";
         frases[104]  = "Thanks";
         frases[105]  = "Thank you very much";
         frases[106]  = "You are welcome";
         
  }

   if (lenguajeFrasest == 2){   //Canciones en espaniol
        frases[0]  = "Viajar en Tren";
        frases[1]  = "Al Escondite Ingles";
        frases[2]  = "Al Pasar la Barca";
        frases[3]  = "El Baile de la Ensalada";
        frases[4]  = "El Baile de la Fruta";
        frases[5]  = "El Baile del Sapito";
        frases[6]  = "El Patio de Mi Casa";
        frases[7]  = "La Mane";
        frases[8]  = "La Taza";
        frases[9]  = "Mi Pelota Ya No Bota";
        frases[10]  = "Noche de Paz";
        frases[11]  = "Para Ser Conductor de Primera";
        frases[12]  = "Pi Pi Pi Piojos";
        frases[13]  = "Todos los Patitos";
        frases[14]  = "Tombola";
        frases[15]  = "Tutira Mai Nga Iwi";
        frases[16]  = "Vamos a Contar Mentiras";
        frases[17]  = "Viajar en Tren";
        frases[18]  = "Comida Light";
        frases[19]  = "Baila en un Lugar Feliz";
        frases[20]  = "Soy una Serpiente";
        frases[21]  = "El Marinero Baila";
        frases[22]  = "Fabuloso Vocabulario";
        frases[23]  = "Ronda de las Vocales";
        frases[24]  = "Fabuloso Vocabularios nuevas";
        frases[25]  = "Los Numeros del uno al cien";
        frases[26]  = "Los Numeros";
        frases[27]  = "Que Canten los Ni*os";
   }
   if (lenguajeFrasest == 3){   //Canciones en Ingles
        frases[0]  = "LingoKids";   
        frases[1]  = "Grandma and Grandpa count";   
   }
}

void resetScreenCondicional(){
      if ( necesitaResetScreen == true) {
         matrix.fillScreen(LOW); //Resetea la pantalla
         matrix.write();
         PosxPF = 0;
         necesitaResetScreen = false;
      }    
}

void dibujaBienvenida(){
     
    int pixelsx[100]= {6,7,8,4,5,9,10,3,11, 2, 12, 2,5,9,12 , 1,13, 1,7,13};
    int pixelsy[100]= {0,0,0,1,1,1,1, 2,2,  3, 3,  4,4,4,4 ,  5,5  ,6,6,6 };

    int pixelsxd[100]= {1,13, 1,4,5,6,7,8,9,10,13 , 2,5,9,12, 2,6,7,8,12 , 3,11 , 4,5,9,10,  6,7,8};
    int pixelsyd[100]= {0,0,  1,1,1,1,1,1,1,1, 1  , 2,2,2,2,  3,3,3,3,3  , 4,4  , 5,5,5,5 ,  6,6,6};
    
    int16_t j;   
    for(j=0; j<20; j++) {       
          matrix.drawPixel( pixelsx[j],pixelsy[j] +1, HIGH);  
          matrix.write(); // Send bitmap to display
          delay(30);
    }
    for(j=0; j<29; j++) {       
          matrix.drawPixel( pixelsxd[j] +64 ,pixelsyd[j], HIGH);  
          matrix.write(); // Send bitmap to display
          delay(30);
    }

    for(j=0; j<20; j++) {       
          matrix.drawPixel( pixelsx[j] + 48 ,pixelsy[j] +1, HIGH);  
          matrix.write(); // Send bitmap to display
          delay(30);
    }
    for(j=0; j<29; j++) {       
          matrix.drawPixel( pixelsxd[j] +112 ,pixelsyd[j], HIGH);  
          matrix.write(); // Send bitmap to display
          delay(30);
    }
    delay(1000);

    
    mainTime();
    necesitaResetScreen=true;
    
}

void setearPantalla(){
         
         
         matrix.setIntensity(0);      // Use a value between 0 and 15 for brightness
         matrix.setPosition(0, 0, 0);    matrix.setPosition(1, 1, 0);    matrix.setPosition(2, 2, 0);    matrix.setPosition(3, 3, 0);    matrix.setPosition(4, 4, 0);    matrix.setPosition(5, 5, 0);    matrix.setPosition(6, 6, 0);    matrix.setPosition(7, 7, 0);
         
         matrix.fillScreen(LOW); //Resetea la pantalla
         matrix.write();
          
         matrix.setPosition(8, 8, 0);    matrix.setPosition(9, 9, 0);    matrix.setPosition(10, 10, 0);  matrix.setPosition(11, 11, 0);  matrix.setPosition(12, 12, 0);  matrix.setPosition(13, 13, 0);  matrix.setPosition(14, 14, 0);  matrix.setPosition(15, 15, 0); 
         matrix.fillScreen(LOW); //Resetea la pantalla
         matrix.write();
         
         matrix.setRotation(0, 1);       matrix.setRotation(1, 1);       matrix.setRotation(2, 1);       matrix.setRotation(3, 1);       matrix.setRotation(4, 1);       matrix.setRotation(5, 1);       matrix.setRotation(6, 1);       matrix.setRotation(7, 1);    
         matrix.setRotation(8, 1);       matrix.setRotation(9, 1);       matrix.setRotation(10, 1);      matrix.setRotation(11, 1);      matrix.setRotation(12, 1);      matrix.setRotation(13, 1);      matrix.setRotation(14, 1);      matrix.setRotation(15, 1);    
         
         matrix.fillScreen(LOW); //Resetea la pantalla
         matrix.write();
}

void resetScreen(){
         matrix.fillScreen(LOW); //Resetea la pantalla
         matrix.write();
         PosxPF = 0;
         necesitaResetScreen = false;
}

String  getMes(DateTime date){
       int imonth = date.month() ;
       return  monthsNames[imonth-1];
}
String  getSFecha(DateTime date){
       String sfecha = "" ;
       int iday = date.day() ;
       int imonth = date.month() ;
       //Serial.print(date.year(), DEC);     Serial.print(date.month(), DEC);     Serial.print(date.day(), DEC);

       //sfecha =  daysOfTheWeek[date.dayOfTheWeek()] + " " + String(iday);
       sfecha =  daysOfTheWeek[date.dayOfTheWeek()] + String(iday);
       return sfecha;
}

String  getSHora(DateTime date){
       String shora =  "" ;
       String sminuto=  "" ;
       String ssegundo=  "" ;
       
       int ihora    =  date.hour() ;
       int iminuto  =  date.minute() ;
       int isegundo =  date.second() ;

    
       if(ihora < 10){      shora   = "0"+ String(ihora);      }else{     shora = String(ihora);       }
       if(iminuto < 10){    sminuto = "0"+ String(iminuto);    }else{     sminuto = String(iminuto);   }
       if(isegundo < 10){   ssegundo = "0"+ String(isegundo);  }else{     ssegundo = String(isegundo);   }
       
       shora =  " " + shora +  ":" + sminuto  + ":" +  ssegundo;
    
       return shora;
}



void mainTime(){
    DateTime now;
    String sfecha;
    String shora;
    String smes;
    unsigned long tiempoAnterior = millis();

    int periodo = 1000;  // tiempo que esta el LED en alto y bajo
    resetScreen();  
   
    
    while(muestraHora == true){
        now = rtc.now(); //Obtiene la fecha y hora del RTC
              
        sfecha = getSFecha(now);
        shora  = getSHora(now);
        smes   = getMes(now);
        // resetScreen();  
        PosxPF = 0;
        
        muestraFechaActual(sfecha);
         
        muestraHoraActual(shora, smes );
        
        tiempoAnterior=millis();
        
        while(millis()-tiempoAnterior <= periodo){  //si ha transcurrido el periodo programado de 1 segundo refresca la pantall
              //No hace nada PUEDE USAR UN SKEDULER AQUI
              if(shora == " 02:51:00" && Alarma1 == true){
                   myDFPlayer.play(457);
                   Alarma1 = false;
              }
              if(shora == " 02:40:00"){
                   myDFPlayer.play(457);
              }
              if(shora == " 02:41:00"){
                   myDFPlayer.play(443);
              }
              if(shora == " 02:42:00"){
                   myDFPlayer.play(443);
              }
              
        }          
      
    }    
}

void muestraFechaActual(String sfecha){
    int retardo = 0;
    for (int i = 0; i < sfecha.length(); i++) {
        escribeLienzo(sfecha.charAt(i),retardo);
    }     

}

void muestraHoraActual(String shora, String smes){
    int retardo = 0;
    PosxPF = 64;
    for (int i = 0; i < smes.length(); i++) {
        escribeLienzo(smes.charAt(i),retardo);
    }     
    
    for (int i = 0; i < shora.length(); i++) {
        escribeLienzo(shora.charAt(i),retardo);
    }       
    
}

void escribeLienzo(char cletra, int idelay){
    if( (PosxPF + 6) >  127 ){
      PosxPF = 0; matrix.fillScreen(LOW);  matrix.write(); //Limpia pantalla
    }

    
    matrix.drawChar(PosxPF, 0, cletra, HIGH, LOW, 1); //drawChar(int16_t x, int16_t y, unsigned char c,
    matrix.write(); // Send bitmap to display
    delay(idelay);
          
    //if(cletra == ' ' || cletra == ':' ){
    if(cletra == ' ' ){
          PosxPF = PosxPF + 2;
    }else{
          PosxPF = PosxPF + espaciadoPF;
    }      
    
}


void blink() {
  muestraHora = false;
  funcionTeclasAvance = 0; lenguajeFrases = 2;  necesitaResetScreen = true;  esEspaniol = true; primerCambioFuncion= true;
  dibujaBienvenida();
  //PosxPF = 0; matrix.fillScreen(LOW);  matrix.write();
  //myDFPlayer.pause();
      
  sumando1= 0;
  sumando2= 0;
  operador=' ';
  countmas= 0;
  detienePlayer = true;
}
