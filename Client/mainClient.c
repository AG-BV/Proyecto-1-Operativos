/*
	C ECHO client example using sockets
*/
#include <stdio.h>       //printf
#include <string.h>      //strlen
#include <sys/socket.h>  //socket
#include <arpa/inet.h>   //inet_addr
#include <json-c/json.h> //JSON
#include <unistd.h>
#include <stdbool.h>
#define MAXCHAR 500


/////////////////////////////////////////////////////
// LEE EL ARCHIVO Y GUARDA LOS DATOS EN UN ARREGLO //
/////////////////////////////////////////////////////

int *readFile(char *pData)
{
    FILE *fp;
    char str[MAXCHAR];
    int save[4];
    int i = 0;
    bool flagT = false;

    fp = fopen(pData, "r");
    if (fp == NULL)
    {
        printf("\n ||==========================================||");
        printf("\n ||---NO--SE--PUDO--ABRIR--EL--ARCHIVO-%s", pData);
        printf("-----||");
        printf("\n ||==========================================||");
        return 1;
    }

    printf("\n ||==========================================||");
    printf("\n ||-------------ARCHIVO-CARGADO--------------||");
    printf("\n ||==========================================||");
    while (fgets(str, MAXCHAR, fp) != NULL)
    {
        printf("\n         %s", str);
        char *token = strtok(str, " ");
        // loop through the string to extract all other tokens

        while (token != NULL && flagT)
        {
            //printf(" %s", token); //printing each token
            int vOut = atoi(token);
            save[i] = vOut;
            token = strtok(NULL, " ");
            i++;
        }
        //printf("\n");
        flagT = true;
    }

    // int j;
    // /* output each array element's value */
    // for (j = 0; j < 4; j++)
    // {
    //     printf("\n Element[%d] = %d\n", j, save[j]);
    // }

    fclose(fp);
    return save;
}






void client()
{
    int sock;
    int flag = 0;
    int flagComunication = 0;
    struct sockaddr_in server;
    char message[1000], server_reply[2000];

    ////////////////////////////////
    //-------CREA EL SOCKET-------//
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
        return 1;
    }

    puts("Connected\n");

    //////////////////////////////////
    //keep communicating with server//
    //////////////////////////////////

    while (flagComunication == 0)
    {
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
        int n, opcion;
        char name[20];
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
                scanf("%s", &name);
                //OBTIENE LOS DATOS PARA CARGAR EN EL HILO
                readFile(name);
                break;

            
            ////////////////////////////////
            //           SALIDA           //
            ////////////////////////////////
            case 3:
                flagComunication++; //ROMPE AMBOS WHILES Y CIERRA EL SOCKET
                break;
            }
        } while (flagComunication == 0);

    }//FIN DE CLIENTE 
    close(sock);
}

int main(int argc, char *argv[])
{

    client();
    // int sock;
    // int flag = 0;
    // int flagComunication = 0;
    // struct sockaddr_in server;
    // char message[1000], server_reply[2000];

    // //Create socket
    // sock = socket(AF_INET, SOCK_STREAM, 0);
    // if (sock == -1)
    // {
    //     printf("Could not create socket");
    // }
    // puts("Socket created");

    // server.sin_addr.s_addr = inet_addr("127.0.0.1");
    // server.sin_family = AF_INET;
    // server.sin_port = htons(8888);

    // //Connect to remote server
    // if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    // {
    //     perror("connect failed. Error");
    //     return 1;
    // }

    // puts("Connected\n");

    // //keep communicating with server

    // while (flagComunication == 0)
    // {
    //     // CONSTRUCCION DEL MENU
    //     char titulo[] = "\n ||---------------MENU-CLIENTE---------------||";
    //     char tituloMM[] = "\n ||----------------MODO-MANUAL---------------||";
    //     char tituloMA[] = "\n ||-------------MODO-AUTOMATICO--------------||";
    //     char makeRandom[] = "\n ||--------GENERANDO-LOS-DATOS-RANDOMS-------||";
    //     char searchFile[] = "\n ||------INTRODUZCA-EL-NOMBRE-DEL-ARCHIVO----||";
    //     char MM[] = "\n || 1. MODO MANUAL                           ||";
    //     char MA[] = "\n || 2. MODO AUTOMATICO                       ||";
    //     char exit[] = "\n || 3. SALIR                                 ||";
    //     char separador[] = "\n ||==========================================||";
    //     char optionN[] = "\n ||------INTRODUZCA-UNA-OPCION-(1-3)---------||";
    //     int n, opcion;
    //     char name[20];
    //     do
    //     {
    //         //MENU PRINCIPAL
    //         printf(separador, sizeof(separador));
    //         printf(titulo, sizeof(titulo));
    //         printf(separador, sizeof(separador));
    //         printf(MM, sizeof(MM));
    //         printf(MA, sizeof(MA));
    //         printf(exit, sizeof(exit));
    //         printf(separador, sizeof(separador));
    //         printf(optionN, sizeof(optionN));
    //         printf(separador, sizeof(separador));
    //         printf("\n");
    //         scanf("%d", &opcion);

    //         switch (opcion)
    //         {

    //         //MENU DE MODO MANUAL
    //         case 1:
    //             printf(separador, sizeof(separador));
    //             printf(tituloMM, sizeof(tituloMM));
    //             printf(separador, sizeof(separador));
    //             printf(makeRandom, sizeof(makeRandom));
    //             printf(separador, sizeof(separador));
    //             scanf("%d", &n);
    //             break;
    //         //MENU DE MODO AUTOMATICO
    //         case 2:
    //             printf(separador, sizeof(separador));
    //             printf(tituloMA, sizeof(tituloMA));
    //             printf(separador, sizeof(separador));
    //             printf(searchFile, sizeof(searchFile));
    //             printf(separador, sizeof(separador));
    //             printf("\n");
    //             scanf("%s", &name);
    //             //OBTIENE LOS DATOS PARA CARGAR EN EL HILO
    //             readFile(name);
    //             break;

    //         case 3:
    //             flagComunication++; //ROMPE AMBOS WHILES Y CIERRA EL SOCKET
    //             break;
    //         }
    //     } while (flagComunication == 0);

    //     /* Fin del anidamiento */

    //     // if (flag == 1)
    //     // {
    //     //     printf("Enter message : ");
    //     //     scanf("%s", message);

    //     //     // //Send some data
    //     //     // if (send(sock, message, strlen(message), 0) < 0)
    //     //     // {
    //     //     //     puts("Send failed");
    //     //     //     return 1;
    //     //     // }

    //     //     //Receive a reply from the server
    //     //     if (recv(sock, server_reply, 2000, 0) < 0)
    //     //     {
    //     //         puts("recv failed");
    //     //         break;
    //     //     }

    //     //     puts("EL SERVER ENVIO :");
    //     //     puts(server_reply);
    //     //     memset(server_reply, 0, sizeof(server_reply));

    //     //     ///////////////////////////////////////
    //     //     ///////////////////////////////////////
    //     //     /*Creating a json object*/
    //     //     json_object *jobj = json_object_new_object();

    //     //     /*Creating a json string*/
    //     //     json_object *jstring = json_object_new_string("PONCHO");

    //     //     /*Creating a json integer*/
    //     //     json_object *jint = json_object_new_int(15);

    //     //     // /*Creating a json boolean*/
    //     //     // json_object *jboolean = json_object_new_boolean(1);

    //     //     // /*Creating a json double*/
    //     //     // json_object *jdouble = json_object_new_double(2.14);

    //     //     /*Creating a json array*/
    //     //     json_object *jarray = json_object_new_array();

    //     //     /*Creating json strings*/
    //     //     json_object *jstring1 = json_object_new_string("PAULA");
    //     //     json_object *jstring2 = json_object_new_string("PAULAA");
    //     //     json_object *jstring3 = json_object_new_string("Y PAULAAA");

    //     //     /*Adding the above created json strings to the array*/
    //     //     json_object_array_add(jarray, jstring1);
    //     //     json_object_array_add(jarray, jstring2);
    //     //     json_object_array_add(jarray, jstring3);

    //     //     /*Form the json object*/
    //     //     /*Each of these is like a key value pair*/
    //     //     json_object_object_add(jobj, "name", jstring);
    //     //     // json_object_object_add(jobj, "Technical blog", jboolean);
    //     //     // json_object_object_add(jobj, "Average posts per day", jdouble);
    //     //     json_object_object_add(jobj, "age", jint);
    //     //     json_object_object_add(jobj, "friends", jarray);

    //     //     printf("Size of JSON object- %lu\n", sizeof(jobj));
    //     //     printf("Size of JSON_TO_STRING- %lu,\n %s\n", sizeof(json_object_to_json_string(jobj)), json_object_to_json_string(jobj));
    //     //     char temp_buff[2000];

    //     //     if (strcpy(temp_buff, json_object_to_json_string(jobj)) == NULL)
    //     //     {
    //     //         perror("strcpy");
    //     //         return EXIT_FAILURE;
    //     //     }

    //     //     //ACA LO ENVIA
    //     //     //send(sock, message, strlen(message), 0) < 0
    //     //     if (send(sock, temp_buff, strlen(temp_buff),0) < 0 )
    //     //     {
    //     //         perror("demodemoserverAddrserverAddr");
    //     //         return EXIT_FAILURE;
    //     //     }
    //     //     /////////////////////////////////////////////
    //     //     /////////////////////////////////////////////
    //     //     /////////////////////////////////////////////
    //     // }
    //     // else
    //     // {
    //     //     // memset(message, 0, sizeof(message));

    //     //     // //Send some data
    //     //     // if (send(sock, message, strlen(message), 0) < 0)
    //     //     // {
    //     //     //     puts("Send failed");
    //     //     //     return 1;
    //     //     // }
    //     //     ///////////////////////////////////////
    //     //     ///////////////////////////////////////
    //     //     /*Creating a json object*/
    //     //     json_object *jobj = json_object_new_object();

    //     //     /*Creating a json string*/
    //     //     json_object *jstring = json_object_new_string("PONCHO");

    //     //     /*Creating a json integer*/
    //     //     json_object *jint = json_object_new_int(15);

    //     //     // /*Creating a json boolean*/
    //     //     // json_object *jboolean = json_object_new_boolean(1);

    //     //     // /*Creating a json double*/
    //     //     // json_object *jdouble = json_object_new_double(2.14);

    //     //     /*Creating a json array*/
    //     //     json_object *jarray = json_object_new_array();

    //     //     /*Creating json strings*/
    //     //     json_object *jstring1 = json_object_new_string("PAULA");
    //     //     json_object *jstring2 = json_object_new_string("PAULAA");
    //     //     json_object *jstring3 = json_object_new_string("Y PAULAAA");

    //     //     /*Adding the above created json strings to the array*/
    //     //     json_object_array_add(jarray, jstring1);
    //     //     json_object_array_add(jarray, jstring2);
    //     //     json_object_array_add(jarray, jstring3);

    //     //     /*Form the json object*/
    //     //     /*Each of these is like a key value pair*/
    //     //     json_object_object_add(jobj, "name", jstring);
    //     //     // json_object_object_add(jobj, "Technical blog", jboolean);
    //     //     // json_object_object_add(jobj, "Average posts per day", jdouble);
    //     //     json_object_object_add(jobj, "age", jint);
    //     //     json_object_object_add(jobj, "friends", jarray);

    //     //     printf("Size of JSON object- %lu\n", sizeof(jobj));
    //     //     printf("Size of JSON_TO_STRING- %lu,\n %s\n", sizeof(json_object_to_json_string(jobj)), json_object_to_json_string(jobj));
    //     //     char temp_buff[2000];

    //     //     if (strcpy(temp_buff, json_object_to_json_string(jobj)) == NULL)
    //     //     {
    //     //         perror("strcpy");
    //     //         return EXIT_FAILURE;
    //     //     }

    //     //     //ACA LO ENVIA
    //     //     //send(sock, message, strlen(message), 0) < 0
    //     //     if (send(sock, temp_buff, strlen(temp_buff),0) < 0 )
    //     //     {
    //     //         perror("demodemoserverAddrserverAddr");
    //     //         return EXIT_FAILURE;
    //     //     }
    //     //     /////////////////////////////////////////////
    //     //     /////////////////////////////////////////////
    //     //     /////////////////////////////////////////////

    //     //     //Receive a reply from the server
    //     //     if (recv(sock, server_reply, 2000, 0) < 0)
    //     //     {
    //     //         puts("recv failed");
    //     //         break;
    //     //     }

    //     //     puts("Server reply :");
    //     //     puts(server_reply);
    //     //     memset(server_reply, 0, sizeof(server_reply));
    //     //     flag++;
    //     // }
    // }

    // close(sock);
    return 0;
}