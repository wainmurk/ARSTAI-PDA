void DisplayInteractiveNow(int page) {
  if (page == 80) {
    if (up.click()) {
      MasterselectedItem = (MasterselectedItem - 1 + MastermenuItemsCount) % MastermenuItemsCount;
      drawMenuMaster();
    }

    if (down.click()) {
      MasterselectedItem = (MasterselectedItem + 1) % MastermenuItemsCount;
      drawMenuMaster();
    }

    if (ok.click()) {
      switch (MasterselectedItem) {
        case 0:
          currPage = prev_page;
          update = 1;
          break;
        case 1: rescue(); break;
        case 2: mp3.playTrack(7); break;  //currPage = 0; update = 1; break;
      }
    }
  } else if (page == 50) {
    if (ok.click()) {
      timerStarted = 0;
      data.is_knocked = 0;
      currPage = 0;
      update = 1;
    }
    if (millis() - knockcheck > 1000) {
      knockcheck = millis();
      printdisplay(50);
    }
  } else if (page == 51) {
    if (millis() - revivecheck > 1000) {
      revivecheck = millis();
      printdisplay(51);
    }
  } else if (page < 4) {
    if (left.click()) {
      currPage--;
      if (currPage < 0) currPage = 3;
      update = 1;
    }
    if (right.click()) {
      currPage++;
      if (currPage > 3) currPage = 0;
      update = 1;
    }
  } else if (page == 6) {

    if (left.click() or right.click()) {
      selectedButton = !selectedButton;
      printdisplay(6);
    }

    if (ok.click()) {
      response = selectedButton;
      if (response) {
        applyCard(card.usage_method);
      }
      selectedButton = 1;
fixDisplay();
      cleardisplay(0);
      update = 1;
      printTime();
    }
  } else if (page == 10) {

    if (left.click() or right.click()) {
      selectedknockButton = !selectedknockButton;
      printdisplay(10);
    }

    if (ok.click()) {
      response = selectedknockButton;
      if (response) {
        ok.tick();
        data.is_knocked = 1;
        currPage = 50;
        cleardisplay(0);
        printdisplay(50);
        startTimer(s.knocked_time);
      } else {
        currPage = (data.is_dead) ? 9 : 0;
      }
      selectedknockButton = 1;
      update = 1;
      printTime();
    }
  } else   if (page == 4) {
    if (up.click()) {
      NPCselectedItem = (NPCselectedItem - 1 + NPCmenuItemsCount) % NPCmenuItemsCount;
      drawMenuNPC();
    }

    if (down.click()) {
      NPCselectedItem = (NPCselectedItem + 1) % NPCmenuItemsCount;
      drawMenuNPC();
    }

    if (ok.click()) {
      switch (NPCselectedItem) {
        case 0:
          if (ChangeUsage(card.uid, 1, 1)) {
            tft.setTextSize(2);
            tft.setTextColor(TFT_GOOD);
            tft.setCursor(30, 200);
            tft.print("Успішно");
            delay(500);
            drawMenuNPC();
          } else {
            tft.setTextSize(2);
            tft.setTextColor(TFT_RED);
            tft.setCursor(30, 200);
            tft.print("Помилка");
            delay(500);
            drawMenuNPC();
          }
          break;
        case 1: rescue(); break;
        case 4:
          currPage = 0;
          update = 1;
          break;
      }
    }
  } 

}