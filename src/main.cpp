#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <SD.h>
#include <IRremote.h>

#define OUT1 8                      // pin led strip 1
#define OUT2 9                      // pin led strip 2
Adafruit_NeoPixel helice1 = Adafruit_NeoPixel(11,OUT1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel helice2 = Adafruit_NeoPixel(11,OUT2, NEO_GRB + NEO_KHZ800);

#define SD_CS 10                    // pin CS pour le lecteur SD
Sd2Card CarteSD;
SdVolume VolumeCarteSD;
SdFile root;
uint32_t volumesize;
#define nomDuFichier "gen.txt" 
File genome_file;

int irInput = 7;                    // pin pour le récepteur IR
int modePin = 5;                    // pour le bouton poussoir de sélection du mode
IRrecv irrecv(irInput);
decode_results results;

int guanine_color[3]={0,128,0};     // vert RGB
int cytosine_color[3]={255,0,0};    // rouge RGB
int adenine_color[3]={0,0,255};     // bleu RGB
int thymine_color[3]={255,255,0};   // jaune RGB
int adn[11][3];                     // couleurs des hélices à l'instant t
int mode = 0; 
int modeCount = 8;

int counter = 0;
boolean buttonReady = true;
int position = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Let's begin !!");
  pinMode(modePin, INPUT);
  digitalWrite(modePin, HIGH);
  helice1.begin(); 
  helice2.begin();

  irrecv.enableIRIn();
  randomSeed(analogRead(A0));

  // init du tableau des couleurs des hélices
  for(int i=0;i<12;i++){
    for(int j=0;j<3;j++){
     adn[i][j] = 0;
    }
  } 
  // carte SD
  pinMode(10, OUTPUT);
  if (!CarteSD.init(SPI_HALF_SPEED, SD_CS)) {
    Serial.println(F("Échec lors de l'initialisation du lecteur SD "));
    Serial.println();
  } else {
    Serial.println(F("Carte SD trouvée."));
    Serial.println();
    if (!VolumeCarteSD.init(CarteSD)) {
      Serial.println(F("Aucune partition FAT16/FAT32 trouvée."));
      Serial.println(F("Vérifiez si votre carte SD est bien formatée !"));
      } else {
        Serial.print(F("Format du volume : FAT"));
        Serial.println(VolumeCarteSD.fatType(), DEC);
        root.openRoot(VolumeCarteSD);
        root.ls(LS_R | LS_DATE | LS_SIZE);
        }
  }
}

void loop() {
  // lecture du fichier genome.txt
  Serial.print("Lecture du fichier génome : ");Serial.println(nomDuFichier);
  genome_file = SD.open(nomDuFichier, FILE_READ);
  if (genome_file) {
    while (genome_file.available()) {              // Lecture, jusqu'à ce qu'il n'y ait plus rien à lire
      Serial.write(genome_file.read());            // ... et affichage sur le moniteur série !
    }
    genome_file.close();          
  } else
  {
    Serial.println("Fichier génome non trouvé");
    while(1);
  }

  if(irrecv.decode(&results)){
    Serial.println(results.value);
    if(results.value==4294967295){    // change the code for the ir remote here
      mode++;
      if(mode==modeCount){
       mode=0;
      }
      delay(100);
    }
    irrecv.resume();
  }
  for(int i=0;i<12;i++){
    //helice1.setPixelColor(i,helice1.Color(r,g,b)); 
  } 
  //helice1.show();
  //helice2.show();

  position+= (analogRead(A0)/128.0);    
  
  delay(20);
}
