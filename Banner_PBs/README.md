# Banner LCD Interactivo - PIXELBITS Edition

![Arduino](https://img.shields.io/badge/Arduino-UNO-blue)
![LCD](https://img.shields.io/badge/Display-16x2-green)
![Shield](https://img.shields.io/badge/Hardware-LCD%20Keypad%20Shield-orange)

**Sistema de mensajes personalizados con desplazamiento dinámico para displays LCD**

---

## **Funcionalidad**

**Editor de Mensajes Interactivo**
Permite escribir mensajes de hasta 16 caracteres usando los botones del LCD Keypad Shield con navegación tipo cursor carácter por carácter.

**Control de Desplazamiento**
Selección de dirección de scroll: derecha a izquierda, izquierda a derecha, arriba y abajo con velocidad ajustable entre 100 y 2000 milisegundos.

**Reproducción en Bucle**
El mensaje configurado se muestra en bucle continuo hasta presionar SELECT para reiniciar la configuración.

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

**UP/DOWN**: Cambiar carácter actual
**LEFT/RIGHT**: Mover cursor
**SELECT**: Confirmar y avanzar al siguiente paso

---

## **Aplicaciones**

Proyectos de identidad visual, menús interactivos, presentaciones creativas, señalización digital y displays informativos personalizables.

---

## **Créditos**

### **Desarrollo**
**PIXELBITS Studios & Pacheco JC 55**

### **Redes Sociales**
**[Instagram](https://www.instagram.com/pixelbits_studios/) | [Twitch](https://www.twitch.tv/pixelbits_studio/about) | [GitHub](https://github.com/Pacheco55/PIXELBITS-Studio-blog/tree/HTMLpbsb)**
