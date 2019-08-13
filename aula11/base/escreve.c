#include "message.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define KEY 0xFF2FF6

static struct cont
{
    long client;
    char data[512];
} container;

int main(void)
{
	int msgid = msg_create(KEY);
    while(1)
    {
		scanf("%s",cont.msg);
		cont.client=1L;
		msg_send_nb(msgid, cont, 512);
    }
    return 0;
}

