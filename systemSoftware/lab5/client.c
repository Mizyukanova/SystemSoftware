//Очереди сообщений SystemV в Linux. Вариант 10

#include <sys/types.h> 
#include <sys/stat.h> 
#include<sys/msg.h>
#include <wait.h> 
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define MAXLINE 60

void client(char *buff, int first, int second)
{
	/* заполнение структуры с тройкой аргументов*/
	int len=sizeof(mybuf);
	strcpy(mybuf.message,buff);

	/* Отправка тройки аргуметов */
	if( msgsnd(first,(struct msgbuf*)&mybuf,len,0) < 0 )
	{
		printf("Error: Can't send message to queue\n");
		msgctl(first,IPC_RMID,0);
	}
	
	//printf("Client sent a message: %s\n",mybuf.message);
	
	int i=0;
	len=sizeof(mybuf);
	/* прием результата обработки сервера*/
	while(1)
	{
		if( (len=msgrcv(second,(struct msgbuf*)&mybuf,len,0,0)) > 0 )
		{
			//printf("Recieved message type = %ld, message: %s\n",mybuf.mtype,mybuf.message);
			break;			
		}
	}
	
}