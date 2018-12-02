#pragma once
#include<string>

//Nag³ówki (serializacja i deserializacja)
#define HEAD_OPERATION  std::string("Operacja: ")
#define HEAD_STATUS	    std::string("Status: ")
#define HEAD_NUMBER     std::string("Liczba: ")
#define HEAD_SEQNUM     std::string("Numer Sekwencyjny: ")
#define HEAD_SESSION_ID std::string("Identyfikator Sesji: ")
#define HEAD_CALC_ID    std::string("Identyfikator Obliczen: ")
#define HEAD_TIME       std::string("Czas: ")
#define HEAD_ADDRESS    std::string("Adres: ")

//Statusy
#define STATUS_SUCCESS       std::string("OPERACJA_UDANA")
#define STATUS_OUT_OF_RANGE  std::string("WYNIK_POZA_ZAKRESEM")
#define STATUS_FORBIDDEN     std::string("ODMOWA_DOSTEPU")
#define STATUS_NOT_FOUND     std::string("NIE_ZNALEZIONO")
#define STATUS_FOUND         std::string("ZNALEZIONO")
#define STATUS_HISTORY_EMPTY std::string("HISTORIA_PUSTA")

//Operacje
#define OP_BEGIN  std::string("ROZPOCZECIE")
#define OP_ACK    std::string("POTWIERDZENIE")
#define OP_END    std::string("ZAKONCZENIE")

#define OP_ADD   std::string("DODAWANIE")
#define OP_SUBT  std::string("ODEJMOWANIE")
#define OP_DIV   std::string("DZIELENIE")
#define OP_MULTP std::string("MNOZENIE")
#define OP_FACT  std::string("SILNIA")

#define OP_STATUS std::string("STATUS")

#define OP_ID_SESSION     std::string("IDENTYFIKATOR_SESJI")
#define OP_ID_CALCULATION std::string("IDENTYFIKATOR_OBLICZEN")

#define OP_HISTORY_WHOLE std::string("HISTORA_CALA")
#define OP_HISTORY_ID    std::string("HISTORIA_ID")

//Pola protoko³u (serializacja)
#define FIELD_NO_ADDITIONAL -1
#define FIELD_OPERATION      0
#define FIELD_NUMBER         1
#define FIELD_CALCULATION_ID 2
#define FIELD_STATUS         3
#define FIELD_ADDRESS        4