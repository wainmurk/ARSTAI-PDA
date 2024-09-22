


void DoRandomVals() {
  if (millis() - updatetime > 500) {
    byte rannum = random(1, 4);
    updatetime = millis();
    if (rannum == 1) {
      data.health = random(0, 101);
    } else if (rannum == 2) {
      data.armor = random(0, 101);
    } else if (rannum == 3) {
      data.radiation = random(0, 350000);
    }
  }
}


void applyCard(int u_method) {
  if (u_method == 1) {
    int check = ChangeUsage(card.uid, 2, 0);
    if (check == 1) {
      doCard();
    } else if (check == 0) {
      currPage = 5;
      printdisplay(currPage);
    }
  } else if (u_method == 3) {
    doCard();
  }
}

void UpdateDisplay(int page, int clear) {
}

void doCard() {
if(card.type == 0){

}else if(card.type == 0){

}else if(card.type == 10){
  ok.tick();
  MasterselectedItem=0;
currPage = 80;
printdisplay(currPage);
update = 1;
}
}


void CheckEvents(int var) {
    File file = LittleFS.open("/events.cfg", "r");
    File tempFile = LittleFS.open("/temp.cfg", "w");
    DateTime now = rtc.getTime();
    bool eventsFound = false; // Флаг для отслеживания наличия событий

    if (file && tempFile) {
        while (file.available()) {
            String line = file.readStringUntil('\n');
            
            // Извлечение данных из строки
            int id;
            char eventName[50];
            int eventHour, eventMin, eventDate, eventMonth, eventYear;
            int duration; // Новое поле длительности события
            int notify2h, notify1h, notify30m, notify10m, notify1m;

            sscanf(line.c_str(), "%d %49s %d %d %d %d %d %d %d %d %d %d %d", 
                   &id, eventName, &eventHour, &eventMin, &eventDate, &eventMonth, 
                   &eventYear, &duration, &notify2h, &notify1h, &notify30m, &notify10m, &notify1m);

            // Получение текущих времени и даты
            int currentMin = now.minute; 
            int currentHour = now.hour;
            int currentDate = now.date;
            int currentMonth = now.month;
            int currentYear = now.year;

            // Расчет оставшегося времени до события в минутах
            int minutesDiff = calculateMinutesDiff(eventMin, eventHour, eventDate, eventMonth, eventYear);
            int eventStartMinutes = eventHour * 60 + eventMin;
            int eventEndMinutes = eventStartMinutes + duration;
            int currentMinutes = currentHour * 60 + currentMin;

            eventsFound = true;

            if (var == 1) {
                Serial2Webln("Подія ID: " + String(id) + " (" + String(eventName) + ") настане через " + String(minutesDiff) + " хвилин.");
            }

            // Проверка, наступило ли время уведомления
            if (var == 0) {
                 if (minutesDiff == 120 && notify2h) {
                    mp3.playTrack(1);
                    CautionDisplay(String(eventName), minutesDiff);

                 } else if (minutesDiff == 60 && notify1h) {
                    mp3.playTrack(2);
                    CautionDisplay(String(eventName), minutesDiff);

                 } else if (minutesDiff == 30 && notify30m) {
                    mp3.playTrack(3);
                    CautionDisplay(String(eventName), minutesDiff);

                 } else if (minutesDiff == 10 && notify10m) {
                    mp3.playTrack(4);
                    CautionDisplay(String(eventName), minutesDiff);

                 } else if (minutesDiff == 1 && notify1m) {
                    mp3.playTrack(5);
                    CautionDisplay(String(eventName), minutesDiff);

                 } else if (minutesDiff == 0) {
                    serialLogln("Подія ID: " + String(id) + " (" + String(eventName) + ") починається зараз.");
                    mp3.playTrack(6);
                 }
            }

            // Если событие не наступило или не завершилось, записываем его во временный файл
            if (currentMinutes < eventEndMinutes) {
                tempFile.println(line);

                // Проверяем, активен ли сейчас ивент и выполняем код каждую минуту
                if (currentMinutes >= eventStartMinutes && currentMinutes < eventEndMinutes) {
                    executeEventCode(id, eventName); // Выполняем нужный код
                }

            } else {
                serialLogln("Подія ID: " + String(id) + " (" + String(eventName) + ") відбулася і була видалена.");
                mp3.playTrack(7);
            }
        }

        file.close();
        tempFile.close();

        LittleFS.remove("/events.cfg");
        LittleFS.rename("/temp.cfg", "/events.cfg");

        // Если не найдено ни одного события
        if (!eventsFound && var == 1) {
            Serial2Webln("Немає подій для відображення.");
        }

    } else {
        serialLogln("Помилка під час відкриття файлу events.cfg.");
    }
}

// Эта функция будет вызвана каждую минуту для каждого активного события
void executeEventCode(int id, const char* eventName) {
  
    serialLogln("Виконання події ID: " + String(id) + " (" + String(eventName) + ")");
    if(!data.is_dead and !in_shelter){
    data.health -= 51;
    causeOfDeath = eventName;
    }
}





// Функция расчета разницы во времени между текущим временем и временем события
int calculateMinutesDiff(int eventMin, int eventHour, int eventDate, int eventMonth, int eventYear) {
  DateTime now = rtc.getTime();
  int currentMin = now.minute; 
  int currentHour = now.hour;
  int currentDate = now.date;
  int currentMonth = now.month;
  int currentYear = now.year;

  // Расчет разницы во времени (в минутах)
  int diffMinutes = (eventYear - currentYear) * 525600 +
                    (eventMonth - currentMonth) * 43200 +
                    (eventDate - currentDate) * 1440 +
                    (eventHour - currentHour) * 60 +
                    (eventMin - currentMin);

  return diffMinutes;
}




void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
         Serial2Web(String(buffer[i] < 0x10 ? " 0" : " "));
         Serial2Web(String(buffer[i], HEX));
    }
   Serial2Webln(""); 
}

void CheckPlayersDeath() {
  WhatsTheReason();

  if (data.health <= 0) {
    data.health = 0;
    data.is_dead = 1;
    data.radiation = 0;
    updateConfig();
    currPage = 9;
    printdisplay(currPage);
    update = 1;
  } else {
    if (data.is_dead) {
      update = 1;
      data.is_dead = 0;
      updateConfig();
      PrintMainPage(1);
      update = 1;
    }
    if (currPage == 9) {
      PrintMainPage(1);
      update = 1;
    }
  }
}
void PrintMainPage(bool change) {
  if (change) currPage = 0;

  cleardisplay(0);
  printdisplay(currPage);
}

void rescue() {

  data.health = 100;
  data.is_dead = 0;
  data.radiation = 0;
  currPage = 0;
  update = 1;
}

void WhatsTheReason() {
  if (data.radiation >= 1000) {
    causeOfDeath = "Радіація";
  }
}

void lock(int type){
        isAuthenticated = false;
        SerialPrintTime();
        if(type == 0){
        serialLogln("Сессію завершено. (UART)");
        }else {
        serialLogln("Сессію завершено. (WiFi)");
        }
}



void Init(){
  Serial.begin(115200);
  pinMode(15, OUTPUT);
  digitalWrite(15, 1);



  if (!LittleFS.begin(false)) {
    Serial.println("LittleFS initialisation failed!");
    while (1) yield();
  }
  if (!LittleFS.exists("/profile.cfg")) {
    writeDefaultConfig();
  }
    if (!LittleFS.exists("/events.cfg")) {
    createEmptyEventsConfig();
  }





  readConfig();
  delay(50);
  loadLogIndex();
  delay(50);
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



  SPI.begin();
  mfrc522.PCD_Init();



  if (mp3.getStatus()) serialLogln("[MP3] Сталась помилка ініціалізації.");else serialLogln("[MP3] Ініціалізація ... Успішно!");

  if (rtc.lostPower()) {
    rtc.setTime(BUILD_SEC, BUILD_MIN, BUILD_HOUR, BUILD_DAY, BUILD_MONTH, BUILD_YEAR);
  }

  Serial.print(F("[Radio] Ініціалізація ... "));
  int state = radio.begin(439.0, 125.0, 9, 7, RADIOLIB_SX127X_SYNC_WORD, 10, 8, 0);
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("Успішно!"));
  } else {
    Serial.print(F("Помилка!, код "));
    Serial.println(state);
  }
  radio.setPacketReceivedAction(setFlag);
  Serial.print(F("[Radio] Спробую просканувати ... "));
  state = radio.startReceive();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("Успішно!"));
  } else {
    Serial.print(F("Помилка!, код "));
    Serial.println(state);
  }




  serialLogln("▶ Запуск системи (Build: " + VERS + ") ◀");
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
}


