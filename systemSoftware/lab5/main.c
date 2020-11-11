// Очереди сообщений SystemV в Linux. Вариант 10
// Компиляция gcc main.c -c, компановка gcc main.o -o main -ldl
// Запуск ./main inputFile.txt outputFile.txt 4 inputFile1.txt outputFile1.txt 3

#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "mesg.h"
#include "server.c"
#include "client.c"

#define MAXLINE 60

int main(int argc, char *argv[]) 
{

  // проверка на количество аргументов в командной строке 
  if ( argc < 4 || (argc-1)%3 != 0 ) 
  {
      printf("Usage: file textfile1 textfile2 ...\n");
      exit(-1);
  }

  int countInput = (argc-1)/3; // количество входных файлов
  int msqid[countInput][2]; // массив дескрипторов System V IPC для очередей сообщений
  char pathname1[]="DataProcessing.c", pathname2[]="DataProcessing.o"; // файлы для генерации ключей
  key_t key[countInput][2]; // массив ключей System V IPC для очередей сообщений
   
  int oflag = 0666 | IPC_CREAT; 
  
  int i, pid[countInput];
  char buff[MAXLINE]; // строка для i-ой тройки аргументов
  char arg1[20], arg2[20], arg3[20]; // для трех аргуметов

  for (i = 0; i < countInput; i+=1) 
  { 
    /* создание пары ключей */
    if( (key[i][0]=ftok(pathname1,0)) < 0 | (key[i][1]=ftok(pathname2,0)) < 0)
    {
      printf("Error: can't generate keys\n");
      exit(-1);
    }
    
    /* создание пары очередей для i-й тройки */
    if( (msqid[i][0]=msgget(key[i][0], oflag)) < 0 | (msqid[i][1]=msgget(key[i][1], oflag)) < 0)
    {
      printf("Error: can't get msqid\n");
      exit(-2);
    }
    
    /* создание i-го дочернего процесса */
    pid[i] = fork(); 
    if (pid[i] == 0) 
    {
      /* child */
      
      server(msqid[i][0],msqid[i][1]);
      exit(0);
    }
    else if (pid[i] < 0)
    {
      printf("Error: can't fork.\n");
      exit(-3);
    }
      /* parent */
      
      strcpy(arg1,argv[i*3+1]);
      strcpy(arg2,argv[i*3+2]);
      strcpy(arg3,argv[i*3+3]);
      /* отправка i-ой строки с аргументами в 1 очередь*/
      sprintf(buff, "%s %s %s", arg1, arg2, arg3); // строка с i-ой тройкой аргументами без исполняемого файла
      /* В качестве типа очереди используется pid дочернего процесса.
      Так все сообщения могут быть прочитаны только теми процессами, которым они адресованы.*/
      mybuf.mtype=pid[i];
      client(buff, msqid[i][0], msqid[i][1]); 
  }

  sleep(1);

  /* ожидание окончания выполнения всех запущенных дочерних процессов */
  int status, stat;
  for (i = 0; i < countInput; i+=1) 
  { 
    status=waitpid(pid[i],&stat,0);
    if (pid[i] == status) 
    {
      printf("File %s done, result=%d\n",argv[3*i+1],WEXITSTATUS(stat));
    }
  }

  return 0; 
}

