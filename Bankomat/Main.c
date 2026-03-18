#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <ctype.h>
#include "bankomat.h"


int main() {
    SetConsoleOutputCP(1250);
    SetConsoleCP(1250);
    //remove("Operacje.dat");   //gdybyœmy chcieli wyczyœciæ historiê
    //plik_klienci(klienci, 10); //zapis jednorazowy do pliku listy klientów

    Klient* klient = logowanie();
    if (klient != NULL) {
        int wybor;
        do {
            printf_s("\n1.SprawdŸ saldo\n2.Wyp³ata\n3.Wp³ata\n4.Historia\n5.Zmieñ kod PIN\n6.Wyjœcie\nWybór: ");
            scanf_s("%d", &wybor);
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            switch (wybor) {
            case 1: sprawdz_saldo(klient); break;
            case 2: wyplata(klient); break;
            case 3: wplata(klient); break;
            case 4: historia_operacji(klient->IDKlienta); break;
            case 5: zmien_pin(klient); break;
            case 6:   free(klient); return;
            default: printf_s("Nieznana operacja\n."); break;
            }
        } while (wybor != 6);

    }

    return 0;
}