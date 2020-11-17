// Разделяемая память с семафорами SystemV в Linux. Вариант 10

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

void server(int semid, int shmid)
{
	
	/* получение строки с тройкой аргументов из очереди */
	if( semop(semid,&lock_res,1) == -1)
    {
      perror("semop: lock_res");
      exit(-1);
    }

	/* подключение сегмента разделяемой памяти */      
    char* ptr=shmat(shmid, NULL,0);
	char* ptr1=ptr;

	/* разбиение строки на три аргумента */
	char args[MAXLINE+1]; 
	strcpy(args,ptr);
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
    mode_t filePerms;
    ssize_t numRead;
    char buf[BUF_SIZE];

    // Загрузка библиотеки
    ext_library = dlopen("/home/user/AA77/systemSoftware/lab6/libfsdyn.so",RTLD_LAZY);
    if (!ext_library)
    {
        //если ошибка, то вывести ее на экран
        result=-1;
        perror("dlopen: library can't load");
        exit(-1);
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
        result=-1;
        exit(-1);
    }
    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; /* rw - rw - rw - */
    outputFd = open (arg [2], openFlags, filePerms);
    if (outputFd == -1)
    {
        printf ("Error opening file %s\n ", arg[2]) ; 
        result=-1;
        exit(-1);
    }

    memset(buf,0,BUF_SIZE);
    /* Перемещение данных до достижения конца файла ввода или возникновения ошибки */
    while ((numRead = read (inputFd, buf, BUF_SIZE)) > 0)
    {
        result=(*func)(buf,numRead,max);  
        if (write (outputFd, buf, numRead) != numRead)
        {
            printf ("couldn't write whole buffer\n ");
            result=-1;
            exit(-1);
        }
        if (numRead == -1)
        {
            printf ("read error\n "); 
            result=-1;
            exit(-1);
        }
        if (close (inputFd ) == -1 )
        {
            printf ("close input error\n"); 
            result=-1;
            exit(-1);
        }
        if (close (outputFd ) == -1 )
        {
            printf ("close output error\n"); 
            result=-1;
            exit(-1);
        }
    }
	
	/* заполнение массива с результатом обработки входного файла*/
	sprintf(buf,"File %s done, result=%d\n", arg[1], result);
    strcpy(ptr1,buf);
	
	/* отключение сегмента памяти */
    shmdt(ptr);
	/* снимается блокировка ресурса */
    semop(semid,&rel_res,1);
	
}