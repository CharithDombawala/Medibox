#include <Adafruit_GFX.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define  SCREEN_WIDTH 128
#define  SCREEN_HEIGHT 64
#define  OLED_RESET -1
#define  SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//Global variable
 int days=0;
 int hours=0;
 int minutes=0;
 int seconds=0;

 unsigned long timeNow=0;
 unsigned long timeLast=0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  //ssd1306_swithhcapvcc = generate display voltage from 3.3v internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
            Serial.println(F("SSD1306 allocation failed"));
            while (true);  //for(;;);
}
//show the display buffer on thescreen. you must call display() after
//drawing commands to make them visisble on screen

  display.display();
  delay(2000);
display.clearDisplay();
//clear the buffer
print_text("Welcome to Medibox",10,20,2);
delay(2000);
display.clearDisplay(); 

}

void loop() {
  // put your main code here, to run repeatedly:
  print_time_now();
}




void print_text(String text, int column , int row, int text_size){
  display.setTextSize(text_size);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(column,row);
  display.println(text);
  display.display();

}

 void print_time_now(){
  print_text(String(days),0,0,2);
  print_text(":",20,0,2);
  print_text(String(hours),30,0,2);
  print_text(":",50,0,2);
  print_text(String(minutes),60,0,2);
  print_text(":",80,0,2);
  print_text(String(seconds),90,0,2);

 }

 void update_time(){
     timeNow=millis()/1000 //seconds passed from bootup
     seconds=timeNow-timelast


 }