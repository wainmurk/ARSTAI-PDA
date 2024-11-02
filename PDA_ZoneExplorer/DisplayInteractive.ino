 void DisplayInteractiveNow(int page){
 if (page == 80){
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
        case 0: currPage = prev_page; update = 1; break;
        case 1: rescue(); break;
        case 2: mp3.playTrack(7); break; //currPage = 0; update = 1; break;
      }
    }
  }
 }