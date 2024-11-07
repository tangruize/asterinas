// SPDX-License-Identifier: MPL-2.0

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>

void acquire_lock(int fd, int lock_type, int offset, int length, const char* process_name) {
    struct flock lock;
    
    lock.l_type = lock_type;        // F_WRLCK for write lock, F_RDLCK for read lock
    lock.l_whence = SEEK_SET;       // Lock relative to the beginning of the file
    lock.l_start = offset;          // Start of the lock
    lock.l_len = length;            // Length of the lock (1 byte in this case)

    // Attempt to acquire the lock
    const char* lock_type_str = (lock_type == F_WRLCK) ? "WRITE" : "READ";
    printf("%s process attempts to acquire %s lock on range [%d, %d)\n", process_name, lock_type_str, offset, offset + length);

    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
    
    printf("%s process acquired %s lock on range [%d, %d)\n", process_name, lock_type_str, offset, offset + length);
}

void release_lock(int fd, int offset, int length, const char* process_name) {
    struct flock lock;
    
    lock.l_type = F_UNLCK;          // Unlock the lock
    lock.l_whence = SEEK_SET;       // Lock relative to the beginning of the file
    lock.l_start = offset;          // Start of the lock
    lock.l_len = length;            // Length of the lock

    // Attempt to release the lock
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
    
    printf("%s process released lock on range [%d, %d)\n", process_name, offset, offset + length);
}

int main() {
    int fd;
    // pid_t pid;
    
    // Create a temporary file for locking (use O_CREAT | O_TRUNC to create a new empty file)
    fd = open("lockfile", O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Write something to the file to extend its size
    if (write(fd, "abc", 3) != 3) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    // Parent acquires WRITE lock on range [0, 1)
    acquire_lock(fd, F_WRLCK, 0, 1, "Parent");

    printf("sleep now!\n");
    sleep(100000);

    // // Fork a child process
    // pid = fork();
    // if (pid < 0) {
    //     perror("fork");
    //     exit(EXIT_FAILURE);
    // }

    // if (pid == 0) {
    //     // Child process: Try to acquire WRITE lock on the same range [0, 1)
    //     acquire_lock(fd, F_WRLCK, 0, 1, "Child");

    //     // Child process exits
    //     close(fd);
    //     printf("Child process exits\n");
    //     exit(EXIT_SUCCESS);
    // } else {
    //     // Parent process: Add a short delay to ensure the child runs and tries to acquire the lock first
    //     sleep(3);

    //     // Parent process: Try to acquire READ lock on the same range [0, 1)
    //     acquire_lock(fd, F_RDLCK, 0, 1, "Parent");

    //     // Simulate holding the read lock for a short period
    //     sleep(3);

    //     // Parent process: Release the read lock
    //     release_lock(fd, 0, 1, "Parent");

    //     // Now wait for the child process to complete after releasing the read lock
    //     int status;
    //     pid_t result = waitpid(pid, &status, WNOHANG);
    //     int timeout = 10;

    //     while (result == 0 && timeout > 0) {
    //         sleep(1);  // Sleep for 1 second
    //         timeout--;
    //         result = waitpid(pid, &status, WNOHANG);  // Check if the child has exited
    //     }

    //     if (result == 0) {
    //         printf("Timeout: Child process did not exit within 10 seconds\n");
    //     } else if (result == -1) {
    //         perror("waitpid");
    //     } else {
    //         printf("Waitpid: Child process exited\n");
    //     }

    //     // Close the file and finish
    //     close(fd);
    //     printf("Parent process exits\n");
    // }

    return 0;
}

