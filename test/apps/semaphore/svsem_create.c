// SPDX-License-Identifier: MPL-2.0

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
	int semid = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
    if (semid == -1)
        printf("semget failed\n");
	else {
		printf("semget %d\n", semid);
	}
}
