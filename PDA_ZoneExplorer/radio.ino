void parseRadioPacket(const String& input, int rssi) {
  String name;
  byte gameCode;
  byte rssiDetection;
  byte rssiActivation;
  byte groupsexcept;
  byte groupsonly;
  byte type;
  byte subtype;
  byte damagetype;
  byte valuepercent;
  byte value;
  byte multiplier;
  byte min;

  int firstComma = input.indexOf(',');
  name = input.substring(0, firstComma);

  sscanf(input.c_str() + firstComma + 1, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu",
         &gameCode, &rssiDetection, &rssiActivation, &groupsexcept,
         &groupsonly, &type, &subtype, &damagetype, &valuepercent,
         &value, &multiplier, &min);

  if (0) {
    Serial.println("Name: " + name);
    Serial.println("Game Code: " + String(gameCode));
    Serial.println("RSSI Detection: " + String(rssiDetection));
    Serial.println("RSSI Activation: " + String(rssiActivation));
    Serial.println("Group Except: " + String(groupsexcept));
    Serial.println("Groups Only: " + String(groupsonly));
    Serial.println("Type: " + String(type));
    Serial.println("Subtype: " + String(subtype));
    Serial.println("Damage Type: " + String(damagetype));
    Serial.println("Value Percent: " + String(valuepercent));
    Serial.println("Value: " + String(value));
    Serial.println("Multiplier: " + String(multiplier));
    Serial.println("Min: " + String(min));
  }

  //if(gameCode == )


  if (type == 15 and in_shelter == false) {
    lastShelterTestTime = millis();
    in_shelter = true;
    Serial.println("Получен сигнал от укрытия. Статус: в укрытии.");
  }
  if (type == 2) {
    updateRadiation(rssiDetection, rssiActivation, rssi, value);
  }
}


void updateRadiation(int rssiDetection, int rssiActivation, int rssi, int value) {
      lastRadiationTime = millis();
    if (-rssi <= rssiActivation) {
        if (rssi >= -60) {
            currentsievert = map(rssi, -60, -30, value, 5*value);
        } else if (rssi > -127 and rssi < -60) {
            currentsievert = map(rssi, -rssiActivation, -60, 0, value);
        }
        data.radiation += currentsievert;
  }
}
