


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
  if (card.type == -1) {

  } else if (card.type == 0) {  // medicament


    if (card.subtype == 1) {

      if (card.value > 0) {
        if ((data.health + (card.value * pow(10, card.multiplier))) > 100) {
          data.health = 100;
        } else {
          data.health += card.value * pow(10, card.multiplier);
        }
      } else if (card.percent > 0) {
        data.health = data.health + (data.health * card.percent / 100);
      }
      if (data.health > 100) {
        data.health = 100;
      }

    } else if (card.subtype == 2) {
      if (card.value > 0) {
        if ((data.radiation - (card.value * pow(10, card.multiplier))) < 0) {
          data.radiation = 0;
        } else {
          data.radiation -= card.value * pow(10, card.multiplier);
        }
      } else if (card.percent > 0) {
        data.radiation = data.radiation - (data.radiation * card.percent / 100);
      }
      if (data.radiation < 0) {
        data.radiation = 0;
      }

    } else if (card.subtype == 4) {
      medical_protection = 1;
    }




  } else if (card.type == 1) {
  }
 else if (card.type == 4) {
  data.is_zombie = 1;
  CheckHealthStatus = 1;
  }




  else if (card.type == 9) {
    rescue();
  } else if (card.type == 13) {
    int time;
    time = card.hours * 3600 + card.minutes * 60 + card.seconds;
    StartReviving(time);
  }
}


void CheckEvents(int var) {
  File file = LittleFS.open("/events.cfg", "r");
  File tempFile = LittleFS.open("/temp.cfg", "w");
  DateTime now = rtc.getTime();
  bool eventsFound = false;  // Флаг для отслеживания наличия событий

  if (file && tempFile) {
    while (file.available()) {
      String line = file.readStringUntil('\n');

      // Извлечение данных из строки
      int id;
      char eventName[50];
      int eventHour, eventMin, eventDate, eventMonth, eventYear;
      int duration;  // Новое поле длительности события
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
          executeEventCode(id, eventName);  // Выполняем нужный код
        }

      } else {
        serialLogln("Подія ID: " + String(id) + " (" + String(eventName) + ") відбулася і була видалена.");
        mp3.playTrack(7);
        medical_protection = 0;
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

int genRandom(int minValue, int maxValue) {
  uint32_t randomValue = esp_random();                          // Получаем аппаратное случайное число
  return minValue + (randomValue % (maxValue - minValue + 1));  // Приводим к диапазону
}

// Эта функция будет вызвана каждую минуту для каждого активного события
void executeEventCode(int id, const char* eventName) {
  if (!data.is_dead and !in_shelter and !medical_protection) {
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
  int diffMinutes = (eventYear - currentYear) * 525600 + (eventMonth - currentMonth) * 43200 + (eventDate - currentDate) * 1440 + (eventHour - currentHour) * 60 + (eventMin - currentMin);

  return diffMinutes;
}

void startTimer(int n) {
  if (!timerStarted) {
    DateTime now = rtc.getTime();  // Получаем текущее время
    startSec = now.second;
    startMin = now.minute;
    startHour = now.hour;
    startDate = now.day;
    startMonth = now.month;
    startYear = now.year;

    targetSeconds = n;  // Устанавливаем нужное количество секунд
    timerStarted = true;
  }
}
void checkRemainingKnockTime() {
  if (timerStarted) {
    unsigned long currentMillis = millis();

    // Проверяем, прошло ли 5 секунд с момента последнего вывода
    if (currentMillis - lastPrintTime >= printInterval) {
      lastPrintTime = currentMillis;

      // Получаем текущее время
      DateTime now = rtc.getTime();
      int currentSec = now.second;
      int currentMin = now.minute;
      int currentHour = now.hour;
      int currentDate = now.day;
      int currentMonth = now.month;
      int currentYear = now.year;

      // Расчет разницы во времени (в секундах)
      int diffSeconds = (currentYear - startYear) * 31536000 + (currentMonth - startMonth) * 2592000 + (currentDate - startDate) * 86400 + (currentHour - startHour) * 3600 + (currentMin - startMin) * 60 + (currentSec - startSec);

      secondsRemaining = targetSeconds - diffSeconds;

      // Если время истекло, запускаем функцию
      if (secondsRemaining <= 0) {
        SerialPrintTime();
        serialLogln("Гравець помер через поранення");
        data.health = 0;
        data.is_dead = 1;
        data.is_knocked = 0;
        causeOfDeath = "Поранення";
        secondsRemaining = 0;
        startMin = 0;
        startHour = 0;
        startDate = 0;
        startMonth = 0;
        startYear = 0;
        timerStarted = false;  // Останавливаем таймер
      } else {
      }
    }
  } else {
    secondsRemaining = 0;
    startMin = 0;
    startHour = 0;
    startDate = 0;
    startMonth = 0;
    startYear = 0;
  }
}

void StartReviving(int time) {
  if (data.is_dead) {
    isReviving = 1;
    currPage = 51;
    cleardisplay(0);
    printdisplay(51);
    startTimer(time);
  }
}

void fixDisplay(){

    if (data.is_dead and !isReviving) currPage = 9;
    else if (data.is_dead and isReviving) currPage = 51;
    else if (data.is_knocked) currPage = 50;
    else currPage = 0;


}

void checkRemainingReviveTime() {
  if (timerStarted) {
    unsigned long currentMillis = millis();

    // Проверяем, прошло ли 5 секунд с момента последнего вывода
    if (currentMillis - lastPrintTime >= printInterval) {
      lastPrintTime = currentMillis;

      // Получаем текущее время
      DateTime now = rtc.getTime();
      int currentSec = now.second;
      int currentMin = now.minute;
      int currentHour = now.hour;
      int currentDate = now.day;
      int currentMonth = now.month;
      int currentYear = now.year;

      // Расчет разницы во времени (в секундах)
      int diffSeconds = (currentYear - startYear) * 31536000 + (currentMonth - startMonth) * 2592000 + (currentDate - startDate) * 86400 + (currentHour - startHour) * 3600 + (currentMin - startMin) * 60 + (currentSec - startSec);

      secondsRemaining = targetSeconds - diffSeconds;

      // Если время истекло, запускаем функцию
      if (secondsRemaining <= 0) {
        SerialPrintTime();
        serialLogln("Гравець воскрес за допомогою RFID або Radio");
        isReviving = 0;
        rescue();
        secondsRemaining = 0;
        startMin = 0;
        startHour = 0;
        startDate = 0;
        startMonth = 0;
        startYear = 0;
        timerStarted = false;  // Останавливаем таймер
      } else {
      }
    }
  } else {
    secondsRemaining = 0;
    startMin = 0;
    startHour = 0;
    startDate = 0;
    startMonth = 0;
    startYear = 0;
  }
}

void dump_byte_array(byte* buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial2Web(String(buffer[i] < 0x10 ? " 0" : " "));
    Serial2Web(String(buffer[i], HEX));
  }
  Serial2Webln("");
}

void CheckPlayersDeath() {
  WhatsTheReason();

  if (data.health <= 0 and !isReviving) {
    data.health = 0;
    data.is_dead = 1;
    data.radiation = 0;
    updateConfig();
    currPage = 9;
    printdisplay(currPage);
    update = 1;
  } else {
    if (data.is_dead and !isReviving) {
      update = 1;
      data.is_dead = 0;
      updateConfig();
      update = 1;
    }
    if (currPage == 9) {
      update = 1;
    }
  }
}
// void PrintMainPage(bool change) {
//   if (change) currPage = 0;

//   cleardisplay(0);
//   printdisplay(currPage);
// }

void rescue() {
  delay(100);
  isReviving = 0;
  data.is_zombie = 0;
  data.health = 100;
  data.is_dead = 0;
  data.is_knocked = 0;
  data.radiation = 0;
  CheckPlayersDeath();
  currPage = 0;
  update = 1;
}

void WhatsTheReason() {
  if (data.radiation >= 1000) {
    causeOfDeath = "Радіація";
  }
}

void lock(int type) {
  isAuthenticated = false;
  SerialPrintTime();
  if (type == 0) {
    serialLogln("Сессію завершено. (UART)");
  } else {
    serialLogln("Сессію завершено. (WiFi)");
  }
}
