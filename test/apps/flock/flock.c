// SPDX-License-Identifier: MPL-2.0

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/wait.h>

int main() {
    const char *filename = "testfile.txt";

    // Open the file twice
    int fd1 = open(filename, O_RDWR | O_CREAT, 0666);
    if (fd1 < 0) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == 0)
    {
        int fd2 = open(filename, O_RDWR);
        if (fd2 < 0) {
            perror("Failed to open file");
            close(fd1);
            exit(EXIT_FAILURE);
        }
        
        sleep(3);

        // Try to place exclusive lock on fd2
        printf("Child tries to place exclusive lock placed on fd2\n");
        if (flock(fd2, LOCK_EX) < 0) {
            perror("Failed to lock fd2");
            flock(fd1, LOCK_UN); // Unlock fd1 before exiting
            close(fd1);
            close(fd2);
            exit(EXIT_FAILURE);
        }
        printf("Child: exclusive lock placed on fd2\n");

        flock(fd2, LOCK_UN);
        printf("Child released lock\n");
        close(fd2);

        printf("Child exits\n");
        exit(EXIT_SUCCESS);
    }

    // Try to place exclusive lock on fd1
    printf("Parent tries to place exclusive lock placed on fd1\n");
    if (flock(fd1, LOCK_EX) < 0) {
        perror("Failed to lock fd1");
        close(fd1);
        exit(EXIT_FAILURE);
    }
    printf("Parent: exclusive lock placed on fd1\n");

    sleep(6);

    // Unlock and close file descriptors
    flock(fd1, LOCK_UN);
    printf("Parent released lock\n");
    close(fd1);

    // Now wait for the child process to complete after releasing the read lock
    int status;
    pid_t result = waitpid(pid, &status, WNOHANG);
    int timeout = 10;

    while (result == 0 && timeout > 0) {
        sleep(1);  // Sleep for 1 second
        timeout--;
        result = waitpid(pid, &status, WNOHANG);  // Check if the child has exited
    }

    if (result == 0) {
        printf("Timeout: Child process did not exit within 10 seconds\n");
    } else if (result == -1) {
        perror("waitpid");
    } else {
        printf("Waitpid: Child process exited\n");
    }

    printf("Parent exits\n");
    return 0;
}

