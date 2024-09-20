void writeDefaultConfig() {
  File file = LittleFS.open("/profile.cfg", FILE_WRITE);
  if (file) {
    file.println("ssid=" + String(data.ssid));
    file.println("password=" + String(data.pass));
    file.println("adminpass=" + String(data.adminpass));
    file.println("health=" + String(data.health));
    file.println("armor=" + String(data.armor));
    file.println("radiation=" + String(data.radiation));
    file.println("fire_protection=" + String(data.fire_protection));
    file.println("gravi_protection=" + String(data.gravi_protection));
    file.println("acid_protection=" + String(data.acid_protection));
    file.println("electro_protection=" + String(data.electro_protection));
    file.println("radiation_protection=" + String(data.radiation_protection));
    file.println("psy_protection=" + String(data.psy_protection));
    file.println("is_npc=" + String(data.is_npc));
    file.println("is_adept=" + String(data.is_adept));
    file.println("is_dead=" + String(data.is_dead));
    file.println("is_zombie=" + String(data.is_zombie));
    file.println("is_under_control=" + String(data.is_under_control));
    file.println("is_nocked=" + String(data.is_nocked));
    file.println("is_ignor=" + String(data.is_ignor));
    file.close();
    Serial2Webln("Файл профілю відсутній. Встановлено стандартні значення.");
  } else {
    Serial2Webln("Помилка у роботі файлової системи.");
  }
}


void updateConfig() {
  File file = LittleFS.open("/profile.cfg", "w");
  if (file) {
    file.println("ssid=" + String(data.ssid));
    file.println("password=" + String(data.pass));
    file.println("adminpass=" + String(data.adminpass));
    file.println("health=" + String(data.health));
    file.println("armor=" + String(data.armor));
    file.println("radiation=" + String(data.radiation));
    file.println("fire_protection=" + String(data.fire_protection));
    file.println("gravi_protection=" + String(data.gravi_protection));
    file.println("acid_protection=" + String(data.acid_protection));
    file.println("electro_protection=" + String(data.electro_protection));
    file.println("radiation_protection=" + String(data.radiation_protection));
    file.println("psy_protection=" + String(data.psy_protection));
    file.println("is_npc=" + String(data.is_npc));
    file.println("is_adept=" + String(data.is_adept));
    file.println("is_dead=" + String(data.is_dead));
    file.println("is_zombie=" + String(data.is_zombie));
    file.println("is_under_control=" + String(data.is_under_control));
    file.println("is_nocked=" + String(data.is_nocked));
    file.println("is_ignor=" + String(data.is_ignor));
    file.close();
  } else {
    Serial2Webln("Помилка у роботі файлової системи.");
  }
}

void updateConfigNow() {
  DateTime now = rtc.getTime();

updateConfig();

  if (now.date <= 9) {  Serial2Web("0"); }
   Serial2Web(String(now.date));
   Serial2Web(".");
  if (now.month <= 9) {  Serial2Web("0"); }
   Serial2Web(String(now.month));
   Serial2Web(" ");
  if (now.hour <= 9) {  Serial2Web("0"); }
   Serial2Web(String(now.hour));
   Serial2Web(":");
  if (now.minute <= 9) {  Serial2Web("0"); }
   Serial2Web(String(now.minute));
   Serial2Webln(" Дані збережено.");
}





void readConfig() {
  bool allValuesPresent = true;

  File file = LittleFS.open("/profile.cfg", "r");
  if (file) {
    String line;
    while (file.available()) {
      line = file.readStringUntil('\n');
      line.trim(); // Удаляем пробелы в начале и конце строки

      int equalSignIndex = line.indexOf('=');
      if (equalSignIndex == -1) continue; // Если нет знака '=', пропускаем строку

      String key = line.substring(0, equalSignIndex);
      String value = line.substring(equalSignIndex + 1);
      value.trim(); // Удаляем пробелы после знака '='

      if (key == "ssid") {
        value.toCharArray(data.ssid, sizeof(data.ssid));
      } else if (key == "password") {
        value.toCharArray(data.pass, sizeof(data.pass));
      } else if (key == "adminpass") {
        value.toCharArray(data.adminpass, sizeof(data.adminpass));
      } else if (key == "health") {
        data.health = value.toFloat();
      } else if (key == "armor") {
        data.armor = value.toInt();
      } else if (key == "radiation") {
        data.radiation = value.toInt();
      } else if (key == "fire_protection") {
        data.fire_protection = value.toInt();
      } else if (key == "gravi_protection") {
        data.gravi_protection = value.toInt();
      } else if (key == "acid_protection") {
        data.acid_protection = value.toInt();
      } else if (key == "electro_protection") {
        data.electro_protection = value.toInt();
      } else if (key == "radiation_protection") {
        data.radiation_protection = value.toInt();
      } else if (key == "psy_protection") {
        data.psy_protection = value.toInt();
      } else if (key == "is_npc") {
        data.is_npc = value.toInt();
      } else if (key == "is_adept") {
        data.is_adept = value.toInt();
      } else if (key == "is_dead") {
        data.is_dead = value.toInt();
      } else if (key == "is_zombie") {
        data.is_zombie = value.toInt();
      } else if (key == "is_under_control") {
        data.is_under_control = value.toInt();
      } else if (key == "is_nocked") {
        data.is_nocked = value.toInt();
      } else if (key == "is_ignor") {
        data.is_ignor = value.toInt();
      } else {
        allValuesPresent = false;
      }
    }
    file.close();

    if (!allValuesPresent) {
      Serial2Webln("Файл профілю неповний. Видалення файлу та створення нового.");
      LittleFS.remove("/profile.cfg");
      writeDefaultConfig();
    } else {
      Serial2Webln("Профіль прочитано успішно.");
    }
  } else {
    Serial2Webln("Файл профілю відсутній. Створення нового файлу.");
    writeDefaultConfig();
  }
}


void listFiles(const char* dirname, uint8_t levels = 0) {
  Serial2Webln("Список файлів: " + String(dirname));
  File root = LittleFS.open(dirname);
  if (!root) {
    Serial2Webln("Не вдалося відкрити директорію: " + String(dirname));
    return;
  }
  if (!root.isDirectory()) {
    Serial2Webln("Це не директорія: " + String(dirname));
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial2Web("  Папка : ");
      Serial2Webln(file.name());
      if (levels) {
        String path = String(dirname)  + file.name();
        listFiles(path.c_str(), levels - 1);
      }
    } else {
      Serial2Web("  Файл: ");
      Serial2Web(file.name());
      Serial2Web("  Розмір: ");
      Serial2Webln(String(file.size()));
    }
    file = root.openNextFile();
  }
  Serial2Webln("");
  root.close();
}


void readFileContent(const char* filename) {
  if(String(filename) == "/images/armor.jpeg"){
    Serial2Webln("Захищений файл. Перегляд заборонено.");
    return;
  } else if(String(filename) == "/images/bg.jpeg"){
    Serial2Webln("Захищений файл. Перегляд заборонено.");
    return;
  }else if(String(filename) == "/images/health.jpeg"){
    Serial2Webln("Захищений файл. Перегляд заборонено.");
    return;
  }else if(String(filename) == "/index.html"){
    Serial2Webln("Захищений файл. Перегляд заборонено.");
    return;
  }else if(String(filename) == "/images/logo.jpeg"){
    Serial2Webln("Захищений файл. Перегляд заборонено.");
    return;
  }else if(String(filename) == "/images/radiation.jpeg"){
    Serial2Webln("Захищений файл. Перегляд заборонено.");
    return;
  }
  File file = LittleFS.open(filename, "r");
  if (!file) {
     Serial2Webln("Такого файлу не існує, або сталась інша помилка.");
    return;
  }

   Serial2Webln("===== Вміст файлу: =====");
  while (file.available()) {
    Serial2Web(String((char)file.read()));
  }
   Serial2Webln("");
   Serial2Webln("====== КІНЕЦЬ ФАЙЛУ =====");
  file.close();
}


void createNewLogFile() {
  currentLogFileName = "/log/log" + String(currentLogIndex) + ".txt";
  logFile = LittleFS.open(currentLogFileName, "w");
  if (!logFile) {
     Serial2Webln("Failed to create log file");
    return;
  }
  logFile.close();
  currentLogIndex = (currentLogIndex + 1) % maxLogFiles;
  saveLogIndex();
}

void serialLog(const String& message) {
  Serial2Web(message);
  writeLog(message.c_str());
}

void serialLogln(const String& message) {
  Serial2Webln(message);
  writeLog(message.c_str());
}


void writeLog(const char* message) {
  File logFile = LittleFS.open(currentLogFileName, "a");
  if (logFile) {
    logFile.print(message);
    logFile.print("\n");
    logFile.close();
  } else {
     Serial2Webln("Failed to open log file for writing");
  }
}


void loadLogIndex() {
  File indexFile = LittleFS.open("/log/logIndex.txt", "r");
  if (!indexFile) {
     Serial2Webln("Failed to load log index, starting from 0");
    currentLogIndex = 0;
    return;
  }
  currentLogIndex = indexFile.parseInt();
  indexFile.close();
}

void saveLogIndex() {
  File indexFile = LittleFS.open("/log/logIndex.txt", "w");
  if (!indexFile) {
     Serial2Webln("Failed to save log index");
    return;
  }
  indexFile.println(currentLogIndex);
  indexFile.close();
}


void deleteFile(const String& filename) {
if(String(filename) == "/images/armor.jpeg"){
    Serial2Webln("Захищений файл. Видалення заборонено.");
    return;
  } else if(String(filename) == "/images/bg.jpeg"){
    Serial2Webln("Захищений файл. Видалення заборонено.");
    return;
  }else if(String(filename) == "/images/health.jpeg"){
    Serial2Webln("Захищений файл. Видалення заборонено.");
    return;
  }else if(String(filename) == "/index.html"){
    Serial2Webln("Захищений файл. Видалення заборонено.");
    return;
  }else if(String(filename) == "/images/logo.jpeg"){
    Serial2Webln("Захищений файл. Видалення заборонено.");
    return;
  }else if(String(filename) == "/images/radiation.jpeg"){
    Serial2Webln("Захищений файл. Видалення заборонено.");
    return;
  }

  if (LittleFS.remove(filename)) {
    serialLogln("Успішно видалено: " + filename);
  } else {
     Serial2Web("Сталася помилка. ");
     Serial2Web("(");
     Serial2Web(filename);
     Serial2Webln(")");
  }
}

void clearLogs() {
  String FileName;
  for (int i = 0; i < maxLogFiles; i++) {
    FileName = "/log/log" + String(i) + ".txt";
    deleteFile(FileName);
  }
}


void createEmptyEventsConfig() {
  File file = LittleFS.open("/events.cfg", "w");
  if (file) {
    file.close();
    serialLogln("Порожній events.cfg створено.");
  } else {
    serialLogln("Помилка під час створення events.cfg.");
  }
}
void addEvent(int id, String eventName, int hour, int min, int date, int month, int year, 
              int duration, int notify2h, int notify1h, int notify30m, int notify10m, int notify1m) {
  
  // Проверка валидности времени, даты и продолжительности
  if (hour < 0 || hour > 23 || min < 0 || min > 59 || date < 1 || date > 31 || 
      month < 1 || month > 12 || year < 2000 || duration < 1 || duration > 240) {
    Serial2Webln("Помилка: Невірна дата, час або тривалість (1-240 хв).");
    return;
  }

  // Проверка валидности значений уведомлений
  if ((notify2h != 0 && notify2h != 1) || (notify1h != 0 && notify1h != 1) ||
      (notify30m != 0 && notify30m != 1) || (notify10m != 0 && notify10m != 1) ||
      (notify1m != 0 && notify1m != 1)) {
    Serial2Webln("Помилка: Значення notify повинні бути 0 або 1.");
    return;
  }

  // Проверка на существование события с таким же id
  File file = LittleFS.open("/events.cfg", "r");
  if (file) {
    while (file.available()) {
      String line = file.readStringUntil('\n');
      int existingId;
      sscanf(line.c_str(), "%d", &existingId);
      if (existingId == id) {
        Serial2Webln("Помилка: Подія з таким ID вже існує.");
        file.close();
        return;
      }
    }
    file.close();
  } else {
    Serial2Webln("Помилка під час відкриття файлу events.cfg для перевірки.");
    return;
  }

  // Добавление события с параметрами уведомления
  file = LittleFS.open("/events.cfg", "a");
  if (file) {
    file.printf("%d %s %d %d %d %d %d %d %d %d %d %d %d\n", 
                id, eventName.c_str(), hour, min, date, month, year, duration, 
                notify2h, notify1h, notify30m, notify10m, notify1m);
    file.close();
    serialLogln("Подія додана: " + eventName);
  } else {
    serialLogln("Помилка під час додавання події.");
  }
}


// Функция для удаления события из файла по ID
void removeEvent(int id) {
  File file = LittleFS.open("/events.cfg", "r");
  File tempFile = LittleFS.open("/temp.cfg", "w");
  bool found = false;

  if (file && tempFile) {
    while (file.available()) {
      String line = file.readStringUntil('\n');
      // Проверка, начинается ли строка с ID события
      if (!line.startsWith(String(id) + " ")) {
        tempFile.println(line);
      } else {
        found = true;
      }
    }
    file.close();
    tempFile.close();

    // Заменяем оригинальный файл на временный
    LittleFS.remove("/events.cfg");
    LittleFS.rename("/temp.cfg", "/events.cfg");

    if (found) {
      serialLogln("Подія видалена: ID " + String(id));
    } else {
      Serial2Webln("Подію не знайдено: ID " + String(id));
    }
  } else {
    serialLogln("Помилка під час видалення події.");
  }
}


