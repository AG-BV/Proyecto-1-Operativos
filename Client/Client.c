/*
	C ECHO client example using sockets
*/
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>       //printf
#include <string.h>      //strlen
#include <sys/socket.h>  //socket
#include <arpa/inet.h>   //inet_addr
#include <json-c/json.h> //JSON
#define MAXCHAR 500
static int iThread;

/////////////////////////////////////////////////////
//                     STRUCS                      //
/////////////////////////////////////////////////////
struct my_Struct_Socket *globalSocket;

struct my_Struct_File
{
    char *tittle;
};

struct my_Struct_Client
{
    int burst;
    int priority;
};

struct my_Struct_Socket
{
    struct sockaddr_in Sserver;
    char *SstrucIP;
    int SstrucSocket;
    int Ssock;
};

/////////////////////////////////////////////////////
//                     RANDOM                      //
/////////////////////////////////////////////////////
int randomData(int pUpper, int pLower)
{

    int num = (rand() % (pUpper - pLower + 1)) + pLower;
    printf("hola %d", num);
    return num;
}

/////////////////////////////////////////////////////
//                 CREA UN JSON                    //
/////////////////////////////////////////////////////
json_object *makeJson(int pData1, int pData2)
{
    /*Creating a json object*/
    json_object *jobj = json_object_new_object();
    /*Creating a json string*/
    json_object *jstring = json_object_new_string("PROCESO");
    /*Creating a json integer*/
    json_object *jint1 = json_object_new_int(pData1);
    json_object *jint2 = json_object_new_int(pData2);
    json_object_object_add(jobj, "name", jstring);
    json_object_object_add(jobj, "burst", jint1);
    json_object_object_add(jobj, "priority", jint2);
    return jobj;
}

/////////////////////////////////////////////////////
//                 SOCKET CLIENTE                  //
/////////////////////////////////////////////////////
void socketClient()
{
    struct sockaddr_in server;
    char strucIP[] = "127.0.0.1";
    int strucSocket = 8888;
    int sock;

    ////////////////////////////////
    //       CREA EL SOCKET       //
    ////////////////////////////////
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr(strucIP);
    server.sin_family = AF_INET;
    server.sin_port = htons(strucSocket);

    ////////////////////////////////
    //     CONECTA AL SERVER      //
    ////////////////////////////////
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("connect failed. Error");
    }
    puts("Connected\n");

    ////////////////////////////////
    //       SAVE SOCKED          //
    ////////////////////////////////
    globalSocket = (struct my_Struct_Socket *)malloc(sizeof(struct my_Struct_Socket));
    globalSocket->Sserver = server;
    globalSocket->SstrucIP = strucIP;
    globalSocket->SstrucSocket = strucSocket;
    globalSocket->Ssock = sock;
}


/////////////////////////////////////////////////////
//                SEND   THREAD                    //
/////////////////////////////////////////////////////
void *foo(void *received_struct)
{
    int dataBurst = ((struct my_Struct_Client *)received_struct)->burst;
    int dataPriority = ((struct my_Struct_Client *)received_struct)->burst;

    char temp_buff[2000];
    char server_reply[50];

    printf("\n | BURS : %d", ((struct my_Struct_Client *)received_struct)->burst);
    printf("\n| PRIORITY : %d", ((struct my_Struct_Client *)received_struct)->priority);

    ///////////////////////////////////////
    // CREA EL JSON Y LO ENVIA AL SERVER //
    ///////////////////////////////////////

    json_object *jobj = makeJson( dataBurst , dataPriority);

    if (strcpy(temp_buff, json_object_to_json_string(jobj)) == NULL)
    {
        perror("strcpy");
    }

    //ACA LO ENVIA
    //send(sock, message, strlen(message), 0) < 0
    if (send(globalSocket->Ssock, temp_buff, strlen(temp_buff), 0) < 0)
    {
        perror("demodemoserverAddrserverAddr");
    }
}

/////////////////////////////////////////////////////
//                  READ THREAD                    //
/////////////////////////////////////////////////////
void *readFile(void *received_struct)
{

    pthread_t id;
    FILE *fp;
    char str[MAXCHAR];
    static int save[2];
    int i = 0;
    int numSleep;
    bool flagT = false;

    char *pData = (char *)((struct my_Struct_File *)received_struct)->tittle;
    fp = fopen(pData, "r");
    if (fp == NULL)
    {
        printf("\n ||==========================================||");
        printf("\n ||---NO--SE--PUDO--ABRIR--EL--ARCHIVO-%s", pData);
        printf("-----||");
        printf("\n ||==========================================||");
    }

    printf("\n ||==========================================||");
    printf("\n ||-------------ARCHIVO-CARGADO--------------||");
    printf("\n ||==========================================||");
    while (fgets(str, MAXCHAR, fp) != NULL)
    {

        char *token = strtok(str, " ");

        while (token != NULL && flagT)
        {
            printf("\n DATOS %s", token); //printing each token
            int vOut = atoi(token);
            save[i] = vOut;
            token = strtok(NULL, " ");
            i++;

            if (i == 2)
            {
                i = 0;
                ///////////////////////////////////////////
                //  DATOS STRUC PARA EL HILO DE LECTURA //
                ///////////////////////////////////////////
                struct my_Struct_Client *data = (struct my_Struct_Client *)malloc(sizeof(struct my_Struct_Client));
                data->burst = save[0];
                data->priority = save[1];

                ////////////////////////////////
                //  LLAMADO A THREAD DE ENVIO //
                ////////////////////////////////
                pthread_create(&id, NULL, foo, (void *)data);
                //pthread_join(id, NULL);

                ////////////////////////////////
                //     SLEEP BEFORE READ      //
                ////////////////////////////////
                numSleep = randomData(8, 3);
                printf(" ESPERA %d  SEGUNDOS ANTES DE LEER \n", numSleep);
                sleep(numSleep);
            }
        }
        flagT = true;
    }
    fclose(fp);
}


/////////////////////////////////////////////////////
//                     CLIENTE                     //
/////////////////////////////////////////////////////
void menuClient()
{
    pthread_t id;
    int flagComunication = 0;
    struct my_Struct_File *data;
    int n, opcion;
    char name[20];

    ////////////////////////////////
    //   CONSTRUCCION DEL MENU    //
    ////////////////////////////////
    char titulo[] = "\n ||---------------MENU-CLIENTE---------------||";
    char tituloMM[] = "\n ||----------------MODO-MANUAL---------------||";
    char tituloMA[] = "\n ||-------------MODO-AUTOMATICO--------------||";
    char makeRandom[] = "\n ||--------GENERANDO-LOS-DATOS-RANDOMS-------||";
    char searchFile[] = "\n ||------INTRODUZCA-EL-NOMBRE-DEL-ARCHIVO----||";
    char MM[] = "\n || 1. MODO MANUAL                           ||";
    char MA[] = "\n || 2. MODO AUTOMATICO                       ||";
    char exit[] = "\n || 3. SALIR                                 ||";
    char separador[] = "\n ||==========================================||";
    char optionN[] = "\n ||------INTRODUZCA-UNA-OPCION-(1-3)---------||";

    do
    {
        ////////////////////////////////
        //       MENU PRINCIPAL       //
        ////////////////////////////////
        printf(separador, sizeof(separador));
        printf(titulo, sizeof(titulo));
        printf(separador, sizeof(separador));
        printf(MM, sizeof(MM));
        printf(MA, sizeof(MA));
        printf(exit, sizeof(exit));
        printf(separador, sizeof(separador));
        printf(optionN, sizeof(optionN));
        printf(separador, sizeof(separador));
        printf("\n");
        scanf("%d", &opcion);

        ////////////////////////////////
        ///          SWITCH           //
        ////////////////////////////////
        switch (opcion)
        {

        ////////////////////////////////
        //    MENU DE MODO MANUAL     //
        ////////////////////////////////
        case 1:
            printf(separador, sizeof(separador));
            printf(tituloMM, sizeof(tituloMM));
            printf(separador, sizeof(separador));
            printf(makeRandom, sizeof(makeRandom));
            printf(separador, sizeof(separador));
            scanf("%d", &n);
            break;

        //////////////////////////////////
        //    MENU DE MODO AUTOMATICO  //
        ////////////////////////////////
        case 2:
            printf(separador, sizeof(separador));
            printf(tituloMA, sizeof(tituloMA));
            printf(separador, sizeof(separador));
            printf(searchFile, sizeof(searchFile));
            printf(separador, sizeof(separador));
            printf("\n");
            scanf("%s", name);

            ///////////////////////////////////////////
            //  DATOS STRUC PARA EL HILO DE LECTURA //
            ///////////////////////////////////////////
            data = (struct my_Struct_File *)malloc(sizeof(struct my_Struct_File));
            data->tittle = name;

            ////////////////////////////////
            //       READ  THREAD         //
            ////////////////////////////////
            pthread_create(&id, NULL, readFile, (void *)data);
            pthread_join(id, NULL);
            break;

        ////////////////////////////////
        //           SALIDA           //
        ////////////////////////////////
        case 3:
            flagComunication++; //ROMPE AMBOS WHILES Y CIERRA EL SOCKET
            close(globalSocket->Ssock);
            break;
        }
    } while (flagComunication == 0);
}

int main(int argc, char *argv[])
{
    socketClient();
    menuClient();
    return 0;
}


