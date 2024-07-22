

AsyncWebServer server(80);

String consoleOutput = "";  // Глобальная переменная для хранения вывода

void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (!index) {
    if (!filename.startsWith("/")) {
      filename = "/" + filename;
    }
    request->_tempFile = LittleFS.open(filename, "w");
  }
  if (len) {
    request->_tempFile.write(data, len);
  }
  if (final) {
    request->_tempFile.close();
    request->send(200, "text/plain; charset=utf-8", "Файл додано успішно.");
  }
}

void setupWebServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", String(), false);
  });

  server.on("/console", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("command", true)) {
      String command = request->getParam("command", true)->value();
      if (!isAuthenticated) {
        isAuthenticated = true;
        SerialPrintTime();
        serialLog("Доступ надано автоматично. (WiFi)");
      }
      handleCommand(command);
    } else {
      request->send(400, "text/plain; charset=utf-8", "Відсутній параметр.");
    }
  });

  server.on("/consoleOutput", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", consoleOutput);
  });

  server.on("/download", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("file")) {
      String file = request->getParam("file")->value();

      // Список запрещённых имен файлов
      const char *forbiddenFiles[] = { "radiation.jpeg", "logo.jpeg", "index.html", "health.jpeg", "bg.jpeg" };

      // Проверка, содержится ли запрещённое имя файла в запросе
      bool isForbidden = false;
      for (int i = 0; i < sizeof(forbiddenFiles) / sizeof(forbiddenFiles[0]); ++i) {
        if (file.indexOf(forbiddenFiles[i]) != -1) {
          isForbidden = true;
          break;
        }
      }

      if (isForbidden) {
        request->send(403, "text/plain; charset=utf-8", "Захищений файл. Завантаження заборонено.");
      } else {
        request->send(LittleFS, file, String(), true);
      }
    } else {
      request->send(400, "text/plain; charset=utf-8", "Отсутствует параметр 'file'.");
    }
  });




  server.on(
    "/upload", HTTP_POST, [](AsyncWebServerRequest *request) {},
    handleFileUpload);

  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not Found");
  });

  server.on(
    "/update", HTTP_POST, [](AsyncWebServerRequest *request) {
      bool shouldReboot = !Update.hasError();
      AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", shouldReboot ? "Оновлено успішно! Перезапуск..." : "Помилка оновлення!");
      response->addHeader("Connection", "close");
      request->send(response);
      if (shouldReboot) {
        Serial.println("Перезапуск...");
        delay(500);
        ESP.restart();
      }
    },
    [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
      if (!index) {
        Serial.printf("Update Start: %s\n", filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
          Update.printError(Serial);
        }
      }
      if (!Update.hasError()) {
        if (Update.write(data, len) != len) {
          Update.printError(Serial);
        }
      }
      if (final) {
        if (Update.end(true)) {
          Serial.printf("Оновлено успішно: %uB\n", index + len);
        } else {
          Update.printError(Serial);
        }
      }
    });
}

void Serial2Web(const String &message) {
  Serial.print(message);
  consoleOutput += message;

  // Обрезаем буфер, если он слишком большой
  if (consoleOutput.length() > 5000) {
    int cutIndex = consoleOutput.indexOf('\n', consoleOutput.length() - 5000);
    consoleOutput = consoleOutput.substring(cutIndex + 1);
  }
}

void Serial2Webln(const String &message) {
  Serial.println(message);
  consoleOutput += message + "\n";

  // Обрезаем буфер, если он слишком большой
  if (consoleOutput.length() > 5000) {
    int cutIndex = consoleOutput.indexOf('\n', consoleOutput.length() - 5000);
    consoleOutput = consoleOutput.substring(cutIndex + 1);
  }
}


void core0(void *p) {

  if(WEB)StartWebServer();else startWiFiScan();


  for (;;) {

      if (currPage != 4) {
        if (readerDisabled) {
          if (millis() - timeLastCardRead > DELAY_BETWEEN_CARDS) {
            readerDisabled = false;
            startListeningToNFC();
          }
        } else {
          irqCurr = digitalRead(PN532_IRQ);

          if (irqCurr == LOW && irqPrev == HIGH) {
            handleCardDetected();
          }

          irqPrev = irqCurr;
        }
      }




    if (!WEB) {

      int16_t WiFiScanStatus = WiFi.scanComplete();
      if (WiFiScanStatus < 0) {  // it is busy scanning or got an error
        if (WiFiScanStatus == WIFI_SCAN_FAILED) {
          Serial.println("WiFi Scan has failed. Starting again.");
          startWiFiScan();
        }
      } else {
        printScannedNetworks(WiFiScanStatus);
        startWiFiScan();
      }
    }
    vTaskDelay(5);
  }
}

void StartWebServer() {

#ifdef DEBUG

  WiFi.begin(DebugWiFiSSID, DebugWiFiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.print("WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());
#else

  WiFi.softAP(data.ssid, data.pass);

  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
#endif
  setupWebServer();
  server.begin();
}




void startListeningToNFC() {
  irqPrev = irqCurr = HIGH;
  if (!nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A)) {
  } else {
    handleCardDetected();
  }
}

void handleCardDetected() {
  String card_uid_str;
  uint8_t success = false;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;
  uint8_t card_text[48];
  uint8_t block_data[16];
  uint8_t card_data[16];
  int block_numbers[] = { 8, 9, 10 };
  int yes = 0;

  success = nfc.readDetectedPassiveTargetID(card.uid, &uidLength);

  if (success) {
    if (uidLength == 4) {
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
      int card_text_index = 0;
      success = nfc.mifareclassic_AuthenticateBlock(card.uid, uidLength, 4, 0, keya);
      if (success) {
        success = nfc.mifareclassic_ReadDataBlock(4, card_data);
        if (success) {
          yes++;
        } else {
          return;
        }
      } else {
        return;
      }
      for (int block = 0; block < 3; block++) {
        success = nfc.mifareclassic_AuthenticateBlock(card.uid, uidLength, block_numbers[block], 0, keya);
        if (success) {
          success = nfc.mifareclassic_ReadDataBlock(block_numbers[block], block_data);
          if (success) {
            if (card_text_index + 16 <= sizeof(card_text)) {
              memcpy(card_text + card_text_index, block_data, 16);
              card_text_index += 16;
              yes++;
            } else {
              return;
            }
          } else {
            return;
          }
        } else {
          return;
        }
      }
      card.card_text_str = "";
      for (int i = 0; i < card_text_index; i++) {
        if (card_text[i] != 0) {
          card.card_text_str += (char)card_text[i];
        }
      }
    }
    timeLastCardRead = millis();
    if (yes == 4) {
      String card_data_str;
      SerialPrintTime();
      serialLog("Відскановано: ");
      serialLog(card.card_text_str);
      for (uint8_t i = 0; i < 16; i++) {
        card_data_str = card_data_str + String(card_data[i]);
        if (i != 15) { card_data_str = card_data_str + ", "; }
      }
      for (uint8_t i = 0; i < 7; i++) {
        card_uid_str = card_uid_str + String(card.uid[i]);
        if (i != 15) { card_uid_str = card_uid_str + " "; }
      }
      decodecard(card_data);
      serialLog(card_data_str);
      serialLog("");
      serialLog(card_uid_str);
      serialLog("");
      cardreturntimer = millis();
      currPage = 4;
      update = 1;
    }
  }
  readerDisabled = true;
}





void decodecard(uint8_t data[]) {

  for (int i = 0; i < 10; i++) {
    switch (i) {
      case 0: card.usage_method = int(data[i]); break;
      case 1: card.type = int(data[i]); break;
      case 2: card.subtype = int(data[i]); break;
      case 3: card.protection_type = int(data[i]); break;
      case 4: card.percent = int(data[i]); break;
      case 5: card.value = int(data[i]); break;
      case 6: card.multiplier = int(data[i]); break;
      case 7: card.seconds = int(data[i]); break;
      case 8: card.minutes = int(data[i]); break;
      case 9: card.hours = int(data[i]); break;
    }
  }
}


int MakeCardUsed(uint8_t *targetUID, uint8_t newValue) {
  uint8_t uid[7] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t foundUidLength;
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &foundUidLength)) {
    uint8_t targetUidLength = sizeof(targetUID) / sizeof(targetUID[0]);
    if (foundUidLength == targetUidLength && memcmp(uid, targetUID, targetUidLength) == 0) {
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
      if (nfc.mifareclassic_AuthenticateBlock(uid, foundUidLength, 4, 0, keya)) {
        uint8_t data[16];
        if (nfc.mifareclassic_ReadDataBlock(4, data)) {
          data[0] = newValue;
          if (nfc.mifareclassic_WriteDataBlock(4, data)) {
            if (nfc.mifareclassic_ReadDataBlock(4, data)) {
              if (data[0] == newValue) {
                return 1;
              }
            }
          }
        }
      }
    } else {
      serialLog("=========[ANTICHEAT]=========");
      SerialPrintTime();
      serialLog("Виявлена спроба обходу системи карток.");
      serialLog("=========");
    }
  }

  return 0;
}


void startWiFiScan() {
  Serial.println("Scan start");
  WiFi.scanNetworks(true);  // 'true' turns Async Mode ON
}

void printScannedNetworks(uint16_t networksFound) {
  if (networksFound == 0) {
    Serial.println("no networks found");
  } else {
    Serial.println("\nScan done");
  }
  Serial.println("");
  WiFi.scanDelete();
}
