#include <stdio.h>
#include <stdlib.h>

struct node
{
    int id;
    int burst;
    int priority;
    struct node *next;
};

struct node *head = NULL;
struct node *current = NULL;

//insert link at the first location
int insert(int pID, int pPrority, int pBurst)
{
    //create a link
    struct node *link = (struct node *)malloc(sizeof(struct node));

    //link->key = key;
    link->id = pID;
    link->priority = pPrority;
    link->burst = pBurst;

    //point it to old first node
    if (head == NULL)
    {
        head = link;
        return 1;
    }
    current = head;
    while (current->next != NULL)
    {
        current = current->next;
    }
    current->next = link;
    link = NULL;
    free(link);
    return 1;
}

void printJobTaskList()
{
    current = head;
    while (current != NULL)
    {
        printf("Burst: %d", current->burst);
        printf("->");
        current = current->next;
    }
}

struct node *getFirstRM()
{
    current = head;
    head = head->next;
    return current;
}

// int main()
// {
//     insert(1, 1, 1);
//     insert(2, 2, 2);
//     insert(3, 3, 3);
//     insert(4, 4, 4);
//     insert(5, 5, 5);
//     insert(6, 6, 6);

//     getFirstRM();
//     return 0;
// }