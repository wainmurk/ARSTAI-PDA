void handleCommand(const String& command) {
  if (!isAuthenticated) {
    Serial2Webln("Авторизуйтесь: ");
    while (Serial.available() == 0) {
    }
    String inputPassword = Serial.readStringUntil('\n');
    inputPassword.trim();

    if (strcmp(inputPassword.c_str(), data.adminpass) == 0) {
      isAuthenticated = true;
      Serial2Webln("Доступ надано. (UART)");
    } else {
      Serial2Webln("У доступі відмовлено.");
      return;
    }
  }

  if (command.startsWith("get ")) {
    String variable = command.substring(4);
    getVariableValue(variable);
  } else if (command.startsWith("set ")) {
    int spaceIndex = command.indexOf(' ', 4);
    if (spaceIndex != -1) {
      String variable = command.substring(4, spaceIndex);
      String value = command.substring(spaceIndex + 1);
      setVariableValue(variable, value);
    } else {
      Serial2Webln("Помилка у команді 'set'");
    }
  } else if (command == "info") {
    debugVars();
  } else if (command == "l") {
    lock(0);
  } else if (command == "wl") {
    lock(1);
  } else if (command == "save") {
    updateConfigNow();
  } else if (command == "revive") {
    rescue();
  } else if (command == "restart") {
    ESP.restart();
  } else if (command == "events") {
    CheckEvents(1);
  } else if (command.startsWith("stime ")) {
    setTimeCommand(command.substring(6));
  } else if (command.startsWith("playsound ")) {
    String number = command.substring(10);
    mp3.playTrack(number.toInt());
  } else if (command.startsWith("addevent ")) {
    addEventCommand(command.substring(9));
  } else if (command.startsWith("delevent ")) {
    removeEventCommand(command.substring(9));
  } else if (command.startsWith("kill ")) {
    DoKill(command.substring(5));
  } else if (command == "time") {
    SerialPrintTime();
  } else if (command == "dlog") {
    clearLogs();
  } else if (command == "c") {
    consoleOutput = "";
  } else if (command == "alog") {
    Serial2Webln("Актульний лог: " + currentLogFileName);
  } else if (command == "files") {
    listFiles("/", 2);
  } else if (command == "") {
  } else if (command.startsWith("read ")) {
    String filename = "/" + command.substring(5);
    readFileContent(filename.c_str());
  } else if (command.startsWith("b ")) {
    String message = command.substring(2);
    serialLogln(message);
  } else if (command.startsWith("del ")) {
    String filename = command.substring(4);  // Предполагается, что команда формата "del имя_файла"
    deleteFile("/" + filename);
  } else if (command == "help") {
    printHelp();
  } else {
    Serial2Webln("Невідома команда. Використовуйте help.");
  }
}





void getVariableValue(const String& variable) {
  if (variable == "uid") {
    dump_byte_array(card.uid, sizeof(card.uid));
  } else if (variable == "usage_method") {
    Serial2Webln(String(card.usage_method));
  } else if (variable == "type") {
    Serial2Webln(String(card.type));
  } else if (variable == "subtype") {
    Serial2Webln(String(card.subtype));
  } else if (variable == "protection_type") {
    Serial2Webln(String(card.protection_type));
  } else if (variable == "percent") {
    Serial2Webln(String(card.percent));
  } else if (variable == "value") {
    Serial2Webln(String(card.value));
  } else if (variable == "multiplier") {
    Serial2Webln(String(card.multiplier));
  } else if (variable == "seconds") {
    Serial2Webln(String(card.seconds));
  } else if (variable == "minutes") {
    Serial2Webln(String(card.minutes));
  } else if (variable == "hours") {
    Serial2Webln(String(card.hours));
  } else if (variable == "ssid") {
    Serial2Webln(String(data.ssid));
  } else if (variable == "pass") {
    Serial2Webln(String(data.pass));
  } else if (variable == "adminpass") {
    Serial2Webln(String(data.adminpass));
  } else if (variable == "health") {
    Serial2Webln(String(data.health));
  } else if (variable == "armor") {
    Serial2Webln(String(data.armor));
  } else if (variable == "radiation") {
    Serial2Webln(String(data.radiation));
  } else if (variable == "fire_protection") {
    Serial2Webln(String(data.fire_protection));
  } else if (variable == "gravi_protection") {
    Serial2Webln(String(data.gravi_protection));
  } else if (variable == "acid_protection") {
    Serial2Webln(String(data.acid_protection));
  } else if (variable == "electro_protection") {
    Serial2Webln(String(data.electro_protection));
  } else if (variable == "radiation_protection") {
    Serial2Webln(String(data.radiation_protection));
  } else if (variable == "psy_protection") {
    Serial2Webln(String(data.psy_protection));
  } else if (variable == "is_adept") {
    Serial2Webln(String(data.is_adept));
  } else if (variable == "is_dead") {
    Serial2Webln(String(data.is_dead));
  } else if (variable == "is_zombie") {
    Serial2Webln(String(data.is_zombie));
  } else if (variable == "is_under_control") {
    Serial2Webln(String(data.is_under_control));
  } else if (variable == "is_nocked") {
    Serial2Webln(String(data.is_nocked));
  } else if (variable == "is_ignor") {
    Serial2Webln(String(data.is_ignor));
  } else if (variable == "is_npc") {
    Serial2Webln(String(data.is_npc));
  } else {
    Serial2Webln("Невідома змінна.");
  }
}


void debugVars() {
  Serial2Webln("=== LastScannedCard ===");
  Serial2Web("uid: ");
  dump_byte_array(card.uid, sizeof(card.uid));
  Serial2Web("usage_method: ");
  Serial2Webln(String(card.usage_method));
  Serial2Web("type: ");
  Serial2Webln(String(card.type));
  Serial2Web("subtype: ");
  Serial2Webln(String(card.subtype));
  Serial2Web("protection_type: ");
  Serial2Webln(String(card.protection_type));
  Serial2Web("percent: ");
  Serial2Webln(String(card.percent));
  Serial2Web("value: ");
  Serial2Webln(String(card.value));
  Serial2Web("multiplier: ");
  Serial2Webln(String(card.multiplier));
  Serial2Web("seconds: ");
  Serial2Webln(String(card.seconds));
  Serial2Web("minutes: ");
  Serial2Webln(String(card.minutes));
  Serial2Web("hours: ");
  Serial2Webln(String(card.hours));
  Serial2Webln("==================");
  Serial2Webln("");

  Serial2Webln("=== PlayerData ===");
  Serial2Web("ssid: ");
  Serial2Webln(String(data.ssid));
  Serial2Web("pass: ");
  Serial2Webln(String(data.pass));
  Serial2Web("adminpass: ");
  Serial2Webln(String(data.adminpass));
  Serial2Web("health: ");
  Serial2Webln(String(data.health));
  Serial2Web("armor: ");
  Serial2Webln(String(data.armor));
  Serial2Web("radiation: ");
  Serial2Webln(String(data.radiation));
  Serial2Web("fire_protection: ");
  Serial2Webln(String(data.fire_protection));
  Serial2Web("gravi_protection: ");
  Serial2Webln(String(data.gravi_protection));
  Serial2Web("acid_protection: ");
  Serial2Webln(String(data.acid_protection));
  Serial2Web("electro_protection: ");
  Serial2Webln(String(data.electro_protection));
  Serial2Web("radiation_protection: ");
  Serial2Webln(String(data.radiation_protection));
  Serial2Web("psy_protection: ");
  Serial2Webln(String(data.psy_protection));
  Serial2Web("is_adept: ");
  Serial2Webln(String(data.is_adept));
  Serial2Web("is_dead: ");
  Serial2Webln(String(data.is_dead));
  Serial2Web("is_zombie: ");
  Serial2Webln(String(data.is_zombie));
  Serial2Web("is_under_control: ");
  Serial2Webln(String(data.is_under_control));
  Serial2Web("is_nocked: ");
  Serial2Webln(String(data.is_nocked));
  Serial2Web("is_ignor: ");
  Serial2Webln(String(data.is_ignor));
  Serial2Web("is_npc: ");
  Serial2Webln(String(data.is_npc));
  Serial2Webln("===============");
}


void setVariableValue(const String& variable, const String& value) {
  if (variable == "usage_method") {
    card.usage_method = value.toInt();
  } else if (variable == "type") {
    card.type = value.toInt();
  } else if (variable == "subtype") {
    card.subtype = value.toInt();
  } else if (variable == "protection_type") {
    card.protection_type = value.toInt();
  } else if (variable == "percent") {
    card.percent = value.toInt();
  } else if (variable == "value") {
    card.value = value.toInt();
  } else if (variable == "multiplier") {
    card.multiplier = value.toInt();
  } else if (variable == "seconds") {
    card.seconds = value.toInt();
  } else if (variable == "minutes") {
    card.minutes = value.toInt();
  } else if (variable == "hours") {
    card.hours = value.toInt();
  } else if (variable == "ssid") {
    strncpy(data.ssid, value.c_str(), sizeof(data.ssid));
  } else if (variable == "pass") {
    strncpy(data.pass, value.c_str(), sizeof(data.pass));
  } else if (variable == "adminpass") {
    strncpy(data.adminpass, value.c_str(), sizeof(data.adminpass));
  } else if (variable == "health") {
    data.health = value.toFloat();
    if (data.is_dead) {
      if (value.toInt() > 0) {
        data.is_dead = 0;
        currPage = 0;
        update = 1;
      }
    }
  } else if (variable == "armor") {
    data.armor = value.toInt();
  } else if (variable == "radiation") {
    data.radiation = value.toInt();
  } else if (variable == "fire_protection") {
    data.fire_protection = value.toInt();
  } else if (variable == "gravi_protection") {
    data.gravi_protection = value.toInt();
  } else if (variable == "acid_protection") {
    data.acid_protection = value.toInt();
  } else if (variable == "electro_protection") {
    data.electro_protection = value.toInt();
  } else if (variable == "radiation_protection") {
    data.radiation_protection = value.toInt();
  } else if (variable == "psy_protection") {
    data.psy_protection = value.toInt();
  } else if (variable == "is_adept") {
    data.is_adept = value.toInt();
  } else if (variable == "is_dead") {
    data.is_dead = value.toInt();
    if (data.is_dead and data.health > 0) { data.health = 0; }
  } else if (variable == "is_zombie") {
    data.is_zombie = value.toInt();
  } else if (variable == "is_under_control") {
    data.is_under_control = value.toInt();
  } else if (variable == "is_nocked") {
    data.is_nocked = value.toInt();
  } else if (variable == "is_ignor") {
    data.is_ignor = value.toInt();
  } else if (variable == "is_npc") {
    data.is_npc = value.toInt();
  } else {
    Serial2Webln("Невідома змінна.");
  }
  SerialPrintTime();
  serialLogln(" Змінено " + variable + " на " + value);
}


void SerialPrintTime() {
  String current_time;
  DateTime now = rtc.getTime();
  if (now.date <= 9) { current_time += "0"; }
  current_time += String(now.date);
  current_time += ".";
  if (now.month <= 9) { current_time += "0"; }
  current_time += (String(now.month));
  current_time += " ";
  if (now.hour <= 9) { current_time += "0"; }
  current_time += String(now.hour);
  current_time += ":";
  if (now.minute <= 9) { current_time += "0"; }
  current_time += String(now.minute);
  serialLogln(current_time);
}

void printHelp() {
  Serial2Webln("Доступні команди:");
  Serial2Webln("");
  Serial2Webln("c - очистити консоль (web).");
  Serial2Webln("b <текст>  - додати запис у лог.");
  Serial2Webln("l - завершити сессію та деавторизуватись.");
  Serial2Webln("");
  Serial2Webln("get <змінна> - отримати значення змінної.");
  Serial2Webln("set <змінна> <значення> - встановити значення змінної.");
  Serial2Webln("info - вивести debug-інформацію.");
  Serial2Webln("save - зберегти конфігурацію.");
  Serial2Webln("restart - перезавантажити ESP.");
  Serial2Webln("");
  Serial2Webln("time - вивести поточний час.");
  Serial2Webln("stime SEC MIN HOUR DAY MONTH YEAR - встановити час");
  Serial2Webln("");
  Serial2Webln("alog - вивести назву актуального логу.");
  Serial2Webln("dlog - видалити всі логи.");
  Serial2Webln("");
  Serial2Webln("files - показати список файлів в кореневому каталозі.");
  Serial2Webln("read <файл> - прочитати вміст файлу.");
  Serial2Webln("del <файл> - видалити файл.");
  Serial2Webln("");
  Serial2Webln("addevent ID Назва Година Хвилина День Місяць Рік Пов2г Пов1г Пов30хв Пов10хв Пов1хв - додати подію.");
  Serial2Webln("Наприклад: addevent 1 Великий_Викид 14 30 25 12 2024 1 1 1 1 1");
  Serial2Webln("delevent ID - видалити подію за вказаним ID.");
  Serial2Webln("events перевірити заплановані події.");
  Serial2Webln("");
}

void setTimeCommand(const String& params) {
  int sec, min, hour, day, month, year;
  sscanf(params.c_str(), "%d %d %d %d %d %d", &sec, &min, &hour, &day, &month, &year);

  // Установка времени на DS3231
  rtc.setTime(sec, min, hour, day, month, year);

  Serial2Webln("Час встановлено.");
}

void DoKill(const String& cause) {
  causeOfDeath = cause;
  data.health = 0;
  data.is_dead = 1;
  serialLogln("Вбито по причині: " + cause);
  currPage = 9;
  printdisplay(currPage);
}



void addEventCommand(const String& params) {
  int id;
  char eventName[50];
  int hour, min, date, month, year, duration;
  int notify2h, notify1h, notify30m, notify10m, notify1m;

  // Проверка на корректное количество аргументов
  int count = sscanf(params.c_str(), "%d %49s %d %d %d %d %d %d %d %d %d %d %d",
                     &id, eventName, &hour, &min, &date, &month, &year, &duration,
                     &notify2h, &notify1h, &notify30m, &notify10m, &notify1m);

  if (count != 13) {
   serialLogln("Помилка: Неправильна кількість аргументів. Очікується 13 аргументів, розділених пробілом.");
    return;
  }

  addEvent(id, String(eventName), hour, min, date, month, year, duration,
           notify2h, notify1h, notify30m, notify10m, notify1m);
}




// Обработка команды удаления события
void removeEventCommand(const String& params) {
  int id;
  sscanf(params.c_str(), "%d", &id);

  removeEvent(id);
}
