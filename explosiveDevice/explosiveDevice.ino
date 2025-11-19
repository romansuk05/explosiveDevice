/*links:
 * https://www.youtube.com/watch?v=NAyt5kQcn-A
 * https://randomnerdtutorials.com/guide-to-1-8-tft-display-with-arduino/
 * https://github.com/inflop/Countimer
 */
 
#include <Keypad.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Countimer.h>
#include <avr/sleep.h>

#define lineNum 4
#define columnNum 3
#define cs   10
#define dc   9
#define rst  12 //display ports

#define buzzer 3
#define buzzerFreq 1000
#define led 6
#define ledVal 200

#define black 0x0000
#define white 0xFFFF //colors

int hours=00;
int minutes=00;
int seconds=00; //timer settings
String activeCode="141421"; //activating code - √(2)
String defuseCode="314159"; //defusing code - pi
String inputCode;
int i, j, k=0; //counters
int phase=0;
boolean firstScreen=true;
boolean defuse;
boolean keypadBlock=!true; //for blocking keyboard after exposion

char keys[lineNum][columnNum] = {
  {'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '9'}, {'*', '0', '#'}
};

byte linePin [lineNum] = {4, 5, 7, 8}; 
byte columnPin [columnNum] = {A0, A1, A2}; //keysboard settings

Countimer timer;
Keypad keypad = Keypad(makeKeymap(keys), linePin, columnPin, lineNum, columnNum);
Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);

void setup() {
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(black);
  tft.setRotation(3);
  tft.setTextWrap(false);//true
  tft.setTextColor(white);
  tft.setTextSize(1); //display
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
}

void loop() {
  timer.run();
  bomb();
}

void settingsScreen() {
  char key=keypad.getKey();
  tft.setCursor(0, 0);
  tft.print("TIMER SETTINGS");
  tft.setCursor(0, 25);
  tft.print("HOURS");
  tft.setCursor(0, 45);
  tft.print("MINUTES");
  tft.setCursor(0, 65);
  tft.print("SECONDS");
  tft.setCursor(0, 110);
  tft.print("# - COMFIRM INPUT \n");
  tft.print("* - DELETE INPUT");
  switch (k) {
    case 0:
      tft.setCursor(60, 25);
      tft.print(inputCode);
      break;
    case 1:
      tft.setCursor(60, 45);
      tft.print(inputCode);
      break;
    case 2:
      tft.setCursor(60, 65);
      tft.print(inputCode);
      break;
    default:
      tft.fillRect(0, 0, 160, 128, black);
      tft.setCursor(0, 0);
      tft.print("SETTING CURSOR ERROR");
  }
}

void activatingScreen() {
  tft.setCursor(0, 0);
  tft.print("ACTIVE BOMB"); 
  tft.setCursor(0, 80);
  tft.print(inputCode);
}

void defusingScreen() {
  tft.setCursor(0, 0);
  tft.print("BOMB ACTIVATED \n");
  tft.print("STARTING TIMER \n");
  tft.print("DEFUSE WITH CODE");
  tft.setCursor(0,50);
  tft.print("STATUS: ");
  tft.setCursor(0, 80);
  tft.print(inputCode);
  tft.setCursor(85, 35);
  tft.print("ATTEMPS: ");
}

void exploded() {
  tft.fillRect(48, 50, 60, 7, black);
  tft.setCursor(48, 50);
  tft.print("EXPLODED");
  tft.fillRect(142, 35, 6, 7, black);
  tft.setCursor(140, 35);
  tft.print("3/3");
  keypadBlock=true;
  timer.stop();
  tone(buzzer, 500);
  sleep_enable();
  sleep_cpu();
}

void refreshTimer() {
  tft.setTextColor(white, black);
  tft.fillRect(0, 70, 0, 0, black);
  tft.setCursor(0, 70);
  tft.print(timer.getCurrentTime());
  tone(buzzer, buzzerFreq);
  analogWrite(led, ledVal);
  switch (j) { //adding more mental pressure for defuser by beeping and blinking:D
    case 1:
      delay(100);
      noTone(buzzer);
      analogWrite(led, 0);
      delay(100);
      tone(buzzer, buzzerFreq);
      analogWrite(led, ledVal);
      delay(100);
      break;
    case 2:
      delay(100);
      noTone(buzzer);
      analogWrite(led, 0);
      delay(100);
      tone(buzzer, buzzerFreq);
      analogWrite(led, ledVal);
      delay(100);
      noTone(buzzer);
      analogWrite(led, 0);
      delay(100);
      tone(buzzer, buzzerFreq);
      analogWrite(led, ledVal);
      delay(100);
      break;
    default:
      delay(100);
  }
  noTone(buzzer);
  analogWrite(led, 0);
}

void deleteCode() {
  tft.fillRect(0, 80, 36, 7, black); //deleting one line (x, y, l, h, color)
  inputCode="";
  i=0;
}

void bomb() {
  char key=keypad.getKey();
  switch (phase) {
    case 0:
      settingsScreen();
      break;
    case 1:
      activatingScreen();
      break;
    case 2:
      defusingScreen();
      break;
    default:
      tft.fillRect(0, 0, 160, 128, black);
      tft.setCursor(0, 0);
      tft.print("SCREEN ERROR"); 
      break;
  }
  if (key && keypadBlock == !true){
    i++;
    if (key == '#') {
      switch (phase) {
        case 0:
          switch (k) {
            case 0:
              hours=inputCode.toInt();
              k++;
              break;
            case 1:
              minutes=inputCode.toInt();
              k++;
              break;
            case 2:
              seconds=inputCode.toInt();
              k++;
              timer.setCounter(hours, minutes, seconds, timer.COUNT_DOWN, exploded); //timer
              phase++;
              tft.fillRect(0, 0, 150, 100, black);
              break;
            }
          break;
        case 1:
          if (inputCode == activeCode) {
            tft.fillRect(0, 0, 83, 15, black);
            tft.setCursor(140, 35);
            tft.print("0/3");
            tft.setCursor(48, 50);
            tft.print("ACTIVATED");
            timer.start();
            timer.setInterval(refreshTimer, 1000);
            phase++;
          }else if (inputCode != activeCode) {
            tft.fillRect(0, 0, 83, 15, black);
            tft.setCursor(0, 0);
            tft.print("WRONG PASSWORD \n");
            tft.print("TRY AGAIN");
            delay(3000);
            tft.fillRect(0, 0, 83, 15, black);
          }else{
            deleteCode();
            tft.fillRect(0, 0, 160, 128, black);
            tft.setCursor(0, 0);
            tft.print("PHASE 1 ERROR");
          }
          break;
        case 2:
          if (inputCode == defuseCode) {
            tft.fillRect(48, 50, 60, 7, black);
            tft.setCursor(48, 50);
            tft.print("DEFUSED");
            timer.stop();
            noTone(buzzer);
            analogWrite(led, 0);
            sleep_enable();
            sleep_cpu();
          }else if (inputCode != defuseCode) {
            tft.fillRect(142, 35, 6, 7, black); 
            tft.setCursor(140, 35);
            switch (j) {
              case 0:
                tft.print("1/3");
                j++;
                break;
              case 1:
                tft.print("2/3");
                j++;
                break;
              case 2:
                exploded();
                break;
              } 
            }else {
              deleteCode();
              tft.fillRect(0, 0, 160, 128, black);
              tft.setCursor(0, 0);
              tft.print("PHASE 2 ERROR");
            }
            break;
        default:
          deleteCode();
          tft.fillRect(0, 0, 160, 128, black);
          tft.setCursor(0, 0);
          tft.print("PHASE ERROR");
          break;
        }
        deleteCode();
      }
      else if (key == '*' || i == 7 ) {
        switch (phase) {//if (phase == 0){
          case 0:
            switch (k) {
              case 0:
                tft.fillRect(60, 25, 100, 7, black);
                inputCode="";
                tft.setCursor(60, 25);
                i=0;
                break;
              case 1:
                tft.fillRect(60, 45, 100, 7, black);
                inputCode="";
                tft.setCursor(60, 45);
                i=0;
                break;
              case 2:
                tft.fillRect(60, 65, 100, 7, black);
                inputCode="";
                tft.setCursor(60, 65);
                i=0;
                break;
              }
            break;
          case 1:
            deleteCode();
            break;
          case 2:
            deleteCode();
            break;
          default:
            deleteCode();
            tft.fillRect(0, 0, 160, 128, black);
            tft.setCursor(0, 0);
            tft.print("DELETE ERROR");
            break;
          }
      }else{
        inputCode += key;
      }
   }
}
