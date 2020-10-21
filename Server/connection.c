#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>    
#include <pthread.h>
#include <json-c/json.h>
#include "JobTaskList.c"

pthread_t jobScheduler;
int GlobalID = 0;

struct arguments
{
    int id;
    int priority;
    int burst;
};

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
    size_t n_friends;
    int burstT;
    int priorityT;

    size_t i;

    int sock = *(int *)socket_desc;
    int read_size;
    char *message;
    char client_message[2000];

    message = " Greetings! I am your connection handler\n Now type something and i shall repeat what you type \n";
    write(sock, message, strlen(message));


    while ((read_size = recv(sock, client_message, 2000, 0)) > 0)
    {
        parsed_json = json_tokener_parse(client_message);
        json_object_object_get_ex(parsed_json, "name", &name);
        json_object_object_get_ex(parsed_json, "burst", &burst);
        json_object_object_get_ex(parsed_json, "priority", &priority);

        burstT = json_object_get_int(burst);
        priorityT = json_object_get_int(priority);

        printf("BURST: %d\n", burstT);
        printf("PRIORITY: %d\n", priorityT);

        struct arguments *args = (struct arguments *)malloc(sizeof(struct arguments));
        args->id = rand();
        args->priority = priorityT;
        args->burst = burstT;

        puts("EL CLIENTE ENVIO :");
        puts(client_message);

        pthread_create(&jobScheduler, NULL, &jobSchedulerTask, (void *)args);

        write(sock, client_message, strlen(client_message));
        memset(client_message, 0, sizeof(client_message));
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
    puts("Socket created");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    listen(socket_desc, 3);

    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    while ((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c)))
    {
        puts("Connection accepted");

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;

        if (pthread_create(&sniffer_thread, NULL, connection_handler, (void *)new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }

        puts("Handler assigned");
    }

    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
}