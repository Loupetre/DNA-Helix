// DNA HELIX
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
#define nomDuFichier "genome.txt" 
File genome_file;

int irInput = 7;                    // pin pour le récepteur IR
int modePin = 5;                    // pour le bouton poussoir de sélection du mode
IRrecv irrecv(irInput);
decode_results results;

char base2;                          // contient la base lue dans le fichier
int guanine_color[3]={0,128,0};     // vert RGB
int cytosine_color[3]={255,0,0};    // rouge RGB
int adenine_color[3]={0,0,255};     // bleu RGB
int thymine_color[3]={255,255,0};   // jaune RGB
int adn[2][11][3];                  // couleurs des hélices à l'instant t
int position_helice;                // là où on doit mettre la couleur dans le tableau ADN
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
  for(int k=0;k<2;k++)
      for(int i=0;i<12;i++)
        for(int j=0;j<3;j++)
        adn[k][i][j] = 0;
    
  
  // carte SD
  pinMode(10, OUTPUT);
  SD.begin(SD_CS);
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
  // quelques initialisations de variables
  position_helice = 0;    // premier étage
  // lecture du fichier genome.txt
  Serial.print("Lecture du fichier génome : ");Serial.println(nomDuFichier);
  genome_file = SD.open(nomDuFichier, FILE_READ);
  if (genome_file) {
    // lecture de la première ligne qui indique de quel génome il s'agit
    Serial.println(genome_file.readStringUntil('\n'));  // lecture de la première ligne - le titre -
    while (genome_file.available()) {             // Lecture du reste : il faut déterminer le complément
      char base = genome_file.read();             
      counter++;
      if (base != '\n' && base != '\r')
      {
        switch (base)               // calcul du complément aux bases ATCG
        {
        case 'a':
          base = 'A';
        case 'A':
          base2 = 'T';
          // on met la couleur correspondante dans le tableau ADN en position courante
          adn[0][0][0] = adenine_color[0];
          adn[0][0][1] = adenine_color[1];
          adn[0][0][2] = adenine_color[2];
          adn[1][0][0] = thymine_color[0];
          adn[1][0][1] = thymine_color[1];
          adn[1][0][2] = thymine_color[2];
          break;
        case 't':
          base = 'T';
        case 'T':
          base2 = 'A';
          // on met la couleur correspondante dans le tableau ADN en position courante
          adn[0][0][0] = thymine_color[0];
          adn[0][0][1] = thymine_color[1];
          adn[0][0][2] = thymine_color[2];
          adn[1][0][0] = adenine_color[0];
          adn[1][0][1] = adenine_color[1];
          adn[1][0][2] = adenine_color[2];
          break;
        case 'c':
          base = 'C';
        case 'C':
          base2 = 'G';
          // on met la couleur correspondante dans le tableau ADN en position courante
          adn[0][0][0] = cytosine_color[0];
          adn[0][0][1] = cytosine_color[1];
          adn[0][0][2] = cytosine_color[2];
          adn[1][0][0] = guanine_color[0];
          adn[1][0][1] = guanine_color[1];
          adn[1][0][2] = guanine_color[2];
          break;
        case 'g':
          base = 'G';
        case 'G':
          base2 = 'C';
          // on met la couleur correspondante dans le tableau ADN en position courante
          adn[0][0][0] = guanine_color[0];
          adn[0][0][1] = guanine_color[1];
          adn[0][0][2] = guanine_color[2];
          adn[1][0][0] = cytosine_color[0];
          adn[1][0][1] = cytosine_color[1];
          adn[1][0][2] = cytosine_color[2];
          break;
        default:
          break;
        }
        //Serial.print(base); Serial.println(base2);delay(250);
        // on allume les leds !!
        // décalage des leds pour la prochaine lecture
      }
    }
    genome_file.close();  
    Serial.print("Nombre de bases lues : ");
    Serial.println(counter);        
    counter = 0;
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
