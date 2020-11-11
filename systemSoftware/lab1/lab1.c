// Обработка текстового файла inputFile средствами системых вызовов Linux
//Компиляция gcc ./lab1.c -o ./lab1.bin -Wall
//Запуск ./lab1.bin inputFile.txt outputFile.txt 2
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#define BUF_SIZE 1024

/* Функция обработки текста*/
int DataProcessing(char *buffer, int nRead, int max)
{
    int i;
    int count = 0;
    for (i = 0; nRead - 2 > i; i++)
    {
        if ((buffer[i] == 32) && (max > count))
        {
            buffer[i] = buffer[0];
            count++;
        }
    }
    return 0;
}

int main (int argc, char * argv [ ])
{
    int inputFd, outputFd, openFlags;
    mode_t filePerms ;
    ssize_t numRead;
    int max;
    char buf[BUF_SIZE];

    if (argc != 4)
    {
        printf("Usage: %s old-file new-file \n", argv[0]); exit(-1);
    }
    /* Открытие файлов ввода и вывода */
    inputFd = open (argv[1], O_RDONLY);
    if (inputFd == -1)
    {
        printf ("Error opening file %s\n", argv[1]) ; exit(-2);
    }
    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; /* rw - rw - rw - */
    outputFd = open (argv [2], openFlags, filePerms);
    if (outputFd == -1)
    {
        printf ("Error opening file %s\n ", argv[2]) ; exit(-3);
    }

    /* Перемещение данных до достижения конца файла ввода или возникновения ошибки */
    while ((numRead = read (inputFd, buf, BUF_SIZE)) > 0)
    {
        max = atoi(argv[3]);
        DataProcessing(buf,numRead,max);  
        if (write (outputFd, buf, numRead) != numRead)
        {
            printf ("couldn't write whole buffer\n "); exit(-4);
        }
        if (numRead == -1)
        {
            printf ("read error\n "); exit(-5);
        }
        if (close (inputFd ) == -1 )
        {
            printf ("close input error\n"); exit(-6);
        }
        if (close (outputFd ) == -1 )
        {
            printf ("close output error\n"); exit(-7);
        }
    }
    exit(EXIT_SUCCESS);
}