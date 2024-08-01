void cleardisplay(int var) {
  if (var == 0) {
    tft.fillRect(4, 21, 311, 213, TFT_BG);
  } else if (var == 1) {
    tft.fillRect(0, 0, 55, 16, TFT_BG);
  } else if (var == 2) {
    tft.fillRect(24, 27, 115, 10, TFT_BG);
  } else if (var == 3) {
    tft.fillRect(24, 47, 115, 10, TFT_BG);
  } else if (var == 4) {
    tft.fillRect(24, 68, 115, 10, TFT_BG);
  } else if (var == 5) {
    tft.fillRect(272, 0, 45, 15, TFT_BG);
  }
}

void printdisplay(int page) {
  cardreturntimer = millis();
  tft.setTextColor(TFT_TEXT);

  if (page == 0) {
    cleardisplay(0);
    TJpgDec.drawFsJpg(18, 48, "/images/health.jpeg", LittleFS);
    drawProgressBar(58, 54, 228, 16, data.health, TFT_TEXT, TFT_HEALTH);
    TJpgDec.drawFsJpg(18, 86, "/images/armor.jpeg", LittleFS);
    drawProgressBar(58, 94, 228, 16, data.armor, TFT_TEXT, TFT_ARMOR);
    TJpgDec.drawFsJpg(18, 125, "/images/radiation.jpeg", LittleFS);
    tft.setTextSize(2);
    tft.setCursor(58, 136);
    if (data.radiation < 250000) tft.print(data.radiation);
    else tft.print(">250000");
    tft.print(" мЗв");
    tft.setCursor(12, 164);
    tft.print("Рад.фон: ");
    if (isWarning and !isAlarm) {
      tft.setTextColor(TFT_WARNING);
      tft.print("Увага!");
    }
    if (isAlarm) {
      tft.setTextColor(TFT_ALARM);
      tft.print("Тревога!");
    }
    if (!isWarning and !isAlarm) {
      tft.setTextColor(TFT_GOOD);
      tft.print("Чисто.");
    }
    tft.setTextColor(TFT_TEXT);
    tft.setCursor(12, 190);
    tft.print("Аномалії: ");
  } else if (page == 1) {
    cleardisplay(0);
    tft.setTextSize(2);
    tft.setCursor(45, 20);
    tft.print("Захист");
    tft.setTextSize(1);
    tft.setCursor(8, 40);
    tft.print("Вогонь");
    tft.setCursor(8, 50);
    tft.print("Гравітац.");
    tft.setCursor(8, 60);
    tft.print("Кислота");
    tft.setCursor(8, 70);
    tft.print("Електро");
    tft.setCursor(8, 80);
    tft.print("Радіація");
    tft.setCursor(8, 90);
    tft.print("Психічн.");


    tft.setCursor(70, 40);
    tft.print(WhatPercent(data.fire_protection));
    tft.setCursor(70, 50);
    tft.print(WhatPercent(data.gravi_protection));
    tft.setCursor(70, 60);
    tft.print(WhatPercent(data.acid_protection));
    tft.setCursor(70, 70);
    tft.print(WhatPercent(data.electro_protection));
    tft.setCursor(70, 80);
    tft.print(WhatPercent(data.radiation_protection));
    tft.setCursor(70, 90);
    tft.print(WhatPercent(data.psy_protection));



  } else if (page == 2) {
    cleardisplay(0);
    if (currentsievert < 100) tft.setTextColor(TFT_TEXT);
    else if (currentsievert >= 100 and currentsievert < 10000) tft.setTextColor(TFT_WARNING);
    else if (currentsievert >= 10000) tft.setTextColor(TFT_ALARM);
    tft.setTextSize(6);
    currentsievert=12345678;
    centerText(currentsievert);



    tft.setTextSize(3);
    tft.setTextColor(TFT_TEXT);
    tft.setCursor(125, 180);
    tft.print("мЗв/c");














  } else if (page == 3) {
    cleardisplay(0);
    tft.setCursor(40, 35);
    tft.setTextSize(3);
    tft.setTextColor(TFT_ALARM);
    tft.print("Помер");
    tft.setTextColor(TFT_TEXT);
    tft.setCursor(55, 80);
    tft.setTextSize(1);
    tft.print("Причина:");
    tft.setCursor(20, 92);
    if (causeOfDeath == "") {
      tft.print("Невідома.");
    } else {
      tft.print(causeOfDeath);
    }

  } else if (page == 4) {
    cleardisplay(0);
    tft.setCursor(6, 20);
    tft.setTextColor(TFT_TEXT);
    tft.setTextSize(1);
    tft.print(card.card_text_str);
    tft.setCursor(6, 30);
    tft.print("Стан:");
    switch (card.usage_method) {
      case 0:
        tft.setTextColor(TFT_ALARM);
        tft.print("Зламаний.");
        break;
      case 1:
        tft.setTextColor(TFT_GOOD);
        tft.print("Одноразовий.");
        break;
      case 2:
        tft.setTextColor(TFT_ALARM);
        tft.print("Використаний.");
        break;
      case 3:
        tft.setTextColor(TFT_GOOD);
        tft.print("Багаторазовий.");
        break;
    }

    tft.setTextColor(TFT_TEXT);
    if (card.usage_method == 1 or card.usage_method == 3) {
      tft.setCursor(16, 48);
      tft.print("Не забирайте картку.");
      tft.setCursor(46, 60);
      tft.print("Застосувати?");
      drawButtons();
    } else {
      delay(1000);
      currPage = 0;
      printdisplay(currPage);
    }
  } else if (page == 5) {
    cleardisplay(0);
    tft.setCursor(10, 20);
    tft.setTextColor(TFT_ALARM);
    tft.print("Сталась якась помилка");
    tft.setCursor(10, 30);
    tft.print("Спробуйте ще раз.");
    tft.setCursor(10, 50);
    tft.print("Не забирайте картку!");
    delay(3000);
    currPage = 4;
    printdisplay(currPage);
  } else if (page == 6) {
  } else if (page == 99) {
    cleardisplay(0);
    tft.setCursor(10, 20);
    tft.setTextColor(TFT_ALARM);
    if(WEB)tft.print("Веб сервер активовано.");else tft.print("Веб сервер вимкнуто.");
    delay(2000);
    currPage = 0;
    printdisplay(currPage);
  }
  tft.setTextSize(1);
  tft.setTextColor(TFT_TEXT);
}

void printTime() {
  DateTime now = rtc.getTime();
  cleardisplay(1);
  tft.setTextSize(1);
  tft.setCursor(11, 5);
  if (now.hour <= 9) { tft.print("0"); }
  tft.print(now.hour);
  tft.print(":");
  if (now.minute <= 9) { tft.print("0"); }
  tft.print(now.minute);
}

void printBattery(){
  cleardisplay(5);
  tft.setTextSize(1);
  tft.setCursor(282, 5);
  tft.print("100%");
  tft.setTextSize(1);
}


void drawProgressBar(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t percent, uint16_t frameColor, uint16_t barColor) {
  if (percent == 0) {
    tft.fillRoundRect(x, y, w, h, 3, TFT_BLACK);
  }
  uint8_t margin = 2;
  uint16_t barHeight = h - 2 * margin;
  uint16_t barWidth = w - 2 * margin;
  tft.drawRoundRect(x, y, w, h, 3, frameColor);
  tft.fillRect(x + margin, y + margin, barWidth * percent / 100.0, barHeight, barColor);
}

void drawButtons() {
  uint16_t unselectedColor = 0x4A4A4A;  // Цвет невыбранной кнопки

  tft.setTextSize(2);
  int buttonWidth = 50;
  int buttonHeight = 30;
  int buttonRadius = 10;
  int buttonYOffset = 30;
  int buttonXOffset = 16;

  int yCenter = (tft.height() - buttonHeight) / 2 + buttonYOffset;

  // Отрисовка кнопки "Ні"
  if (!selectedButton) {
    tft.fillRoundRect(buttonXOffset + 78, yCenter, buttonWidth, buttonHeight, buttonRadius, TFT_WHITE);
    tft.drawRoundRect(buttonXOffset + 78, yCenter, buttonWidth, buttonHeight, buttonRadius, TFT_DARKGREY);
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
  } else {
    tft.fillRoundRect(buttonXOffset + 78, yCenter, buttonWidth, buttonHeight, buttonRadius, unselectedColor);
    tft.drawRoundRect(buttonXOffset + 78, yCenter, buttonWidth, buttonHeight, buttonRadius, TFT_DARKGREY);
    tft.setTextColor(TFT_WHITE, unselectedColor);
  }
  tft.setCursor(buttonXOffset + 104 + (buttonWidth - tft.textWidth("Так")) / 2, yCenter + (buttonHeight - tft.fontHeight()) / 2 - 0);  // Центрируем текст внутри кнопки
  tft.print("Ні");


  if (selectedButton) {
    tft.fillRoundRect(buttonXOffset, yCenter, buttonWidth, buttonHeight, buttonRadius, TFT_WHITE);
    tft.drawRoundRect(buttonXOffset, yCenter, buttonWidth, buttonHeight, buttonRadius, TFT_DARKGREY);
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
  } else {
    tft.fillRoundRect(buttonXOffset, yCenter, buttonWidth, buttonHeight, buttonRadius, unselectedColor);
    tft.drawRoundRect(buttonXOffset, yCenter, buttonWidth, buttonHeight, buttonRadius, TFT_DARKGREY);
    tft.setTextColor(TFT_WHITE, unselectedColor);
  }
  tft.setCursor(buttonXOffset + 8 + (buttonWidth - tft.textWidth("Ні")) / 2, yCenter + (buttonHeight - tft.fontHeight()) / 2 - 0);  // Центрируем текст внутри кнопки
  tft.print("Так");

  tft.setTextSize(1);
}

String WhatPercent(int pro) {
  String output;
  switch (pro) {
    case 0: output = " (-0%)"; break;
    case 1: output = " (-25%)"; break;
    case 2: output = " (-50%)"; break;
    case 3: output = " (-90%)"; break;
  }

  return output;
}


void centerText(int number) {
  String numStr = String(number);
  
  // Получить ширину текста в пикселях (8 пикселей на символ для стандартного шрифта)
  int16_t textWidth = numStr.length() * 36;

  // Вычислить начальную позицию для центрирования
  int xPos = 160 - textWidth / 2;
  int yPos = 80; // Установленное значение y
  
  // Установить курсор и вывести текст
  tft.setCursor(xPos, yPos);
  tft.print(numStr);
}
