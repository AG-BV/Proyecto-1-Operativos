/*
	C socket server example, handles multiple clients using threads
*/
#include <stdio.h>
#include "connection.c"
#define clrscr() printf("\e[1;1H\e[2J")

pthread_t menuThread;

void *menuServer(void *unused)
{
    int opcion;
    int wState = 1;
    clrscr();
    while (wState==1)
    {
        printf("||==========================================||\n");
        printf("||----------------MENU-SERVER---------------||\n");
        printf("||==========================================||\n");
        printf("||-------------SELECCIONE-OPCION------------||\n");
        printf("|| 1. VER JOBTASKLIST                       ||\n");
        printf("|| 2. DETENER SIMULACION                    ||\n");
        printf("|| 3. SALIR                                 ||\n");
        printf("|| Quantum: %d                               ||\n", getQuantum());
        printf("||==========================================||\n");
        printf("||--------INTRODUZCA-UNA-OPCION-(1-3)-------||\n");
        printf("||Opcion: ");
        scanf("%d", &opcion);

        switch (opcion)
        {
        case 1:
            /* code */
            // clrscr();
            connectionPrintJobTaskList();
            break;

        case 2:
            /* code */
            // clrscr();
            connectionPrintJobTaskListF();
            break;

        case 3:
            /* code */
            wState = 0;
            break;

        default:
            break;
        }
    }
}

int main()
{
    int opcion;
    printf("||==========================================||\n");
    printf("||----------------MENU-SERVER---------------||\n");
    printf("||==========================================||\n");
    printf("||-----------SELECCIONE-ALGORITMO-----------||\n");
    printf("|| 1. FIFO                                  ||\n");
    printf("|| 2. SJF                                   ||\n");
    printf("|| 3. HPF                                   ||\n");
    printf("|| 4. ROUND ROBIN                           ||\n");
    printf("|| 5. SALIR                                 ||\n");
    printf("||==========================================||\n");
    printf("||--------INTRODUZCA-UNA-OPCION-(1-4)-------||\n");
    printf("||Opcion: ");
    scanf("%d", &opcion);

    switch (opcion)
    {
    case 1:
        /* code */
        pthread_create(&menuThread, NULL, &menuServer, NULL);
        connection(1);
        break;

    case 2:
        /* code */
        pthread_create(&menuThread, NULL, &menuServer, NULL);
        connection(2);
        break;

    case 3:
        /* code */
        pthread_create(&menuThread, NULL, &menuServer, NULL);
        connection(3);
        break;

    case 4:
        /* code */
        pthread_create(&menuThread, NULL, &menuServer, NULL);
        connection(4);
        break;

    case 5:
        /* code */
        break;
    }

    return 0;
}
