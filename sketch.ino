#include <TinyWireM.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>

SSD1306AsciiWire oled;

unsigned long prevMillis = 0;
int seconds = 0, minutes = 53, hours = 14;
int dayOfWeek = 0;
int day = 2, month = 11, year = 2025;

const char* daysOfWeek[] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};

int pomodoroMinutes = 25;
int pomodoroSeconds = 0;
bool isWorkSession = 1;
bool prevSessionState = 1;

bool isLeapYear(int y) {
  return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

int daysInMonth(int m, int y) {
  if (m == 2) return isLeapYear(y) ? 29 : 28;
  if (m == 4 || m == 6 || m == 9 || m == 11) return 30;
  return 31;
}

void drawSessionText(bool s) {
  oled.setCursor(0, 6);
  oled.clearToEOL();
  oled.setCursor(s ? 52 : 49, 6);
  oled.print(s ? "work" : "break");
}

void printTime() {
  char buf[9];
  sprintf(buf, "%02d:%02d:%02d", hours, minutes, seconds);
  oled.setCursor(80, 0);
  oled.print(buf);
}

void printDate() {
  char buf[6];
  oled.setCursor(80, 1);
  oled.print(daysOfWeek[dayOfWeek]);
  oled.print(' ');
  sprintf(buf, "%02d/%02d", day, month);
  oled.print(buf);
}

void printPomodoro() {
  char buf[6];
  sprintf(buf, "%02d:%02d", pomodoroMinutes, pomodoroSeconds);
  oled.set2X();
  oled.setCursor(34, 4);
  oled.print(buf);
  oled.set1X();
}

void setup() {
  TinyWireM.begin();
  oled.begin(&Adafruit128x64, 0x3C);
  oled.setFont(Adafruit5x7);
  oled.clear();

  oled.set2X();
  oled.setCursor(0, 0);
  oled.print("Ketsui");
  oled.set1X();

  printTime();
  printDate();

  oled.setCursor(0, 2);
  for(int i=0;i<21;i++)oled.print('-');

  printPomodoro();
  drawSessionText(isWorkSession);

  oled.setCursor(106, 4);
  oled.print("o");
  oled.setCursor(100, 5);
  oled.print("\\|\\");
  oled.setCursor(100, 6);
  oled.print("/ \\");

  oled.setCursor(14, 3);
  oled.print("o");
  oled.setCursor(8, 4);
  oled.print("\\|\\");
  oled.setCursor(8, 5);
  oled.print("/ \\");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - prevMillis >= 1000) {
    prevMillis = currentMillis;

    seconds++;
    if (seconds >= 60) {
      seconds = 0;
      minutes++;
      if (minutes >= 60) {
        minutes = 0;
        hours++;
        if (hours >= 24) {
          hours = 0;
          dayOfWeek++;
          if (dayOfWeek >= 7) dayOfWeek = 0;
          day++;
          if (day > daysInMonth(month, year)) {
            day = 1;
            month++;
            if (month > 12) {
              month = 1;
              year++;
            }
          }
        }
      }
    }

    if (pomodoroSeconds == 0) {
      if (pomodoroMinutes == 0) {
        isWorkSession = !isWorkSession;
        pomodoroMinutes = isWorkSession ? 25 : 5;
        pomodoroSeconds = 0;
        if (isWorkSession != prevSessionState) {
          prevSessionState = isWorkSession;
          drawSessionText(isWorkSession);
        }
      } else {
        pomodoroMinutes--;
        pomodoroSeconds = 59;
      }
    } else {
      pomodoroSeconds--;
    }

    printTime();
    printDate();
    printPomodoro();

    oled.setCursor(0, 7);
  for(int i=0;i<21;i++)oled.print('-');
  }
}
