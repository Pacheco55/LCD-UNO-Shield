/*
  Reloj digital con formato 12h/24h y actualización 
  automática de fecha por PIXELBITS Studio 

  Funcionalidad:
  Este código simula un reloj en tiempo real que muestra hora y fecha en un LCD 16x2.
  Permite alternar entre formato de 12h y 24h presionando el botón SELECT del LCD Keypad Shield.
  La fecha se actualiza correctamente incluyendo lógica para años bisiestos.

  Conexiones:
  - LCD conectado mediante interfaz paralela: RS=8, EN=9, D4=4, D5=5, D6=6, D7=7
  - Botones leídos desde el pin analógico A0
  - Para arduino LCD Display Shield

  Lógica principal:
  - Incremento de segundos, minutos, horas, días, meses y años
  - Cálculo de días por mes y verificación de año bisiesto
  - Conversión de hora a formato 12h con indicador AM/PM
  - Visualización continua en pantalla con actualización cada segundo
  - Cambio de formato de hora mediante botón SELECT

  Créditos:
  Desarrollado por PIXELBITS Studios & Pacheco JC 55

  Redes sociales:
    - Instagram: https://www.instagram.com/pixelbits_studios/
    - Twitch: https://www.twitch.tv/pixelbits_studio/about
    - GitHub: https://github.com/Pacheco55/PIXELBITS-Studio-blog/tree/HTMLpbsb

*/

#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);  // 8 = rs del display , 9 = enabled , 4 5 6 7 = pin digital 4 5 6 7  .

#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5

int segundos = 55;
int minutos = 41;
int horas = 15;
int dia = 6;
int mes = 3;
int anio = 2025;

int lcd_key = 0;
int adc_key_in = 0;

// Variables globales
bool formato24h = true;  // true para formato 24h, false para 12h
bool AM_PM = false;      // false para AM, true para PM


// Función para verificar si un año es bisiesto
bool esBisiesto(int year) {
  return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}


void setup() {
Serial.begin(9600);

  lcd.begin(16, 2);            // start the library y la pantalla como tal y define el tamaño del matrix del display 16 columans * 2 filas .
  for (int i = 0; i < 5; i++)  //Ciclo para parpadeo de mensaje de developer .
  {
    lcd.setCursor(0, 0);
    lcd.print("Developed By : ");
    lcd.setCursor(0, 1);
    lcd.print(" JULIO PACHECO");
    delay(300);
    lcd.clear();
    delay(200);
    lcd.setCursor(3, 0);
    lcd.print("PIXELBITS ");
    lcd.setCursor(6, 1);
    lcd.print(" Studio");
    delay(300);
    lcd.clear();
    delay(200);
  }
}

void loop() {

// Incrementar segundos y actualizar tiempo
segundos++;
if (segundos > 59) {
  segundos = 0;
  minutos++;
  
  if (minutos > 59) {
    minutos = 0;
    horas++;
    
    if (horas >= 24) {
      horas = 0;
      dia++;
      
      // Determinar días por mes y actualizar mes/año
      byte diasEnMes;
      
      if (mes == 2) {
        // Febrero: 28 días, 29 en año bisiesto
        diasEnMes = esBisiesto(anio) ? 29 : 28;
      } else if (mes == 4 || mes == 6 || mes == 9 || mes == 11) {
        diasEnMes = 30; // Meses con 30 días
      } else {
        diasEnMes = 31; // Meses con 31 días
      }
      
      if (dia > diasEnMes) {
        dia = 1;
        mes++;
        
        if (mes > 12) {
          mes = 1;
          anio++;
        }
      }
    }
  }
}

// Determinar AM/PM para formato 12h
if (!formato24h) {
  if (horas >= 12) {
    AM_PM = true;  // PM
  } else {
    AM_PM = false; // AM
  }
}

// Mostrar en LCD
lcd.clear();

// Primera línea: Hora
lcd.print("Hora: ");

if (formato24h) {
  // Formato 24 horas
  if (horas < 10) lcd.print("0");
  lcd.print(horas);
} else {
  // Formato 12 horas
  byte horasMostradas = horas;
  if (horasMostradas == 0) {
    horasMostradas = 12;  // 12 AM a medianoche
  } else if (horasMostradas > 12) {
    horasMostradas -= 12; // Convertir a formato 12h
  }
  
  if (horasMostradas < 10) lcd.print("0");
  lcd.print(horasMostradas);
}

// Continuar con minutos y segundos
lcd.print(":");
if (minutos < 10) lcd.print("0");
lcd.print(minutos);
lcd.print(":");
if (segundos < 10) lcd.print("0");
lcd.print(segundos);

// Añadir AM/PM si estamos en formato 12h
if (!formato24h) {
  lcd.print(AM_PM ? " PM" : " AM");
}

// Segunda línea: Fecha
lcd.setCursor(0, 1);
lcd.print("Fecha: ");
if (dia < 10) lcd.print("0");
lcd.print(dia);
lcd.print("/");
if (mes < 10) lcd.print("0");
lcd.print(mes);
lcd.print("/");
lcd.print(anio);

delay(1000);

lcd_key = read_LCD_buttons();  // read the buttons .  // como funcion para empezar odarle salida al menu de abajo
                                 /// OPCIONES DE ACCION "MENU" SIN INTERFAZ AL 10/AGO/24
  switch (lcd_key) {             // depending on which button is pushed is the perform an action .
    case btnSELECT:
      {
        formato24h = !formato24h;
        break;
      }
  }
}


// read buttons
int read_LCD_buttons() {
  adc_key_in = analogRead(0);  // read the value  .
  // my buttons when read are centered at these values: 0, 144, 329, 504, 741 .
  // we add approx 50 to those values and check to see if we are close .
  if (adc_key_in > 1000) return btnNONE;  // no Key pressed .
  if (adc_key_in < 50) return btnRIGHT;
  if (adc_key_in < 250) return btnUP;
  if (adc_key_in < 450) return btnDOWN;
  if (adc_key_in < 650) return btnLEFT;
  if (adc_key_in < 850) return btnSELECT;
  return btnNONE;  // when all others fail return btnNONE .
}