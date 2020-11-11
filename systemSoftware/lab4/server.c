// Неименованные каналы в Linux. Вариант 10

#include <sys/types.h> 
#include <sys/stat.h> 
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

void server(int readfd, int writefd)
{
	ssize_t n;
	char args[MAXLINE+1]; 
	
	/* получение строки с тройкой аргументов из канала IPC */
	if ((n = read(readfd, args, MAXLINE)) == 0)
	{
		printf("end-of-file while reading pathname");
		exit(1);
	}
	args[n] = '\0';	/* завершается 0 */
	
	/* разбиение строки на три аргумента */
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
	
	int max = atoi(arg[3]);
	void *ext_library;  // хандлер внешней библиотеки
    double value=0;     // значение для теста
    int (*func)(char *buffer, int nRead, int max);  // переменная для хранения адреса функции
    
	/* загрузка библиотеки */
    ext_library = dlopen("/home/user/AA77/СПО/ЛР4/libfsdyn.so",RTLD_LAZY);
    if (!ext_library)
    {
        //если ошибка, то вывести ее на экран
        fprintf(stderr,"dlopen() error: %s\n", dlerror());
        return;
    };
    printf("Library loaded!\n");
    
    /* загрузка требуемой процедуры из библиотеки */
    func = dlsym(ext_library, "DataProcessing");    
    value=3.0;
	
	/* открытие входного файла */
	int inputFd;
	char buff[BUF_SIZE];
	if ( (inputFd = open(arg[1], O_RDONLY)) < 0) 
	{	
		/* ошибка открытия входного файла */
		sprintf(buff, "%s: can't open. Error: %s\n",arg[1],strerror(errno));
		n = strlen(buff);
		 /* запись ошибки в канал IPC */
		write(writefd, buff, n);
	} 
	else 
	{
		/* входной файл успешно открыт, открываем выходной */
		int outputFd;
		int openFlags = O_CREAT | O_WRONLY | O_TRUNC;;
    	mode_t filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; /* rw - rw - rw - */
	    outputFd = open(arg[2], openFlags, filePerms);
	    if (outputFd == -1)
	    {
	        /* ошибка открытия выходного файла */
			sprintf(buff, "%s: can't open. Error: %s\n",arg[2],strerror(errno));
			n = strlen(buff);
			 /* запись ошибки в канал IPC */
			write(writefd, buff, n);
		}
		
		memset(buff,0,BUF_SIZE);
		int result;
		if ( (n = read(inputFd, buff, BUF_SIZE)) > 0)
		{
			 
	        result=(*func)(buff,n,max);  
	        if ( write(outputFd, buff, n) != n )
	        {
	            /* ошибка записи в файл */
				sprintf(buff, "%s: couldn't write whole buffer", arg[2]);
				n = strlen(buff);
				/* запись ошибки в канал IPC */
				write(writefd, buff, n);
	        }
	        if (n == -1)
	        {
	            /* ошибка чтения из файла */
				sprintf(buff, "%s: read error", arg[2]);
				n = strlen(buff);
				/* запись ошибки в канал IPC */
				write(writefd, buff, n);
	        }
	        if (close (outputFd ) == -1 )
	        {
	            /* ошибка закрытия файла */
				sprintf(buff, "%s: close output error", arg[2]);
				n = strlen(buff);
				/* запись ошибки в канал IPC */
				write(writefd, buff, n);
	        }
	        
	        sprintf(buff, "File %s done, result=%d\n", arg[1], result);
			write(writefd, buff, strlen(buff));
		}
		close(inputFd);
	}  
}