#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

const int STR_LEN = 20;
const char* toServerPath = "/tmp/toServer";
char toClientPath[100];

main()
{
    char input[STR_LEN];
    FILE *fdw;
    FILE *fdr;

    // utworzenie potoku do zapisu
    if (mkfifo(toServerPath, 0777) == -1 && errno != EEXIST)
    {
        // wyswietlenie bledu w przypadku niepowodzenia
        perror("Nie udalo sie utworzyc pliku fifo");
        exit(EXIT_FAILURE);
    }
    // otwarcie potoku do zapisu
    if (!(fdw = fopen(toServerPath, "w")))
    {
        // wyswietlenie bledu w przypadku niepowodzenia
        perror("Nie udalo sie utworzyc pliku do zapisu");
        exit(EXIT_FAILURE);
    }

    puts("Polaczono z Serwerem.\nPodaj liczbe punktow do wygenerowania: ");
    if(fgets(input, STR_LEN, stdin) != NULL)
    {
        input[strlen(input)-1] = '\0';
        sprintf(toClientPath, "%s", "/tmp/toClient");
        strcat(toClientPath, input);
        // utworzenie potoku do odczytu
        if (mknod(toClientPath, 0777, 0) == -1 && errno != EEXIST)
        {
            // wyswietlenie bledu w przypadku niepowodzenia
            perror("Nie udalo sie utworzyc pliku fifo");
            exit(EXIT_FAILURE);
        }

        printf("Wyslano zapytanie: N = %s\n", input);
        // wyslanie zapytania
        fprintf(fdw, " %s\n", input);
        fflush(fdw);
        // otwarcie potoku do odczytu
        sleep(1);
        if(!(fdr = fopen(toClientPath, "r")))
        {
            // wyswietlenie bledu w przypadku niepowodzenia
            printf("Nie udalo sie otworzyc pliku do odczytu\n");
            exit(EXIT_FAILURE);
        }
        // pobranie odpowiedzi od serwera
        while (fscanf(fdr, " %s", input) != EOF)
        {
            printf("Odpowiedz Serwera: %s\n", input);
            break;        
        }
    }
    fclose(fdw);
    fclose(fdr);
    remove(toClientPath);
    return EXIT_SUCCESS;
}
