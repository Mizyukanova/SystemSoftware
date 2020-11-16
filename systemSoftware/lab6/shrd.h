#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <errno.h>
#include <limits.h>

#define MAXLINE 256
#define BUF_SIZE 1024

union semun 
{ 
	int val;   // значение для SETVAL 
	struct semid_ds *buf; //буферы для IPC_STAT, IPC_SET 
	unsigned short *array;// массивы для GETALL, SETALL 
	struct seminfo *__buf; // буфер для IPC_INFO 
}; 

struct sembuf lock_res={0,-1,0};
struct sembuf rel_res={0,1,0};

