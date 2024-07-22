


void DoRandomVals() {
  if (millis() - updatetime > 500) {
    byte rannum = random(1, 4);
    updatetime = millis();
    if (rannum == 1) {
      data.health = random(0, 101);
    } else if (rannum == 2) {
      data.armor = random(0, 101);
    } else if (rannum == 3) {
      data.radiation = random(0, 350000);
    }
  }
}


void applyCard(int u_method) {
  if (u_method == 1) {
    int check = MakeCardUsed(card.uid, 2);
    if (check == 1) {
      doCard();
    } else if (check == 0) {
      currPage = 5;
      printdisplay(currPage);
    }
  } else if (u_method == 3) {
    doCard();
  }
}

void UpdateDisplay(int page, int clear) {
}

void doCard() {
if(card.type == 10)ChangeWebServerStatus();







}

void ChangeWebServerStatus(){
WEB = !WEB;
if(WEB){
  StartWebServer();
}else{
  server.end();
}
currPage = 99;
printdisplay(currPage);




}


void CheckPlayersDeath() {
  WhatsTheReason();

  if (data.health <= 0) {
    data.health = 0;
    data.is_dead = 1;
    data.radiation = 0;
    updateConfig();
    currPage = 3;
    printdisplay(currPage);
    update = 1;
  } else {
    if (data.is_dead) {
      update = 1;
      data.is_dead = 0;
      updateConfig();
      PrintMainPage(1);
      update = 1;
    }
    if (currPage == 3) {
      PrintMainPage(1);
      update = 1;
    }
  }
}
void PrintMainPage(bool change) {
  if (change) currPage = 0;

  cleardisplay(0);
  printdisplay(currPage);
}

void rescue() {

  data.health = 100;
  data.is_dead = 0;
  data.radiation = 0;
  currPage = 0;
  update = 1;
}


void WhatsTheReason() {
  if (data.radiation >= 1000) {
    causeOfDeath = "Радіація";
  }
}
