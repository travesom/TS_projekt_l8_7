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
};//jako ze 4 maks pola to status bedzie informowa� czy w nastepnym komunikacjie bedzie 1,2 argument czy to nawiazywanie  czy konczenie po��czenia a ns kt�ry to komunikat
//naziazywanie po�aczenie bedzie wygl�da�o tak Time|ST|NS|ID gdzie identyfikator to 0 na pocz�tek a NS liczymy  ile jescze pakit�w powinno sie wys�a�(czyli przy nawiazywaniu 0 mamy);()time ma rozmiar 4 czyli razem komunikat nawiazywania polaczenia ma rozmiar 13


//st 'p'-po�oczenie,'o'-info o operacje,'a' argumenty,'r'-rozlaczenie,'n'-nadawanie identyfikatora;