// Очереди сообщений SystemV в Linux. Вариант 10

#include <sys/types.h> 
#include <sys/stat.h> 
#include<sys/msg.h>
#include <wait.h> 
#include <fcntl.h> 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <dlfcn.h> 

#define MAXLINE 60
#define BUF_SIZE 1024

void server(int first, int second)
{
	int len=sizeof(mybuf);

	/* получение строки с тройкой аргументов из очереди */
	while(1)
	{
		if( (msgrcv(first,(struct msgbuf*)&mybuf,len,0,0)) > 0 )
		{
			/* разбиение строки на три аргумента */
			char args[MAXLINE+1]; 
			strcpy(args,mybuf.message);
			char *arg[3];
			char *tok;
			int i = 1;
			tok = strtok(args," ");
			while (tok != NULL) 
			{	
			    arg[i] = tok;
			    i=i+1;
				tok = strtok(NULL," ");
			}
			
			int max = atoi(arg[3]); //  количество замен

			void *ext_library;  // хандлер внешней библиотеки
		    double value=0;     // значение для теста
		    int (*func)(char *buffer, int nRead, int max);  // переменная для хранения адреса функции
		    int result;

		    int inputFd, outputFd, openFlags;
		    mode_t filePerms ;
		    ssize_t numRead;
		    char buf[BUF_SIZE];

		    // Загрузка библиотеки
		    ext_library = dlopen("/home/user/AA77/systemSoftware/lab5/libfsdyn.so",RTLD_LAZY);
		    if (!ext_library)
		    {
		        //если ошибка, то вывести ее на экран
		        fprintf(stderr,"dlopen() error: %s\n", dlerror());
		        result=-1;
		    };
		    printf("Library loaded!\n");

		    //загружаем из библиотеки требуемую процедуру
		    func = dlsym(ext_library, "DataProcessing");    
		    value=3.0;

		    /* Открытие файлов ввода и вывода */
		    inputFd = open (arg[1], O_RDONLY);
		    if (inputFd == -1)
		    {
		        printf ("Error opening file %s\n", arg[1]) ; 
		        result=-2;
		    }
		    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
		    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; /* rw - rw - rw - */
		    outputFd = open (arg [2], openFlags, filePerms);
		    if (outputFd == -1)
		    {
		        printf ("Error opening file %s\n ", arg[2]) ; 
		        result=-3;
		    }

		    memset(buf,0,BUF_SIZE);

		    /* Перемещение данных до достижения конца файла ввода или возникновения ошибки */
		    while ((numRead = read (inputFd, buf, BUF_SIZE)) > 0)
		    {
		        result=(*func)(buf,numRead,max);  
		        if (write (outputFd, buf, numRead) != numRead)
		        {
		            printf ("couldn't write whole buffer\n ");
		            result=-4;
		        }
		        if (numRead == -1)
		        {
		            printf ("read error\n "); 
		            result=-5;
		        }
		        if (close (inputFd ) == -1 )
		        {
		            printf ("close input error\n"); 
		            result=-6;
		        }
		        if (close (outputFd ) == -1 )
		        {
		            printf ("close output error\n"); 
		            result=-7;
		        }
		    }

			/* заполнение структуры с результатом обработки входного файла*/
			len=sizeof(mybuf);
			sprintf(mybuf.message,"%d\n", result);
			
			/* отправка в очередь результата обработки*/
			if(msgsnd(second,(struct msgbuf*)&mybuf,len,0) < 0)
			{
				printf("Error: Can't send message to queue\n");
				msgctl(second,IPC_RMID,0);
			}
			//printf("Server sent message: %s. PID %ld\n",mybuf.mtype,mybuf.message);
			exit(result);
		}
	}
	
}