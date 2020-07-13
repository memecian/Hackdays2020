/* 
 Von
  _______                     _  _   
 |__   __|                   | || |  
    | | ___  __ _ _ __ ___   | || |_ 
    | |/ _ \/ _` | '_ ` _ \  |__   _|
    | |  __/ (_| | | | | | |    | |  
    |_|\___|\__,_|_| |_| |_|    |_|  
                                  
  ... mit Liebe gemacht!                                   

  Kettcar-Programm v1.1
  Features : 
     - Geschwindigkeitsmessung (halbwegs)
  Geplant : 
     - Temperaturmessung
     - Spannungsmessung
     - Ausgabe auf ein TFT-Display !!
*/
void setup() {
  pinMode(2, INPUT);
  pinMode(3, INPUT); //Magnetsensoren auf Ports 1,2 gesetzt

const double radius  = 12.5;
const double pi      = 3.141;
const double umfang  = 2*pi*radius;
volatile bool MagSI  = digitalRead(2);
volatile bool MagSII = digitalRead(3);
volatile  int geschwindigkeit;
volatile long start;

void loop() {
  start = millis();
  
  while (millis != start + 500)
  {
    MagSI = digitalRead(2);  
    MagSII = digitalRead(3);
    
    if (MagSI != digitalRead(2) || MagSII != digitalRead(3))
    {
     geschwindigkeit = (umfang/2) / millis() - start; 
    }
  }
}
