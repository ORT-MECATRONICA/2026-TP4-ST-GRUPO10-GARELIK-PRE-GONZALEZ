// Grupo 10 -- GARELIK,GONZALEZ,PRE

#include <Preferences.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define DHTPIN 23
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

unsigned long T;

const int SW1 = 35;
const int SW2 = 34;

enum Estados { MONITOR, ESPERA1, ENTRETIEMPO, CONFIG, SW1L, SW2L, ESPERA2 };
Estados estadoActual = MONITOR;

Preferences preferences;

int umbral = 25;
float temperatura = 0;


void setup() {
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);

  preferences.begin("my-app", false);
  umbral = preferences.getInt("umbral", 0); 
 
  dht.begin();
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for(;;);
  }
  display.clearDisplay();
}


void loop() {
  temperatura = dht.readTemperature();
  // El estado actual de la pantalla
  switch (estadoActual) {
    case MONITOR:
      mostrarMonitor(); //Modo pantalla 1
      if (digitalRead(SW1) == LOW){
        T = millis();
        estadoActual = ESPERA1;
      }
      
      break;

      case ESPERA1:
        if(digitalRead(SW1) == HIGH){
          estadoActual = MONITOR;
      } else if(digitalRead(SW1) == LOW && (millis() - T >= 5000)){
        estadoActual = ENTRETIEMPO;
      }
      break;

      case ENTRETIEMPO:
        if (digitalRead(SW1) == HIGH){
          estadoActual = CONFIG;
        }
     break;

    case CONFIG: 
      ajustarUmbral();
      if (digitalRead(SW1) == LOW){
      estadoActual = SW1L;
    }
    if (digitalRead(SW2) == LOW){
      T = millis();
     estadoActual = SW2L;
    }
    break;

    case SW1L:
    if (digitalRead(SW1) == HIGH){
      umbral++;
      estadoActual = CONFIG;
    }
    break;

    case SW2L:
        if(digitalRead(SW2) == HIGH){
          umbral--;
          estadoActual = CONFIG;
      } else if(digitalRead(SW2) == LOW && (millis() - T >= 5000)){
        estadoActual = ESPERA2;
      }
      break;

      case ESPERA2:
      if (digitalRead(SW2) == HIGH){
        preferences.putInt("umbral", umbral);
        estadoActual= MONITOR;
      }

}
}

void mostrarMonitor() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  display.setTextSize(2);
  display.setCursor(0,20);
  display.print("Temp: ");
  display.println(temperatura, 1);
 
  display.setTextSize(2);
  display.setCursor(0,50);
  display.print("Umbral: ");
  display.print(umbral);
  display.display();
}


void ajustarUmbral() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("MODO: CONFIG");
 
  display.setTextSize(2);
  display.setCursor(0,25);
  display.print("SET: ");
  display.print(umbral);
  display.display();
}
