/*
      PIXEL TIME SYSTEM

  Desarrollado por: PIXELBITS Studios & Pacheco JC 55

  Redes sociales:
  Instagram: instagram.com/pixelbits_studios
  Twitch: twitch.tv/pixelbits_studio/about
  GitHub: github.com/Pacheco55/PIXELBITS-Studio-blog/tree/HTMLpbsb

  Descripción general:
  Sistema de gestión de tiempo con múltiples funciones:
  - Reloj en tiempo real con formato 12h/24h
  - Temporizador regresivo con notificación por buzzer
  - Registro de eventos en EEPROM
  - Programador de tareas para control automático de pines

  Conexiones:
  LCD: RS=8, E=9, D4=4, D5=5, D6=6, D7=7
  Botones: Pin analógico A0 (divisor de voltaje)
  Relay: Pin 15
  Buzzer: Pin 11

  Funcionalidades principales:
  - Reloj: Muestra hora y fecha con cálculo de años bisiestos
  - Temporizador: Cuenta regresiva configurable (hh:mm:ss)
  - Registro: Guarda eventos del sistema en EEPROM (activaciones, tareas)
  - Tareas: Programa hasta 5 tareas temporizadas para controlar salidas

  Uso de EEPROM:
  - 0–9: Contador de eventos y metadatos
  - 10–99: Almacenamiento de eventos (máx. 10)
  - 100: Contador de tareas
  - 110–209: Almacenamiento de tareas (máx. 5)

  Navegación:
  - UP/DOWN: Navegar por menús
  - LEFT: Salir/volver
  - RIGHT/SELECT: Confirmar/entrar

  Créditos:
  Código original por PIXELBITS Studios
  Implementación de hardware por Pacheco JC 55
*/



#include <LiquidCrystal.h>
#include <EEPROM.h>

// LCD pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);  // RS, E, D4, D5, D6, D7

// Botones
#define BTN_RIGHT 0
#define BTN_UP 1
#define BTN_DOWN 2
#define BTN_LEFT 3
#define BTN_SELECT 4
#define BTN_NONE 5

// Menu Options
#define MENU_SIZE 4
#define MENU_CLOCK 0
#define MENU_TIMER 1
#define MENU_EVENT_LOG 2
#define MENU_TASK_SCHEDULER 3

// EEPROM Addresses
#define EEPROM_EVENT_COUNT 0
#define EEPROM_EVENT_START 10
#define EEPROM_TASK_COUNT 100
#define EEPROM_TASK_START 110

// Pins
#define RELAY_PIN 15
#define BUZZER_PIN 11

// Global variables
// Clock
unsigned long lastMillis = 0;
int segundos = 0;
int minutos = 58;
int horas = 2;
int dia = 14;
int mes = 4;
int anio = 2025;
bool formato24h = true;
bool isPM = false;

// Menu
int currentMenu = MENU_CLOCK;
int menuPosition = 0;
bool inSubMenu = false;

// Timer
int timerHours = 0;
int timerMinutes = 0;
int timerSeconds = 0;
bool timerRunning = false;
unsigned long timerStartMillis = 0;
unsigned long timerDurationMillis = 0;

// Event handler 
struct Event {
  byte hour;
  byte minute;
  byte second;
  byte day;
  byte month;
  int year;
  byte type; // 0=Timer, 1=Task
};

// Agendamiento de tareas , Task
struct Task {
  byte startHour;
  byte startMinute;
  byte durationHours;
  byte durationMinutes;
  byte pin;
  bool enabled;
  bool isRunning;
  unsigned long startTimeMillis;
};

#define MAX_TASKS 5
Task tasks[MAX_TASKS];
byte taskCount = 0;
byte currentTask = 0;
byte taskSetting = 0;  // 0=startHour, 1=startMinute, 2=durationHours, 3=durationMinutes, 4=pin, 5=enabled

// Button handling
int lcd_key = BTN_NONE;
int adc_key_in = 0;
bool buttonPressed = false;
unsigned long lastButtonPressMillis = 0;

// Event logging
#define MAX_EVENTS 10
byte eventCount = 0;
byte currentEvent = 0;

// Function prototypes
void updateClock();
void displayClock();
void displayMenu();
void handleButtons();
int read_LCD_buttons();
void handleTimer();
void displayTimer();
void logEvent(byte type);
void displayEventLog();
void handleTasks();
void displayTaskScheduler();
void updateTaskStatus();
bool esBisiesto(int year);
void saveTasksToEEPROM();
void loadTasksFromEEPROM();
void saveEventsToEEPROM();
void loadEventsFromEEPROM();

void setup() {
  Serial.begin(9600);
  
  // Initialize the LCD
  lcd.begin(16, 2);
  
  // Initialize pins
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  
  // Welcome message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  PIXEL TIME");
  lcd.setCursor(0, 1);
  lcd.print("    System");
  delay(2000);

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
  
  // Load saved data
  loadTasksFromEEPROM();
  loadEventsFromEEPROM();
  
  lcd.clear();
}

void loop() {
  // Update clock
  updateClock();
  
  // Check tasks status
  updateTaskStatus();
  
  // Handle timer
  if (timerRunning) {
    handleTimer();
  }
  
  // Read buttons
  handleButtons();
  
  // Display current menu
  if (!inSubMenu) {
    displayMenu();
  } else {
    // Display submenu based on current selection
    switch (currentMenu) {
      case MENU_CLOCK:
        displayClock();
        break;
      case MENU_TIMER:
        displayTimer();
        break;
      case MENU_EVENT_LOG:
        displayEventLog();
        break;
      case MENU_TASK_SCHEDULER:
        displayTaskScheduler();
        break;
    }
  }
  
  delay(100);  // Small delay to prevent LCD flickering
}

// Function to update the clock time
void updateClock() {
  unsigned long currentMillis = millis();
  
  // Check if a second has passed
  if (currentMillis - lastMillis >= 1000) {
    lastMillis = currentMillis;
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
          
          // Determine days per month
          byte diasEnMes;
          
          if (mes == 2) {
            diasEnMes = esBisiesto(anio) ? 29 : 28;
          } else if (mes == 4 || mes == 6 || mes == 9 || mes == 11) {
            diasEnMes = 30; 
          } else {
            diasEnMes = 31;
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
    
    // Update AM/PM
    isPM = (horas >= 12);
  }
}

// Function to display the clock
void displayClock() {
  lcd.clear();
  
  // First line: Time
  lcd.setCursor(0, 0);
  lcd.print("Hora: ");
  
  if (formato24h) {
    // 24-hour format
    if (horas < 10) lcd.print("0");
    lcd.print(horas);
  } else {
    // 12-hour format
    byte horasMostradas = horas;
    if (horasMostradas == 0) {
      horasMostradas = 12;
    } else if (horasMostradas > 12) {
      horasMostradas -= 12;
    }
    
    if (horasMostradas < 10) lcd.print("0");
    lcd.print(horasMostradas);
  }
  
  lcd.print(":");
  if (minutos < 10) lcd.print("0");
  lcd.print(minutos);
  lcd.print(":");
  if (segundos < 10) lcd.print("0");
  lcd.print(segundos);
  
  if (!formato24h) {
    lcd.print(isPM ? " PM" : " AM");
  }
  
  // Second line: Date
  lcd.setCursor(0, 1);
  lcd.print("Fecha:");
  if (dia < 10) lcd.print("0");
  lcd.print(dia);
  lcd.print("/");
  if (mes < 10) lcd.print("0");
  lcd.print(mes);
  lcd.print("/");
  lcd.print(anio);
}

// Function to display main menu
void displayMenu() {
  lcd.clear();
  
  String menuItems[MENU_SIZE] = {
    "Reloj",
    "Timer",
    "Registro Eventos",
    "Programar Tareas"
  };
  
  lcd.setCursor(0, 0);
  lcd.print("> ");
  lcd.print(menuItems[menuPosition]);
  
  if (menuPosition < MENU_SIZE - 1) {
    lcd.setCursor(0, 1);
    lcd.print("  ");
    lcd.print(menuItems[menuPosition + 1]);
  }
}

// Function to handle button presses
void handleButtons() {
  lcd_key = read_LCD_buttons();
  
  // Debounce buttons
  if (lcd_key != BTN_NONE && !buttonPressed && (millis() - lastButtonPressMillis > 250)) {
    buttonPressed = true;
    lastButtonPressMillis = millis();
    
    if (!inSubMenu) {
      // Main Menu Navigation
      switch (lcd_key) {
        case BTN_UP:
          menuPosition = (menuPosition > 0) ? menuPosition - 1 : MENU_SIZE - 1;
          break;
        case BTN_DOWN:
          menuPosition = (menuPosition < MENU_SIZE - 1) ? menuPosition + 1 : 0;
          break;
        case BTN_SELECT:
          currentMenu = menuPosition;
          inSubMenu = true;
          break;
      }
    } else {
      // Submenu Actions
      switch (currentMenu) {
        case MENU_CLOCK:
          handleClockButtons(lcd_key);
          break;
        case MENU_TIMER:
          handleTimerButtons(lcd_key);
          break;
        case MENU_EVENT_LOG:
          handleEventLogButtons(lcd_key);
          break;
        case MENU_TASK_SCHEDULER:
          handleTaskSchedulerButtons(lcd_key);
          break;
      }
    }
  } else if (lcd_key == BTN_NONE) {
    buttonPressed = false;
  }
}

// Handle buttons in clock mode
void handleClockButtons(int button) {
  switch (button) {
    case BTN_LEFT:
      inSubMenu = false;  // Return to main menu
      break;
    case BTN_SELECT:
      formato24h = !formato24h;  // Toggle 12/24 hour format
      break;
  }
}

// Handle buttons in timer mode
void handleTimerButtons(int button) {
  switch (button) {
    case BTN_LEFT:
      inSubMenu = false;  // Return to main menu
      break;
    case BTN_SELECT:
      if (!timerRunning) {
        // Start timer
        timerDurationMillis = (long)timerHours * 3600000L + (long)timerMinutes * 60000L + (long)timerSeconds * 1000L;
        if (timerDurationMillis > 0) {
          timerStartMillis = millis();
          timerRunning = true;
          logEvent(0);  // Log timer event
        }
      } else {
        // Stop timer
        timerRunning = false;
        digitalWrite(RELAY_PIN, LOW);
      }
      break;
    case BTN_UP:
      if (!timerRunning) {
        timerMinutes++;
        if (timerMinutes > 59) {
          timerMinutes = 0;
          timerHours++;
          if (timerHours > 23) timerHours = 0;
        }
      }
      break;
    case BTN_DOWN:
      if (!timerRunning) {
        if (timerMinutes > 0) {
          timerMinutes--;
        } else {
          if (timerHours > 0) {
            timerHours--;
            timerMinutes = 59;
          }
        }
      }
      break;
    case BTN_RIGHT:
      if (!timerRunning) {
        timerSeconds += 10;
        if (timerSeconds > 59) {
          timerSeconds = 0;
          timerMinutes++;
          if (timerMinutes > 59) {
            timerMinutes = 0;
            timerHours++;
            if (timerHours > 23) timerHours = 0;
          }
        }
      }
      break;
  }
}

// Handle buttons in event log mode
void handleEventLogButtons(int button) {
  switch (button) {
    case BTN_LEFT:
      inSubMenu = false;  // Return to main menu
      break;
    case BTN_UP:
      if (eventCount > 0) {
        currentEvent = (currentEvent > 0) ? (currentEvent - 1) : (eventCount - 1);
      }
      break;
    case BTN_DOWN:
      if (eventCount > 0) {
        currentEvent = (currentEvent < eventCount - 1) ? (currentEvent + 1) : 0;
      }
      break;
  }
}

// Handle buttons in task scheduler mode
void handleTaskSchedulerButtons(int button) {
  switch (button) {
    case BTN_LEFT:
      if (taskSetting == 0) {
        inSubMenu = false;  // Return to main menu
      } else {
        taskSetting--;
      }
      break;
    case BTN_RIGHT:
      if (taskSetting < 5) {
        taskSetting++;
      } else {
        taskSetting = 0;
        saveTasksToEEPROM();
      }
      break;
    case BTN_UP:
      // Modify task setting values
      switch (taskSetting) {
        case 0:  // startHour
          tasks[currentTask].startHour = (tasks[currentTask].startHour + 1) % 24;
          break;
        case 1:  // startMinute
          tasks[currentTask].startMinute = (tasks[currentTask].startMinute + 1) % 60;
          break;
        case 2:  // durationHours
          tasks[currentTask].durationHours = (tasks[currentTask].durationHours + 1) % 24;
          break;
        case 3:  // durationMinutes
          tasks[currentTask].durationMinutes = (tasks[currentTask].durationMinutes + 1) % 60;
          break;
        case 4:  // pin
          tasks[currentTask].pin = (tasks[currentTask].pin >= 13) ? 2 : tasks[currentTask].pin + 1;
          break;
        case 5:  // enabled
          tasks[currentTask].enabled = !tasks[currentTask].enabled;
          break;
      }
      break;
    case BTN_DOWN:
      // Modify task setting values
      switch (taskSetting) {
        case 0:  // startHour
          tasks[currentTask].startHour = (tasks[currentTask].startHour > 0) ? tasks[currentTask].startHour - 1 : 23;
          break;
        case 1:  // startMinute
          tasks[currentTask].startMinute = (tasks[currentTask].startMinute > 0) ? tasks[currentTask].startMinute - 1 : 59;
          break;
        case 2:  // durationHours
          tasks[currentTask].durationHours = (tasks[currentTask].durationHours > 0) ? tasks[currentTask].durationHours - 1 : 23;
          break;
        case 3:  // durationMinutes
          tasks[currentTask].durationMinutes = (tasks[currentTask].durationMinutes > 0) ? tasks[currentTask].durationMinutes - 1 : 59;
          break;
        case 4:  // pin
          tasks[currentTask].pin = (tasks[currentTask].pin <= 2) ? 13 : tasks[currentTask].pin - 1;
          break;
        case 5:  // enabled
          tasks[currentTask].enabled = !tasks[currentTask].enabled;
          break;
      }
      break;
    case BTN_SELECT:
      // Navigate through tasks
      currentTask = (currentTask + 1) % MAX_TASKS;
      taskSetting = 0;
      break;
  }
}

// Timer management
void handleTimer() {
  unsigned long elapsedMillis = millis() - timerStartMillis;
  
  if (elapsedMillis >= timerDurationMillis) {
    // Timer completed
    timerRunning = false;
    
    // Activate buzzer
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(300);
    digitalWrite(BUZZER_PIN, LOW);
    
    // Log event
    logEvent(0);  // 0 = Timer event
  } else {
    // Timer is running - keep relay activated
    digitalWrite(RELAY_PIN, HIGH);
    
    // Calculate remaining time
    unsigned long remainingMillis = timerDurationMillis - elapsedMillis;
    timerHours = remainingMillis / 3600000L;
    timerMinutes = (remainingMillis % 3600000L) / 60000L;
    timerSeconds = (remainingMillis % 60000L) / 1000L;
  }
}

// Display timer screen
void displayTimer() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Timer: ");
  if (timerRunning) {
    lcd.print("ACTIVO");
  } else {
    lcd.print("PARADO");
  }
  
  lcd.setCursor(0, 1);
  if (timerHours < 10) lcd.print("0");
  lcd.print(timerHours);
  lcd.print(":");
  if (timerMinutes < 10) lcd.print("0");
  lcd.print(timerMinutes);
  lcd.print(":");
  if (timerSeconds < 10) lcd.print("0");
  lcd.print(timerSeconds);
}

// Log an event
void logEvent(byte type) {
  if (eventCount < MAX_EVENTS) {
    // Store at next available position
    byte index = eventCount;
    
    // Address calculation
    int address = EEPROM_EVENT_START + (index * sizeof(Event));
    
    // Store event
    EEPROM.update(address++, horas);    // Hour
    EEPROM.update(address++, minutos);  // Minute
    EEPROM.update(address++, segundos); // Second
    EEPROM.update(address++, dia);      // Day
    EEPROM.update(address++, mes);      // Month
    EEPROM.update(address++, anio & 0xFF); // Year (low byte)
    EEPROM.update(address++, (anio >> 8) & 0xFF); // Year (high byte)
    EEPROM.update(address++, type);     // Event type
    
    eventCount++;
    EEPROM.update(EEPROM_EVENT_COUNT, eventCount);
  } else {
    // Shift all events one position back
    for (byte i = 0; i < MAX_EVENTS - 1; i++) {
      int srcAddr = EEPROM_EVENT_START + ((i + 1) * sizeof(Event));
      int destAddr = EEPROM_EVENT_START + (i * sizeof(Event));
      
      for (byte j = 0; j < sizeof(Event); j++) {
        EEPROM.update(destAddr + j, EEPROM.read(srcAddr + j));
      }
    }
    
    // Add new event at the end
    int address = EEPROM_EVENT_START + ((MAX_EVENTS - 1) * sizeof(Event));
    EEPROM.update(address++, horas);    // Hour
    EEPROM.update(address++, minutos);  // Minute
    EEPROM.update(address++, segundos); // Second
    EEPROM.update(address++, dia);      // Day
    EEPROM.update(address++, mes);      // Month
    EEPROM.update(address++, anio & 0xFF); // Year (low byte)
    EEPROM.update(address++, (anio >> 8) & 0xFF); // Year (high byte)
    EEPROM.update(address++, type);     // Event type
  }
}

// Display event log screen
void displayEventLog() {
  lcd.clear();
  
  if (eventCount == 0) {
    lcd.setCursor(0, 0);
    lcd.print("No hay eventos");
    lcd.setCursor(0, 1);
    lcd.print("registrados");
  } else {
    // Calculate address of current event
    int address = EEPROM_EVENT_START + (currentEvent * sizeof(Event));
    
    // Read event data
    byte evHour = EEPROM.read(address++);
    byte evMinute = EEPROM.read(address++);
    byte evSecond = EEPROM.read(address++);
    byte evDay = EEPROM.read(address++);
    byte evMonth = EEPROM.read(address++);
    int evYear = EEPROM.read(address++) | (EEPROM.read(address++) << 8);
    byte evType = EEPROM.read(address);
    
    // Display event details
    lcd.setCursor(0, 0);
    lcd.print("Ev");
    lcd.print(currentEvent + 1);
    lcd.print("/");
    lcd.print(eventCount);
    
    // Display event type
    if (evType == 0) {
      lcd.print(" Timer");
    } else {
      lcd.print(" Tarea");
    }
    
    // Display date and time
    lcd.setCursor(0, 1);
    if (evDay < 10) lcd.print("0");
    lcd.print(evDay);
    lcd.print("/");
    if (evMonth < 10) lcd.print("0");
    lcd.print(evMonth);
    lcd.print(" ");
    if (evHour < 10) lcd.print("0");
    lcd.print(evHour);
    lcd.print(":");
    if (evMinute < 10) lcd.print("0");
    lcd.print(evMinute);
  }
}

// Display task scheduler screen
void displayTaskScheduler() {
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("Tarea ");
  lcd.print(currentTask + 1);
  lcd.print(" ");
  
  // Show settings based on current taskSetting variable
  switch (taskSetting) {
    case 0:
      lcd.print("Hr:");
      if (tasks[currentTask].startHour < 10) lcd.print("0");
      lcd.print(tasks[currentTask].startHour);
      break;
    case 1:
      lcd.print("Min:");
      if (tasks[currentTask].startMinute < 10) lcd.print("0");
      lcd.print(tasks[currentTask].startMinute);
      break;
    case 2:
      lcd.print("DurHr:");
      lcd.print(tasks[currentTask].durationHours);
      break;
    case 3:
      lcd.print("DurMin:");
      lcd.print(tasks[currentTask].durationMinutes);
      break;
    case 4:
      lcd.print("Pin:");
      lcd.print(tasks[currentTask].pin);
      break;
    case 5:
      lcd.print("Estado:");
      lcd.print(tasks[currentTask].enabled ? "ON" : "OFF");
      break;
  }
  
  // Display status
  lcd.setCursor(0, 1);
  lcd.print("Estado: ");
  if (tasks[currentTask].isRunning) {
    lcd.print("ACTIVO");
  } else if (tasks[currentTask].enabled) {
    lcd.print("ESPERA");
  } else {
    lcd.print("APAGADO");
  }
}

// Update task status
void updateTaskStatus() {
  for (byte i = 0; i < MAX_TASKS; i++) {
    if (tasks[i].enabled) {
      // Check if it's time to start the task
      if (!tasks[i].isRunning && 
          horas == tasks[i].startHour && 
          minutos == tasks[i].startMinute) {
        
        // Start task
        tasks[i].isRunning = true;
        tasks[i].startTimeMillis = millis();
        
        // Activate the pin
        pinMode(tasks[i].pin, OUTPUT);
        digitalWrite(tasks[i].pin, HIGH);
        
        // Log event
        logEvent(1);  // 1 = Task event
      }
      
      // Check if task needs to stop
      if (tasks[i].isRunning) {
        unsigned long taskDurationMillis = (long)tasks[i].durationHours * 3600000L + 
                                           (long)tasks[i].durationMinutes * 60000L;
        
        if (millis() - tasks[i].startTimeMillis >= taskDurationMillis) {
          // Stop task
          tasks[i].isRunning = false;
          
          // Deactivate the pin
          digitalWrite(tasks[i].pin, LOW);
        }
      }
    } else if (tasks[i].isRunning) {
      // Task is running but has been disabled, stop it
      tasks[i].isRunning = false;
      digitalWrite(tasks[i].pin, LOW);
    }
  }
}

// Function to check if a year is a leap year
bool esBisiesto(int year) {
  return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}

// Save tasks to EEPROM
void saveTasksToEEPROM() {
  EEPROM.update(EEPROM_TASK_COUNT, taskCount);
  
  int address = EEPROM_TASK_START;
  for (byte i = 0; i < MAX_TASKS; i++) {
    EEPROM.update(address++, tasks[i].startHour);
    EEPROM.update(address++, tasks[i].startMinute);
    EEPROM.update(address++, tasks[i].durationHours);
    EEPROM.update(address++, tasks[i].durationMinutes);
    EEPROM.update(address++, tasks[i].pin);
    EEPROM.update(address++, tasks[i].enabled ? 1 : 0);
    // isRunning and startTimeMillis are not stored in EEPROM
    address += 2;  // Skip space for these runtime values
  }
}

// Load tasks from EEPROM
void loadTasksFromEEPROM() {
  taskCount = EEPROM.read(EEPROM_TASK_COUNT);
  if (taskCount > MAX_TASKS) taskCount = MAX_TASKS;
  
  int address = EEPROM_TASK_START;
  for (byte i = 0; i < MAX_TASKS; i++) {
    tasks[i].startHour = EEPROM.read(address++);
    tasks[i].startMinute = EEPROM.read(address++);
    tasks[i].durationHours = EEPROM.read(address++);
    tasks[i].durationMinutes = EEPROM.read(address++);
    tasks[i].pin = EEPROM.read(address++);
    tasks[i].enabled = EEPROM.read(address++) == 1;
    tasks[i].isRunning = false;
    tasks[i].startTimeMillis = 0;
    address += 2;  // Skip space for runtime values
  }
}

// Load events from EEPROM
void loadEventsFromEEPROM() {
  eventCount = EEPROM.read(EEPROM_EVENT_COUNT);
  if (eventCount > MAX_EVENTS) eventCount = MAX_EVENTS;
  currentEvent = 0;
}

// Button reading function (adapted from your code)
int read_LCD_buttons() {
  adc_key_in = analogRead(0);  // read the value
  
  if (adc_key_in > 1000) return BTN_NONE;
  if (adc_key_in < 50) return BTN_RIGHT;
  if (adc_key_in < 250) return BTN_UP;
  if (adc_key_in < 450) return BTN_DOWN;
  if (adc_key_in < 650) return BTN_LEFT;
  if (adc_key_in < 850) return BTN_SELECT;
  
  return BTN_NONE;
}