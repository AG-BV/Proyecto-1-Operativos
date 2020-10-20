/*
	C ECHO client example using sockets
*/
#include <stdio.h>       //printf
#include <string.h>      //strlen
#include <sys/socket.h>  //socket
#include <arpa/inet.h>   //inet_addr
#include <json-c/json.h> //JSON
#include <unistd.h>

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in server;
    char message[1000], server_reply[2000];

    //Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(8888);

    //Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");

    //keep communicating with server
    int flag = 0;
    while (1)
    {
        if (flag == 1)
        {
            printf("Enter message : ");
            scanf("%s", message);

            // //Send some data
            // if (send(sock, message, strlen(message), 0) < 0)
            // {
            //     puts("Send failed");
            //     return 1;
            // }

            //Receive a reply from the server
            if (recv(sock, server_reply, 2000, 0) < 0)
            {
                puts("recv failed");
                break;
            }

            puts("EL SERVER ENVIO :");
            puts(server_reply);
            memset(server_reply, 0, sizeof(server_reply));

            ///////////////////////////////////////
            ///////////////////////////////////////
            /*Creating a json object*/
            json_object *jobj = json_object_new_object();

            /*Creating a json string*/
            json_object *jstring = json_object_new_string("PONCHO");

            /*Creating a json integer*/
            json_object *jint = json_object_new_int(15);

            // /*Creating a json boolean*/
            // json_object *jboolean = json_object_new_boolean(1);

            // /*Creating a json double*/
            // json_object *jdouble = json_object_new_double(2.14);

            /*Creating a json array*/
            json_object *jarray = json_object_new_array();

            /*Creating json strings*/
            json_object *jstring1 = json_object_new_string("PAULA");
            json_object *jstring2 = json_object_new_string("PAULAA");
            json_object *jstring3 = json_object_new_string("Y PAULAAA");

            /*Adding the above created json strings to the array*/
            json_object_array_add(jarray, jstring1);
            json_object_array_add(jarray, jstring2);
            json_object_array_add(jarray, jstring3);

            /*Form the json object*/
            /*Each of these is like a key value pair*/
            json_object_object_add(jobj, "name", jstring);
            // json_object_object_add(jobj, "Technical blog", jboolean);
            // json_object_object_add(jobj, "Average posts per day", jdouble);
            json_object_object_add(jobj, "age", jint);
            json_object_object_add(jobj, "friends", jarray);

            printf("Size of JSON object- %lu\n", sizeof(jobj));
            printf("Size of JSON_TO_STRING- %lu,\n %s\n", sizeof(json_object_to_json_string(jobj)), json_object_to_json_string(jobj));
            char temp_buff[2000];

            if (strcpy(temp_buff, json_object_to_json_string(jobj)) == NULL)
            {
                perror("strcpy");
                return EXIT_FAILURE;
            }

            //ACA LO ENVIA
            //send(sock, message, strlen(message), 0) < 0
            if (send(sock, temp_buff, strlen(temp_buff),0) < 0 )
            {
                perror("demodemoserverAddrserverAddr");
                return EXIT_FAILURE;
            }
            /////////////////////////////////////////////
            /////////////////////////////////////////////
            /////////////////////////////////////////////
        }
        else
        {
            // memset(message, 0, sizeof(message));

            // //Send some data
            // if (send(sock, message, strlen(message), 0) < 0)
            // {
            //     puts("Send failed");
            //     return 1;
            // }
            ///////////////////////////////////////
            ///////////////////////////////////////
            /*Creating a json object*/
            json_object *jobj = json_object_new_object();

            /*Creating a json string*/
            json_object *jstring = json_object_new_string("PONCHO");

            /*Creating a json integer*/
            json_object *jint = json_object_new_int(15);

            // /*Creating a json boolean*/
            // json_object *jboolean = json_object_new_boolean(1);

            // /*Creating a json double*/
            // json_object *jdouble = json_object_new_double(2.14);

            /*Creating a json array*/
            json_object *jarray = json_object_new_array();

            /*Creating json strings*/
            json_object *jstring1 = json_object_new_string("PAULA");
            json_object *jstring2 = json_object_new_string("PAULAA");
            json_object *jstring3 = json_object_new_string("Y PAULAAA");

            /*Adding the above created json strings to the array*/
            json_object_array_add(jarray, jstring1);
            json_object_array_add(jarray, jstring2);
            json_object_array_add(jarray, jstring3);

            /*Form the json object*/
            /*Each of these is like a key value pair*/
            json_object_object_add(jobj, "name", jstring);
            // json_object_object_add(jobj, "Technical blog", jboolean);
            // json_object_object_add(jobj, "Average posts per day", jdouble);
            json_object_object_add(jobj, "age", jint);
            json_object_object_add(jobj, "friends", jarray);

            printf("Size of JSON object- %lu\n", sizeof(jobj));
            printf("Size of JSON_TO_STRING- %lu,\n %s\n", sizeof(json_object_to_json_string(jobj)), json_object_to_json_string(jobj));
            char temp_buff[2000];

            if (strcpy(temp_buff, json_object_to_json_string(jobj)) == NULL)
            {
                perror("strcpy");
                return EXIT_FAILURE;
            }

            //ACA LO ENVIA
            //send(sock, message, strlen(message), 0) < 0
            if (send(sock, temp_buff, strlen(temp_buff),0) < 0 )
            {
                perror("demodemoserverAddrserverAddr");
                return EXIT_FAILURE;
            }
            /////////////////////////////////////////////
            /////////////////////////////////////////////
            /////////////////////////////////////////////

            //Receive a reply from the server
            if (recv(sock, server_reply, 2000, 0) < 0)
            {
                puts("recv failed");
                break;
            }

            puts("Server reply :");
            puts(server_reply);
            memset(server_reply, 0, sizeof(server_reply));
            flag++;
        }
    }

    close(sock);
    return 0;
}