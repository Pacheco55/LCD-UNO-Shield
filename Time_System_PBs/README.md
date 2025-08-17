# PIXEL TIME SYSTEM - PIXELBITS Edition

![Arduino](https://img.shields.io/badge/Arduino-UNO-blue)
![LCD](https://img.shields.io/badge/Display-16x2-green)
![EEPROM](https://img.shields.io/badge/Storage-EEPROM-orange)
![Timer](https://img.shields.io/badge/Function-Multi%20Timer-red)

**Sistema completo de gestión temporal con múltiples funciones avanzadas**

---

## **Funcionalidad**

**Reloj Tiempo Real**
Reloj con formato 12h/24h, fecha automática con cálculo de años bisiestos y visualización continua.

**Temporizador Regresivo**
Cuenta regresiva configurable en formato hh:mm:ss con notificación por buzzer al finalizar.

**Registro de Eventos**
Almacenamiento permanente en EEPROM de eventos del sistema, activaciones y ejecución de tareas programadas.

**Programador de Tareas**
Hasta 5 tareas temporizadas para control automático de pines con ejecución diferida.

---

## **Requerimientos**

### **Hardware**
Arduino UNO/Nano, LCD Keypad Shield D1 Robot (16x2), buzzer, relé, conexión USB

### **Conexiones**
```
LCD: RS=8, E=9, D4=4, D5=5, D6=6, D7=7
Botones: Pin analógico A0
Relé: Pin 15
Buzzer: Pin 11
```

### **Software**
Arduino IDE 1.8.5+, librerías LiquidCrystal.h y EEPROM.h (incluidas)

---

## **Uso de Memoria EEPROM**

**Almacenamiento de Datos**
```
0-9: Contador de eventos y metadatos
10-99: Almacenamiento de eventos (máximo 10)
100: Contador de tareas
110-209: Almacenamiento de tareas (máximo 5)
```

---

## **Controles**

**UP/DOWN**: Navegar por menús
**LEFT**: Salir/volver al menú anterior
**RIGHT/SELECT**: Confirmar selección/entrar a submenu

---

## **Aplicaciones**

Automatización doméstica, control de procesos temporizados, registro de eventos del sistema y gestión de tareas programadas.

---

## **Créditos**

### **Desarrollo**
**PIXELBITS Studios & Pacheco JC 55**

### **Redes Sociales**
**[Instagram](https://instagram.com/pixelbits_studios) | [Twitch](https://twitch.tv/pixelbits_studio/about) | [GitHub](https://github.com/Pacheco55/PIXELBITS-Studio-blog/tree/HTMLpbsb)**
