#include <X11/Xlib.h>
#include <X11/X.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define OWCA 'o'
#define WILK 'w'

typedef struct pionek {
      int x;
      int y;
      char typ;
} pion;

typedef struct Ruch {
      int x1;
      int y1;
      int x2;
      int y2;
} ruch;

void inicjujOkno();
void inicjujPlansze();
void eventy();
void rysujPlansze();
void rysujPionek(pion);
void wykonajRuch(ruch);
void sprawdzStan();
int sprawdzRuch(ruch);
ruch odbierz();
void wyslij(ruch);
void dodajPion(int,int,char);
void koniec(char);

Display *mydisplay;
Window mywindow;
XSetWindowAttributes mywindowattributes;
XGCValues mygcvalues;
GC mygc;
Visual *myvisual;
int mydepth, myscreen, sock;
int wyswietlaj = 1;
Colormap colorMap;
XColor darkBrown,lightBrown,white,black,dummy;
XEvent myevent;
pion plansza[8][8];
ruch ostatniRuch;
char mojTyp;
struct sockaddr_in sad;

pion null;
int clickCounter=0;
int wykonalemRuch = 0;

int main(int argc, char* argv[]){
  inicjujOkno();
  inicjujPlansze();
  
  if(argc == 1) {
    mojTyp = WILK;
  }else {
    mojTyp = OWCA;
  }
  
  while(wyswietlaj){
    eventy();
    if(wykonalemRuch == 1){
      wykonalemRuch = 0;
      sprawdzStan();
    
      //ruchPrzeciwnika = odbierz();
      //wykonajRuch(ruchPrzeciwnika);
      sprawdzStan();
    }
  }
  
  return 0;
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
  dodajPion(0,1,OWCA);
  dodajPion(0,3,OWCA);
  dodajPion(0,5,OWCA);
  dodajPion(0,7,OWCA);
  
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
  XSelectInput(mydisplay,mywindow,ExposureMask|KeyPressMask|ButtonPressMask);
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
        printf("1\n");
        clickCounter=1;
        move.y1=myevent.xbutton.x/50;
        move.x1=myevent.xbutton.y/50;
        if(plansza[move.x1][move.y1].typ == null.typ || plansza[move.x1][move.y1].typ != mojTyp){
          clickCounter=0;
        }
      }else{
        printf("2\n");
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
    XFillArc(mydisplay,mywindow,mygc,5+pionek.y*50,5+pionek.x*50,40,40,0,360*64);
  }
}

int sprawdzRuch(ruch move){
  if(move.x2 <0 || move.y2 <0 || move.x2 >8 || move.y2 >8){
    return 0;
  }
  if(plansza[move.x2][move.y2].typ != null.typ){
    return 0;
  }
  if(move.x1 == move.x2 && move.y1 == move.y2){
    return 0;
  }
  switch (plansza[move.x1][move.y1].typ) {
    case WILK:
      if(!((move.x1 +1 == move.x2 && move.y1-1 == move.y2) || (move.x1 +1 == move.x2 && move.y1+1 == move.y2) 
         || (move.x1 -1 == move.x2 && move.y1-1 == move.y2) || (move.x1 -1 == move.x2 && move.y1+1 == move.y2))) {
      return 0;
      }
      break;
    case  OWCA:
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
