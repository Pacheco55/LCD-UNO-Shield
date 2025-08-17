/*
   Banner LCD interactivo con desplazamiento 
        personalizado por PIXELBITS Studio

   Funcionalidad:
    - Permite escribir un mensaje de hasta 16 caracteres usando los botones del LCD Keypad Shield.
    - El usuario selecciona la dirección de desplazamiento (horizontal o vertical).
    - También puede ajustar la velocidad del scroll en milisegundos.
    - El mensaje se muestra en bucle hasta que se presione SELECT para reiniciar.

   Conexiones:
    - Utiliza un LCD 16x2 con interfaz paralela (LiquidCrystal).
    - Pines conectados: RS=8, EN=9, D4=4, D5=5, D6=6, D7=7.
    - Botones del shield leídos por el pin analógico A0.

   Lógica principal:
    - `escribirMensaje()`: Edición carácter por carácter con navegación tipo cursor.
    - `seleccionarDireccion()`: Escoge entre R->L, L->R, Arriba, Abajo.
    - `seleccionarVelocidad()`: Ajusta el tiempo de desplazamiento entre 100 y 2000 ms.
    - `mostrarBanner()`: Ejecuta el scroll según configuración.

   Creado por: PIXELBITS Studios & Pacheco JC 55
  Síguenos en:
    - Instagram: https://www.instagram.com/pixelbits_studios/
    - Twitch: https://www.twitch.tv/pixelbits_studio/about
    - GitHub: https://github.com/Pacheco55/PIXELBITS-Studio-blog/tree/HTMLpbsb

 Ideal para proyectos de identidad visual, menús interactivos o presentaciones creativas en Arduino.
*/


#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Botones
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

int lcd_key = 0;
int adc_key_in = 0;

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

// Variables para el mensaje
char message[17] = "PIXELBITS Studio"; // máx 16 caracteres
int cursorPos = 0;
char currentChar = 'A';
bool messageConfirmed = false;

// Variables de desplazamiento
int menuState = 0;
int scrollDir = 0; // 0=R->L, 1=L->R, 2=Up, 3=Down
int scrollSpeed = 300; // milisegundos

void setup() {
  lcd.begin(16, 2);
  lcd.print("Escribir texto");
  delay(1000);
  lcd.clear();
}

void loop() {
  if (!messageConfirmed) {
    escribirMensaje();
  } else {
    seleccionarDireccion();
    seleccionarVelocidad();
    mostrarBanner();
  }
}

void escribirMensaje() {
  lcd.setCursor(0, 0);
  lcd.print("Msg:");
  lcd.print(message);

  lcd.setCursor(cursorPos + 4, 1); // posicionar cursor
  lcd.blink();

  int btn = read_LCD_buttons();
  switch (btn) {
    case btnUP:
      currentChar++;
      if (currentChar > 'Z') currentChar = ' ';
      message[cursorPos] = currentChar;
      break;
    case btnDOWN:
      currentChar--;
      if (currentChar < ' ') currentChar = 'Z';
      message[cursorPos] = currentChar;
      break;
    case btnRIGHT:
      cursorPos = (cursorPos + 1) % 16;
      currentChar = message[cursorPos];
      break;
    case btnLEFT:
      cursorPos = (cursorPos - 1 + 16) % 16;
      currentChar = message[cursorPos];
      break;
    case btnSELECT:
      lcd.noBlink();
      messageConfirmed = true;
      lcd.clear();
      break;
  }
  delay(200);
}

void seleccionarDireccion() {
  lcd.clear();
  lcd.print("Dir:");
  String opciones[] = {"R->L", "L->R", "Arriba", "Abajo"};
  int opcion = scrollDir;

  while (true) {
    lcd.setCursor(0, 1);
    lcd.print(opciones[opcion] + "     ");

    int btn = read_LCD_buttons();
    if (btn == btnUP) opcion = (opcion + 3) % 4;
    if (btn == btnDOWN) opcion = (opcion + 1) % 4;
    if (btn == btnSELECT) {
      scrollDir = opcion;
      break;
    }
    delay(200);
  }
  lcd.clear();
}

void seleccionarVelocidad() {
  lcd.clear();
  lcd.print("Vel (ms):");
  int vel = scrollSpeed;

  while (true) {
    lcd.setCursor(0, 1);
    lcd.print("   ");
    lcd.print(vel);
    lcd.print("     ");

    int btn = read_LCD_buttons();
    if (btn == btnUP) vel += 100;
    if (btn == btnDOWN) vel -= 100;
    if (vel < 100) vel = 100;
    if (vel > 2000) vel = 2000;
    if (btn == btnSELECT) {
      scrollSpeed = vel;
      break;
    }
    delay(200);
  }
  lcd.clear();
}

void mostrarBanner() {
  while (true) {
    if (scrollDir == 0) scrollRightToLeft();
    if (scrollDir == 1) scrollLeftToRight();
    if (scrollDir == 2) scrollUp();
    if (scrollDir == 3) scrollDown();

    int btn = read_LCD_buttons();
    if (btn == btnSELECT) {
      messageConfirmed = false;
      menuState = 0;
      lcd.clear();
      break;
    }
  }
}

void scrollRightToLeft() {
  String msg = String(message);
  msg += " "; // Espacio al final
  for (int i = 0; i < msg.length(); i++) {
    lcd.setCursor(0, 0);
    lcd.print(msg.substring(i) + msg.substring(0, i));
    delay(scrollSpeed);
  }
}

void scrollLeftToRight() {
  String msg = String(message);
  msg += " ";
  for (int i = msg.length() - 1; i >= 0; i--) {
    lcd.setCursor(0, 0);
    lcd.print(msg.substring(i) + msg.substring(0, i));
    delay(scrollSpeed);
  }
}

void scrollUp() {
  for (int i = 0; i < 2; i++) {
    lcd.clear();
    lcd.setCursor(0, i);
    lcd.print(message);
    delay(scrollSpeed);
  }
}

void scrollDown() {
  for (int i = 1; i >= 0; i--) {
    lcd.clear();
    lcd.setCursor(0, i);
    lcd.print(message);
    delay(scrollSpeed);
  }
}
