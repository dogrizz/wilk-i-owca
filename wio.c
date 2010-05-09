/* Program: Wilk i owce
 * author: Dominik Zmitrowicz
 * 
 * Plik główny programu.
 */

#include "wio.h"

int main(int argc, char* argv[]){
  ruch ruchPrzeciwnika;
  inicjujOkno();
  inicjujPlansze();
  if(argc ==2){
    dodajOwce();
  }
  eventy();
  
  if(argc == 1) {
    mojTyp = WILK;
    zainicjujPolaczenie();
    odbierz();
    dodajOwce();
  }else if(argc ==2 ) {
  
    //sztuczne dane potrzebne tylko do wyslania sygnalu startu
    ruch inicjalizacja;
    inicjalizacja.x1 =9;
    inicjalizacja.y1 =9;
    inicjalizacja.x2 =9;
    inicjalizacja.y2 =9;
    
    adresPrzeciwnika = argv[1];
    zainicjujPolaczenie();
    wyslij(inicjalizacja);
    ruchPrzeciwnika = odbierz();
    wykonajRuch(ruchPrzeciwnika);
    sprawdzStan();
    mojTyp = OWCA;
  }else{
    printf("Podano zbyt duza liczba argumentow\n Poprawne uzycie ./wio adres_ip\n");
    return 0;
  }
  
  while(1){
    XSync(mydisplay,True); //usuniecie wszystkich eventow wyklikanych w czasie oczekiwania na ruch
    eventy();
    if(wykonalemRuch == 1){
      wyslij(ostatniRuch); // wysłanie ruchu przeciwnikowi
      wykonalemRuch = 0;
      sprawdzStan();
      ruchPrzeciwnika = odbierz();
      //poki ktos nowy probuje się dopiąć, a nie normalny ruch
      while(ruchPrzeciwnika.x1 ==9 || ruchPrzeciwnika.y1 ==9 || ruchPrzeciwnika.x2 ==9 || ruchPrzeciwnika.y2 ==9 ){
        ruchPrzeciwnika = odbierz();
      }
      wykonajRuch(ruchPrzeciwnika);
      sprawdzStan();
    }
  }
  
  return 0;
}

void dodajOwce(){
  dodajPion(0,1,OWCA);
  dodajPion(0,3,OWCA);
  dodajPion(0,5,OWCA);
  dodajPion(0,7,OWCA);
  XClearWindow(mydisplay,mywindow);
  rysujPlansze();
}

void inicjujPlansze(){
  
  int i;
  int j;
  null.typ = ' ';
  
  for(i=0;i<8;i++) {
    for(j=0;j<8;j++) {
      dodajPion(i,j,' ');
    }
  }
  
  dodajPion(7,0,WILK);
  
}

void inicjujOkno(){
  mydisplay = XOpenDisplay("");
  myscreen = DefaultScreen(mydisplay);
  myvisual = DefaultVisual(mydisplay,myscreen);
  mydepth = DefaultDepth(mydisplay,myscreen);
  mywindowattributes.background_pixel = XWhitePixel(mydisplay,0);
  mywindowattributes.override_redirect = False;
  mywindow = XCreateWindow(mydisplay,XRootWindow(mydisplay,myscreen),
                      100,100,400,400,10,mydepth,InputOutput,
                      myvisual,CWBackPixel|CWOverrideRedirect,
                      &mywindowattributes);
  XSelectInput(mydisplay,mywindow,ExposureMask|ButtonPressMask);
  colorMap = DefaultColormap(mydisplay,myscreen);                 
  XAllocNamedColor(mydisplay,colorMap,"sienna4",&darkBrown,&dummy);
  XAllocNamedColor(mydisplay,colorMap,"WhiteSmoke",&white,&dummy);
  XAllocNamedColor(mydisplay,colorMap,"black",&black,&dummy);
  XAllocNamedColor(mydisplay,colorMap,"chocolate1",&lightBrown,&dummy);
  XStoreName(mydisplay,mywindow,"Wilk i owce");   
  XMapWindow(mydisplay,mywindow);
  mygc = DefaultGC(mydisplay,myscreen);
  
}

void dodajPion(int x,int y,char typ) {
  pion pionek;
  pionek.x = x;
  pionek.y = y;
  pionek.typ = typ;
  plansza[x][y] = pionek;
}

void rysujPlansze(){
  int i=0;
  int j=0;

  for(i=0;i<8;i++) {
    for(j=0;j<8;j++) {

      if(0== (j+i) % 2) {
        XSetForeground(mydisplay,mygc,lightBrown.pixel);
      }
      else{
        XSetForeground(mydisplay,mygc,darkBrown.pixel);
      }
      XFillRectangle(mydisplay,mywindow,mygc,j*50,i*50,50,50);
      if(plansza[i][j].typ != null.typ) {
        rysujPionek(plansza[i][j]);
      }
    }
  }
  XFlush(mydisplay);

}

void eventy(){
  ruch move;
  XNextEvent(mydisplay,&myevent); 
  
  switch (myevent.type) {
     
     case Expose:
      rysujPlansze(1);
      break;
     case ButtonPress:
      if(clickCounter ==0){
        clickCounter=1;
        move.y1=myevent.xbutton.x/50;
        move.x1=myevent.xbutton.y/50;
        if(plansza[move.x1][move.y1].typ == null.typ || plansza[move.x1][move.y1].typ != mojTyp){
          clickCounter=0;
        }
      }else{
        move.y2=myevent.xbutton.x/50;
        move.x2=myevent.xbutton.y/50;
        if(sprawdzRuch(move)){
          wykonajRuch(move);
          ostatniRuch = move;
          wykonalemRuch=1;
        }else{
          printf("ruch niedozwolony\n");
        }
        clickCounter = 0;
        
      }
     
  }
}

void rysujPionek(pion pionek) {
  if(pionek.typ != ' '){
    if(pionek.typ == OWCA) {
      XSetForeground(mydisplay,mygc,white.pixel);
    } else {
      XSetForeground(mydisplay,mygc,black.pixel);
    }
    //zeby zgadzal sie obraz wyswietlany z planszą trzeba zamienic x y.
    XFillArc(mydisplay,mywindow,mygc,5+pionek.y*50,5+pionek.x*50,40,40,0,360*64);
  }
}

int sprawdzRuch(ruch move){
  // jeśli nie mieści się w zakresie
  if(move.x2 <0 || move.y2 <0 || move.x2 >8 || move.y2 >8){
    return 0;
  }
  //jeśli na docelowym polu jest już pionek
  if(plansza[move.x2][move.y2].typ != null.typ){
    return 0;
  }
  //jeśli punkt docelowy i źródłowy są takie same
  if(move.x1 == move.x2 && move.y1 == move.y2){
    return 0;
  }
  switch (plansza[move.x1][move.y1].typ) {
    case WILK:
      //wilk moze sie ruszac tylko diagonalnie
      if(!((move.x1 +1 == move.x2 && move.y1-1 == move.y2) || (move.x1 +1 == move.x2 && move.y1+1 == move.y2) 
         || (move.x1 -1 == move.x2 && move.y1-1 == move.y2) || (move.x1 -1 == move.x2 && move.y1+1 == move.y2))) {
      return 0;
      }
      break;
    case  OWCA:
      //owca moze ruszac sie tylko na diagonalnie do przodu   
      if(!((move.x1 +1 == move.x2 && move.y1+1 == move.y2) || (move.x1 +1 == move.x2 && move.y1-1 == move.y2))){
        return 0;
    }
  }
  return 1;
}

void wykonajRuch(ruch move){
  dodajPion(move.x2,move.y2,plansza[move.x1][move.y1].typ);
  plansza[move.x1][move.y1].typ = null.typ;
  XClearWindow(mydisplay,mywindow);
  rysujPlansze();
}

void sprawdzStan(){
  int i;
  int j;
  ruch move;

  for(i=0;i<8;i++)
    for(j=0;j<8;j++)
      if(plansza[i][j].typ == WILK){
        if(i==0) //wilk dotarl do konca
          koniec(WILK);
        else{
          move.x1 = i;
          move.y1 = j;
          move.x2 = i+1;
          move.y2 = j+1;
          if(sprawdzRuch(move)){
            return;
          }
          move.y2 = j-1;
          if(sprawdzRuch(move)){
            return;
          }
          move.x2 = i-1;
          if(sprawdzRuch(move)){
            return;
          }
          move.y2 = j+1;
          if(sprawdzRuch(move)){
            return;
          }
          //wilk nie ma już ruchów
          koniec(OWCA);
        }           
        break;
      }

  return;
}

void koniec(char kto){
  XClearWindow(mydisplay,mywindow);
  rysujPlansze();
    
  if(mojTyp == kto){
    XSetForeground(mydisplay,mygc,white.pixel);
    XDrawString(mydisplay,mywindow,mygc,180,200,"Wygrales!",9);
    
    while(1) {
      XNextEvent(mydisplay,&myevent); 
  
      switch (myevent.type) {
        case Expose:
          rysujPlansze();
          XSetForeground(mydisplay,mygc,white.pixel);
          XDrawString(mydisplay,mywindow,mygc,180,200,"Wygrales!",9);    
        }
    }
  }
  else{
    XSetForeground(mydisplay,mygc,white.pixel);
    XDrawString(mydisplay,mywindow,mygc,180,200,"Przegrales",10);
    
    while(1) {
      XNextEvent(mydisplay,&myevent); 
  
      switch (myevent.type) {
        case Expose:
          rysujPlansze();
          XSetForeground(mydisplay,mygc,white.pixel);
          XDrawString(mydisplay,mywindow,mygc,180,200,"Przegrales",10);
        }
    }
    
  }
}

void zainicjujPolaczenie(){
  sock=socket(AF_INET,SOCK_DGRAM,0);
  sad.sin_family=AF_INET;
  if(adresPrzeciwnika == NULL){
    sad.sin_addr.s_addr=htonl(INADDR_ANY);
  }
  else{
    sad.sin_addr.s_addr=inet_addr(adresPrzeciwnika);
  }
  sad.sin_port=htons((ushort) PORT);
  bind(sock,(struct sockaddr *) &sad,sizeof(sad));
  
}

void wyslij(ruch move){
  int val,len;
  len=sizeof(sad);
  val = 1000* move.x1 + 100* move.y1 + 10 * move.x2 + move.y2;

  val=htonl(val);
  sendto(sock,&val,sizeof(int),0,(struct sockaddr *) &sad,len);
  printf("wyslalem\n");
}


ruch odbierz(){
  int len,val;
  ruch move;
  
  len=sizeof(sad);
  printf("czekam\n");
  recvfrom(sock,&val,sizeof(int),0,(struct sockaddr *) &sad,(socklen_t *)&len);
 
  //niepoprawne źródło paczki
  if(adresPrzeciwnika!= NULL && strcmp(inet_ntoa(sad.sin_addr),adresPrzeciwnika)){
    move.x1 = 9;
    move.y1 = 9;
    move.x2 = 9;
    move.y2 = 9;
    return move;
  }
 
  val=ntohl(val);
  printf("juz nie\n");
  
  move.x1 = val/1000;
  move.y1 = (val%1000)/100;
  move.x2 = (val%100 )/10;
  move.y2 = (val%10);
  
  if(adresPrzeciwnika==NULL){
    adresPrzeciwnika = inet_ntoa(sad.sin_addr);
  }
  
  return move;
}
