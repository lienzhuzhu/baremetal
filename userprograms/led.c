#include <stdio.h>
#include <sys/types.h> // getpid() pid_t, open() syscall
#include <sys/stat.h> // open() syscall
#include <fcntl.h> // open() syscall
#include <unistd.h> // getpid(), write() syscall, usleep() library function
#include <signal.h>
#include <stdlib.h> // exit() syscall

static int fd;

void sig_handler(int signum) {
    write(fd, "21,0", 4);
    close(fd);
    exit(0);
}

int main() {
    signal(SIGINT, sig_handler);

    fd = open("/proc/lz-gpio", O_RDWR);

    printf( "pid: %d\n", getpid() );

    while (1) {
        write(fd, "21,1", 4);
        usleep(500000);
        write(fd, "21,0", 4);
        usleep(500000);
    }

    return 0;
}