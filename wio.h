#include <X11/Xlib.h>
#include <X11/X.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 5000
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
void zainicjujPolaczenie();

Display *mydisplay;
Window mywindow;
XSetWindowAttributes mywindowattributes;
XGCValues mygcvalues;
GC mygc;
Visual *myvisual;
int mydepth, myscreen;
int wyswietlaj = 1;
Colormap colorMap;
XColor darkBrown, lightBrown, white, black, dummy;
XEvent myevent;
pion plansza[8][8];
ruch ostatniRuch;
char mojTyp;
int sock;
struct sockaddr_in sad;
char *adresPrzeciwnika = NULL;

pion null;
int clickCounter=0;
int wykonalemRuch = 0;
