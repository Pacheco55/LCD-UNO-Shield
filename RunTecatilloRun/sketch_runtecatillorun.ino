/*
* =====================================================================================
 * RUN TECATILLO RUN - MICROCONTROLLER VERSION
 * =====================================================================================
 * 
 * DESCRIPCION:
 * Versión para microcontroladores del juego "Run Tecatillo Run" originalmente 
 * desarrollado en Godot Engine por PACHECO JC 55 para la UPT y su comunidad . 
 * Este port para Arduino implementa el gameplay 
 * principal del endless runner con sprites personalizados y controles adaptados 
 * para shield LCD de 16x2.
 * 
 * FUNCIONALIDAD PRINCIPAL:
 * El jugador controla un personaje que corre automáticamente y debe saltar obstáculos.
 * Incluye sistema de puntuación por distancia, velocidad variable, pausa y sprites
 * animados. 
 * El terreno se genera proceduralmente con obstáculos aleatorios en 
 * fila superior e inferior.
 * 
 * CONTROLES:
 * UP - Saltar obstáculos
 * RIGHT - Acelerar juego 
 * LEFT - Reducir velocidad
 * SELECT - Pausar/Continuar
 * DOWN - Restablecer velocidad
 * 
 * HARDWARE REQUERIDO:
 * Arduino UNO/Nano + LCD Shield 16x2 con botones analógicos (pin A0)
 * Conexiones estándar del shield: LCD(8,9,4,5,6,7) + Botones(A0)
 * 
 * CREDITOS:
 * Juego original: PIXELBITS Studios - Pacheco JC 55
 * Plataforma original: Godot Engine
 * Repositorio: GitHub (PIXELBITS Studio)
 * Port Arduino: Adaptación para microcontroladores
 
 * CARACTERISTICAS TECNICAS:
 * 7 sprites personalizados, terreno procedural, sistema de colisiones,
 * animaciones de personaje, velocidad adaptable, manejo de memoria optimizado
 * para plataformas de 2KB RAM.

 * 
 * REDES SOCIALES:
 * Instagram: https://www.instagram.com/pixelbits_studios/
 * Twitch: https://www.twitch.tv/pixelbits_studio/about
 * GitHub: https://github.com/Pacheco55/PIXELBITS-Studio-blog/tree/HTMLpbsb
 * 
 * =====================================================================================
 */


#include <LiquidCrystal.h>

// Configuración del LCD Shield
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Definiciones de botones
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

// Variables para botones
int lcd_key = 0;
int adc_key_in = 0;

// Sprites del juego
#define SPRITE_RUN1 1
#define SPRITE_RUN2 2
#define SPRITE_JUMP 3
#define SPRITE_JUMP_UPPER '.'
#define SPRITE_JUMP_LOWER 4
#define SPRITE_TERRAIN_EMPTY ' '
#define SPRITE_TERRAIN_SOLID 5
#define SPRITE_TERRAIN_SOLID_RIGHT 6
#define SPRITE_TERRAIN_SOLID_LEFT 7

// Posición del héroe
#define HERO_HORIZONTAL_POSITION 1

// Configuración del terreno
#define TERRAIN_WIDTH 16
#define TERRAIN_EMPTY 0
#define TERRAIN_LOWER_BLOCK 1
#define TERRAIN_UPPER_BLOCK 2

// Estados del héroe
#define HERO_POSITION_OFF 0
#define HERO_POSITION_RUN_LOWER_1 1
#define HERO_POSITION_RUN_LOWER_2 2
#define HERO_POSITION_JUMP_1 3
#define HERO_POSITION_JUMP_2 4
#define HERO_POSITION_JUMP_3 5
#define HERO_POSITION_JUMP_4 6
#define HERO_POSITION_JUMP_5 7
#define HERO_POSITION_JUMP_6 8
#define HERO_POSITION_JUMP_7 9
#define HERO_POSITION_JUMP_8 10
#define HERO_POSITION_RUN_UPPER_1 11
#define HERO_POSITION_RUN_UPPER_2 12

// Variables del juego
static char terrainUpper[TERRAIN_WIDTH + 1];
static char terrainLower[TERRAIN_WIDTH + 1];
static bool buttonPushed = false;
static bool gamePaused = false;
static int gameSpeed = 50; // Velocidad base del juego
static unsigned long lastButtonPress = 0;
static const unsigned long buttonDelay = 150; // Anti-rebote

void initializeGraphics() {
  static byte graphics[] = {
    // Run position 1
    B01100,
    B01100,
    B00000,
    B01110,
    B11100,
    B01100,
    B11010,
    B10011,
    // Run position 2
    B01100,
    B01100,
    B00000,
    B01100,
    B01100,
    B01100,
    B01100,
    B01110,
    // Jump
    B01100,
    B01100,
    B00000,
    B11110,
    B01101,
    B11111,
    B10000,
    B00000,
    // Jump lower
    B11110,
    B01101,
    B11111,
    B10000,
    B00000,
    B00000,
    B00000,
    B00000,
    // Ground
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    // Ground right
    B00011,
    B00011,
    B00011,
    B00011,
    B00011,
    B00011,
    B00011,
    B00011,
    // Ground left
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
  };
  
  // Crear caracteres personalizados
  for (int i = 0; i < 7; ++i) {
    lcd.createChar(i + 1, &graphics[i * 8]);
  }
  
  // Inicializar terreno vacío
  for (int i = 0; i < TERRAIN_WIDTH; ++i) {
    terrainUpper[i] = SPRITE_TERRAIN_EMPTY;
    terrainLower[i] = SPRITE_TERRAIN_EMPTY;
  }
}

void advanceTerrain(char* terrain, byte newTerrain) {
  for (int i = 0; i < TERRAIN_WIDTH; ++i) {
    char current = terrain[i];
    char next = (i == TERRAIN_WIDTH-1) ? newTerrain : terrain[i+1];
    switch (current) {
      case SPRITE_TERRAIN_EMPTY:
        terrain[i] = (next == SPRITE_TERRAIN_SOLID) ? SPRITE_TERRAIN_SOLID_RIGHT : SPRITE_TERRAIN_EMPTY;
        break;
      case SPRITE_TERRAIN_SOLID:
        terrain[i] = (next == SPRITE_TERRAIN_EMPTY) ? SPRITE_TERRAIN_SOLID_LEFT : SPRITE_TERRAIN_SOLID;
        break;
      case SPRITE_TERRAIN_SOLID_RIGHT:
        terrain[i] = SPRITE_TERRAIN_SOLID;
        break;
      case SPRITE_TERRAIN_SOLID_LEFT:
        terrain[i] = SPRITE_TERRAIN_EMPTY;
        break;
    }
  }
}

bool drawHero(byte position, char* terrainUpper, char* terrainLower, unsigned int score) {
  bool collide = false;
  char upperSave = terrainUpper[HERO_HORIZONTAL_POSITION];
  char lowerSave = terrainLower[HERO_HORIZONTAL_POSITION];
  byte upper, lower;
  
  switch (position) {
    case HERO_POSITION_OFF:
      upper = lower = SPRITE_TERRAIN_EMPTY;
      break;
    case HERO_POSITION_RUN_LOWER_1:
      upper = SPRITE_TERRAIN_EMPTY;
      lower = SPRITE_RUN1;
      break;
    case HERO_POSITION_RUN_LOWER_2:
      upper = SPRITE_TERRAIN_EMPTY;
      lower = SPRITE_RUN2;
      break;
    case HERO_POSITION_JUMP_1:
    case HERO_POSITION_JUMP_8:
      upper = SPRITE_TERRAIN_EMPTY;
      lower = SPRITE_JUMP;
      break;
    case HERO_POSITION_JUMP_2:
    case HERO_POSITION_JUMP_7:
      upper = SPRITE_JUMP_UPPER;
      lower = SPRITE_JUMP_LOWER;
      break;
    case HERO_POSITION_JUMP_3:
    case HERO_POSITION_JUMP_4:
    case HERO_POSITION_JUMP_5:
    case HERO_POSITION_JUMP_6:
      upper = SPRITE_JUMP;
      lower = SPRITE_TERRAIN_EMPTY;
      break;
    case HERO_POSITION_RUN_UPPER_1:
      upper = SPRITE_RUN1;
      lower = SPRITE_TERRAIN_EMPTY;
      break;
    case HERO_POSITION_RUN_UPPER_2:
      upper = SPRITE_RUN2;
      lower = SPRITE_TERRAIN_EMPTY;
      break;
  }
  
  if (upper != ' ') {
    terrainUpper[HERO_HORIZONTAL_POSITION] = upper;
    collide = (upperSave == SPRITE_TERRAIN_EMPTY) ? false : true;
  }
  if (lower != ' ') {
    terrainLower[HERO_HORIZONTAL_POSITION] = lower;
    collide |= (lowerSave == SPRITE_TERRAIN_EMPTY) ? false : true;
  }
  
  byte digits = (score > 9999) ? 5 : (score > 999) ? 4 : (score > 99) ? 3 : (score > 9) ? 2 : 1;
  
  // Dibujar la escena
  terrainUpper[TERRAIN_WIDTH] = '\0';
  terrainLower[TERRAIN_WIDTH] = '\0';
  char temp = terrainUpper[16-digits];
  terrainUpper[16-digits] = '\0';
  lcd.setCursor(0,0);
  lcd.print(terrainUpper);
  terrainUpper[16-digits] = temp;  
  lcd.setCursor(0,1);
  lcd.print(terrainLower);
  
  lcd.setCursor(16 - digits,0);
  lcd.print(score);

  terrainUpper[HERO_HORIZONTAL_POSITION] = upperSave;
  terrainLower[HERO_HORIZONTAL_POSITION] = lowerSave;
  return collide;
}

// Leer botones del shield LCD
int read_LCD_buttons() {
  adc_key_in = analogRead(0);
  if (adc_key_in > 1000) return btnNONE;
  if (adc_key_in < 50)   return btnRIGHT;  
  if (adc_key_in < 250)  return btnUP; 
  if (adc_key_in < 450)  return btnDOWN; 
  if (adc_key_in < 650)  return btnLEFT; 
  if (adc_key_in < 850)  return btnSELECT;  
  return btnNONE;
}

// Manejar entrada de botones con anti-rebote
void handleButtons() {
  lcd_key = read_LCD_buttons();
  unsigned long currentTime = millis();
  
  if (lcd_key != btnNONE && (currentTime - lastButtonPress) > buttonDelay) {
    lastButtonPress = currentTime;
    
    switch (lcd_key) {
      case btnUP:
        buttonPushed = true; // Saltar
        break;
      case btnRIGHT:
        // Acelerar (reducir delay)
        if (gameSpeed > 20) {
          gameSpeed -= 5;
        }
        break;
      case btnLEFT:
        // Reducir velocidad (aumentar delay)
        if (gameSpeed < 100) {
          gameSpeed += 5;
        }
        break;
      case btnSELECT:
        gamePaused = !gamePaused; // Alternar pausa
        if (gamePaused) {
          lcd.setCursor(4, 0);
          lcd.print("PAUSADO");
        }
        break;
      case btnDOWN:
        // Función adicional - reiniciar velocidad
        gameSpeed = 50;
        break;
    }
  }
}

void showStartScreen() {
  lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Developed By:");
    lcd.setCursor(0, 1);
    lcd.print("JULIO PACHECO");
    delay(800);
    lcd.clear();
    delay(200);
    
    lcd.setCursor(3, 0);
    lcd.print("PIXELBITS");
    lcd.setCursor(5, 1);
    lcd.print("Studio");
    delay(800);
    lcd.clear();
    delay(200);
    
    lcd.setCursor(5, 0);
    lcd.print("Micro");
    lcd.setCursor(0, 1);
    lcd.print("run tecatilo run");
    delay(800);
    lcd.clear();
    delay(200);
  
}

void setup() {
  lcd.begin(16, 2);
  showStartScreen();
  initializeGraphics();
  randomSeed(analogRead(1)); // Semilla para números aleatorios
}

void loop() {
  static byte heroPos = HERO_POSITION_RUN_LOWER_1;
  static byte newTerrainType = TERRAIN_EMPTY;
  static byte newTerrainDuration = 1;
  static bool playing = false;
  static bool blink = false;
  static unsigned int distance = 0;
  
  handleButtons();
  
  if (!playing) {
    drawHero((blink) ? HERO_POSITION_OFF : heroPos, terrainUpper, terrainLower, distance >> 3);
    if (blink) {
      lcd.setCursor(3, 0);
      lcd.print("UP=Jugar");
      lcd.setCursor(0, 1);
      lcd.print("R=+ L=- S=Pausa");
    }
    delay(250);
    blink = !blink;
    
    if (buttonPushed) {
      initializeGraphics();
      heroPos = HERO_POSITION_RUN_LOWER_1;
      playing = true;
      buttonPushed = false;
      distance = 0;
      gameSpeed = 50;
      gamePaused = false;
      lcd.clear();
    }
    return;
  }
  
  // Si el juego está pausado, mostrar mensaje y esperar
  if (gamePaused) {
    lcd.setCursor(4, 0);
    lcd.print("PAUSADO");
    lcd.setCursor(0, 1);
    lcd.print("SELECT=Continuar");
    delay(100);
    return;
  }

  // Avanzar terreno
  advanceTerrain(terrainLower, newTerrainType == TERRAIN_LOWER_BLOCK ? SPRITE_TERRAIN_SOLID : SPRITE_TERRAIN_EMPTY);
  advanceTerrain(terrainUpper, newTerrainType == TERRAIN_UPPER_BLOCK ? SPRITE_TERRAIN_SOLID : SPRITE_TERRAIN_EMPTY);
  
  // Generar nuevo terreno
  if (--newTerrainDuration == 0) {
    if (newTerrainType == TERRAIN_EMPTY) {
      newTerrainType = (random(3) == 0) ? TERRAIN_UPPER_BLOCK : TERRAIN_LOWER_BLOCK;
      newTerrainDuration = 2 + random(10);
    } else {
      newTerrainType = TERRAIN_EMPTY;
      newTerrainDuration = 10 + random(10);
    }
  }
    
  if (buttonPushed) {
    if (heroPos <= HERO_POSITION_RUN_LOWER_2) heroPos = HERO_POSITION_JUMP_1;
    buttonPushed = false;
  }  

  if (drawHero(heroPos, terrainUpper, terrainLower, distance >> 3)) {
    playing = false; // Colisión - fin del juego
    lcd.setCursor(4, 0);
    lcd.print("GAME OVER");
    delay(2000);
    lcd.clear();
  } else {
    if (heroPos == HERO_POSITION_RUN_LOWER_2 || heroPos == HERO_POSITION_JUMP_8) {
      heroPos = HERO_POSITION_RUN_LOWER_1;
    } else if ((heroPos >= HERO_POSITION_JUMP_3 && heroPos <= HERO_POSITION_JUMP_5) && terrainLower[HERO_HORIZONTAL_POSITION] != SPRITE_TERRAIN_EMPTY) {
      heroPos = HERO_POSITION_RUN_UPPER_1;
    } else if (heroPos >= HERO_POSITION_RUN_UPPER_1 && terrainLower[HERO_HORIZONTAL_POSITION] == SPRITE_TERRAIN_EMPTY) {
      heroPos = HERO_POSITION_JUMP_5;
    } else if (heroPos == HERO_POSITION_RUN_UPPER_2) {
      heroPos = HERO_POSITION_RUN_UPPER_1;
    } else {
      ++heroPos;
    }
    ++distance;
  }
  
  delay(gameSpeed); // Velocidad variable del juego
}