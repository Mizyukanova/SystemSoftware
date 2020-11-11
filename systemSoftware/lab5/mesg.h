#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/msg.h>
#include <errno.h>
#include <limits.h>



#define MY_DATA 60

struct mymsgbuf
{
	long mtype; /*тип сообщения*/
	char message[MY_DATA]; /*данные*/
} mybuf;
