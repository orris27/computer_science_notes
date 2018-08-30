#include <sys/types.h>
#include <sys/sem.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#define ERR_EXIT(m) do { perror(m); exit(EXIT_FAILURE); } while(0)


int sem_create(key_t key);
int sem_open(key_t key);
void sem_setval(int semid,int val);
int sem_getval(int semid);
void sem_delete(int semid);
void sem_p(int semid);
void sem_v(int semid);

