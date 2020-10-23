#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>       //printf
#include <string.h>      //strlen
#include <sys/socket.h>  //socket
#include <arpa/inet.h>   //inet_addr
#include <json-c/json.h> //JSON

/////////////////////////////////////////////////////
//                     STRUCS                      //
/////////////////////////////////////////////////////
struct my_Struct_Socket *globalSocket;
int idGlobalProcess;
int lenFile;
int errorName = 1;
/////////////////////////////////////////////////////
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
    char strucIP[] = "192.168.100.7";
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
void *sendData(void *received_struct)
{
    int dataBurst = ((struct my_Struct_Client *)received_struct)->burst;
    int dataPriority = ((struct my_Struct_Client *)received_struct)->priority;

    char temp_buff[2000];
    char server_reply[2000];
    char separador[] = "||==========================================|| \n";

    struct json_object *idP;
    struct json_object *idName;
    struct json_object *idN;
    json_object *jobj;
    int read_size;

    // COPIA DE SOCK PARA LA ESCUCHA DEL HILO//
    printf(separador, sizeof(separador));
    printf("|| HILO DE ENVIO,LOS DATOS DEL PROCESO SON: ||\n");
    printf(separador, sizeof(separador));
    printf("||              BURS : %d                    || \n", dataBurst);
    printf(separador, sizeof(separador));
    printf("||            PRIORITY : %d                  || \n", dataPriority);
    printf(separador, sizeof(separador));

    ///////////////////////////////////////
    // CREA EL JSON Y LO ENVIA AL SERVER //
    ///////////////////////////////////////
    jobj = makeJson(dataBurst, dataPriority);

    if (strcpy(temp_buff, json_object_to_json_string(jobj)) == NULL)
    {
        perror("strcpy");
    }

    //ACA LO ENVIA
    printf("|| ESPERA 2 SEGUNDOS ANTES HACER EL ENVIO : ||\n");
    sleep(2);
    if (send(globalSocket->Ssock, temp_buff, strlen(temp_buff), 0) < 0)
    {
        perror("demodemoserverAddrserverAddr");
    }

    //ESCUCHA EL SERVER
    while (read_size = recv(globalSocket->Ssock, server_reply, 2000, 0) > 0)
    {
        idP = json_tokener_parse(server_reply);
        json_object_object_get_ex(idP, "id", &idName);
        json_object_object_get_ex(idP, "idN", &idN);

        char *nameT = (char *)json_object_get_string(idName);
        int id = json_object_get_int(idN);
        printf("|| EL ID DEL PROCESO ES :        %d          || \n", id);

        if (id > 0)
        {
            idGlobalProcess++;
            break;
        }
    }
    printf(separador, sizeof(separador));
    printf("|| BORRANDO HILO DEL PROCESO %d DEL ARCHIVO  || \n", idGlobalProcess);
    printf(separador, sizeof(separador));
    pthread_cancel(pthread_self());
}

/////////////////////////////////////////////////////
//                  READ THREAD                    //
/////////////////////////////////////////////////////
void *readFile(void *received_struct)
{

    pthread_t id;
    FILE *fp;
    bool falgFirst = false;
    int MAXCHAR = 500;
    char str[MAXCHAR];
    static int save[2];
    int i = 0;
    int numSleep;
    bool flagT = false;
    char *token;
    int vOut;
    char *pData = (char *)((struct my_Struct_File *)received_struct)->tittle;
    struct my_Struct_Client *data;

    fp = fopen(pData, "r");
    if (fp == NULL)
    {
        printf("||==========================================||\n");
        printf("||---NO--SE--PUDO--ABRIR--EL--ARCHIVO-%s", pData);
        printf("-----||\n");
        printf("||==========================================||\n");
    }

    printf("||==========================================|| \n");
    printf("||       HILO DE LECTURA DE ARCHIVO         || \n");
    printf("||==========================================|| \n");

    while (fgets(str, MAXCHAR, fp) != NULL)
    {

        token = strtok(str, " ");
        lenFile++;

        while (token != NULL && flagT)
        {
            //printf("\n DATOS %s", token); //printing each token
            vOut = atoi(token);
            save[i] = vOut;
            token = strtok(NULL, " ");
            i++;

            if (i == 2)
            {
                i = 0;
                ///////////////////////////////////////////
                //  DATOS STRUC PARA EL HILO DE LECTURA //
                ///////////////////////////////////////////
                data = (struct my_Struct_Client *)malloc(sizeof(struct my_Struct_Client));
                data->burst = save[0];
                data->priority = save[1];

                ////////////////////////////////
                //     SLEEP BEFORE READ      //
                ////////////////////////////////
                if (falgFirst)
                {
                    numSleep = randomData(8, 3);
                    printf("||==========================================|| \n");
                    printf("|| ESPERA %d SEGUNDOS ANTES DE OTRO PROCESO  || \n", numSleep);
                    printf("||==========================================|| \n");
                    sleep(numSleep);
                }

                ////////////////////////////////
                //  LLAMADO A THREAD DE ENVIO //
                ////////////////////////////////
                pthread_create(&id, NULL, sendData, (void *)data);
                //pthread_join(id, NULL);
                falgFirst = true;
            }
        }
        flagT = true;
    }
    fclose(fp);

    printf("||==========================================|| \n");
    printf("||  BORRANDO HILO DE LECTURA DE ARCHIVO     || \n");
    printf("||==========================================|| \n");
    pthread_cancel(pthread_self());
}

/////////////////////////////////////////////////////
//                AUTOMATIC THREAD                  //
/////////////////////////////////////////////////////
void *automatic(void *received_struct)
{

    pthread_t id;
    int numSleep;
    struct my_Struct_Client *data;

    while (1)
    {
        ///////////////////////////////////////////
        //  DATOS STRUC PARA EL HILO DE LECTURA //
        ///////////////////////////////////////////
        data = (struct my_Struct_Client *)malloc(sizeof(struct my_Struct_Client));
        data->burst = randomData(5, 1);
        data->priority = randomData(5, 1);

        ////////////////////////////////
        //     SLEEP BEFORE READ      //
        ////////////////////////////////
        numSleep = randomData(8, 3);
        printf("||==========================================|| \n");
        printf("|| ESPERA %d SEGUNDOS ANTES DE OTRO PROCESO  || \n", numSleep);
        printf("||==========================================|| \n");
        sleep(numSleep);

        ////////////////////////////////
        //  LLAMADO A THREAD DE ENVIO //
        ////////////////////////////////
        pthread_create(&id, NULL, sendData, (void *)data);
        pthread_join(id, NULL);
    }
}

/////////////////////////////////////////////////////
//                     CLIENTE                     //
/////////////////////////////////////////////////////
void menuClient()
{
    pthread_t id;
    pthread_t AutoMode;
    int flagComunication = 0;
    struct my_Struct_File *data;
    int n, opcion;
    char name[20];

    ////////////////////////////////
    //   CONSTRUCCION DEL MENU    //
    ////////////////////////////////
    char titulo[] = "||---------------MENU-CLIENTE---------------||\n";
    char tituloMA[] = "||----------------MODO-MANUAL---------------||\n";
    char tituloMM[] = "||-------------MODO-AUTOMATICO--------------||\n";
    char makeRandom[] = "||--------GENERANDO-LOS-DATOS-RANDOMS-------||\n";
    char searchFile[] = "||------INTRODUZCA-EL-NOMBRE-DEL-ARCHIVO----||\n";
    char MM[] = "|| 1. MODO AUTOMATICO                       ||\n";
    char MA[] = "|| 2. MODO MANUAL                           ||\n";
    char exit[] = "|| 3. SALIR                                 ||\n";
    char separador[] = "||==========================================||\n";
    char optionN[] = "||------INTRODUZCA-UNA-OPCION-(1-3)---------||\n";

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
            ////////////////////////////////
            //       READ  THREAD         //
            ////////////////////////////////
            pthread_create(&AutoMode, NULL, automatic, NULL);
            pthread_join(AutoMode, NULL);
            break;

        //////////////////////////////////
        //    MENU DE MODO AUTOMATICO  //
        ////////////////////////////////
        case 2:

            do
            {
                printf(separador, sizeof(separador));
                printf(tituloMA, sizeof(tituloMA));
                printf(separador, sizeof(separador));
                printf(searchFile, sizeof(searchFile));
                printf(separador, sizeof(separador));
                printf("\n");
                idGlobalProcess = 0;
                lenFile = 0;
                scanf("%s", name);

                ///////////////////////////////////////////
                //  DATOS STRUC PARA EL HILO DE LECTURA //
                ///////////////////////////////////////////
                data = (struct my_Struct_File *)malloc(sizeof(struct my_Struct_File));
                if (isalpha(*name))
                {
                    data->tittle = name;
                    errorName = 0;
                }
                else
                {
                    printf("||==========================================|| \n");
                    printf("||    ERRO EN LA SELECCION DEL ARCHIVO      || \n");
                    printf("||==========================================|| \n");
                }
            } while (errorName);
            errorName = 1;

            ////////////////////////////////
            //       READ  THREAD         //
            ////////////////////////////////
            pthread_create(&id, NULL, readFile, (void *)data);
            pthread_join(id, NULL);
            while (1)
            {
                if (idGlobalProcess == lenFile - 1)
                {
                    break;
                }
            }
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
