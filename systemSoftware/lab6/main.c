// Разделяемая память с семафорами SystemV в Linux. Вариант 10
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

#include "shrd.h"
#include "server.c"

int main(int argc, char *argv[]) 
{

  // проверка на количество аргументов в командной строке 
  if ( argc < 4 || (argc-1)%3 != 0 ) 
  {
      printf("Usage: file textfile1 textfile2 ...\n");
      exit(-1);
  }

  int countInput = (argc-1)/3; // количество входных файлов
  
  key_t key[countInput]; // массив ключей для разделяемой памяти и семафоров
  char* pathname=argv[1]; // переменная для хранения имени файла для генерации ключа
  
  int i, oflag, pid[countInput];
  char buff[MAXLINE]; // строка для i-ой тройки аргументов
  char arg1[20], arg2[20], arg3[20]; // для трех аргуметов
  union semun arg[countInput];
  int shmid[countInput]; // массив дескрипторов для разделяемой памяти
  int semid[countInput]; // массив дескрипторов для семафором
  char* ptr[countInput]; // массив областей разделяемой памяти
  
  for (i = 0; i < countInput; i+=1) 
  { 
    /* формирование строки с i-ой тройкой необходимых параметров */
    strcpy(arg1,argv[i*3+1]);
    strcpy(arg2,argv[i*3+2]);
    strcpy(arg3,argv[i*3+3]);
    sprintf(buff, "%s %s %s", arg1, arg2, arg3); // строка с i-ой тройкой аргументами без исполняемого файла
  
    /* создание ключа */
    strcpy(pathname,arg1); // имя i-го входного файла
    if( (key[i]=ftok(pathname,0)) < 0 )
    {
      perror("ftok: can't generate key");
      break;
    }

    /* создание нового сегмента памяти и семафора для i-й тройки */
    oflag = 0666 | IPC_CREAT; 
    if( (shmid[i]=shmget(key[i], 256, oflag)) < 0 )
    {
      perror("shmget: can't get shmid");
      break;
    }
    if( (semid[i]=semget(key[i], 1, oflag)) < 0 )
    {
      perror("semget: can't get semid");
      break;
    }

    /* инициализация семафора */
    arg[i].val=1;
    semctl(semid[i], 0, SETVAL, arg[i]);

    /* блокировка ресурса */
    if (semop(semid[i],&lock_res,1) == -1)
    {
      perror("semop: lock_res");
      break;
    }
    
    /* подключение сегмента разделяемой памяти */
    *(ptr+i)=shmat(shmid[i], NULL,0);

    /* копирование строки с i-ой тройкой в разделяемую память */
    strcpy(*(ptr+i),buff);

    /* отключение сегмента памяти */
    shmdt(*(ptr+i));
    
    /* снимается блокировка ресурса */
    semop(semid[i],&rel_res,1);

    /* создание i-го дочернего процесса */
    pid[i] = fork(); 
    if (pid[i] == 0) 
    {
      /* child */
      server(semid[i],shmid[i]);
      exit(0);
    }
    else if (pid[i] < 0)
    {
      perror("fork: can't fork");
      break;
    }
    /* parent */
  }

  sleep(1);

  /* ожидание окончания выполнения всех запущенных дочерних процессов */
  int status;
  for (i = 0; i < countInput; i+=1) 
  { 
    status=waitpid(pid[i],NULL,0);
    if (pid[i] == status) 
    {
      /* прием результата обработки сервера*/
      if( semop(semid[i],&lock_res,1) == -1)
      {
        perror("semop: lock_res");
        exit(-7);
      }
      
      /* подключение сегмента разделяемой памяти */      
      *(ptr+i)=shmat(shmid[i], NULL,0);

      strcpy(buff, *(ptr+i));
      printf("%s",buff);
      
      /* отключение сегмента памяти */
      shmdt(*(ptr+i));
      /* снимается блокировка ресурса */
      semop(semid[i],&rel_res,1);
      
      /* удаление семафора и сегмента разделяемой памяти*/
      shmctl(shmid[i],IPC_RMID, NULL);
      semctl(semid[i],0,IPC_RMID);

    }
  }

  return 0; 
}

