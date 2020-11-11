// Обработка текстового файла inputFile средствами системых вызовов Linux, сама обработка находится в библиотеке libfsdyn.so
//Компиляция gcc ./lab2.c -o resultdyn -ldl

#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <dlfcn.h> 

#define BUF_SIZE 1024

int main (int argc, char * argv [ ])
{
    void *ext_library;  // хандлер внешней библиотеки
    double value=0;     // значение для теста
    int (*func)(char *buffer, int nRead, int max);  // переменная для хранения адреса функции
    int result;

    int inputFd, outputFd, openFlags;
    mode_t filePerms ;
    ssize_t numRead;
    int max;
    char buf[BUF_SIZE];

    // Загрузка библиотеки
    ext_library = dlopen("/home/user/AA77/systemSoftware/lab3/libfsdyn.so",RTLD_LAZY);
    if (!ext_library)
    {
        //если ошибка, то вывести ее на экран
        fprintf(stderr,"dlopen() error: %s\n", dlerror());
        return 1;
    };
    printf("Library loaded!\n");
    //загружаем из библиотеки требуемую процедуру
    func = dlsym(ext_library, "DataProcessing");    
    value=3.0;

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
        result=(*func)(buf,numRead,max);  
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
    exit(result);

    //закрываем библиотеку
    dlclose(ext_library); 
}