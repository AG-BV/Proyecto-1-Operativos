/*
	C socket server example, handles multiple clients using threads
*/
#include <stdio.h>
#include "connection.c"

int main(int argc, char *argv[]){
    connection();
    return 0;
}
