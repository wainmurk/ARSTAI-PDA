void parseRadioPacket(const String& input, int rssi) {
  String name;
  byte rxgameCode;
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
         &rxgameCode, &rssiDetection, &rssiActivation, &groupsexcept,
         &groupsonly, &type, &subtype, &damagetype, &valuepercent,
         &value, &multiplier, &min);

  if (0) {
    Serial.println("Name: " + name);
    Serial.println("Game Code: " + String(rxgameCode));
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

  if (rxgameCode == s.gameCode) {
    if (groupsexcept == 0 or groupsexcept != data.group) {
      if (groupsonly == 0 or groupsonly == data.group) {

        if (type == 15 and in_shelter == false) {
          lastShelterTestTime = millis();
          in_shelter = true;
          Serial.println("Получен сигнал от укрытия. Статус: в укрытии.");
        }


        if (type == 2) {
          updateRadiation(rssiDetection, rssiActivation, rssi, value, multiplier);
        }



        if (type == 6) {
          mineExplode(name, rssiActivation, rssi, value);
        }




      }
    }
  }
}


void updateRadiation(int rssiDetection, int rssiActivation, int rssi, int sourcevalue, int power) {
  lastRadiationTime = millis();
  int applyedsievert;
  int value;
  value = sourcevalue * pow(10, power);

  if (-rssi <= rssiActivation) {
    if (rssi >= -60) {
      currentsievert = map(rssi, -60, -30, value, 5 * value);
    } else if (rssi > -127 and rssi < -60) {
      currentsievert = map(rssi, -rssiActivation, -60, 0, value);
    }
    switch (data.radiation_protection) {
      case 1: applyedsievert = currentsievert * 0.75; break;
      case 2: applyedsievert = currentsievert * 0.50; break;
      case 3: applyedsievert = currentsievert * 0.10; break;
      default: applyedsievert = currentsievert;
    }
    data.radiation += applyedsievert;
    mp3.playTrack(genRandom(8, 11));
  }
}
void mineExplode(String name, int rssiActivation, int rssi, int sourcevalue) {
  unsigned long currentTime = millis();

  if (currentTime - lastMineExplodeTime >= 20000) {
    if (-rssi <= rssiActivation) {
      if (data.health - sourcevalue <= 0) {
        data.health = 0;
        causeOfDeath = name;
      } else {
        data.health -= sourcevalue;
      }
      mp3.playTrack(12);
      lastMineExplodeTime = currentTime;
    }
  }
}
