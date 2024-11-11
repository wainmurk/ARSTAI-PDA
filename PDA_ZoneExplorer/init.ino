void Init() {
  Serial.begin(115200);
  pinMode(15, OUTPUT);
  digitalWrite(15, 1);



  if (!LittleFS.begin(false)) {
    Serial.println("LittleFS не вдалося ініціалізувати!");
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
  mp3.begin(mp3Serial, MP3_SERIAL_TIMEOUT, DFPLAYER_MINI, false);                                             //"DFMINI" see NOTE, false=no response from module after the command
  mp3.stop();                                                                                                 //if player was runing during ESP8266 reboot
  mp3.reset();                                                                                                //reset all setting to default
  mp3.setSource(2);                                                                                           //1=USB-Disk, 2=TF-Card, 3=Aux, 4=Sleep, 5=NOR Flash
  mp3.setEQ(0);                                                                                               //0=Off, 1=Pop, 2=Rock, 3=Jazz, 4=Classic, 5=Bass
  mp3.setVolume(30);                                                                                          //0..30, module persists volume on power failure
  mp3.sleep();                                                                                                //inter sleep mode, 24mA
  mp3.wakeup(2);                                                                                              //exit sleep mode & initialize source 1=USB-Disk, 2=TF-Card, 3=Aux, 5=NOR Flash
  mp3Serial.enableRx(true);                                                                                   //enable interrupts on RX-pin for better response detection, less overhead than mp3Serial.listen()



  SPI.begin();
  mfrc522.PCD_Init();



  if (mp3.getStatus()) serialLogln("[MP3] Сталась помилка ініціалізації.");
  else serialLogln("[MP3] Ініціалізація ... Успішно!");

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
  if (data.is_knocked and currPage != 50 and currPage != 6) {
    if (secondsRemaining == 0) {
      data.health = 0;
      data.is_dead = 1;
      data.is_knocked = 0;
      causeOfDeath = "Поранення";
    } else {
      currPage = 50;
      update = 1;
    }
  }


  if (data.is_dead and currPage != 9 and currPage != 6) {
    data.health = 0;
    data.is_dead = 1;
    data.is_knocked = 0;
    currPage = 9;
    update = 1;
  }
}
