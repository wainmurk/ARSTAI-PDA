

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
        serialLogln("Доступ надано автоматично. (WiFi)");
      }
      if(command == "l")command = "wl";
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

void setFlag(void) {
  // we got a packet, set the flag
  receivedFlag = true;
}

void core0(void *p) {
StartWebServer();
  for (;;) {


  static uint32_t rebootTimer = millis();  // Важный костыль против зависания модуля!
  if (millis() - rebootTimer >= 1000) {    // Таймер с периодом 1000 мс
    rebootTimer = millis();                // Обновляем таймер
    digitalWrite(RST_PIN, HIGH);           // Сбрасываем модуль
    delayMicroseconds(2);                  // Ждем 2 мкс
    digitalWrite(RST_PIN, LOW);            // Отпускаем сброс
    mfrc522.PCD_Init();                    // Инициализируем заного
  }



      if (currPage != 6 and currPage != 4 and currPage != 80) {
        if (readerDisabled) {
          if (millis() - timeLastCardRead > DELAY_BETWEEN_CARDS) {
            timeLastCardRead = millis();
            readerDisabled = false;
            handleCardDetected();
          }
        }
      }



  if(receivedFlag) {
    // reset flag
    receivedFlag = false;

    // you can read received data as an Arduino String
    String str;
    int state = radio.readData(str);

    // you can also read received data as byte array
    /*
      byte byteArr[8];
      int numBytes = radio.getPacketLength();
      int state = radio.readData(byteArr, numBytes);
    */

    if (state == RADIOLIB_ERR_NONE) {
      serialLog("[Radio] Дані:\t");
      serialLogln(str);

      // print RSSI (Received Signal Strength Indicator)
      serialLog("[Radio] RSSI:\t");
      serialLog(String(radio.getRSSI()));
      serialLogln(" dBm");
      parseRadioPacket(str, radio.getRSSI());

    } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
      // packet was received, but is malformed
      serialLogln("[Radio] Помилка пакету!");

    } else {
      // some other error occurred
      serialLog("[Radio] Сталась помилка, код ");
      serialLogln(String(state));

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
    Serial.println("Під'єднання до WiFi ...");
  }

  Serial.print("Під'єднано, IP адреса: ");
  Serial.println(WiFi.localIP());
#else

  WiFi.softAP(data.ssid, data.pass);

  Serial.print("AP IP адреса: ");
  Serial.println(WiFi.softAPIP());
#endif
  setupWebServer();
  server.begin();
}




void handleCardDetected() {
    String card_uid_str;
    uint8_t success = false;
    uint8_t card_uid[7];
    uint8_t card_text[48];
    uint8_t block_data[16];
    uint8_t card_data[16];
    byte buffer[18];
    byte size = sizeof(buffer);
    int block_numbers[] = { 8, 9, 10 };
    int yes = 0;

    if (!mfrc522.PICC_IsNewCardPresent()) {
        readerDisabled = true;
        return;
    }

    if (!mfrc522.PICC_ReadCardSerial()) {
        readerDisabled = true;
        return;
    }

    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
        piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
        piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        readerDisabled = true;
        return;
    }

    memcpy(card_uid, mfrc522.uid.uidByte, mfrc522.uid.size);
    memcpy(card.uid, mfrc522.uid.uidByte, mfrc522.uid.size);

    MFRC522::StatusCode status;
    byte trailerBlock = 7;
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        readerDisabled = true;
        return;
    }
    status = mfrc522.MIFARE_Read(4, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        readerDisabled = true;
        return;
    }
    yes++;
    memcpy(card_data, buffer, 16);
      for (byte i = 0; i < 16; i++) {
      buffer[i] == 0;
    }

    int card_text_index = 0;
    for (int block = 0; block < 3; block++) {
        status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block_numbers[block], &key, &(mfrc522.uid));
        if (status != MFRC522::STATUS_OK) {
            readerDisabled = true;
            return;
        }
        status = mfrc522.MIFARE_Read(block_numbers[block], buffer, &size);
        if (status != MFRC522::STATUS_OK) {
            readerDisabled = true;
            return;
        }

        if (card_text_index + 16 <= sizeof(card_text)) {
            memcpy(card_text + card_text_index, buffer, 16);
            card_text_index += 16;
            yes++;
        } else {
            readerDisabled = true;
            return;
        }
    }

    card.card_text_str = "";
    for (int i = 0; i < card_text_index; i++) {
        if (card_text[i] != 0) {
            card.card_text_str += (char)card_text[i];
        }
    }

    if (yes == 4) {
        String card_data_str;
        SerialPrintTime();
        serialLog("Знайдено картку: ");
        serialLogln(card.card_text_str);
        for (uint8_t i = 0; i < 16; i++) {
            card_data_str += String(card_data[i]);
            if (i != 15) { card_data_str += ", "; }
        }
        for (uint8_t i = 0; i < mfrc522.uid.size; i++) {
            card_uid_str += String(card_uid[i], HEX);
            if (i != mfrc522.uid.size - 1) { card_uid_str += " "; }
        }
        decodecard(card_data);
        serialLog("Дані: ");
        serialLogln(card_data_str);
        serialLog("ID: ");
        serialLogln(card_uid_str);
        serialLogln("");
        cardreturntimer = millis();
        currPage = 6;
        update = 1;
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



int ChangeUsage(uint8_t *targetUID, uint8_t newValue, bool bypass) {
    // Проверка наличия новой карты
    if (!mfrc522.PICC_IsNewCardPresent()) {
        return 0;
    }

    // Чтение серийного номера карты
    if (!mfrc522.PICC_ReadCardSerial()) {
        return 0;
    }

    if (memcmp(mfrc522.uid.uidByte, targetUID, 7) == 0 or bypass == 1) {
        MFRC522::StatusCode status;

        // Аутентификация блока 4
        status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid));
        if (status != MFRC522::STATUS_OK) {
            return 0;
        }

        byte data[18];
        byte size = sizeof(data);
        
        // Чтение блока 4
        status = mfrc522.MIFARE_Read(4, data, &size);
        if (status != MFRC522::STATUS_OK) {
            return 0;
        }
        // Запись нового значения в блок 4
        data[0] = newValue;
        status = mfrc522.MIFARE_Write(4, data, 16);
        if (status != MFRC522::STATUS_OK) {
            return 0;
        }

        // Проверка записи
        status = mfrc522.MIFARE_Read(4, data, &size);
        if (status != MFRC522::STATUS_OK) {
            return 0;
        }

        if (data[0] == newValue) {
            return 1;
        }
    } else {
        // Античит: выводим сообщение о попытке обхода
        serialLogln("✘=========[ANTICHEAT]=========✘");
        SerialPrintTime();
        serialLogln("Виявлена спроба обходу системи карток.");
        serialLogln("=========");
        serialLogln("");
    }
    return 0;
}


