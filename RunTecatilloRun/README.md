# RUN TECATILLO RUN - Microcontroller Edition

![Arduino](https://img.shields.io/badge/Arduino-UNO-blue)
![Game](https://img.shields.io/badge/Type-Endless%20Runner-green)
![LCD](https://img.shields.io/badge/Display-16x2-orange)
![Port](https://img.shields.io/badge/Original-Godot%20Engine-red)

**Port para microcontroladores del endless runner desarrollado originalmente en Godot Engine**

---

## **Funcionalidad**

**Endless Runner**
El personaje corre automáticamente y debe saltar obstáculos generados proceduralmente en fila superior e inferior.

**Sistema de Puntuación**
Puntuación por distancia recorrida con velocidad variable y progresión de dificultad.

**Controles Adaptados**
Gameplay completo adaptado para LCD Keypad Shield con pausa, aceleración y sprites animados.

**Generación Procedural**
Terreno y obstáculos aleatorios optimizados para memoria limitada de microcontroladores.

---

## **Requerimientos**

### **Hardware**
Arduino UNO/Nano, LCD Keypad Shield 16x2 con botones analógicos, conexión USB

### **Conexiones LCD Keypad Shield**
```
LCD: Pines 8,9,4,5,6,7 (estándar)
Botones: Pin analógico A0
```

### **Software**
Arduino IDE 1.8.5+, librería LiquidCrystal.h (incluida)

### **Memoria**
Optimizado para plataformas con 2KB RAM

---

## **Controles**

**UP**: Saltar obstáculos
**RIGHT**: Acelerar juego
**LEFT**: Reducir velocidad  
**DOWN**: Restablecer velocidad
**SELECT**: Pausar/Continuar

---

## **Características Técnicas**

**Sprites Personalizados**
7 sprites únicos para personaje, obstáculos y terreno con animaciones fluidas.

**Sistema de Colisiones**
Detección precisa de colisiones optimizada para LCD de caracteres.

**Gestión de Memoria**
Manejo optimizado para limitaciones de RAM en microcontroladores.

---

## **Origen del Juego**

**Plataforma Original**: Godot Engine
**Desarrollado para**: UPT y su comunidad
**Port Arduino**: Adaptación completa para microcontroladores

---

## **Créditos**

### **Desarrollo Original y Port**
**PIXELBITS Studios - Pacheco JC 55**

### **Redes Sociales**
**[Instagram](https://www.instagram.com/pixelbits_studios/) | [Twitch](https://www.twitch.tv/pixelbits_studio/about) | [GitHub](https://github.com/Pacheco55/PIXELBITS-Studio-blog/tree/HTMLpbsb)**
