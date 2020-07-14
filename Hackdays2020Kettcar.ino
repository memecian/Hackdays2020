/*
  Von
  _______                     _  _
  |__   __|                  | || |
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
  attachInterrupt(digitalPinToInterrupt(2), zeiterfassung, RISING);
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(3), zeiterfassung, RISING); //Interrupts auf den Magnetsensorpins anknüpfen
}
/*
 * Variablendeklaration für die Geschwindigkeitsmessung
 */
const double radius  = 0.125;           //in Metern
const double pi      = 3.141;           //
const double umfang  = 2 * pi * radius;

volatile long tletzte = 0;
volatile long t  = 0;

int deltat;
int vvorherig;
int vmomentan;

/*
 * Variablendeklaration für Spannungsmessung 
 */



long zyklusstart = millis(); 

void loop() {                                               
// Geschwindigkeitsberechnung:
  deltat = t - tleztze;
  vmomentan = (umfang / 2) / deltat; // m/ms
  vmomentan = (vmomentan * 3600)   ;  // Umwandlung zu km/h
  vvorherig = vmomentan;
  

// Akkustandsanzeige:


}

void zeiterfassung() {
  tletzte = t;
  t = millis();
}
