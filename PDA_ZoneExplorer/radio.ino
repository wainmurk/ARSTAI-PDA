
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

  // Serial.println("Name: " + name);
  // Serial.println("Game Code: " + String(rxgameCode));
  // Serial.println("RSSI Detection: " + String(rssiDetection));
  // Serial.println("RSSI Activation: " + String(rssiActivation));
  // Serial.println("Group Except: " + String(groupsexcept));
  // Serial.println("Groups Only: " + String(groupsonly));
  // Serial.println("Type: " + String(type));
  // Serial.println("Subtype: " + String(subtype));
  // Serial.println("Damage Type: " + String(damagetype));
  // Serial.println("Value Percent: " + String(valuepercent));
  // Serial.println("Value: " + String(value));
  // Serial.println("Multiplier: " + String(multiplier));
  // Serial.println("Min: " + String(min));

  if (rxgameCode == s.gameCode) {
    if (groupsexcept == 0 or groupsexcept != data.group) {
      if (groupsonly == 0 or groupsonly == data.group) {

        if (type == 15) {
          lastShelterTestTime = millis();
          if (in_shelter == false) {
            in_shelter = true;
            serialLogln("Отримано сигнал від укриття. Статус: в укритті.");
          }
        }


        if (type == 2) {
          updateRadiation(rssiDetection, rssiActivation, rssi, value, multiplier);
        }



        if (type == 6) {
          mineExplode(name, rssiActivation, rssi, value);
        }
        if (type == 10) {
          GlobalReviveTime = min * 60;
          GlobalGoRevive = 1;
        }
      }
    }
  }
}


void updateRadiation(int rssiDetection, int rssiActivation, int rssi, int sourcevalue, int power) {
  if (!data.is_dead and !isReviving and !data.is_knocked) {
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
      if (data.armor > 0) {
          applyedsievert = currentsievert * ((100 - constrain(data.radiation_protection, 0, 100)) / 100);
      } else applyedsievert = currentsievert;
      if (s.GroupIDRadiationIgnore != data.group){
         data.radiation += applyedsievert;
         if(s.DoRadiationAffectOnArmor){
          if(currentsievert <= 200)data.armor -= 0.02;
          else if(currentsievert >= 200 and currentsievert < 1000)data.armor -= 0.1;
          else if(currentsievert >= 1000 and currentsievert < 10000)data.armor -= 0.3;
          else if(currentsievert >= 10000 and currentsievert < 30000)data.armor -= 0.6;
          else if(currentsievert >= 30000)data.armor -= 1;
          data.armor = constrain(data.armor, 0, 100);
         }
      }
      mp3.playTrack(genRandom(8, 11));
    }
  }
}


void mineExplode(String name, int rssiActivation, int rssi, int sourcevalue) {
  if (!data.is_dead and !isReviving and !data.is_knocked) {
    unsigned long currentTime = millis();

    if (currentTime - lastMineExplodeTime >= 20000) {
      if (-rssi <= rssiActivation) {
        int remainingDamage = sourcevalue;

        if (data.armor > 0) {
          if (data.armor >= remainingDamage) {
            data.armor -= remainingDamage;
            remainingDamage = 0;
          } else {
            remainingDamage -= data.armor;
            data.armor = 0;
          }
        }
        if (remainingDamage > 0) {
          if (data.health - remainingDamage <= 0) {
            data.health = 0;
            causeOfDeath = name;
          } else {
            data.health -= remainingDamage;
          }
        }

        mp3.playTrack(12);
        lastMineExplodeTime = currentTime;
      }
    }
  }
}
