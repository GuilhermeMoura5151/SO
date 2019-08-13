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
	int msgid = msg_connect(KEY);
    while(1)
    {
		msg_receive(msgid,);
    }
    return 0;
}
