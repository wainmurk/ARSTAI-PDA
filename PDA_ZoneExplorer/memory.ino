void writeDefaultConfig() {
  File file = LittleFS.open("/data.cfg", FILE_WRITE);
  if (!file) {
     Serial2Webln("Не взалося створити файл.");
    return;
  }

  file.write((const uint8_t*)&data, sizeof(Data));
  file.close();
   Serial2Webln("Встановлено стандартні значення.");
}


void updateConfig() {
  DateTime now = rtc.getTime();
  File file = LittleFS.open("/data.cfg", FILE_WRITE);
  if (!file) {
     Serial2Webln("Не вдалося відкрити файл для запису.");
    return;
  }
  file.write((const uint8_t*)&data, sizeof(Data));
  file.close();
}

void updateConfigNow() {
  DateTime now = rtc.getTime();
  File file = LittleFS.open("/data.cfg", FILE_WRITE);
  if (!file) {
     Serial2Webln("Не вдалося відкрити файл для запису.");
    return;
  }else{
  file.write((const uint8_t*)&data, sizeof(Data));
  file.close();
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
}




void readConfig() {
  File file = LittleFS.open("/data.cfg", FILE_READ);
  if (!file) {
     Serial2Webln("Не вдалося відкрити файл.");
    return;
  }

  file.read((uint8_t*)&data, sizeof(Data));
  file.close();
   Serial2Webln("Конфіг прочитано.");
}

void deleteConfigFile() {
  if (LittleFS.remove("/data.cfg")) {
     Serial2Webln("File '/data.cfg' deleted successfully");
  } else {
     Serial2Webln("Failed to delete file '/data.cfg'");
  }
}


void listFiles(const char* dirname) {
  Serial2Webln("Список файлів:");
  File root = LittleFS.open(dirname);
  if (!root) {
     Serial2Webln("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
     Serial2Webln("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial2Web("  Папка : ");
      Serial2Web(file.name());
     Serial2Webln(" [directory]");
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
  if(String(filename) == "/armor.jpeg"){
    Serial2Webln("Захищений файл. Перегляд заборонено.");
    return;
  } else if(String(filename) == "/bg.jpeg"){
    Serial2Webln("Захищений файл. Перегляд заборонено.");
    return;
  }else if(String(filename) == "/health.jpeg"){
    Serial2Webln("Захищений файл. Перегляд заборонено.");
    return;
  }else if(String(filename) == "/index.html"){
    Serial2Webln("Захищений файл. Перегляд заборонено.");
    return;
  }else if(String(filename) == "/logo.jpeg"){
    Serial2Webln("Захищений файл. Перегляд заборонено.");
    return;
  }else if(String(filename) == "/radiation.jpeg"){
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
  currentLogFileName = "/log" + String(currentLogIndex) + ".txt";
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
  File indexFile = LittleFS.open("/logIndex.txt", "r");
  if (!indexFile) {
     Serial2Webln("Failed to load log index, starting from 0");
    currentLogIndex = 0;
    return;
  }
  currentLogIndex = indexFile.parseInt();
  indexFile.close();
}

void saveLogIndex() {
  File indexFile = LittleFS.open("/logIndex.txt", "w");
  if (!indexFile) {
     Serial2Webln("Failed to save log index");
    return;
  }
  indexFile.println(currentLogIndex);
  indexFile.close();
}


void deleteFile(const String& filename) {
if(String(filename) == "/armor.jpeg"){
    Serial2Webln("Захищений файл. Видалення заборонено.");
    return;
  } else if(String(filename) == "/bg.jpeg"){
    Serial2Webln("Захищений файл. Видалення заборонено.");
    return;
  }else if(String(filename) == "/health.jpeg"){
    Serial2Webln("Захищений файл. Видалення заборонено.");
    return;
  }else if(String(filename) == "/index.html"){
    Serial2Webln("Захищений файл. Видалення заборонено.");
    return;
  }else if(String(filename) == "/logo.jpeg"){
    Serial2Webln("Захищений файл. Видалення заборонено.");
    return;
  }else if(String(filename) == "/radiation.jpeg"){
    Serial2Webln("Захищений файл. Видалення заборонено.");
    return;
  }

  if (LittleFS.remove(filename)) {
    serialLog("Успішно видалено: " + filename);
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
    FileName = "/log" + String(i) + ".txt";
    deleteFile(FileName);
  }
}


