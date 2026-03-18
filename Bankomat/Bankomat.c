#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "bankomat.h"


Klient klienci[10] =
{
    {1, "Heinz", "Dundersztyc", "1294", 1250.0f, 0},
    {2, "Joe", "Mama", "8842", 2100.0f, 1 },
    {3, "Robert", "Mak³owicz", "0210", 1802.5f, 0},
    {4, "Fineasz", "Fletcher", "4403", 99.9f, 1},
    {5, "Agent", "P", "6720", 78560.7f, 0},
    {6, "Skipper", "Pingwin", "8922", 4590.1f, 0},
    {7, "Twilight", "Sparkle", "6769", 78.9f, 0},
    {8, "Satoru", "Gojou", "1234", 956789.2f, 0},
    {9, "Suguru", "Getou", "6666", 45819.5f, 1},
    {10, "Hannibal", "Lecter", "2061", 5672.1f, 1}
};

void plik_klienci(Klient* klienci, int liczbaK)
{
    FILE* plik = fopen("Klienci.dat", "wb");        //wb write binary
    if (plik == NULL)
    {
        perror("Nie mo¿na otworzyæ pliku");                //perror doda nam 'systemowy' opis b³êdu
        exit(1);                                        //koñczymy z b³êdem
    }
    fwrite(klienci, sizeof(Klient), liczbaK, plik);
    fclose(plik);
}


Klient* logowanie()
{
    int c;
    int podaneID, znaleziono = 0;
    char podany_PIN[5] = { 0 };

    printf("Proszê podaæ swój numer klienta: ");
    scanf_s("%5d", &podaneID);

    FILE* klienci_dane = fopen("Klienci.dat", "r+b");
    if (klienci_dane == NULL)
    {
        perror("Nie mo¿na otworzyæ pliku");                //perror doda nam 'systemowy' opis b³êdu
        exit(1);                                        //koñczymy z b³êdem
    }

    Klient temp;
    while (fread(&temp, sizeof(Klient), 1, klienci_dane))
    {
        if (temp.IDKlienta == podaneID)
        {
            znaleziono = 1;
            if (temp.blokada == 1) {
                printf("Twoje konto jest zablokowane. Skontaktuj siê z bankiem.\n");
                fclose(klienci_dane);
                return NULL;
            }
            for (int i = 3; i > 0; i--) {
                printf("Podaj swój PIN: ");
                scanf_s("%s", &podany_PIN, (unsigned)_countof(podany_PIN));
                while ((c = getchar()) != '\n' && c != EOF);
                if (strlen(podany_PIN) != 4) {
                    printf("Niepoprawna iloœæ znaków.\n");
                    printf("Niepoprawny PIN! Pozosta³o prób: %d\n", i - 1);
                }
                else if (strcmp(temp.PIN, podany_PIN) == 0 && strlen(podany_PIN) == 4)
                {
                    printf("Zalogowano");
                    fclose(klienci_dane);

                    Klient* zalogowany = malloc(sizeof(Klient));
                    if (zalogowany == NULL) {
                        printf("B³¹d alokacji pamiêci.\n");
                        return NULL;
                    }
                    *zalogowany = temp;
                    return zalogowany;
                }
                else {
                    if (i > 1) {
                        printf("Niepoprawny PIN! Pozosta³o prób: %d\n", i - 1);
                        //printf("Podaj swój PIN: ");
                        //scanf_s("%4s", podany_PIN, (unsigned)_countof(podany_PIN));
                    }
                    else {
                        printf("Trzykrotnie wpisano b³êdny PIN!\nTwoje konto zosta³o zablokowane!\nSkontaktuj siê z bankiem.");
                        temp.blokada = 1;
                        fseek(klienci_dane, -(long)sizeof(Klient), SEEK_CUR);
                        fwrite(&temp, sizeof(Klient), 1, klienci_dane);
                        fclose(klienci_dane);
                        return NULL;
                    }
                }
            }
        }
    }
    if (znaleziono != 1) {
        printf("Klient nie zosta³ znaleziony w bazie.");
    }
    fclose(klienci_dane);
    return NULL;
}


void sprawdz_saldo(Klient* klient) {
    if (klient == NULL) {
        printf("B³¹d");
        return;
    }
    printf_s("\nTwoje saldo: %.2f zl\n", klient->saldo_pocz);
}

void transakcja(Klient* klient, float kwota)
{
    if (klient == NULL) {
        printf_s("B³¹d");
        return;
    }

    if (kwota == 0) {
        printf_s("Kwota musi byæ wiêksza od 0\n");
        return;
    }
    if (kwota < 0 && klient->saldo_pocz + kwota < 0) {
        printf_s("Brak wystarczaj¹cyh œrodków\n");
        return;
    }

    klient->saldo_pocz = klient->saldo_pocz + kwota;

    if (kwota > 0) {
        printf_s("Wp³acono %.2f zl, Nowe saldo: %.2f zl\n", kwota, klient->saldo_pocz);
    }
    else {
        printf_s("Wyp³acono %.2f zl, Nowe saldo: %.2f zl\n", kwota, klient->saldo_pocz);
    }
    zapisz_operacje(klient->IDKlienta, (kwota > 0) ? "Wplata" : "Wyplata", fabs(kwota));        //fabs = abs ale na float

    FILE* klienci_dane = fopen("Klienci.dat", "r+b");    //r+b read + binary
    if (klienci_dane == NULL) {
        perror("B³¹d");
        return;
    }

    Klient temp;
    while (fread(&temp, sizeof(Klient), 1, klienci_dane)) {
        if (temp.IDKlienta == klient->IDKlienta) {
            fseek(klienci_dane, -(long)sizeof(Klient), SEEK_CUR);
            fwrite(klient, sizeof(Klient), 1, klienci_dane);
            break;
        }
    }
    fclose(klienci_dane);
}

void wplata(Klient* klient) {
    float kwota;
    char buf[100];
    char* end;
    printf_s("Podaj kwotê do wp³aty: ");
    if (fgets(buf, sizeof(buf), stdin)) {
        kwota = strtof(buf, &end);
        if (end == buf || *end != '\n') {
            printf("Niepoprawna wartoœæ\n");
            return;
        }
        transakcja(klient, kwota);
    }
    else {
        printf("B³¹d odczytu\n");
    }
}

void wyplata(Klient* klient) {
    char buf[100];
    float kwota;
    char* end;
    printf_s("Podaj kwot do wyp³aty: ");
    if (fgets(buf, sizeof(buf), stdin)) {
        kwota = strtof(buf, &end);
        if (end == buf || *end != '\n') {
            printf("Niepoprawna wartoœæ\n");
            return;
        }
        transakcja(klient, -kwota);
    }
    else {
        printf("B³¹d odczytu\n");
    }

}

void zmien_pin(Klient* klient) {
    int c;
    if (klient == NULL) {
        printf("B³¹d\n");
        return;
    }
    char staryPIN[5] = { 0 }, nowyPIN[5] = { 0 };
    printf_s("Podaj aktualny PIN: ");
    scanf_s("%s", staryPIN, (unsigned)_countof(staryPIN));
    while ((c = getchar()) != '\n' && c != EOF);
    if (strlen(staryPIN) != 4) {
        printf("Niepoprawna iloœæ znaków aktualnego kodu PIN.\n");
        return;
    }


    if (strcmp(staryPIN, klient->PIN) != 0) {
        printf_s("B³êdny aktualny PIN\n");
        while ((c = getchar()) != '\n' && c != EOF);
        return;
    }

    printf_s("Podaj nowy PIN (4 cyfry): ");
    scanf_s("%5s", nowyPIN, (unsigned)_countof(nowyPIN));

    if (strlen(nowyPIN) != 4) {
        printf_s("Nowy PIN musi mieæ dok³adnie 4 cyfry");
        while ((c = getchar()) != '\n' && c != EOF);
        return;
    }
    for (int i = 0; i < 4; i++) {
        if (!isdigit((unsigned char)nowyPIN[i])) {
            printf_s("PIN mo¿e zawieraæ tylko cyfry (0-9)\n");
            while ((c = getchar()) != '\n' && c != EOF);
            return;
        }
    }

    strcpy_s(klient->PIN, (rsize_t)sizeof(klient->PIN), nowyPIN);
    printf_s("PIN zosta³ zmieniony\n");

    FILE* klienci_dane = fopen("Klienci.dat", "r+b");
    if (klienci_dane == NULL) {
        perror("B³¹d");
        return;
    }

    Klient temp;
    while (fread(&temp, sizeof(Klient), 1, klienci_dane)) {
        if (temp.IDKlienta == klient->IDKlienta) {
            fseek(klienci_dane, -(long)sizeof(Klient), SEEK_CUR);
            fwrite(klient, sizeof(Klient), 1, klienci_dane);
            break;
        }
    }
    fclose(klienci_dane);
    zapisz_operacje(klient->IDKlienta, "Zamiana PIN", 0.0f);

}

void zapisz_operacje(int IDKlienta, const char* typ, float kwota)
{
    FILE* operacje = fopen("Operacje.dat", "ab");    //ab - append binary
    if (operacje == NULL) {
        perror("B³¹d");
        return;
    }
    Operacja operacja;
    operacja.IDKlienta = IDKlienta;
    strcpy_s(operacja.typOperacji, sizeof(operacja.typOperacji), typ);
    operacja.kwota = kwota;

    fwrite(&operacja, sizeof(Operacja), 1, operacje);
    fclose(operacje);
}

void historia_operacji(int IDKlienta)
{
    FILE* plik = fopen("Operacje.dat", "rb");                //read binary, sam odczyt
    if (plik == NULL) {
        printf_s("Brak historii operacji\n");
        return;
    }
    Operacja operacja;
    printf_s("\nHistoria operacji:\n");
    while (fread(&operacja, sizeof(Operacja), 1, plik))
    {
        if (operacja.IDKlienta == IDKlienta) {
            printf_s("- %s: %.2f zl\n", operacja.typOperacji, operacja.kwota);
            //printf_s("DEBUG: ID=%d, typ=%s, kwota=%.2f\n", operacja.IDKlienta, operacja.typOperacji, operacja.kwota);
        }
    }
    fclose(plik);

}