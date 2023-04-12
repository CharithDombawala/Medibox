#include <Adafruit_GFX.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define  SCREEN_WIDTH 128
#define  SCREEN_HEIGHT 64
#define  OLED_RESET -1
#define  SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
            Serial.println(F("SSD1306 allocation failed"));
            while (true);  //for(;;);
}
  display.display();
  delay(2000);
  display.clearDisplay(); 
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println(F("Wlecome to Medibox"));
  display.display();

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10); // this speeds up the simulation
}