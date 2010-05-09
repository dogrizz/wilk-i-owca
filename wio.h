/* Program: Wilk i owce
 * author: Dominik Zmitrowicz
 * 
 * Plik nagłówkowy zawierający definicje struktur oraz metod potrzebnych 
 * do programu właściwego.
 */

#include <X11/Xlib.h>
#include <X11/X.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 5000
#define OWCA 'o'
#define WILK 'w'

/*
 * Struktura reprezentująca pionek na szachownicy
 */
typedef struct pionek {
      int x;
      int y;
      char typ; // [WILK,OWCA,' ']
} pion;

/*
 * Struktura reprezentująca ruch pionka
 */
typedef struct Ruch {
      int x1;
      int y1;
      int x2;
      int y2;
} ruch;

/*
 * Metoda rysuje podstawowe okno i inicjuje zmienne dotyczące interfejsu graficznego.
 */
void inicjujOkno();

/*
 * Metoda ustawia pola na szachownicy i dodaje wilka.
 */
void inicjujPlansze();

/*
 * Dodaje do planszy owce
 */
void dodajOwce();

/*
 * Obsluguje eventy X-ow. W tym, co najwazniejsze, clickniecia.
 */
void eventy();

/*
 * Odpowiada za narysowanie planszy wraz z pionkami.
 */
void rysujPlansze();

/*
 * Rysuje pojedynczy pionek na ekranie.
 */
void rysujPionek(pion);

/*
 * Odpowiada za przesuniecie piona na planszy i jej przerysowanie.
 */
void wykonajRuch(ruch);

/*
 * Sprawdza czy któraś z stron spełniła warunki zwycięstwa.
 */
void sprawdzStan();

/*
 * Sprawdza poprawność ruchu.
 */
int sprawdzRuch(ruch);

/*
 * Odbiera ruch od przeciwnika.
 * Jezeli odebrany ruch nie pochodzi od przeciwnika
 * zwraca ruch inicjalizacji (9,9,9,9)
 */
ruch odbierz();

/*
 * Wysyła ruch przeciwnikowi.
 */
void wyslij(ruch);

/*
 * Dodaje pojedyńczy pion do planszy.
 */
void dodajPion(int,int,char);

/*
 * Odpowiada za wyrysowanie ekranu zakończenia.
 */
void koniec(char);

/*
 * Inicjuje zmienne potrzebne do wymiany danych między graczami.
 */
void zainicjujPolaczenie();

Display *mydisplay;
Window mywindow;
XSetWindowAttributes mywindowattributes;
XGCValues mygcvalues;
GC mygc;
Visual *myvisual;
int mydepth, myscreen;
Colormap colorMap;
XColor darkBrown, lightBrown, white, black, dummy;
XEvent myevent;
pion plansza[8][8];
ruch ostatniRuch; // ostatni ruch klienta
char mojTyp; // [WILK,OWCA]
int sock;
struct sockaddr_in sad;
char *adresPrzeciwnika = NULL;

pion null;
// zmienna potrzebna do stwierdzenia czy to bylo pierwsze clickniecie 
// czy juz okreslenie gdzie ma sie przesunac pion
int clickCounter=0;

// zmienna mówiąca czy już wykonałem ruch i mogę 
// przesłać go przeciwnikowi i czekacj na jego ruch
int wykonalemRuch = 0;

