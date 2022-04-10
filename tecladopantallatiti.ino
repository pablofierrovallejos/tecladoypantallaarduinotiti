#include "GFButton.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// Create button instances 
GFButton button1(49);  GFButton button2(48);  
GFButton button3(47);  GFButton button4(46);  
GFButton button5(45);  GFButton button6(44);  
GFButton button7(43);  GFButton button8(42);
GFButton button9(41);  GFButton button10(40); 
GFButton button11(39); GFButton button12(38); 
GFButton button13(37); GFButton button14(36); 
GFButton button15(35); GFButton button16(34); 
GFButton button17(33); GFButton button18(32); 
GFButton button19(31); GFButton button20(30); 
GFButton button21(29); GFButton button22(28);
GFButton button23(27); GFButton button24(26);
GFButton button25(25); GFButton button26(24);
GFButton button27(23); GFButton button28(22);
GFButton button29(13); GFButton button30(12);
GFButton button31(9); GFButton button32(8);
GFButton button33(7); GFButton button34(6);
GFButton button35(5); GFButton button36(4);
GFButton button37(3); GFButton button38(2);

GFButton button39(A11); GFButton button40(A12);
GFButton button41(A13);// GFButton button42(A14);

//GFButton button43(A10); GFButton button44(A9);
//GFButton button45(A8); GFButton button46(A7);
//GFButton button47(A6); GFButton button48(A5);
GFButton button49(A4); GFButton button50(A3);
GFButton button51(A2); GFButton button52(A1);
//GFButton button53(A0); 


SoftwareSerial mySoftwareSerial(10, 11); // RX, TX para comunicacion con dfplayer
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

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

int espaciadoPF = 6;
int PosxPF = 0;
int totletras = 0;  //cuando hay mas de 20 letras en pantalla la limpia

int primeraCancion = 101;
int ultimaCancion  = 125;
int actualCancion  = 101;

int primeraPalabra = 50;
int ultimaPalabra  = 100;
int indxpalabra = 0;

int sumando1= 0;
int sumando2= 0;
char operador=' ';

void setup() {
   totletras = 0;

   //Setea la pantalla  setPosition(byte display, byte x, byte y) {
   matrix.setIntensity(1);      // Use a value between 0 and 15 for brightness
   matrix.setPosition(0, 0, 0); // The first display is at <0, 0>
   matrix.setPosition(1, 1, 0); // The second display is at <1, 0>
   matrix.setPosition(2, 2, 0); // The third display is at <2, 0>
   matrix.setPosition(3, 3, 0); // And the last display is at <3, 0>
   matrix.setPosition(4, 4, 0); // The first display is at <0, 0>
   matrix.setPosition(5, 5, 0); // The second display is at <1, 0>
   matrix.setPosition(6, 6, 0); // The third display is at <2, 0>
   matrix.setPosition(7, 7, 0); // And the last display is at <3, 0>
   matrix.setPosition(8, 8, 0); // The first display is at <0, 0>
   matrix.setPosition(9, 9, 0); // The second display is at <1, 0>
   matrix.setPosition(10, 10, 0); // The third display is at <2, 0>
   matrix.setPosition(11, 11, 0); // And the last display is at <3, 0>
   matrix.setPosition(12, 12, 0); // The first display is at <0, 0>
   matrix.setPosition(13, 13, 0); // The second display is at <1, 0>
   matrix.setPosition(14, 14, 0); // The third display is at <2, 0>
   matrix.setPosition(15, 15, 0); // And the last display is at <3, 0>

   matrix.setRotation(0, 1);    // Display is position upside down
   matrix.setRotation(1, 1);    // Display is position upside down
   matrix.setRotation(2, 1);    // Display is position upside down
   matrix.setRotation(3, 1);    // Display is position upside down
   matrix.setRotation(4, 1);    // Display is position upside down
   matrix.setRotation(5, 1);    // Display is position upside down
   matrix.setRotation(6, 1);    // Display is position upside down
   matrix.setRotation(7, 1);    // Display is position upside down
   matrix.setRotation(8, 1);    // Display is position upside down
   matrix.setRotation(9, 1);    // Display is position upside down
   matrix.setRotation(10, 1);    // Display is position upside down
   matrix.setRotation(11, 1);    // Display is position upside down
   matrix.setRotation(12, 1);    // Display is position upside down
   matrix.setRotation(13, 1);    // Display is position upside down
   matrix.setRotation(14, 1);    // Display is position upside down
   matrix.setRotation(15, 1);    // Display is position upside down
   
   matrix.fillScreen(LOW); //Resetea la pantalla
   matrix.write();
   // Prepare serial interface
   Serial.begin(9600);

   mySoftwareSerial.begin(9600);        //Inicia puerto serial para dfplayer
   if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
      Serial.println(F("Unable to begin:"));
      Serial.println(F("1.Please recheck the connection!"));
      Serial.println(F("2.Please insert the SD card!"));
      while(true);
   }else {
      Serial.println(F("DFPlayer Mini online."));
   }
   myDFPlayer.volume(15);  //Set volume value. From 0 to 30
   myDFPlayer.play(49);     //Play the file 99 mp3 de bienvenida
  

  //ultimaCancion = getUltimaCancion();
  
}

void escribeLienzo(char cletra, int idelay){
    if(totletras == 21 ){
      PosxPF = 0; matrix.fillScreen(LOW);  matrix.write(); //Limpia pantalla
      totletras = 0;
    }
    matrix.drawChar(PosxPF, 0, cletra, HIGH, LOW, 1); //drawChar(int16_t x, int16_t y, unsigned char c,
    matrix.write(); // Send bitmap to display
    PosxPF = PosxPF + espaciadoPF;
    delay(idelay);
    totletras++;
}

void getInfoPlayer(){
  Serial.println("Mp3 state:" + String(myDFPlayer.readState()) ); //read mp3 state
  Serial.println("Volumne:" +  String(myDFPlayer.readVolume())  ); //read current volume
  Serial.println("Equializador:" + String(myDFPlayer.readEQ())); //read EQ setting
  Serial.println("Tot files in sd:" + String(myDFPlayer.readFileCounts())); //read all file counts in SD card
  Serial.println("Nro file actual:" + String(myDFPlayer.readCurrentFileNumber())); //read current play file number
  Serial.println("Tot files in folder 03:" + String(myDFPlayer.readFileCountsInFolder(3))); //read fill counts in folder SD:/03
}  

int getUltimaCancion(){
  return myDFPlayer.readFileCounts(); //read all file counts in SD card
}   

void escribePalabraRandom(){
  if(indxpalabra > ultimaPalabra ){
    indxpalabra = 0;
  }
   
  String palabras[51] = {"Hola","Adios","Hasta luego","Hasta ma*ana","Buenos dias","Buenas tardes","Buenas noches","Que hora es?","Son las 2 en punto","A que hora empieza?","Este es?","Esta es?","Encantado de conocerte","Gracias","Muchas gracias","De nada",
                         "Como te llamas?","Me llamo","De donde eres?","Soy de","Donde vives?","Vivo en","Que interesante","Que dia es hoy?","Hoy es","Ma*ana es","Como se dice?","Se dice","Como se deletrea?","Se deletrea","Se escribe","Cuantos a*os tienes?",
                         "Como estas?","Estoy bien","Estoy muy contento","Estoy cansado","Estoy triste","Estoy enfadado","Estoy enfermo","Estoy asustado","Estoy pensativo","Tengo frio","Tengo hambre","Tengo sueno","Tengo calor","Tengo fiebre",
                         "De que color es tu coche?","Mi coche es azul","No entiendo","Como?","Que?"};

  String tape = palabras[indxpalabra]; 
 
  
  int demora = 25;
  int spacer2 = 1;            // Espacio entre letras

  int width = 1 + spacer;    // The font width is 5 pixels

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
   
  myDFPlayer.play(primeraPalabra + indxpalabra);
  indxpalabra++; 
}

void cargaOperando(int op){
     if( sumando1 == 0){ 
         sumando1 =op;
     }else{
         sumando2 =op;
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

void playNumerosConvertido(int numero){
  switch (numero) {
    case 0:
       myDFPlayer.play(28);  escribeLienzo('0',wait);  break;
    case 1:
       myDFPlayer.play(29);  escribeLienzo('1',wait);  break;
    case 2:
       myDFPlayer.play(30);  escribeLienzo('2',wait);  break;   
    case 3:
       myDFPlayer.play(31);  escribeLienzo('3',wait);  break;   
    case 4:
       myDFPlayer.play(32);  escribeLienzo('4',wait);  break;   
    case 5:
       myDFPlayer.play(33);  escribeLienzo('5',wait);  break;   
    case 6:
       myDFPlayer.play(34);  escribeLienzo('6',wait);  break;   
    case 7:
       myDFPlayer.play(35);  escribeLienzo('7',wait);  break;   
    case 8:
       myDFPlayer.play(36);  escribeLienzo('8',wait);  break;   
    case 9:
       myDFPlayer.play(37);  escribeLienzo('9',wait);  break;   
    case 10:
       myDFPlayer.play(38);  escribeLienzo('1',wait);escribeLienzo('0',wait);  break;   
    case 11:
       myDFPlayer.play(39);  escribeLienzo('1',wait);escribeLienzo('1',wait);  break;   
    case 12:
       myDFPlayer.play(40);  escribeLienzo('1',wait);escribeLienzo('2',wait);  break;   
    case 13:
       myDFPlayer.play(41);  escribeLienzo('1',wait);escribeLienzo('3',wait);  break;   
    case 14:
       myDFPlayer.play(42);  escribeLienzo('1',wait);escribeLienzo('4',wait);  break;   
    case 15:
       myDFPlayer.play(43);  escribeLienzo('1',wait);escribeLienzo('5',wait);  break;   
    case 16:
       myDFPlayer.play(44);  escribeLienzo('1',wait);escribeLienzo('6',wait);  break;   
    case 17:
       myDFPlayer.play(45);  escribeLienzo('1',wait);escribeLienzo('7',wait);  break;   
    case 18:
       myDFPlayer.play(46);  escribeLienzo('1',wait);escribeLienzo('8',wait);  break;   
    case 19:
       myDFPlayer.play(47);  escribeLienzo('1',wait);escribeLienzo('9',wait);  break;   
    case 20:
       myDFPlayer.play(48);  escribeLienzo('2',wait);escribeLienzo('0',wait);  break;   

   
  }
}

void loop() {
   //   matrix.fillScreen(LOW); //Resetea la pantalla
  // Check if  button is pressed
  if (button1.wasPressed())  {    Serial.println(F("9 was pressed"));   escribeLienzo('9',wait);  myDFPlayer.play(37) ; cargaOperando(9);} 
  if (button2.wasPressed())  {    Serial.println(F("8 was pressed"));   escribeLienzo('8',wait);  myDFPlayer.play(36) ; cargaOperando(8);} 
  if (button3.wasPressed())  {    Serial.println(F("7 was pressed"));   escribeLienzo('7',wait);  myDFPlayer.play(35) ; cargaOperando(7);}  
  if (button4.wasPressed())  {    Serial.println(F("6 was pressed"));   escribeLienzo('6',wait);  myDFPlayer.play(34) ; cargaOperando(6);} 
  if (button5.wasPressed())  {    Serial.println(F("5 was pressed"));   escribeLienzo('5',wait);  myDFPlayer.play(33) ; cargaOperando(5);} 
  if (button9.wasPressed())  {    Serial.println(F("4 was pressed"));   escribeLienzo('4',wait);  myDFPlayer.play(32) ; cargaOperando(4);} 
  if (button10.wasPressed()) {    Serial.println(F("3 was pressed"));   escribeLienzo('3',wait);  myDFPlayer.play(31) ; cargaOperando(3);} 
  if (button11.wasPressed()) {    Serial.println(F("2 was pressed"));   escribeLienzo('2',wait);  myDFPlayer.play(30) ; cargaOperando(2);} 
  if (button12.wasPressed()) {    Serial.println(F("1 was pressed"));   escribeLienzo('1',wait);  myDFPlayer.play(29) ; cargaOperando(1);} 
  if (button13.wasPressed()) {    Serial.println(F("0 was pressed"));   escribeLienzo('0',wait);  myDFPlayer.play(28) ; cargaOperando(0);} 
  if (button14.wasPressed()) {    Serial.println(F("Z was pressed"));   escribeLienzo('Z',wait);  myDFPlayer.play(27) ;}
  if (button15.wasPressed()) {    Serial.println(F("Y was pressed"));   escribeLienzo('Y',wait);  myDFPlayer.play(26) ;} 
  if (button16.wasPressed()) {    Serial.println(F("X was pressed"));   escribeLienzo('X',wait);  myDFPlayer.play(25) ;} 
  if (button17.wasPressed()) {    Serial.println(F("W was pressed"));   escribeLienzo('W',wait);  myDFPlayer.play(24) ;} 
  if (button18.wasPressed()) {    Serial.println(F("V was pressed"));   escribeLienzo('V',wait);  myDFPlayer.play(23) ;} 
  if (button19.wasPressed()) {    Serial.println(F("U was pressed"));   escribeLienzo('U',wait);  myDFPlayer.play(22) ;} 
  if (button20.wasPressed()) {    Serial.println(F("T was pressed"));   escribeLienzo('T',wait);  myDFPlayer.play(21) ;} 
  if (button21.wasPressed()) {    Serial.println(F("S was pressed"));   escribeLienzo('S',wait);  myDFPlayer.play(20) ;} 
  if (button22.wasPressed()) {    Serial.println(F("R was pressed"));   escribeLienzo('R',wait);  myDFPlayer.play(19) ;} 
  if (button23.wasPressed()) {    Serial.println(F("Q was pressed"));   escribeLienzo('Q',wait);  myDFPlayer.play(18) ;} 
  if (button24.wasPressed()) {    Serial.println(F("P was pressed"));   escribeLienzo('P',wait);  myDFPlayer.play(17) ;} 
  if (button25.wasPressed()) {    Serial.println(F("O was pressed"));   escribeLienzo('O',wait);  myDFPlayer.play(16) ;}   
  if (button26.wasPressed()) {    Serial.println(F("Enie was pressed"));   escribeLienzo(165,wait);  myDFPlayer.play(15) ;} 
  if (button27.wasPressed()) {    Serial.println(F("N was pressed"));   escribeLienzo('N',wait);  myDFPlayer.play(14) ;}  
  if (button28.wasPressed()) {    Serial.println(F("M was pressed"));   escribeLienzo('M',wait);  myDFPlayer.play(13) ;}
  if (button38.wasPressed()) {    Serial.println(F("L was pressed"));   escribeLienzo('L',wait);  myDFPlayer.play(12) ;}
  if (button37.wasPressed()) {    Serial.println(F("K was pressed"));   escribeLienzo('K',wait);  myDFPlayer.play(11) ;}
  if (button36.wasPressed()) {    Serial.println(F("J was pressed"));   escribeLienzo('J',wait);  myDFPlayer.play(10) ;}
  if (button35.wasPressed()) {    Serial.println(F("I was pressed"));   escribeLienzo('I',wait);  myDFPlayer.play(9) ;}

  
  if (button34.wasPressed()) {    Serial.println(F("E was pressed"));   escribeLienzo('E',wait);  myDFPlayer.play(5) ;}
  if (button33.wasPressed()) {    Serial.println(F("D was pressed"));   escribeLienzo('D',wait);  myDFPlayer.play(4) ;}
  if (button32.wasPressed()) {    Serial.println(F("C was pressed"));   escribeLienzo('C',wait);  myDFPlayer.play(3) ;}
  if (button31.wasPressed()) {    Serial.println(F("B was pressed"));   escribeLienzo('B',wait);  myDFPlayer.play(2) ;}
  if (button30.wasPressed()) {    Serial.println(F("A was pressed"));   escribeLienzo('A',wait);  myDFPlayer.play(1) ;}

  if (button40.wasPressed()) {    Serial.println(F("F was pressed"));   escribeLienzo('F',wait);  myDFPlayer.play(6) ;}
  if (button41.wasPressed()) {    Serial.println(F("G was pressed"));   escribeLienzo('G',wait);  myDFPlayer.play(7) ;}
  if (button39.wasPressed()) {    Serial.println(F("H was pressed"));   escribeLienzo('H',wait);  myDFPlayer.play(8) ;}

   
   //if (button43.wasPressed()) {    Serial.println(F(". was pressed"));   escribeLienzo('A',wait);  myDFPlayer.play(1) ;}
   //if (button44.wasPressed()) {    Serial.println(F(".. was pressed"));   escribeLienzo('A',wait);  myDFPlayer.play(1) ;}
   //if (button45.wasPressed()) {    Serial.println(F("... was pressed"));   escribeLienzo('A',wait);  myDFPlayer.play(1) ;}
   //if (button46.wasPressed()) {    Serial.println(F(".... was pressed"));   escribeLienzo('A',wait);  myDFPlayer.play(1) ;}
   //if (button47.wasPressed()) {    Serial.println(F(",A was pressed"));   escribeLienzo('A',wait);  myDFPlayer.play(1) ;}
   //if (button48.wasPressed()) {    Serial.println(F(",, was pressed"));   escribeLienzo('A',wait);  myDFPlayer.play(1) ;}
   if (button49.wasPressed()) {    Serial.println(F("palabra was pressed"));   escribePalabraRandom();}
   if (button50.wasPressed()) {    Serial.println(F("+ was pressed"));   escribeLienzo('+',wait);  myDFPlayer.play(126) ; operador = '+'; }
   if (button51.wasPressed()) {    Serial.println(F("- was pressed"));   escribeLienzo('-',wait);  myDFPlayer.play(128) ; operador = '-';}
   if (button52.wasPressed()) {    Serial.println(F("= was pressed"));   escribeLienzo('=',wait);  myDFPlayer.play(127) ; delay(1000); calculaSuma();}
   //if (button53.wasPressed()) {    Serial.println(F("*** was pressed"));   escribeLienzo('A',wait);  myDFPlayer.play(1) ;}
   //Enie mayuscula  165  minuscula 165
   //int16_t j;   
   //for(j=39; j<43; j++) {       
   //   escribeLienzo(j,500);  
   //   Serial.println(j);
   //} 
  //escribeLienzo('*',wait);
  //escribeLienzo(42,wait);
  
  

  //los siguientes botones son para adelantar y retroceder musica 
  if (button6.wasPressed()) {    
       Serial.println("Tocando cacion actual FWD:" + String(actualCancion) );
       escribeLienzo('.',wait);  
       myDFPlayer.play(actualCancion) ;
  
       if(actualCancion >= ultimaCancion ) {    
             actualCancion = primeraCancion;
       }else{
             actualCancion++;
       }
  }   
  if (button7.wasPressed()) {    
       Serial.println(F(".. was pressed"));      // DETENER musica y limpiar pantalla
       PosxPF = 0; matrix.fillScreen(LOW);  matrix.write();
       myDFPlayer.pause();

      sumando1= 0;
      sumando2= 0;
      operador=' ';

  }  
  if (button8.wasPressed()) {  
       Serial.println("Tocando cacion actual REV:" + String(actualCancion) );
       escribeLienzo('-',wait);  
       myDFPlayer.play(actualCancion) ;

       if(actualCancion <= primeraCancion ) {    
             actualCancion = ultimaCancion;
       }else{
             actualCancion--;
       }
  }   


  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
  
}



void printDetail(uint8_t type, int value){  //Imprime errores del Dfplayer por puerto serial
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}