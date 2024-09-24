// SPDX-License-Identifier: MPL-2.0

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>
#include <time.h>
#include "semun.h"

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

void errExit(const char *msg) {
    err(EXIT_FAILURE, "%s\n", msg);
}

int main(int argc, char *argv[])
{
    // struct semid_ds ds;
    union semun // arg, 
    dummy;             /* Fourth argument for semctl() */
    int semid, j;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        printf("%s semid\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    semid = atoi(argv[1]);

    // arg.buf = &ds;
    // if (semctl(semid, 0, IPC_STAT, arg) == -1)
    //     errExit("semctl");

    // printf("Semaphore changed: %s", ctime(&ds.sem_ctime));
    // printf("Last semop():      %s", ctime(&ds.sem_otime));

    /* Display per-semaphore information */

    // arg.array = calloc(ds.sem_nsems, sizeof(arg.array[0]));
    // if (arg.array == NULL)
    //     errExit("calloc");
    // if (semctl(semid, 0, GETALL, arg) == -1)
    //     errExit("semctl-GETALL");

    printf("Sem #  Value  SEMPID  SEMNCNT  SEMZCNT\n");

    for (j = 0; j < 1; j++)
        printf("%3d   %5d   %5d  %5d    %5d\n", j,
                semctl(semid, j, GETVAL, dummy),
                semctl(semid, j, GETPID, dummy),
                semctl(semid, j, GETNCNT, dummy),
                semctl(semid, j, GETZCNT, dummy));

    exit(EXIT_SUCCESS);
}
