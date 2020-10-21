#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>       //printf
#include <string.h>      //strlen
#include <sys/socket.h>  //socket
#include <arpa/inet.h>   //inet_addr
#include <json-c/json.h> //JSON

static int iThread;

/////////////////////////////////////////////////////
//                     THREAD                      //
/////////////////////////////////////////////////////
void *foo(void *p)
{
    int *my_data = p; /* data received by thread */
    printf("\n");
    printf("\n");
    printf("   SOY EL HILO LOS DATOS DEL PROCESO SON : \n");
    printf("   EL BURST DEL PROCESO ES DE : %d\n", my_data[iThread]);
    printf("   LA PRIOIRDAD DEL PROCESO ES DE : %d\n", my_data[iThread + 1]);
    printf("   ESPERAMOS 2 SEGUNDOS \n");
    // Return reference to global variable:
    pthread_exit(my_data);
}

/////////////////////////////////////////////////////
// LEE EL ARCHIVO Y GUARDA LOS DATOS EN UN ARREGLO //
/////////////////////////////////////////////////////
int *readFile(char *pData)
{
    FILE *fp;
    int max = 500;
    char str[max];
    static int save[4];
    int i = 0;
    bool flagT = false;

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
    while (fgets(str,max, fp) != NULL)
    {
        //printf("\n         %s", str);
        char *token = strtok(str, " ");

        while (token != NULL && flagT)
        {
            int vOut = atoi(token);
            save[i] = vOut;
            token = strtok(NULL, " ");
            i++;
        }
        flagT = true;
    }
    fclose(fp);
    return save;
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
//                     CLIENTE                     //
/////////////////////////////////////////////////////
void  client()
{

    ////////////////////////////////
    //         VARIABLES          //
    ////////////////////////////////

    pthread_t id;
    json_object *jobj;
    struct sockaddr_in server;

    int n, opcion, sock;
    int flag = 0;
    int flagComunication = 0;

    int *ptr, *saveData;
    char temp_buff[2000], server_reply[2000], name[20];

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

    ////////////////////////////////
    //       CREA EL SOCKET       //
    ////////////////////////////////
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(8888);

    ////////////////////////////////
    //--Connect to remote server--//
    ////////////////////////////////
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("connect failed. Error");
    }

    puts("Connected\n");

    //////////////////////////////////
    //keep communicating with server//
    //////////////////////////////////

    while (flagComunication == 0)
    {
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

            ////////////////////////////////////////
            //   VALIDA QUE EL DATO SEA UN INT   //
            ///////////////////////////////////////
            if (scanf("%d", &opcion) == 0)
            {
                // Eliminamos todos los caracteres hasta el primer espacio o salto de linea
                for (int c = getchar(); c != EOF && c != ' ' && c != '\n'; c = getchar())
                    ;
                printf(separador, sizeof(separador));
                printf("\n");
                puts("\n ||---------ERROR-DIGITE-UN-NUMERO-----------||");
                printf(separador, sizeof(separador));
                printf("\n");
            }

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

                ////////////////////////////////////////////////
                //  OBTIENE LOS DATOS PARA CARGAR EN EL HILO //
                ///////////////////////////////////////////////
                saveData = readFile(name); // ocupa un try catch
                size_t len = sizeof(saveData) / sizeof(saveData[0]);

                ////////////////////////////////////////
                //          CICLO DE PROCESOS         //
                ////////////////////////////////////////
                for (iThread = 0; iThread < len + 1; iThread += 2)
                {

                    ////////////////////////////////////////
                    //     ENVIADO LOS DATOS AL HILO      //
                    ////////////////////////////////////////
                    pthread_create(&id, NULL, foo, (void *)saveData);
                    sleep(2);

                    ////////////////////////////////////////
                    //     RESIVIENDO LOS DATOS DEL HILO  //
                    ///////////////////////////////////////
                    pthread_join(id, (void **)&ptr);
                    printf(separador, sizeof(separador));
                    printf("\n");
                    printf("\n    ENVIANDO LOS VALORES AL SERVER: ");
                    int *answer_beeing_an_int_arr = ptr;
                    int dataBurst = ((int *)ptr)[iThread];
                    int dataPriority = ((int *)ptr)[iThread + 1];
                    printf("\n    BURST :  %d", dataBurst);
                    printf("\n    PRIORITY :  %d", dataPriority);
                    printf("\n");
                    printf(separador, sizeof(separador));
                    printf("\n");

                    ///////////////////////////////////////
                    // CREA EL JSON Y LO ENVIA AL SERVER //
                    ///////////////////////////////////////
                    jobj = makeJson(dataBurst, dataPriority);

                    if (strcpy(temp_buff, json_object_to_json_string(jobj)) == NULL)
                    {
                        perror("strcpy");
                       
                    }

                    ///////////////////////////////////////
                    // ACA SE ENVIAN LOS DATOS AL SERVER //
                    ///////////////////////////////////////
                    //send(sock, message, strlen(message), 0) < 0//

                    if (send(sock, temp_buff, strlen(temp_buff), 0) < 0)
                    {
                        perror("demodemoserverAddrserverAddr");
                       
                    }

                    ///////////////////////////////////////
                    //   LEE LA RESPUESTA DEL  SERVER    //
                    ///////////////////////////////////////
                    if (recv(sock, server_reply, 2000, 0) < 0)
                    {
                        puts("recv failed");
                        break;
                    }
                    else
                    {
                        ///////////////////////////////////////
                        //          LEE EL ID ASIGNADO       //
                        ///////////////////////////////////////
                        puts("EL SERVER ENVIO :");
                        puts(server_reply);
                        memset(server_reply, 0, sizeof(server_reply));

                        ///////////////////////////////////////
                        //          ELIMINA EL HILO          //
                        ///////////////////////////////////////
                        //int pthread_cancel(pthread_t id);
                        void pthread_exit(void *id);
                    }
                }
                break;

            ////////////////////////////////
            //           SALIDA           //
            ////////////////////////////////
            case 3:
                flagComunication++; //ROMPE AMBOS WHILES Y CIERRA EL SOCKET
                break;
            }
        } while (flagComunication == 0);
    }
    ////////////////////////////////
    //      FIN DEL CLIENTE       //
    ////////////////////////////////
    close(sock);
}

int main(int argc, char *argv[])
{
    client();
    return 0;
}