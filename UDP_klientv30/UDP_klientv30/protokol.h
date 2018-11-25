#pragma once
#include <time.h>
#include <stdio.h>
class protokol
{
public:

	char OP, ST;//pole operacji,pole statusu
	int liczba1, liczba2, NS, ID, IO;//watrosci1, wartosci2,numer sekwencyjny,identyfikator,identyfikator obliczne
	long int  time;//czas
	protokol();
	~protokol();
};//jako ze 4 maks pola to status bedzie informowa³ czy w nastepnym komunikacjie bedzie 1,2 argument czy to nawiazywanie  czy konczenie po³¹czenia a ns który to komunikat
//naziazywanie po³aczenie bedzie wygl¹da³o tak Time|ST|NS|ID gdzie identyfikator to 0 na pocz¹tek a NS liczymy  ile jescze pakitów powinno sie wys³aæ(czyli przy nawiazywaniu 0 mamy);()time ma rozmiar 4 czyli razem komunikat nawiazywania polaczenia ma rozmiar 13


//st 'p'-po³oczenie,'o'-info o operacje,'a' argumenty,'r'-rozlaczenie,'n'-nadawanie identyfikatora;