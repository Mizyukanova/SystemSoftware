// Многозадачное программирование в Linux
// Входные параметры: по три аргумента для обработки
// Компиляция и компановка gcc parent.c -o parent
// Запуск ./parent inputFile.txt outputFile.txt 4 inputFile1.txt outputFile1.txt 3

#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) 
{
  int i, pid[argc], status, stat;
  char arg1[20], arg2[20], arg3[20];
  
  // для всех файлов, перечисленных в командной строке 
  if ( argc < 4 || (argc-1)%3 != 0 ) 
  {
      printf("Usage: file textfile1 textfile2 ...\n");
      exit(-1);
  }
  
  for (i = 1; i< argc; i+=3) 
  { 
  // запускаем дочерний процесс 
    strcpy(arg1,argv[i]);
    strcpy(arg2,argv[i+1]);
    strcpy(arg3,argv[i+2]);

    pid[i] = fork(); 
    if (pid[i] == 0) 
    {
      // если выполняется дочерний процесс 
      // вызов функции счета количества пробелов в файле
      if (execl("./resultdyn","resultdyn", arg1, arg2, arg3, NULL)<0) 
      {
       printf("ERROR while start processing file %s\n",argv[i]);
       exit(-2);
      }
      else printf( "processing of file %s started (pid=%d)\n", argv[i],pid[i]);
    }
  // если выполняется родительский процесс
  }
  sleep(1);
  // ожидание окончания выполнения всех запущенных процессов
  for (i = 1; i< argc; i+=3) 
  { 
      status=waitpid(pid[i],&stat,0);
      if (pid[i] == status) 
      {
        printf("File %s done,  result=%d\n",argv[i],WEXITSTATUS(stat));
      }
  }
  return 0; 
}