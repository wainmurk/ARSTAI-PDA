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
#include <RadioLib.h>


#define DEBUG 1
bool WEB = 1;

const String VERS = "dev 0.3.0";

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
unsigned long lastShelterTestTime = 0;
unsigned long lastRadiationTime = 0;
unsigned long lastMineExplodeTime = 0;
uint64_t radiationcheck;


const int DELAY_BETWEEN_CARDS = 1000;
long timeLastCardRead = 0;
bool readerDisabled = true;
int8_t currPage;
int8_t prevcurrPage;
int irqCurr;
int irqPrev;

bool globalUpdate;

bool in_shelter = 0;

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
  int group = 0;


  int fire_protection = 0;
  int gravi_protection = 0;
  int acid_protection = 0;
  int electro_protection = 0;
  int radiation_protection = 0;
  int psy_protection = 0;

  bool is_npc = 0;
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

struct Settings {
byte gameCode = 88;

};
Settings s;


Button ok(0);
Button up(35);
Button down(36);
Button left(34);
Button right(39);


#if defined(ESP8266) || defined(ESP32)
  ICACHE_RAM_ATTR
#endif

TFT_eSPI tft = TFT_eSPI();

MicroDS3231 rtc;
SoftwareSerial mp3Serial;
DFPlayer mp3;
SX1278 radio = new Module(2, 12, 27, 26);
const int NPCmenuItemsCount = 5;
String NPCmenuItems[NPCmenuItemsCount] = { "Поновити картку", "Вилікуватись", "Item 3", "Item 4", "Вийти" };
int NPCselectedItem = 0;

const int MastermenuItemsCount = 11;
String MastermenuItems[MastermenuItemsCount] = { "Тест1", "Тест1", "Item 2", "Item 3", "Exit 4" , "Item 5", "Item 6", "Item 7", "Item 8", "Item 9", "Item 10"};
int MasterselectedItem = 0;



volatile bool receivedFlag = false;



bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
  if (y >= tft.height()) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  return 1;
}


TaskHandle_t Task0;

int dx, dy;

bool update = 1;



unsigned long lastScanTime = 0;
const unsigned long scanInterval = 1000;  // Интервал сканирования 5 секунд


MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;


void setup() {
  Init();
  TJpgDec.drawFsJpg(0, 0, "/images/logo.jpeg", LittleFS);
  delay(2000);
  TJpgDec.drawFsJpg(0, 0, "/images/bg.jpeg", LittleFS);
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
        currPage = 9;
        update = 1;
        globalUpdate = 0;
      }
    }
  }

    if (in_shelter && (millis() - lastShelterTestTime > 20000)) {
        in_shelter = false;
        Serial.println("Не было сигнала от укрытия в течение 20 секунд. Статус: не в укрытии.");
    }

        if (currentsievert > 0 && (millis() - lastRadiationTime > 5000)) {
        currentsievert = 0;
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
    drawProgressBar(58, 44, 228, 16, data.health, TFT_TEXT, TFT_HEALTH);
    prev_health = data.health;
  }

  if (data.armor != prev_armor and currPage == 0) {
    cleardisplay(3);
    drawProgressBar(58, 84, 228, 16, data.armor, TFT_TEXT, TFT_ARMOR);
    prev_armor = data.armor;
  }
  if (data.radiation != prev_radiation and currPage == 0) {
    cleardisplay(4);
    tft.setTextSize(2);
    tft.setCursor(58, 122);
    if (data.radiation < 250000) tft.print(data.radiation);
    else tft.print(">250000");
    tft.print(" мЗв");
    prev_radiation = data.radiation;
    tft.setTextSize(1);
  }




  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    handleCommand(command);
  }




  if (now.minute != prev_min and now.minute != 165) {
    CheckEvents(0);
    cleardisplay(1);
    printTime();
    prev_min = now.minute;
    updateConfig();
  }




  if (currPage < (4 + data.is_npc)) {
    if (left.click()) {
      currPage--;
      if (currPage < 0) currPage = (3 + data.is_npc);
      update = 1;
    }
    if (right.click()) {
      currPage++;
      if (currPage > (3 + data.is_npc)) currPage = 0;
      update = 1;
    }
  }



  if (currPage == 6) {

    if (left.click() or right.click()) {
      selectedButton = !selectedButton;
      printdisplay(6);
    }

    if (ok.click()) {
      response = selectedButton;
      if (response) {
        applyCard(card.usage_method);
      }
      selectedButton = 1;
      currPage = (data.is_dead) ?  9 : currPage;
      update = 1;
      printTime();
    }
  }


  if (currPage == 4) {
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
              tft.setTextSize(2);
              tft.setTextColor(TFT_GOOD);
              tft.setCursor(30, 200);
              tft.print("Успішно");
              delay(500);
              drawMenuNPC();
          } else {
              tft.setTextSize(2);
              tft.setTextColor(TFT_RED);
              tft.setCursor(30, 200);
              tft.print("Помилка");
              delay(500);
              drawMenuNPC();
          }
          break;
        case 1: rescue();break;
        case 4: break;
      }
    }
  }

  if (currPage == 80) {
  if (up.click()) {
    MasterselectedItem = (MasterselectedItem - 1 + MastermenuItemsCount) % MastermenuItemsCount;
    drawMenuMaster();
  }

  if (down.click()) {
    MasterselectedItem = (MasterselectedItem + 1) % MastermenuItemsCount;
    drawMenuMaster();
  }



   if (ok.click()) {
      switch (MasterselectedItem) {
        case 0:
          if (ChangeUsage(card.uid, 1,1)) {
              tft.setTextSize(2);
              tft.setTextColor(TFT_GOOD);
              tft.setCursor(30, 200);
              tft.print("Успішно");
              delay(500);
              drawMenuNPC();
          } else {
              tft.setTextSize(2);
              tft.setTextColor(TFT_RED);
              tft.setCursor(30, 200);
              tft.print("Помилка");
              delay(500);
              drawMenuNPC();
          }
          break;
        case 1: rescue();break;
        case 4: currPage = 0; update = 1;break;
      }
    }













  }



  if (millis() - radiationcheck > 1000 and currPage == 2) {
    radiationcheck = millis();


    update = 1;
  }



  if (update == 1) {
    printdisplay(currPage);
    update = 0;
  }
}
