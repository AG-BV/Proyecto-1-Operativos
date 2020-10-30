#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <json-c/json.h>
#include <ctype.h>
#define clrscr() printf("\e[1;1H\e[2J")

// pthread_t jobScheduler;
pthread_t CPUScheduler;
int GlobalID = 0;
int countBurst = 0;
int timeSchedule = 0;
int optionG = 0;
int globalProcess = 0;
int timeInjm;
int timellegada;
struct node *headTaskList = NULL;
struct node *headFinishList = NULL;
struct node *RRpointer = NULL;

int quantum;

/////////////////////////////////////////////////////
//                     RANDOM                      //
/////////////////////////////////////////////////////
void randomData(int pUpper, int pLower)
{
    quantum = (rand() % (pUpper - pLower + 1)) + pLower;
}

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
    int TimeInAux;
    int TimeIn;
    int TimeOut;
    int TAT;
    int wt;
    int burstBK;
    struct node *next;
};

int insert(int pID, int pPrority, int pBurst)
{
    //create a link
    struct node *link = (struct node *)malloc(sizeof(struct node));
    struct node *current = (struct node *)malloc(sizeof(struct node));

    //link->key = key;
    link->id = pID;
    link->wt = 0;
    link->priority = pPrority;
    link->burst = pBurst;
    link->burstBK = pBurst;
    link->TimeIn = timeInjm;
    link->TimeInAux = timeInjm;

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

int insertF(struct node *pCurrent)
{
    //create a link
    struct node *current = (struct node *)malloc(sizeof(struct node));

    //point it to old first node
    if (headFinishList == NULL)
    {
        headFinishList = pCurrent;
        return 1;
    }
    current = headFinishList;
    while (current->next != NULL)
    {
        current = current->next;
    }
    current->next = pCurrent;
    current = NULL;
    free(current);
    return 1;
}

/////////////////////////////////////////////////////
//               CLEAN RR LIST                     //
/////////////////////////////////////////////////////
void clean()
{
    struct node *currente = (struct node *)malloc(sizeof(struct node));
    currente = headTaskList;
    if (currente == RRpointer)
    {
        /* code */
        headTaskList = headTaskList->next;
        currente->next = NULL;
        insertF(currente);
        currente = NULL;
        free(currente);
        RRpointer = headTaskList;
    }
    else
    {
        struct node *auxBack = (struct node *)malloc(sizeof(struct node));
        auxBack = headTaskList;
        while (currente != NULL)
        {
            while (currente->next != RRpointer)
            {
                /* code */
                currente = currente->next;
            }
            currente->next = RRpointer->next;
            auxBack = RRpointer;
            RRpointer = RRpointer->next;
            auxBack->next = NULL;
            insertF(auxBack);
            currente = NULL;
            free(currente);
            auxBack = NULL;
            free(auxBack);
        }
    }
}

/////////////////////////////////////////////////////
//                     GET MIN                     //
/////////////////////////////////////////////////////
void *getMinBurst()
{
    struct node *currente = (struct node *)malloc(sizeof(struct node));
    struct node *min = (struct node *)malloc(sizeof(struct node));
    struct node *auxBack = (struct node *)malloc(sizeof(struct node));
    currente = headTaskList;
    min = currente;

    while (currente->next != NULL)
    {
        if (currente->next->burst < min->burst)
        {
            min = currente->next;
            auxBack = currente;
            currente = currente->next;
        }
        else
        {
            currente = currente->next;
        }
    }
    if (min != headTaskList)
    {
        auxBack->next = min->next;
        min->next = headTaskList;
        headTaskList = min;
    }
}

void *getMinPriority()
{
    struct node *currente = (struct node *)malloc(sizeof(struct node));
    struct node *min = (struct node *)malloc(sizeof(struct node));
    struct node *auxBack = (struct node *)malloc(sizeof(struct node));
    currente = headTaskList;
    min = currente;

    while (currente->next != NULL)
    {
        if (currente->next->priority < min->priority)
        {
            min = currente->next;
            auxBack = currente;
            currente = currente->next;
        }
        else
        {
            currente = currente->next;
        }
    }
    if (min != headTaskList)
    {
        auxBack->next = min->next;
        min->next = headTaskList;
        headTaskList = min;
    }
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
    else
    {
        printf("\n||Cola de tareas: \n");
        while (current != NULL)
        {
            printf("||ID: %d \n", current->id);
            printf("||Burst: %d \n", current->burst);
            printf("||WT: %d \n", current->wt);
            printf("||->Next: \n");
            current = current->next;
        }
    }
}

void printJobTaskListF()
{
    struct node *current = (struct node *)malloc(sizeof(struct node));
    current = headFinishList;
    if (current == NULL)
    {
        printf("||Tareas Listas: ");
        printf("Lista Vacia");
    }
    else
    {
        printf("||Tareas Listas: \n");
        while (current != NULL)
        {
            printf("\n||==========================================|| \n");
            printf("|| TIME IN : %d                             \n", current->TimeIn);
            printf("|| TIME OUT : %d                            \n", current->TimeOut);
            printf("|| ID: %d                                   \n", current->id);
            printf("|| BURST: %d                                \n", current->burstBK);
            printf("|| PRIORITY: %d                             \n", current->priority);
            printf("|| TAT: %d                                  \n", current->TAT);
            printf("|| WT : %d                                  \n", current->wt);
            printf("||==========================================|| \n");
            printf("||                  NEXT                    ||  \n");
            current = current->next;
        }
    }
}

void printJobTaskListR()
{
    struct node *current = (struct node *)malloc(sizeof(struct node));
    current = headFinishList;
    if (current == NULL)
    {
        printf("||Tareas Listas: ");
        printf("Lista Vacia");
    }
    else
    {
        printf("||Tareas Listas: \n");
        while (current != NULL)
        {
            printf("\n||==========================================|| \n");
            printf("|| TIME IN ORIGINAL: %d                             \n", current->TimeInAux);
            printf("|| TIME IN : %d                             \n", current->TimeIn);
            printf("|| TIME OUT : %d                            \n", current->TimeOut);
            printf("|| ID: %d                                   \n", current->id);
            printf("|| BURST: %d                                \n", current->burstBK);
            printf("|| PRIORITY: %d                             \n", current->priority);
            printf("|| TAT: %d                                  \n", current->TAT);
            printf("|| WT : %d                                  \n", current->wt);
            printf("||==========================================|| \n");
            printf("||                  NEXT                    ||  \n");
            current = current->next;
        }
    }
}

struct node *getFirstRM()
{
    struct node *current = (struct node *)malloc(sizeof(struct node));
    current = headTaskList;
    headTaskList = headTaskList->next;
    current->next = NULL;
    return current;
}

void connectionPrintJobTaskList()
{
    printJobTaskList();
}

void finalprint()
{
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    int play = 0;

    pthread_cancel(CPUScheduler);
    pthread_cancel(jobScheduler);
    int cantidad = 0;
    float promWT = 0;
    float promTAT = 0;
    struct node *current = (struct node *)malloc(sizeof(struct node));
    current = headFinishList;
    clrscr();
    if (current == NULL)
    {
        /* code */
        printf("Lista vacia");
    }
    while (current != NULL)
    {
        /* code */
        cantidad = cantidad + 1;
        promTAT = promTAT + current->TAT;
        promWT = promWT + current->wt;
        printf("\n||==========================================|| \n");
        printf("|| TIME IN : %d                             \n", current->TimeIn);
        printf("|| TIME OUT : %d                            \n", current->TimeOut);
        printf("|| ID: %d                                   \n", current->id);
        printf("|| BURST: %d                                \n", current->burstBK);
        printf("|| PRIORITY: %d                             \n", current->priority);
        printf("|| TAT: %d                                  \n", current->TAT);
        printf("|| WT : %d                                  \n", current->wt);
        printf("||==========================================|| \n");
        printf("||                  NEXT                    ||  \n");
        current = current->next;
    }

    promTAT = promTAT / cantidad;
    promWT = promWT / cantidad;

    printf("||==========================================|| \n");
    printf("||==============Datos generales=============|| \n");
    printf("||Promedio cantidad total de procesos: %d      \n", cantidad);
    printf("||Promedio del Waiting time: %f                \n", promWT);
    printf("||Promedio del Turn Around time: %f            \n", promTAT);
    printf("||==========================================|| \n");
}

void connectionPrintJobTaskListF()
{
    if (optionG == 4)
    {
        /* code */
        printJobTaskListR();
    }
    else
    {
        /* code */
        printJobTaskListF();
    }
}
/////////////////////////////////////////////////////
//                 CREA UN JSON                    //
/////////////////////////////////////////////////////

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

void setQuantum(int pQuantum)
{
    quantum = pQuantum;
}

int getQuantum()
{
    return quantum;
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

        globalProcess++;
        printf("\n||==========================================|| \n");
        printf("|| ID : %d                                   \n", globalProcess);
        printf("|| BURST: %d                                 \n", burstT);
        printf("|| PRIORITY: %d                              \n", priorityT);
        printf("|| TIME IN : %d                                   \n", timeInjm);
        printf("||==========================================|| \n");
        timellegada = timeInjm;

        struct arguments *args = (struct arguments *)malloc(sizeof(struct arguments));
        GlobalID = GlobalID + 1;
        args->id = GlobalID;
        args->name = nameT;
        args->priority = priorityT;
        args->burst = burstT;

        // pthread_create(&jobScheduler, NULL, &jobSchedulerTask, (void *)args);
        jobSchedulerTask(args);

        jobj = makeJson(args->id);

        if (strcpy(temp_buff, json_object_to_json_string(jobj)) == NULL)
        {
            perror("strcpy");
        }

        //ACA LO ENVIA
        //send(sock, message, strlen(message), 0) < 0
        // printf("|| ESPERA 2 SEGUNDOS ANTES HACER EL ENVIO : ||\n");

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

/////////////////////////////////////////////////////
//                    ALG FIFO                    //
////////////////////////////////////////////////////
void *algorithmFIFO(void *unused)
{
    timeInjm = 0;
    while (1)
    {
        if (headTaskList == NULL)
        {
            countBurst = 0;
            timeSchedule = timeSchedule + 1;
            timeInjm = 0;
        }
        else if (countBurst == headTaskList->burst)
        {
            countBurst = 0;
            struct node *current = (struct node *)malloc(sizeof(struct node));
            current = (struct node *)getFirstRM();
            current->TAT = current->burst + current->wt;
            current->TimeOut = timeInjm;
            printf("\n||==========================================||\n");
            printf("||================Finalizado================||\n");
            printf("||El proceso %d con burst %d prioridad %d      ||\n", current->id, current->burst, current->priority);
            printf("||Ha terminado exitosamente su ejecucion    ||\n");
            printf("||==========================================||\n");
            insertF(current);
        }
        else
        {
            struct node *current = (struct node *)malloc(sizeof(struct node));
            countBurst = countBurst + 1;
            current = headTaskList->next;
            while (current != NULL)
            {
                current->wt = current->wt + 1;
                current = current->next;
            }
            current = NULL;
            free(current);
            timeInjm++;
            sleep(1);
        }
    }
}

/////////////////////////////////////////////////////
//                    ALG SJF                     //
////////////////////////////////////////////////////
void *algorithmSJF(void *unused)
{
    timeInjm = 0;
    while (1)
    {
        if (headTaskList == NULL)
        {
            countBurst = 0;
            timeInjm = 0;
            timeSchedule = timeSchedule + 1;
        }
        else if (countBurst == headTaskList->burst)
        {
            countBurst = 0;
            struct node *current = (struct node *)malloc(sizeof(struct node));
            current = (struct node *)getFirstRM();
            current->TAT = current->burst + current->wt;
            current->TimeOut = timeInjm;
            printf("\n||==========================================||\n");
            printf("||================Finalizado================||\n");
            printf("||El proceso %d con burst %d prioridad %d      ||\n", current->id, current->burst, current->priority);
            printf("||Ha terminado exitosamente su ejecucion    ||\n");
            printf("||==========================================||\n");
            insertF(current);
            if (headTaskList != NULL)
            {
                getMinBurst();
            }
        }
        else
        {
            struct node *current = (struct node *)malloc(sizeof(struct node));
            countBurst = countBurst + 1;
            current = headTaskList->next;
            while (current != NULL)
            {
                current->wt = current->wt + 1;
                current = current->next;
            }
            current = NULL;
            free(current);
            timeInjm++;
            sleep(1);
        }
    }
}

/////////////////////////////////////////////////////
//                    ALG HDF                     //
////////////////////////////////////////////////////
void *algorithmHDF(void *unused)
{
    timeInjm = 0;
    while (1)
    {
        if (headTaskList == NULL)
        {
            countBurst = 0;
            timeInjm = 0;
            timeSchedule = timeSchedule + 1;
        }
        else if (countBurst == headTaskList->burst)
        {
            countBurst = 0;
            struct node *current = (struct node *)malloc(sizeof(struct node));
            current = (struct node *)getFirstRM();
            current->TAT = current->burst + current->wt;
            current->TimeOut = timeInjm;
            printf("\n||==========================================||\n");
            printf("||================Finalizado================||\n");
            printf("||El proceso %d con burst %d prioridad %d      ||\n", current->id, current->burst, current->priority);
            printf("||Ha terminado exitosamente su ejecucion    ||\n");
            printf("||==========================================||\n");
            insertF(current);
            if (headTaskList != NULL)
            {
                getMinPriority();
            }
        }
        else
        {
            struct node *current = (struct node *)malloc(sizeof(struct node));
            countBurst = countBurst + 1;
            current = headTaskList->next;
            while (current != NULL)
            {
                current->wt = current->wt + 1;
                current = current->next;
            }
            current = NULL;
            free(current);
            timeInjm++;
            sleep(1);
        }
    }
}

/////////////////////////////////////////////////////
//                    ALG RR                      //
////////////////////////////////////////////////////

void *algorithmRR(void *unused)
{
    timeInjm = 0;
    RRpointer = headTaskList;
    while (1)
    {
        if (RRpointer != NULL)
        {
            if (headTaskList == NULL)
            {
                countBurst = 0;
                timeInjm = 0;
                timeSchedule = timeSchedule + 1;
            }
            else if (RRpointer->burst > quantum)
            {
                RRpointer->burst = RRpointer->burst - quantum;
                struct node *current = (struct node *)malloc(sizeof(struct node));
                current = headTaskList;
                while (current != NULL)
                {
                    if (current == RRpointer)
                    {
                        /* code */
                        current = current->next;
                    }
                    else
                    {
                        /* code */
                        current->wt = current->wt + quantum;
                        current = current->next;
                    }
                }
                timeInjm = timeInjm + quantum;
                sleep(quantum);
                RRpointer = RRpointer->next;
                if (RRpointer != NULL)
                {
                    /* code */
                    printf("\n||==========================================||\n");
                    printf("||=============Cambio de proceso============||\n");
                    printf("||El proceso %d con burst %d prioridad %d      ||\n", RRpointer->id, RRpointer->burst, RRpointer->priority);
                    printf("||Entra en ejecucion...                     ||\n");
                    printf("||==========================================||\n");
                }
            }
            else
            {
                struct node *current = (struct node *)malloc(sizeof(struct node));
                current = headTaskList;
                while (current != NULL)
                {
                    if (current == RRpointer)
                    {
                        /* code */
                        current = current->next;
                    }
                    else
                    {
                        /* code */
                        current->wt = current->wt + RRpointer->burst;
                        current = current->next;
                    }
                }
                RRpointer->TimeIn = timeInjm;
                timeInjm = timeInjm + RRpointer->burst;
                RRpointer->TimeOut = timeInjm;

                RRpointer->TAT = RRpointer->TimeOut - RRpointer->TimeInAux;
                RRpointer->wt = RRpointer->TAT - RRpointer->burstBK;

                RRpointer->burst = 0;
                printf("\n||==========================================||\n");
                printf("||================Finalizado================||\n");
                printf("||El proceso %d con burst %d prioridad %d      ||\n", RRpointer->id, RRpointer->burst, RRpointer->priority);
                printf("||Proceso finalizado...                     ||\n");
                printf("||==========================================||\n");
                sleep(RRpointer->burst);
                clean();
            }
        }
        else
        {
            RRpointer = headTaskList;
        }
    }
}

int connection(int pParameter)
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

    switch (pParameter)
    {
    case 1:
        /* code */
        pthread_create(&CPUScheduler, NULL, &algorithmFIFO, NULL);
        break;

    case 2:
        /* code */
        pthread_create(&CPUScheduler, NULL, &algorithmSJF, NULL);
        break;

    case 3:
        /* code */
        pthread_create(&CPUScheduler, NULL, &algorithmHDF, NULL);
        break;

    case 4:
        /* code */
        pthread_create(&CPUScheduler, NULL, &algorithmRR, NULL);
        optionG = 4;
        break;
    }

    while ((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c)))
    {
        // puts("Connection accepted");

        pthread_t jobScheduler;
        new_sock = malloc(1);
        *new_sock = client_sock;

        if (pthread_create(&jobScheduler, NULL, connection_handler, (void *)new_sock) < 0)
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
