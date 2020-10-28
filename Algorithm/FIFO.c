#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <json-c/json.h>

struct node *headTaskList = NULL;

struct node
{
    int id;
    int burst;
    int priority;
    int wt;
    struct node *next;
};

struct node *getMinBurst()
{
    struct node *current = (struct node *)malloc(sizeof(struct node));
    struct node *min = (struct node *)malloc(sizeof(struct node));
    struct node *auxBack = (struct node *)malloc(sizeof(struct node));
    current = headTaskList;
    min = current;
    
    while (current->next != NULL)
    {
        if (current->next->burst < min->burst)
        {
            min = current->next;
            auxBack = current;
            current = current->next;
        }
        else
        {
            current = current->next;
        }
    }
    if(min != headTaskList){
        auxBack->next = min->next;
        min->next = headTaskList;
        headTaskList = min;
    }
    else
    {
        
    }
    return min;
}

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

int main()
{
    insert(1, 1, 5);
    insert(1, 1, 4);
    insert(1, 1, 3);
    insert(1, 1, 1);
    insert(1, 1, 2);

    getMinBurst();
}