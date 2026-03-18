#pragma once

typedef struct
{
	int IDKlienta;
	char imie[50];
	char nazwisko[50];
	char PIN[5];
	float saldo_pocz;
	int blokada;	// 0=odblokowane, 1=zablokowane
}Klient;

typedef struct
{
	int IDKlienta;
	char typOperacji[20];
	float kwota;
}Operacja;

extern Klient klienci[10];
void plik_klienci(Klient* klienci, int liczbaK);

Klient* logowanie();
void sprawdz_saldo(Klient* klient);
void wyplata(Klient* klient);
void wplata(Klient* klient);
void transakcja(Klient* klient, float kwota);
void zmien_pin(Klient* klient);

void zapisz_operacje(int IDKlienta, const char* typ, float kwota);
void historia_operacji(int IDKlienta);
