# Reloj Digital 12h/24h - PIXELBITS Edition

![Arduino](https://img.shields.io/badge/Arduino-UNO-blue)
![LCD](https://img.shields.io/badge/Display-16x2-green)
![RTC](https://img.shields.io/badge/Time-Real%20Time-orange)

**Reloj en tiempo real con formato dual y gestión automática de fecha**

---

## **Funcionalidad**

**Reloj Tiempo Real**
Simulación de reloj con hora y fecha que se actualiza automáticamente cada segundo con cálculo preciso de días, meses y años.

**Formato Dual**
Alternancia entre formato 12 horas (AM/PM) y 24 horas presionando el botón SELECT del LCD Keypad Shield.

**Gestión de Fecha Avanzada**
Actualización automática de fecha con lógica completa para años bisiestos y diferentes días por mes (28, 30, 31 días).

---

## **Requerimientos**

### **Hardware**
Arduino UNO/Nano, LCD Keypad Shield D1 Robot (16x2), conexión USB para programación

### **Conexiones LCD Keypad Shield**
```
Pin 8  → RS del LCD
Pin 9  → Enable del LCD  
Pin 4  → Datos D4
Pin 5  → Datos D5
Pin 6  → Datos D6
Pin 7  → Datos D7
Pin A0 → Lectura de botones (analógico)
```

### **Software**
Arduino IDE 1.8.5+, librería LiquidCrystal.h (incluida)

---

## **Controles**

**SELECT**: Alternar entre formato 12h/24h

---

## **Características Técnicas**

**Cálculo Temporal**
Incremento automático de segundos, minutos, horas, días, meses y años con verificación de año bisiesto y días correctos por mes.

**Visualización**
Pantalla dividida mostrando hora en línea superior y fecha en línea inferior con actualización continua.

---

## **Créditos**

### **Desarrollo**
**PIXELBITS Studios & Pacheco JC 55**

### **Redes Sociales**
**[Instagram](https://www.instagram.com/pixelbits_studios/) | [Twitch](https://www.twitch.tv/pixelbits_studio/about) | [GitHub](https://github.com/Pacheco55/PIXELBITS-Studio-blog/tree/HTMLpbsb)**
