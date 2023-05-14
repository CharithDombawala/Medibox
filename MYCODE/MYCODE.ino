#include <Adafruit_GFX.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
//#include <DHTesp.h>
//#include <WiFi.h>

#define  SCREEN_WIDTH 128
#define  SCREEN_HEIGHT 64
#define  OLED_RESET -1
#define  SCREEN_ADDRESS 0x3C

//#define BUZZER 5
//#define LED_1 15
#define pb_cancel 3  //34//3
#define pb_ok 2 //35//2
#define pb_down 5 //32//5
#define pb_up 4 //33//4
//#define DHTpin 12


//#define NTP_SERVER  "pool.ntp.org"
//#define UTC_OFFSET     0
//#define UTC_OFFSET_DST 0

//DHTesp dhtsensor;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Global variable

 int days=0;
 int hours=0;
 int minutes=0;
 int seconds=0;

 unsigned long timeNow=0;
 unsigned long timeLast=0;

bool alarm_enabled=true;
int n_alarms=2;
int alarm_hours[]={0,0};
int alarm_minutes[]={1,2};
bool alarm_triggered[]={false,false};

int n_tunes=8;
int C=262;
int D=294;
int E=330;
int F=349;
int G=392;
int A=440;
int B=494;
int C_H=523;
int tunes[]={C,D,E,F,G,A,B,C_H};

int current_mode=0;
int n_mode=4;
String modes[]={"1-Set Time","2-Set Temp Threshold","3-Set Humidity Threshold","4-Set Alarm"};

void setup() {
  // put your setup code here, to run once:


  //pinMode(BUZZER,OUTPUT);
  //pinMode(LED_1,OUTPUT);
  pinMode(pb_cancel,INPUT_PULLUP);
  pinMode(pb_ok, INPUT_PULLUP);
  pinMode(pb_down, INPUT_PULLUP);
  pinMode(pb_up, INPUT_PULLUP);
  //pinMode(DHTpin, INPUT);

  //dhtsensor.setup(DHTpin,DHTesp::DHT22);

  Serial.begin(115200);

  //ssd1306_swithhcapvcc = generate display voltage from 3.3v internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
            Serial.println(F("SSD1306 allocation failed"));
            while (true);  //for(;;);
}

/* WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    display.clearDisplay();
    print_text("connecting to Wi-FI",0,0,1);
  }

  display.clearDisplay();
  print_text("connected to Wi-FI",0,0,1);

  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
*/
 

  display.display();
  delay(2000);
  
  display.clearDisplay();
  //clear the buffer
  print_text("Welcome to Medibox",10,28,1);
  delay(2000);
  display.clearDisplay(); 

}

void loop() {
  // put your main code here, to run repeatedly:
  update_time_with_check_alarm();
if (digitalRead(pb_ok)==LOW){
     delay(200);
     go_to_menu();
}
  // check_temp();
}




void print_text(String text, int column , int row, int text_size){
  display.setTextSize(text_size);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(column,row);
  display.println(text);
  display.display();

}

void print_time_now(){
  display.clearDisplay(); 
  print_text(String(days),0,0,2);
  print_text(":",20,0,2);
  print_text(String(hours),30,0,2);
  print_text(":",50,0,2);
  print_text(String(minutes),60,0,2);
  print_text(":",80,0,2);
  print_text(String(seconds),90,0,2);

}

/*void update_time(){
  struct tm timeinfo;
  getLocalTime(&timeinfo);

  char timeHour[3];
  strftime(timeHour,3,"%H",&timeinfo);
  hours=atoi(timeHour);

  char timeMinute[3];
  strftime(timeMinute,3,"%M",&timeinfo);
  minutes=atoi(timeMinute);

  char timeSecond[3];
  strftime(timeSecond,3,"%S",&timeinfo);
  seconds=atoi(timeSecond);

  char timeDay[3];
  strftime(timeDay,3,"%H",&timeinfo);
  days=atoi(timeDay);

}*/


void update_time(){
  timeNow=millis()/1000;
  seconds=timeNow - timeLast;
  if (seconds>=60){
       minutes+=1;
       timeLast+=60;
  }       
  if (minutes==60){
       hours+=1;
       minutes=0;
  } 
  if (hours==24){
      days+=1;
      hours=0;
  }   
} 


void ring_alarm(){
  display.clearDisplay();
  print_text("medicine time!",0,0,2);

  //digitalWrite(LED_1, HIGH);
 
  bool break_happened = false;
 
 while(break_happened == false && digitalRead(pb_cancel) == HIGH){
    for(int j=0; j<n_tunes; j++){
      if(digitalRead(pb_cancel)==LOW){
        delay(200);
        break_happened=true;
        break;
      }
      //tone(BUZZER,tunes[j]);
      //delay(500);
      //noTone(BUZZER);
     // delay(2);

    }
 }
  //digitalWrite(LED_1, LOW);
  display.clearDisplay();

}

void update_time_with_check_alarm(){
  update_time();
  print_time_now();

  if(alarm_enabled){
    for(int i=0;i<n_alarms;i++){
      if(alarm_triggered[i]==false && alarm_hours[i]==hours && alarm_minutes[i]==minutes){
          ring_alarm();
          alarm_triggered[i]=true;
      }

    }
  }
}


int wait_for_button_press(){
  while(true){
    if(digitalRead(pb_ok)==LOW){
      delay(200);
      return pb_ok;
    }
    else if(digitalRead(pb_up)==LOW){
      delay(200);
      return pb_up;
    }
    else if(digitalRead(pb_down)==LOW){
      delay(200);
      return pb_down;
    }
    else if(digitalRead(pb_cancel)==LOW){
      delay(200);
      return pb_cancel;
    }
    update_time();
  }
}


void go_to_menu(){
  while(digitalRead(pb_cancel)==HIGH){
    display.clearDisplay();
    print_text(modes[current_mode],0,0,2);

    int pressed=wait_for_button_press();

    if(pressed==pb_up){
      delay(200);
      current_mode+=1;
      current_mode=current_mode % n_mode;
    }

    else if(pressed==pb_down){
      delay(200);
      current_mode-=1;
      if(current_mode<0){
        current_mode=n_mode-1;
      }
    }

    else if(pressed==pb_ok){
      delay(200);
      run_mode(current_mode);
    }

    else if(pressed==pb_cancel){
      delay(200);
      break;
    }

  }

}

void set_time(){
  int temp_hour=hours;
  while(true){
    display.clearDisplay();
    print_text("Enter hour :"+String(temp_hour),0,0,2);

    int pressed=wait_for_button_press();

    if(pressed==pb_up){
        delay(200);
        temp_hour+=1;
        temp_hour=temp_hour % 24;
      }

    else if(pressed==pb_down){
        delay(200);
        temp_hour-=1;
        if(temp_hour<0){
          temp_hour=23;
        }
      }

    else if(pressed==pb_ok){
        delay(200);
        hours=temp_hour;
        break;
      }

    else if(pressed==pb_cancel){
        delay(200);
        break;
    }
    }

    int temp_minute=minutes;
    while(true){
      display.clearDisplay();
      print_text("Enter minute :"+String(temp_minute),0,0,2);

      int pressed=wait_for_button_press();

      if(pressed==pb_up){
          delay(200);
          temp_minute+=1;
          temp_minute=temp_minute % 60;
        }

      else if(pressed==pb_down){
          delay(200);
          temp_minute-=1;
          if(temp_minute<0){
            temp_minute=59;
          }
        }

      else if(pressed==pb_ok){
          delay(200);
          minutes=temp_minute;
          break;
        }

      else if(pressed==pb_cancel){
          delay(200);
          break;
        }
      }

      display.clearDisplay();
      print_text("Time is set",0,0,2);
      delay(1500);
  }
  



void set_alarm(int alarm){
    int temp_hour=alarm_hours[alarm];
    while(true){
      display.clearDisplay();
      print_text("Enter hour :"+String(temp_hour),0,0,2);

      int pressed=wait_for_button_press();

      if(pressed==pb_up){
          delay(200);
          temp_hour+=1;
          temp_hour=temp_hour % 24;
        }

      else if(pressed==pb_down){
          delay(200);
          temp_hour-=1;
          if(temp_hour<0){
            temp_hour=23;
          }
        }

      else if(pressed==pb_ok){
          delay(200);
          alarm_hours[alarm]=temp_hour;
          break;
        }

      else if(pressed==pb_cancel){
          delay(200);
          break;
        }
      }

      int temp_minute=alarm_minutes[alarm];
      while(true){
        display.clearDisplay();
        print_text("Enter minute :"+String(temp_minute),0,0,2);

        int pressed=wait_for_button_press();

        if(pressed==pb_up){
            delay(200);
            temp_minute+=1;
            temp_minute=temp_minute % 60;
          }

        else if(pressed==pb_down){
            delay(200);
            temp_minute-=1;
            if(temp_minute<0){
              temp_minute=59;
            }
          }

        else if(pressed==pb_ok){
            delay(200);
            alarm_minutes[alarm]=temp_minute;
            break;
          }

        else if(pressed==pb_cancel){
            delay(200);
            break;
          }
        }

        display.clearDisplay();
        print_text("Alarm is set",0,0,2);
        delay(1500);

  }



void run_mode(int current_mode){
    if(current_mode==0){
      set_time();
    }

    else if(current_mode==1 || current_mode==2){
      set_alarm(current_mode-1);
    }

    else if(current_mode==3){
      alarm_enabled=false;
    }
}


// void check_temp(){
//   TempAndHumidity data= dhtsensor.getTempAndHumidity();
//   if(data.temperature>35){
//     display.clearDisplay();
//     print_text("Temp High",0,40,2);
//   }
//   else if(data.temperature<25){
//     display.clearDisplay();
//     print_text("Temp Low",0,40,2);
//   }
//   if(data.humidity>40){
//     display.clearDisplay();
//     print_text("Humidity High",50,40,2);
//   }
//   else if(data.humidity<20){
//     display.clearDisplay();
//     print_text("Humidity Low",50,40,2);
//   }
// }