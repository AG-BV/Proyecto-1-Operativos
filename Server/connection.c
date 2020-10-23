#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <json-c/json.h>

pthread_t jobScheduler;
int GlobalID = 0;
struct node *headTaskList = NULL;
// struct node *headThreads = NULL;


struct arguments
{
    int id;
    char *name;
    int priority;
    int burst;
};

struct node
{
    int id;
    int burst;
    int priority;
    struct node *next;
};

int insert(int pID, int pPrority, int pBurst)
{
    //create a link
    struct node *link = (struct node *)malloc(sizeof(struct node));
    struct node *current = (struct node *)malloc(sizeof(struct node));

    //link->key = key;
    link->id = pID;
    link->priority = pPrority;
    link->burst = pBurst;

    //point it to old first node
    if (headTaskList == NULL)
    {
        headTaskList = link;
        return 1;
    }
    current = headTaskList;
    while (current->next != NULL)
    {
        current = current->next;
    }
    current->next = link;
    link = NULL;
    current = NULL;
    free(link);
    free(current);
    return 1;
}

void printJobTaskList()
{
    struct node *current = (struct node *)malloc(sizeof(struct node));
    current = headTaskList;
    if (current == NULL)
    {
        printf("||Cola de tareas: ");
        printf("Lista Vacia");
    }
    while (current != NULL)
    {
        printf("Burst: %d", current->burst);
        printf("->");
        current = current->next;
    }
    printf("\n");
}

struct node *getFirstRM()
{
    struct node *current = (struct node *)malloc(sizeof(struct node));
    current = headTaskList;
    headTaskList = headTaskList->next;
    return current;
}


/////////////////////////////////////////////////////
//                 CREA UN JSON                    //
/////////////////////////////////////////////////////

void connectionPrintJobTaskList()
{
    printJobTaskList(headTaskList);
}

json_object *makeJson(int pData1)
{
    /*Creating a json object*/
    json_object *jobj = json_object_new_object();
    /*Creating a json string*/
    json_object *jstring = json_object_new_string("ID");
    /*Creating a json integer*/
    json_object *jint1 = json_object_new_int(pData1);
    json_object_object_add(jobj, "id", jstring);
    json_object_object_add(jobj, "idN", jint1);
    return jobj;
}


void *jobSchedulerTask(void *pArgs)
{
    struct arguments *args = (struct arguments *)pArgs;
    insert(args->id, args->priority, args->burst);
}

void *connection_handler(void *socket_desc)
{
    struct json_object *parsed_json;
    struct json_object *name;
    struct json_object *burst;
    struct json_object *priority;

    json_object *jobj;
    char temp_buff[200];

    size_t n_friends;
    char *nameT;
    int burstT;
    int priorityT;

    size_t i;

    int sock = *(int *)socket_desc;
    int read_size;
    char *message;
    char client_message[2000];
    char msg;

    while ((read_size = recv(sock, client_message, 2000, 0)) > 0)
    {
        parsed_json = json_tokener_parse(client_message);
        json_object_object_get_ex(parsed_json, "name", &name);
        json_object_object_get_ex(parsed_json, "burst", &burst);
        json_object_object_get_ex(parsed_json, "priority", &priority);

        nameT = (char *)json_object_get_string(name);
        burstT = json_object_get_int(burst);
        priorityT = json_object_get_int(priority);

        // printf("Name: %c\n", *nameT);
        // printf("BURST: %d\n", burstT);
        // printf("PRIORITY: %d\n", priorityT);

        struct arguments *args = (struct arguments *)malloc(sizeof(struct arguments));
        GlobalID = GlobalID + 1;
        args->id = GlobalID;
        args->name = nameT;
        args->priority = priorityT;
        args->burst = burstT;

        // puts("EL CLIENTE ENVIO :");
        // puts(client_message);

        pthread_create(&jobScheduler, NULL, &jobSchedulerTask, (void *)args);
        jobj = makeJson(args->id);

        if (strcpy(temp_buff, json_object_to_json_string(jobj)) == NULL)
        {
            perror("strcpy");
        }

        //ACA LO ENVIA
        //send(sock, message, strlen(message), 0) < 0
        // printf("|| ESPERA 2 SEGUNDOS ANTES HACER EL ENVIO : ||\n");
        sleep(2);
        if (send(sock, temp_buff, strlen(temp_buff), 0) < 0)
        {
            perror("demodemoserverAddrserverAddr");
        }
    }

    if (read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if (read_size == -1)
    {
        perror("recv failed");
    }

    //Free the socket pointer
    free(socket_desc);

    return 0;
}

int connection()
{
    int socket_desc, client_sock, c, *new_sock;
    struct sockaddr_in server, client;

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    // puts("Socket created");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("bind failed. Error");
        return 1;
    }
    // puts("bind done");

    listen(socket_desc, 3);

    // puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    while ((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c)))
    {
        // puts("Connection accepted");

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;

        if (pthread_create(&sniffer_thread, NULL, connection_handler, (void *)new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }

        // puts("Handler assigned");
    }

    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
}