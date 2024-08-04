#include <DFPlayer.h>
#include "SoftwareSerial.h"
#include <microDS3231.h>
#include <FS.h>
#include "LittleFS.h"
#include <EncButton.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Update.h>
#include <SPI.h>
#include <MFRC522.h>


#define DEBUG 1
bool WEB = 1;

const String VERS = "std 0.2.5";

#define DebugWiFiSSID "Lebben"
#define DebugWiFiPassword "viktor26"


#define SS_PIN 14
#define RST_PIN 13
#define MP3_RX_PIN 4               //GPIO4/D2 to DFPlayer Mini TX
#define MP3_TX_PIN 5               //GPIO5/D1 to DFPlayer Mini RX
#define MP3_SERIAL_SPEED 9600      //DFPlayer Mini suport only 9600-baud
#define MP3_SERIAL_BUFFER_SIZE 32  //software serial buffer size in bytes, to send 8-bytes you need 11-bytes buffer (start byte+8-data bytes+parity-byte+stop-byte=11-bytes)
#define MP3_SERIAL_TIMEOUT 350     //average DFPlayer response timeout 200msec..300msec for YX5200/AAxxxx chip & 350msec..500msec for GD3200B/MH2024K chip


#define TFT_BG 0x0000
#define TFT_TEXT 0xbdd6
#define TFT_HEALTH 0xe081
#define TFT_ARMOR 0x02b6
#define TFT_GOOD 0x6fd3
#define TFT_WARNING 0xdee8
#define TFT_ALARM 0xf800


File logFile;
const int maxLogFiles = 10;
int currentLogIndex = 0;

String currentLogFileName;
String causeOfDeath;

bool response = 0;
bool selectedButton = 1;


uint64_t updatetime;
uint64_t checktimetimer;
uint64_t cardreturntimer;
uint64_t globalcheckplayerstats;
uint64_t globaldoeffects;
uint64_t radiationcheck;


const int DELAY_BETWEEN_CARDS = 1000;
long timeLastCardRead = 0;
bool readerDisabled = true;
int8_t currPage;
int8_t prevcurrPage;
int irqCurr;
int irqPrev;

bool globalUpdate;

bool isAnomaly;
bool isRadiation;
bool isArtefact;
bool isVykyd;
bool isWarning;
bool isAlarm;
bool isAuthenticated = false;

uint64_t currentsievert;
float prev_health = -1;
int prev_armor = -1;
int prev_radiation = -1;
int prev_min;


uint8_t card_data[16];
//uint8_t card_text[48];


struct Data {
  char ssid[20] = "ARSTAI:PDA";
  char pass[20] = "12345678";
  char adminpass[20] = "1122";
  float health = 100;
  int armor = 0;
  uint32_t radiation = 0;

  int fire_protection = 0;
  int gravi_protection = 0;
  int acid_protection = 0;
  int electro_protection = 0;
  int radiation_protection = 0;
  int psy_protection = 0;

  bool is_adept = 0;
  bool is_dead = 0;
  bool is_zombie = 0;
  bool is_under_control = 0;
  bool is_nocked = 0;
  bool is_ignor = 0;
};

Data data;

struct DecodedCard {
  String card_text_str;
  byte uid[7] = { 0, 0, 0, 0, 0, 0, 0 };
  byte usage_method = 0;
  byte type = 0;
  byte subtype = 0;
  byte protection_type = 0;
  byte percent = 0;
  byte value = 0;
  byte multiplier = 0;
  byte seconds = 0;
  byte minutes = 0;
  byte hours = 0;
};
DecodedCard card;

Button ok(0);
Button up(35);
Button down(36);
Button left(34);
Button right(39);

TFT_eSPI tft = TFT_eSPI();

MicroDS3231 rtc;
SoftwareSerial mp3Serial;
DFPlayer mp3;

const int NPCmenuItemsCount = 5;
String NPCmenuItems[NPCmenuItemsCount] = { "Поновити картку", "Вилікуватись", "Item 3", "Item 4", "Вийти" };
int NPCselectedItem = 0;




bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
  if (y >= tft.height()) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  return 1;
}


TaskHandle_t Task0;



bool update = 1;



unsigned long lastScanTime = 0;
const unsigned long scanInterval = 1000;  // Интервал сканирования 5 секунд


MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;


void setup() {
  Serial.begin(115200);
  pinMode(15, OUTPUT);
  digitalWrite(15, 1);

  if (!LittleFS.begin(false)) {
    Serial.println("LittleFS initialisation failed!");
    while (1) yield();  // Stay here twiddling thumbs waiting
  }
  if (!LittleFS.exists("/data.cfg")) {
    writeDefaultConfig();
  }
  readConfig();
  delay(50);

  // Load the current log index from a file
  loadLogIndex();
  delay(50);
  // Create a new log file for the current session
  createNewLogFile();
  delay(50);

  mp3Serial.begin(MP3_SERIAL_SPEED, SWSERIAL_8N1, MP3_RX_PIN, MP3_TX_PIN, false, MP3_SERIAL_BUFFER_SIZE, 0);  //false=signal not inverted, 0=ISR/RX buffer size (shared with serial TX buffer)

  mp3.begin(mp3Serial, MP3_SERIAL_TIMEOUT, DFPLAYER_MINI, false);  //"DFMINI" see NOTE, false=no response from module after the command

  mp3.stop();  //if player was runing during ESP8266 reboot

  mp3.reset();  //reset all setting to default

  mp3.setSource(2);  //1=USB-Disk, 2=TF-Card, 3=Aux, 4=Sleep, 5=NOR Flash

  mp3.setEQ(0);       //0=Off, 1=Pop, 2=Rock, 3=Jazz, 4=Classic, 5=Bass
  mp3.setVolume(30);  //0..30, module persists volume on power failure

  mp3.sleep();  //inter sleep mode, 24mA

  mp3.wakeup(2);  //exit sleep mode & initialize source 1=USB-Disk, 2=TF-Card, 3=Aux, 5=NOR Flash

  mp3Serial.enableRx(true);  //enable interrupts on RX-pin for better response detection, less overhead than mp3Serial.listen()



  SPI.begin();  // Инициализация SPI
  mfrc522.PCD_Init();



  if (mp3.getStatus()) serialLog("Помилка у роботі mp3.");

  mp3Serial.enableRx(false);  //disable interrupts on RX-pin, less overhead than mp3Serial.listen()
  if (rtc.lostPower()) {      // выполнится при сбросе батарейки
    rtc.setTime(BUILD_SEC, BUILD_MIN, BUILD_HOUR, BUILD_DAY, BUILD_MONTH, BUILD_YEAR);
  }
  serialLog("=== Запуск системи (Build " + VERS + ") ===");
  SerialPrintTime();
  delay(50);

  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);  // Установка усиления антенны
  mfrc522.PCD_AntennaOff();                        // Перезагружаем антенну
  mfrc522.PCD_AntennaOn();                         // Включаем антенну


  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }



  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BG);
  TJpgDec.setSwapBytes(true);
  TJpgDec.setCallback(tft_output);
  TJpgDec.drawFsJpg(0, 0, "/images/logo.jpeg", LittleFS);
  delay(2000);
  TJpgDec.drawFsJpg(0, 0, "/images/bg.jpeg", LittleFS);
  mp3.playTrack(1);
  xTaskCreatePinnedToCore(core0, "Task0", 10000, NULL, 1, &Task0, 0);
  PrintMainPage(1);
  CheckPlayersDeath();
  printBattery();
  printTime();
}


void loop() {
  ok.tick();
  up.tick();
  down.tick();
  left.tick();
  right.tick();

  DateTime now = rtc.getTime();



  if (millis() - globalcheckplayerstats > 1000) {
    globalcheckplayerstats = millis();

    if (!data.is_dead) {
      globalUpdate = 1;
      CheckPlayersDeath();
    } else {
      if (globalUpdate) {
        currPage = 3;
        update = 1;
        globalUpdate = 0;
      }
    }
  }


  if (millis() - globaldoeffects > 60000) {
    globaldoeffects = millis();

    if (!data.is_dead) {
      if (data.radiation >= 1000 and data.radiation < 100000) {
        data.health -= 0.4;
      } else if (data.radiation >= 100000 and data.radiation < 200000) {
        data.health -= 3;
      } else if (data.radiation >= 200000) {
        data.health -= 30;
      }
      if (data.health < 0) data.health = 0;
    }
  }



  if (data.health != prev_health and currPage == 0) {
    cleardisplay(2);
    drawProgressBar(24, 27, 115, 10, data.health, TFT_TEXT, TFT_HEALTH);
    prev_health = data.health;
  }

  if (data.armor != prev_armor and currPage == 0) {
    cleardisplay(3);
    drawProgressBar(24, 47, 115, 10, data.armor, TFT_TEXT, TFT_ARMOR);
    prev_armor = data.armor;
  }
  if (data.radiation != prev_radiation and currPage == 0) {
    cleardisplay(4);
    tft.setTextSize(1);
    tft.setCursor(27, 68);
    if (data.radiation < 250000) tft.print(data.radiation);
    else tft.print(">250000");
    tft.print(" мЗв");
    prev_radiation = data.radiation;
  }




  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    handleCommand(command);
  }




  if (now.minute != prev_min and now.minute != 165) {
    cleardisplay(1);
    printTime();
    prev_min = now.minute;
    updateConfig();
  }




  if (currPage < 3) {
    if (left.click()) {
      currPage--;
      if (currPage < 0) currPage = 2;
      update = 1;
    }
    if (right.click()) {
      currPage++;
      if (currPage > 2) currPage = 0;
      update = 1;
    }
  }



  if (currPage == 4) {

    if (left.click() or right.click()) {
      selectedButton = !selectedButton;
      printdisplay(4);
    }

    if (ok.click()) {
      response = selectedButton;
      if (response) {
        applyCard(card.usage_method);
      }
      selectedButton = 1;
      currPage = 0;
      update = 1;
      printTime();
    }
  }


  if (currPage == 6) {
    if (up.click()) {
      NPCselectedItem = (NPCselectedItem - 1 + NPCmenuItemsCount) % NPCmenuItemsCount;
      drawMenuNPC();
    }

    if (down.click()) {
      NPCselectedItem = (NPCselectedItem + 1) % NPCmenuItemsCount;
      drawMenuNPC();
    }

    if (ok.click()) {
      switch (NPCselectedItem) {
        case 0:
          if (ChangeUsage(card.uid, 1,1)) {
            Serial.println("RESET GOOD");
              tft.setTextSize(2);
              tft.setTextColor(TFT_GOOD);
              tft.setCursor(30, 200);
              tft.print("Успішно");
              delay(1000);
              drawMenuNPC();
          } else {
              Serial.println("RESET ERR");
              tft.setTextSize(2);
              tft.setTextColor(TFT_RED);
              tft.setCursor(30, 200);
              tft.print("Помилка");
              delay(1000);
              drawMenuNPC();
          }
          break;
        case 1: break;
        case 4:
        readerDisabled = true;
          currPage = 0;
          update = 1;
          NPCselectedItem = 0;
          break;
      }
    }
  }

  if (millis() - radiationcheck > 1000 and currPage == 2) {
    radiationcheck = millis();


    update = 1;
  }


  if (left.hold()) {
    currPage = 6;
    update = 1;
  }


  if (update == 1) {
    printdisplay(currPage);
    update = 0;
  }
}
