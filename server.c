#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

const int STR_LEN = 20;
const char* toServerPath = "/tmp/toServer";

void calculatePi(char* str, int n)
{
	if (n <= 0)
	{
		// zwrocenie bledu w przypadku wartosci N mniejszej lub rownej 0
		sprintf(str, "BŁĄD:N<=0");
		return;
	}
	float x, y;
	int k = 0;
	for (int i = 0; i < n; i++)
	{
		// losowanie punktu
		x = rand() / (float)RAND_MAX;
		y = rand() / (float)RAND_MAX;
		if (x * x + y * y <= 1)
		{
			// zliczenie punktu, jesli zawiera sie w kole
			k++;
		}
	};
	//
	float pi = 4.0 * k / n;
	// zapis wyniku to zmiennej jako tekst
	sprintf(str, "%f", pi);
}

void* threadFunc(void* arg)
{
	char* string = (char*)arg;
	char toClientPath[100];
	sprintf(toClientPath, "%s", "/tmp/toClient");
	strcat(toClientPath, string);
	printf("Otrzymano zapytanie od Klienta (N = %s)\n", string);
	FILE* fdw;

	// utworzenie potoku do zapisu
	if (mkfifo(toClientPath, 0777) == -1 && errno != EEXIST)
	{
		// wyswietlenie bledu w przypadku niepowodzenia
		printf("Nie udalo sie utworzyc pliku fifo");
		exit(EXIT_FAILURE);
	}

	// otwarcie potoku do zapisu
	if (!(fdw = fopen(toClientPath, "w")))
	{
		// wyswietlenie bledu w przypadku niepowodzenia
		printf("Nie udalo sie otworzyc pliku do zapisu\n");
		exit(EXIT_FAILURE);
	}

	int n;
	// konwersja wartosci n na liczbe calkowita
	sscanf(string, "%d", &n);
	char result[STR_LEN];
	// obliczenie wartosci pi
	calculatePi(result, n);

	// przeslanie odpowiedzi do klienta
	fprintf(fdw, " %s\n", result);
	fflush(fdw);
	printf("Wyslano odpowiedz do Klienta (PI = %s)\n", result);
}

int main()
{
	srand(time(NULL));
	char s[STR_LEN];
	FILE* fdr;

	while (1)
	{
		// utworzenie potoku do odczytu
		if (mkfifo(toServerPath, 0777) == -1 && errno != EEXIST)
		{
			// wyswietlenie bledu w przypadku niepowodzenia
			perror("Nie udalo sie utworzyc pliku fifo");
			exit(EXIT_FAILURE);
		}

		printf("Oczekiwanie na Klienta...\n");
		// otwarcie potoku do odczytu
		if (!(fdr = fopen(toServerPath, "r")))
		{
			// wyswietlenie bledu w przypadku niepowodzenia
			perror("Nie udalo sie otworzyc pliku do odczytu");
			exit(EXIT_FAILURE);
		}

		printf("Wykryto Klienta, oczekiwanie na zapytanie...\n");
		while (fscanf(fdr, " %s", s) != EOF)
		{
			int retcode;
			pthread_t t1;
			// utworzenie watku do obliczen
			retcode = pthread_create(&t1, NULL, &threadFunc, (void*)(s));
			if (retcode != 0)
			{
				// wyswietlenie bledu w przypadku niepowodzenia
				printf("Nie udalo sie utworzyc watku. Blad: %d\n", retcode);
			}
		}
		printf("Klient rozlaczony\n");
		fclose(fdr);
	}

	remove(toServerPath);
	return EXIT_SUCCESS;
}
