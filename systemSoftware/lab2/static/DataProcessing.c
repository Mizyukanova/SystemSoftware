/* Функция обработки текста*/
// Компиляция gcc -c DataProcessing.c 
// Упаковка в библиотеку ar rc libfs.a DataProcessing.c 
// Индексация ranlib libfs.a

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