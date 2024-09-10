// SPDX-License-Identifier: MPL-2.0

#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	assert(argc == 2);
	// printf("start app\n");
	fflush(0);
	int thread_cnt = atoi(argv[1]);
	syscall(500, thread_cnt);
	// printf("end app\n");
	// fflush(0);
	return 0;
}
