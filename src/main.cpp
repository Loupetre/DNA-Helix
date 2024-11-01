// DNA HELIX --
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <SD.h>

#define OUT1 8                      // pin led strip 1
#define OUT2 9                      // pin led strip 2
Adafruit_NeoPixel helice1 = Adafruit_NeoPixel(11,OUT1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel helice2 = Adafruit_NeoPixel(11,OUT2, NEO_GRB + NEO_KHZ800);
int delai = 200;                      // delai entre chaque mise à jour allumage des bandes led

#define SD_CS 10                    // pin CS pour le lecteur SD
Sd2Card CarteSD;
SdVolume VolumeCarteSD;
SdFile root;
uint32_t volumesize;
#define nomDuFichier "genome.txt" 
File genome_file;
bool fromSD;                        // pas de SD on va afficher des bases au pif

char base2;                          // contient la base lue dans le fichier
int guanine_color[3]={1,128,0};     // vert RGB
int cytosine_color[3]={255,0,0};    // rouge RGB
int adenine_color[3]={2,0,255};     // bleu RGB
int thymine_color[3]={250,255,0};   // jaune RGB
int adn[2][11][3];                  // couleurs des hélices à l'instant t
long counter = 0;

boolean buttonReady = true;
int position = 0;

bool get_carte_SD();
void fill_adn_tab(char);
void lights_up(int);
void decalage();

void setup() {
  Serial.begin(9600);
  Serial.println("Let's begin !!");

  helice1.begin(); 
  helice2.begin();

  // carte SD
  pinMode(10, OUTPUT);
  SD.begin(SD_CS);

  delai = 1000-analogRead(A0); // valeur initiale du délai via le potar
  if (delai<30) { delai = 30;}  // pour éviter de se retrouver avec un delai négatif qui peut bloquer l'arduino
}

// ********************************************* LOOP ******************************************
// *********************************************************************************************

void loop() {
  for(int k=0;k<2;k++)            // init du tableau des couleurs des hélices
      for(int i=0;i<11;i++)
        for(int j=0;j<3;j++)
        adn[k][i][j] = 0;
        
  fromSD = get_carte_SD();          // init SD. Indique si on a réussi l'init

  if (fromSD) {
    // lecture du fichier genome.txt
    Serial.print("Lecture du fichier génome : ");
    Serial.println(nomDuFichier);
    genome_file = SD.open(nomDuFichier, FILE_READ);
    fromSD = genome_file;          // indique si on a réussi à lire le fichier
  }
  if (fromSD) {
    // lecture de la première ligne qui indique de quel génome il s'agit
    Serial.println(genome_file.readStringUntil('\n'));  // lecture de la première ligne - le titre -
    while (genome_file.available()) {             // Lecture du reste : il faut déterminer le complément
      char base = genome_file.read();             
      counter++;
      if (base != '\n' && base != '\r')
      {
        fill_adn_tab(base);
        //Serial.print(base); Serial.println(base2);delay(250);
        lights_up(delai);                // on allume les leds !! Le délai est réglable avec le potar
        decalage();                    // décalage des leds pour la prochaine lecture

        delai = 1000-analogRead(A0);
        if (delai<30) { delai = 30;}  // pour éviter de se retrouver avec un delai négatif ou trop bas
      }
    }
    genome_file.close();  
    Serial.print("Nombre de bases lues : ");
    Serial.println(counter);        
    counter = 0;
  } else {
    Serial.println("Allumage par défaut");  // on fait clignoter les hélices en rouge du coup 
    for (int i=0; i<100; i++)
    {
      if (i % 2 == 0) {
        fill_adn_tab('X');              // code X spécial (rouge voir fonction fill_adn_tab)
      } else {
        fill_adn_tab('Z');              // code spécial (pas de couleur)
      }
      lights_up(delai);                  // on allume les leds !! Le délai est réglable avec le potar
      decalage();                      // décalage des leds pour la prochaine lecture
    }
  }

 
}

// ********************************************* FUNCTIONS *************************************
// *********************************************************************************************

bool get_carte_SD() {
  fromSD = true;
  if (!CarteSD.init(SPI_HALF_SPEED, SD_CS)) {                     // init du lecteur de carte SD
    Serial.println(F("Échec lors de l'initialisation du lecteur SD "));
    return false;
    } else {
      Serial.println(F("Carte SD trouvée."));
      if (!VolumeCarteSD.init(CarteSD)) {
        Serial.println(F("Aucune partition FAT16/FAT32 trouvée."));
        Serial.println(F("Vérifiez si votre carte SD est bien formatée !"));
        return false;
        } else {
          Serial.print(F("Format du volume : FAT"));
          Serial.println(VolumeCarteSD.fatType(), DEC);
          root.openRoot(VolumeCarteSD);
          root.ls(LS_R | LS_DATE | LS_SIZE);
          }
    }
    return true;
}

void fill_adn_tab(char base) {
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
    case 'X':
      // on met en rouge les deux bases
      adn[0][0][0] = 255;
      adn[0][0][1] = 0;
      adn[0][0][2] = 0;
      adn[1][0][0] = 255;
      adn[1][0][1] = 0;
      adn[1][0][2] = 0;
      break;
    case 'Z':
      // on met en "noir" les deux bases
      adn[0][0][0] = 0;
      adn[0][0][1] = 0;
      adn[0][0][2] = 0;
      adn[1][0][0] = 0;
      adn[1][0][1] = 0;
      adn[1][0][2] = 0;
      break;
    default:
      break;
    }
}

void lights_up(int valeur_delai) {
  for(int i=0;i<11;i++){
    helice1.setPixelColor(i,helice1.Color(adn[0][i][0],adn[0][i][1],adn[0][i][2]));
    helice2.setPixelColor(i,helice2.Color(adn[1][i][0],adn[1][i][1],adn[1][i][2]));
  } 
  helice1.show();
  helice2.show();
  delay(valeur_delai);
}

void decalage() {
  for (int i=10;i>0;i--) {
    adn[0][i][0] = adn[0][i-1][0];
    adn[0][i][1] = adn[0][i-1][1];
    adn[0][i][2] = adn[0][i-1][2];
    adn[1][i][0] = adn[1][i-1][0];
    adn[1][i][1] = adn[1][i-1][1];
    adn[1][i][2] = adn[1][i-1][2];
  }
}